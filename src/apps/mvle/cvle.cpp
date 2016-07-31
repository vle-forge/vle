/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/manager/Simulation.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Package.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/version.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>

#include <locale>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stack>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <getopt.h>

#ifdef VLE_HAVE_NLS
# ifndef ENABLE_NLS
#  define ENABLE_NLS
# endif
#  include <libintl.h>
#  include <locale.h>
#  define _(x) gettext(x)
#  define gettext_noop(x) x
#  define N_(x) gettext_noop(x)
#else
#  define _(x) x
#  define N_(x) x
#endif

typedef std::shared_ptr <vle::vpz::Vpz> VpzPtr;

void generate_template_line(std::ostream& header, std::ostream& firstline,
                            std::string name,
                            const std::unique_ptr<vle::value::Value>& value)
{
    std::stack <std::tuple<std::string, const vle::value::Value*>> stack;

    stack.push(std::make_tuple(name, value.get()));

    while (not stack.empty()) {
        std::string name;
        const vle::value::Value* value;

        std::tie(name, value) = stack.top();
        stack.pop();

        switch (value->getType()) {
        case vle::value::Value::SET:
            {
                auto& set = value->toSet();
                for (std::size_t i = 0, e = set.size(); i != e; ++i) {
                    std::string newname = name + '.' + std::to_string(i);
                    const vle::value::Value* newvalue = set.get(i).get();
                    stack.emplace(newname, newvalue);
                }
            }
        case vle::value::Value::MAP:
            for (auto& value_map: value->toMap()) {
                std::string newname = name + '.' + value_map.first;
                const vle::value::Value* newvalue = value_map.second.get();

                stack.emplace(newname, newvalue);
            }
            break;
        case vle::value::Value::BOOLEAN:
        case vle::value::Value::INTEGER:
        case vle::value::Value::DOUBLE:
        case vle::value::Value::STRING:
            header << name;
            firstline << value->writeToString();
            if (not stack.empty()) {
                header << ',';
                firstline << ',';
            }
            break;
        default:
            fprintf(stderr, _("fail to read a value (only bool, integer, double"
                " and string are available"));
            break;
        }
    }
}

void generate_template(std::string file, std::string package, std::string exp) noexcept
{
    std::ofstream ofs(file);
    if (not ofs.is_open()) {
        fprintf(stderr, "Failed to open `%s' to generate template", file.c_str());
        return;
    }

    std::ostringstream oss;

    try {
        auto ctx = vle::utils::make_context();
        vle::utils::Package pack(ctx);
        pack.select(package);
        vle::vpz::Vpz vpz(pack.getExpFile(exp, vle::utils::PKG_BINARY));

        auto it = vpz.project().experiment().conditions().begin();
        auto end = vpz.project().experiment().conditions().end();

        while (it != end) {
            auto jt = it->second.begin();
            auto endj = it->second.end();

            while (jt != endj) {
                if (not jt->second
                    or not jt->second->isSet()
                    or not jt->second->get(0))
                    continue;

                std::string name = it->first;
                name += '.';
                name += jt->first;

                generate_template_line(ofs, oss, name, jt->second->get(0));

                ++jt;

                if (jt != endj) {
                    ofs << ',';
                    oss << ',';
                }
            }

            ++it;

            if (it != end) {
                ofs << ',';
                oss << ',';
            }
        }

        ofs << '\n';
        ofs << oss.str() << '\n';
    } catch(const std::exception& e) {
        fprintf(stderr, "Failed to generate template file: %s\n", e.what());
    }
}

/** @e Accessor stores an access to a VPZ or an undefined string.
 * @code
 * Accessor a("generic");
 * Accessor b("cond.port");
 * Accessor c("cond.port.1.value");
 * @endcode
 */
struct Access
{
    Access(const std::string& str)
    {
        namespace ba = boost::algorithm;

        std::vector <std::string> tokens;
        ba::split(tokens, str, ba::is_any_of("."));

        switch (tokens.size()) {
        case 0:
            condition = str;
            return;
        case 2:
            port = tokens[1];
            condition = tokens[0];
            break;
        case 1:
            condition = tokens[0];
            break;
        default:
            condition = tokens[0];
            port = tokens[1];
            std::copy(tokens.begin() + 2, tokens.end(),
                      std::back_inserter(params));
            break;
        }

        assert(not condition.empty());
        assert((not params.empty() and not port.empty())
               or (params.empty() and not port.empty())
               or port.empty());
    }

    friend
    std::ostream& operator<<(std::ostream& os, const Access& access)
    {
        os << access.condition;

        if (not access.port.empty())
            os << '.' << access.port;

        for (std::size_t i = 0, e = access.params.size(); i != e; ++i)
            os << '.' << access.params[i];

        return os;
    }

    inline
    bool is_undefined_string() const
    {
        return port.empty();
    }

    /**
     * \c value function tries to convert the \c Access object into a
     * \c vle::value::Value pointer by browsing the experimental condition
     * of the \e vpz arguement.
     */
    const std::unique_ptr<vle::value::Value>* value(VpzPtr vpz) const
    {
        auto& cnd = vpz->project().experiment().conditions().get(condition);
        auto& set = cnd.getSetValues(port);

        if (params.empty())
            return &set.get(0);

        const std::unique_ptr<vle::value::Value>* current = &set.get(0);
        for (std::size_t i = 0, e = params.size(); i != e; ++i) {
            if ((*current)->isSet()) {
                errno = 0;
                long int val = strtol(params[i].c_str(), NULL, 10);

                if (errno or val < 0 or
                    val >= boost::numeric_cast<long int>(
                        (*current)->toSet().size()))
                    throw vle::utils::ArgError(
                        _("Fails to convert '%s.%s' parameter '%zu'"
                          " as correct set index"),
                        condition.c_str(), port.c_str(), i);

                current = &(*current)->toSet().get(val);
            } else if ((*current)->isMap()) {
                auto it = (*current)->toMap().find(params[i]);

                if (it == (*current)->toMap().end())
                    throw vle::utils::ArgError(
                        _("Fails to convert '%s.%s' parameter '%zu'"
                          " as correct map index"),
                        condition.c_str(), port.c_str(), i);

                current = &it->second;
            } else {
                throw vle::utils::ArgError(
                    _("Fails to convert '%s.%s' parameter '%zu' as correct "
                      "map index"), condition.c_str(), port.c_str(), i);
            }

            if (not current or not current->get())
                throw vle::utils::ArgError(
                    _("Fails to convert '%s'"), condition.c_str());
        }

        return current;
    }

    std::string condition;
    std::string port;
    std::vector <std::string> params;
};

/**
 * @brief Remove quotes from a string.
 * @details Remove quote in start and end position in the string.
 * @example
 * assert(cleanup_token("\"toto\"") == std::string("toto"));
 * assert(cleanup_token("ok") == std::string("ok"));
 * assert(cleanup_token("") == std::string());
 * assert(cleanup_token("\"xxxx") == std::string("\"xxxx"));
 * @endexemple
 *
 * @param str [description]
 * @return [description]
 */
static
std::string cleanup_token(const std::string &str)
{
    if (str.size() >= 2u and str[0] == '"' and str[str.size() - 1] == '"')
        return str.substr(1, str.size() - 2u);

    return str;
}

/**
 * @brief Convert and assign a string to a @e vle::value::Value.
 * @details Convert the string @e str and convert it into the @e
 *     vle::value::Value. Conversion only works for boolean, integer, double
 *     and string. Otherwise, an std::runtime_error is thrown.
 *
 * @param value The value to assign.
 * @param str The string to convert.
 *
 * @return [description]
 */
static
void assign_string_to_value(const std::unique_ptr<vle::value::Value>* value,
                            const std::string &str)
{
    assert(value);
    assert(value->get());

    switch ((*value)->getType()) {
    case vle::value::Value::BOOLEAN:
        (*value)->toBoolean().value() = vle::utils::to <bool>(str);
        break;

    case vle::value::Value::INTEGER:
        (*value)->toInteger().value() = vle::utils::to <std::int32_t>(str);
        break;

    case vle::value::Value::DOUBLE:
        (*value)->toDouble().value() = vle::utils::to <double>(str);
        break;

    case vle::value::Value::STRING:
        (*value)->toString().value() = str;
        break;

    default:
        throw vle::utils::ArgError(
            _("fail to assign string token `%s' to value"), str.c_str());
    }
}

struct ColumnDefinition
{
    ColumnDefinition(const std::string &str)
        : str(str)
        , value(NULL)
    {}

    ColumnDefinition(const std::unique_ptr<vle::value::Value> *value)
        : str(std::string())
        , value(value)
    {}

    std::string str;
    const std::unique_ptr<vle::value::Value> *value;
};

struct Columns
{
    typedef std::vector <ColumnDefinition> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

    void add(const std::string &str)
    {
        data.emplace_back(str);
    }

    void add(const std::unique_ptr<vle::value::Value> *value)
    {
        data.emplace_back(value);
    }

    std::size_t size() const
    {
        return data.size();
    }

    void update(std::size_t id, const std::string &str)
    {
        if (id >= data.size())
            throw vle::utils::InternalError(_("Too many column data"));

        if (not data[id].value)
            data[id].str = str;
        else
            assign_string_to_value(data[id].value, str);
    }

    void printf(FILE *f) const
    {
        for (std::size_t i = 0, e = data.size(); i != e; ++i)
            if (not data[i].value)
                fprintf(f, "%s ", data[i].str.c_str());
    }

    friend
    std::ostream& operator<<(std::ostream& os, const Columns& columns)
    {
        for (std::size_t i = 0, e = columns.data.size(); i != e; ++i)
            if (not columns.data[i].value)
                os << columns.data[i].str << ' ';

        return os;
    }

    container_type data;
};

std::ostream& operator<<(std::ostream &os,
                         const std::unique_ptr<vle::value::Matrix>& value)
{
    if  (value->rows() <= 0 or value->columns() <= 0)
        return os;

    for (std::size_t i = 0; i < value->columns(); ++i) {
        if (value->get(i, value->rows() - 1)) {
            switch (value->get(i, value->rows() - 1)->getType()) {
            case vle::value::Value::BOOLEAN:
                os << (value->get(i, value->rows() - 1)->toBoolean().value()
                       == true ? "true" : "false");
                break;

            case vle::value::Value::INTEGER:
                os << value->get(i, value->rows() - 1)->toInteger().value();
                break;

            case vle::value::Value::DOUBLE:
                os << value->get(i, value->rows() - 1)->toDouble().value();
                break;

            case vle::value::Value::STRING:
                os << '"' << value->get(i, value->rows() - 1)->toString()
                   << '"';
                break;

            default:
                throw vle::utils::FileError(
                    _("fail to write a value (only bool, integer, double "
                      "and string are available"));
            }
        }

        if (i != value->columns() - 1)
            os << ',';
    }

    return os;
}

class Worker
{
private:
    using MapPtr = std::unique_ptr<vle::value::Map>;

    vle::utils::ContextPtr m_context;
    std::string m_packagename;
    std::string m_vpzfilename;
    vle::manager::Simulation m_simulator;
    VpzPtr m_vpz;
    Columns m_columns;
    bool m_warnings;

    void simulate(std::ostream &os)
    {
        vle::manager::Error error;

        auto vpz = std::make_unique<vle::vpz::Vpz>(*m_vpz.get());

        auto result = m_simulator.run(std::move(vpz),  &error);

        if (error.code) {
            if (m_warnings) {
                fprintf(stderr, _("Simulation failed. %s [code: %d] in "),
                        error.message.c_str(), error.code);
                m_columns.printf(stderr);
            }
        } else if (result == NULL) {
            if (m_warnings) {
                fprintf(stderr, _("Simulation without result (try storage as"
                                  " output plug-in) in"));
                m_columns.printf(stderr);
            }
        } else {
            for (auto it = result->begin(), et = result->end(); it != et; ++it) {
                if (it->second && it->second->isMatrix()) {
                    os << vle::value::toMatrixValue(it->second);
                }
            }
        }
    }

public:
    Worker(const std::string &package, const std::string &vpz, bool warnings)
        : m_context(vle::utils::make_context())
        , m_packagename(package)
        , m_simulator(m_context, vle::manager::LOG_NONE,
                      vle::manager::SIMULATION_NONE, nullptr)
        , m_warnings(warnings)
    {
        vle::utils::Package pack(m_context);
        pack.select(m_packagename);
        m_vpz = std::make_unique<vle::vpz::Vpz>(
            pack.getExpFile(vpz, vle::utils::PKG_BINARY));
    }

    void init(const std::string &header)
    {
        namespace ba = boost::algorithm;

        std::vector <std::string> tokens;
        ba::split(tokens, header, ba::is_any_of(","));

        for (std::size_t i = 0, e = tokens.size(); i != e; ++i) {
            Access access(cleanup_token(tokens[i]));

            if (access.is_undefined_string()) {
                m_columns.add(access.condition);
            } else {
                m_columns.add(access.value(m_vpz));
            }
        }
    }

    std::string run(const std::string &block)
    {
        std::ostringstream result;
        std::vector <std::string> output;
        std::string::size_type begin = 0u;
        std::string::size_type end;
        result.imbue(std::locale::classic());
        result << std::setprecision(
            std::floor(
                std::numeric_limits<double>::digits * std::log10(2) + 2))
               << std::scientific;

        for (begin = 0, end = block.find('\n');
             begin < block.size();
             begin = end + 1, end = block.find('\n', end + 1)) {
            std::string buffer(block, begin, end - begin);
            boost::algorithm::split(output, buffer,
                                    boost::algorithm::is_any_of(","));

            for (std::size_t i = 0, e = output.size(); i != e; ++i) {
                std::string current = cleanup_token(output[i]);
                m_columns.update(i, current);
            }

            result << m_columns;
            simulate(result);
            result << '\n';
        }

        return result.str();
    }
};

template <typename T>
struct no_deleter
{
    void operator()(T *)
    {
    }
};

template <typename T>
std::shared_ptr <T> open(const std::string &file)
{
    auto fs = std::make_shared<T>(file.c_str());

    if (!fs->is_open())
        throw vle::utils::FileError(_("Fail to open file %s"), file.c_str());

    return fs;
}

class Root
{
    std::shared_ptr <std::istream> m_is;
    std::shared_ptr <std::ostream> m_os;
    std::ofstream m_ofs;
    int m_blocksize;

public:
    Root(const std::string &input, const std::string &output, int blocksize)
        : m_is(&std::cin, no_deleter <std::istream>())
        , m_os(&std::cout, no_deleter <std::ostream>())
        , m_blocksize(blocksize)
    {
        if (!input.empty())
            m_is = open <std::ifstream>(input);

        if (!output.empty())
            m_os = open <std::ofstream>(output);
    }

    bool header(std::string &header)
    {
        std::getline(*m_is.get(), header);
        return m_is.get()->good();
    }

    bool read(std::string &block)
    {
        int i = 0;
        block.clear();

        if (m_is->fail())
            return false;

        while (m_is->good() && i < m_blocksize) {
            std::string tmp;
            std::getline(*m_is.get(), tmp);

            if (not tmp.empty()) {
                block += tmp;
                block += '\n';
                ++i;
            } else {
                return not block.empty();
            }
        }

        return true;
    }

    void write(const std::string &block)
    {
        (*m_os.get()) << block;
        m_os->flush();
    }
};

enum CommunicationTag {
    worker_block_todo_tag,
    worker_block_end_tag,
    worker_end_tag
};

int run_as_master(const std::string &inputfile,
                  const std::string &outputfile,
                  int blocksize)
{
    int ret = EXIT_SUCCESS;
    int blockid = 0;

    try {
        Root r(inputfile, outputfile, blocksize);
        boost::mpi::communicator comm;
        std::vector <bool> workers(comm.size(), false);
        std::string block, header;
        r.header(header);
        boost::mpi::broadcast(comm, header, 0);

        for (int child = 1; child < comm.size(); ++child) {
            if (r.read(block)) {
                printf(_("master sends block %d to %d\n"), blockid++, child);
                comm.send(child, worker_block_todo_tag, block);
                workers[child] = true;
            } else
                break;
        }

        bool end = false;

        while (end == false) {
            boost::mpi::status msg = comm.probe();
            comm.recv(msg.source(), worker_block_end_tag, block);
            workers[msg.source()] = false;
            r.write(block);
            end = !r.read(block);

            if (!block.empty()) {
                printf(_("master sends block %d to %d\n"), blockid++,
                       msg.source());
                comm.send(msg.source(), worker_block_todo_tag, block);
                workers[msg.source()] = true;
            }
        }

        while (std::find(workers.begin(), workers.end(), true)
               != workers.end()) {
            boost::mpi::status msg = comm.probe();
            comm.recv(msg.source(), worker_block_end_tag, block);
            workers[msg.source()] = false;
            r.write(block);
        }

        for (int child = 1; child < comm.size(); ++child)
            comm.send(child, worker_end_tag);
    } catch (const std::exception &e) {
        std::cerr << "master fails: " << e.what() << std::endl;
        ret = EXIT_SUCCESS;
    }

    return ret;
}

int run_as_worker(const std::string& package,
                  const std::string& vpz,
                  bool warnings)
{
    int ret = EXIT_SUCCESS;

    try {
        boost::mpi::communicator comm;
        Worker w(package, vpz, warnings);
        std::string block;
        boost::mpi::broadcast(comm, block, 0);
        w.init(block);

        bool end = false;

        while (not end) {
            boost::mpi::status msg = comm.probe();

            switch (msg.tag()) {
            case worker_block_todo_tag:
                comm.recv(0, worker_block_todo_tag, block);
                block = w.run(block);
                comm.send(0, worker_block_end_tag, block);
                break;

            case worker_end_tag:
                comm.recv(0, worker_end_tag);
                end = true;
                break;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "worker fails: " << e.what() << std::endl;
        ret = EXIT_FAILURE;
    }

    return ret;
}

void show_help()
{
    printf(_("cvle [options...]\n\n"
             "  help,h                          Produce help message\n"
             "  package,P package file [file..] Set package name and files\n"
             "  input-file,i file               csv input file\n"
             "  output-file,o file              csv output file\n"
             "  warnings                        Show warnings in output\n"
             "  template,t file                 Generate a template csv input file\n"
             "  block-size,b size               Set number of lines to be sent"
             " [default 5000]\n"));
}

int main(int argc, char *argv[])
{
    std::string package_name;
    std::string input_file, output_file, template_file;
    int warnings = 0;
    int block_size = 5000;
    int ret = EXIT_SUCCESS;

    const char* const short_opts = "hP:i:o:t:b:";
    const struct option long_opts[] = {
        {"help", 0, nullptr, 'h'},
        {"package", 1, nullptr, 'P'},
        {"input-file", 1, nullptr, 'i'},
        {"output-file", 1, nullptr, 'o'},
        {"template", 1, nullptr, 't'},
        {"warnings", 0, &warnings, 1},
        {"block-size", 1, nullptr, 'b'},
        {0, 0, nullptr, 0}};
    int opt_index;

    for (;;) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts,
                                     &opt_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0:
            break;
        case 'h':
            show_help();
            break;
        case 'P':
            package_name = ::optarg;
            break;
        case 'i':
            input_file = ::optarg;
            break;
        case 'o':
            output_file = ::optarg;
            break;
        case 't':
            template_file = ::optarg;
            break;
        case 'b':
            try {
                block_size = std::stoi(::optarg);
            } catch(const std::exception& /* e */) {
                fprintf(stderr, _("Bad block size: %s. Assume block size=%d"),
                        ::optarg, block_size);
            }
            break;
        case '?':
        default:
            ret = EXIT_FAILURE;
            fprintf(stderr, _("Unknown command line option\n"));
            break;
        };
    }

    if (package_name.empty()) {
        printf(_("Usage: cvle --package test tutu.vpz -i in.csv -o out.csv\n"));
        return ret;
    }

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    std::vector<std::string> vpz(argv + ::optind, argv + argc);
    if (vpz.size() > 1)
        fprintf(stderr, _("Use only the first vpz: %s\n"), vpz.front().c_str());

    if (comm.rank() == 0 and not template_file.empty())
        generate_template(template_file, package_name, vpz.front());

    if (comm.size() == 1)  {
        fprintf(stderr, _("cvle needs two processors.\n"));
        return EXIT_FAILURE;
    }

    if (comm.rank() == 0) {
        printf(_("block size: %d\n"
                 "package   : %s\n"
                 "input csv : %s\n"
                 "output csv: %s\n"
                 "vpz       :"), block_size, package_name.c_str(),
               (input_file.empty()) ? "stdin" : input_file.c_str(),
               (output_file.empty()) ? "stdin" : output_file.c_str());
        for (const auto& elem : vpz)
            printf("%s ", elem.c_str());
        printf("\n");

        return run_as_master(input_file, output_file, block_size);
    }

    return run_as_worker(package_name, vpz.front(), warnings);
}
