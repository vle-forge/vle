/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2017 INRA http://www.inra.fr
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
#include <vle/utils/Package.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/vpz/SaxParser.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <stack>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#ifdef VLE_HAVE_NLS
#ifndef ENABLE_NLS
#define ENABLE_NLS
#endif
#include <libintl.h>
#include <locale.h>
#define _(x) gettext(x)
#define gettext_noop(x) x
#define N_(x) gettext_noop(x)
#else
#define _(x) x
#define N_(x) x
#endif

typedef std::shared_ptr<vle::vpz::Vpz> VpzPtr;

void
generate_template_line(std::ostream& header,
                       std::ostream& firstline,
                       std::string name,
                       const std::shared_ptr<vle::value::Value>& value)
{
    std::stack<std::tuple<std::string, const vle::value::Value*>> stack;

    stack.push(std::make_tuple(name, value.get()));

    while (not stack.empty()) {
        std::string name;
        const vle::value::Value* value;

        std::tie(name, value) = stack.top();
        stack.pop();

        switch (value->getType()) {
        case vle::value::Value::SET: {
            auto& set = value->toSet();
            for (std::size_t i = 0, e = set.size(); i != e; ++i) {
                std::string newname = name + '.' + std::to_string(i);
                const vle::value::Value* newvalue = set.get(i).get();
                stack.emplace(newname, newvalue);
            }
        } break;
        case vle::value::Value::MAP:
            for (auto& value_map : value->toMap()) {
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
            fprintf(stderr,
                    _("fail to read a value (only bool, integer,"
                      " double and string are available.\n"));
            break;
        }
    }
}

void
generate_template(std::string file,
                  std::string package,
                  std::string exp) noexcept
{
    std::ofstream ofs(file);
    if (not ofs.is_open()) {
        fprintf(
          stderr, "Failed to open `%s' to generate template\n", file.c_str());
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
                if (not jt->second.empty() or not jt->second[0])
                    continue;

                std::string name = it->first;
                name += '.';
                name += jt->first;

                generate_template_line(ofs, oss, name, jt->second[0]);

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
    } catch (const std::exception& e) {
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

        std::vector<std::string> tokens;
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
            std::copy(
              tokens.begin() + 2, tokens.end(), std::back_inserter(params));
            break;
        }

        assert(not condition.empty());
        assert((not params.empty() and not port.empty()) or
               (params.empty() and not port.empty()) or port.empty());
    }

    friend std::ostream& operator<<(std::ostream& os, const Access& access)
    {
        os << access.condition;

        if (not access.port.empty())
            os << '.' << access.port;

        for (std::size_t i = 0, e = access.params.size(); i != e; ++i)
            os << '.' << access.params[i];

        return os;
    }

    inline bool is_undefined_string() const
    {
        return port.empty();
    }

    /**
     * \c value function tries to convert the \c Access object into a
     * \c vle::value::Value pointer by browsing the experimental condition
     * of the \e vpz arguement.
     */
    vle::value::Value* value(VpzPtr vpz) const
    {
        auto& cnd = vpz->project().experiment().conditions().get(condition);
        auto& set = cnd.getSetValues(port);

        if (params.empty())
            return set[0].get();

        vle::value::Value* current = set[0].get();
        for (std::size_t i = 0, e = params.size(); i != e; ++i) {
            if (current->isSet()) {
                errno = 0;
                long int val = strtol(params[i].c_str(), NULL, 10);

                if (errno or val < 0 or
                    val >=
                      boost::numeric_cast<long int>(current->toSet().size()))
                    throw vle::utils::ArgError(
                      _("Fails to convert '%s.%s' parameter '%zu'"
                        " as correct set index"),
                      condition.c_str(),
                      port.c_str(),
                      i);

                current = current->toSet()[val].get();
            } else if (current->isMap()) {
                auto it = current->toMap().find(params[i]);

                if (it == current->toMap().end())
                    throw vle::utils::ArgError(
                      _("Fails to convert '%s.%s' parameter '%zu'"
                        " as correct map index"),
                      condition.c_str(),
                      port.c_str(),
                      i);

                current = it->second.get();
            } else {
                throw vle::utils::ArgError(
                  _("Fails to convert '%s.%s' parameter '%zu' as correct "
                    "map index"),
                  condition.c_str(),
                  port.c_str(),
                  i);
            }

            if (not current)
                throw vle::utils::ArgError(_("Fails to convert '%s'"),
                                           condition.c_str());
        }

        return current;
    }

    std::string condition;
    std::string port;
    std::vector<std::string> params;
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
static std::string
cleanup_token(const std::string& str)
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
static void
assign_string_to_value(vle::value::Value* value, const std::string& str)
{
    assert(value);

    switch (value->getType()) {
    case vle::value::Value::BOOLEAN:
        value->toBoolean().value() = vle::utils::to<bool>(str);
        break;

    case vle::value::Value::INTEGER:
        value->toInteger().value() = vle::utils::to<std::int32_t>(str);
        break;

    case vle::value::Value::DOUBLE:
        value->toDouble().value() = vle::utils::to<double>(str);
        break;

    case vle::value::Value::STRING:
        value->toString().value() = str;
        break;

    default:
        throw vle::utils::ArgError(
          _("fail to assign string token `%s' to value"), str.c_str());
    }
}

struct ColumnDefinition
{
    ColumnDefinition(const std::string& str_)
      : str(str_)
      , value(nullptr)
    {
    }

    ColumnDefinition(vle::value::Value* value_)
      : str()
      , value(value_)
    {
    }

    std::string str;
    vle::value::Value* value;
};

struct Columns
{
    typedef std::vector<ColumnDefinition> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

    void add(const std::string& str)
    {
        data.emplace_back(str);
    }

    void add(vle::value::Value* value)
    {
        data.emplace_back(value);
    }

    std::size_t size() const
    {
        return data.size();
    }

    void update(std::size_t id, const std::string& str)
    {
        if (id >= data.size())
            throw vle::utils::InternalError(_("Too many column data"));

        if (not data[id].value)
            data[id].str = str;
        else
            assign_string_to_value(data[id].value, str);
    }

    void printf(FILE* f) const
    {
        for (std::size_t i = 0, e = data.size(); i != e; ++i)
            if (not data[i].value)
                fprintf(f, "%s ", data[i].str.c_str());
    }

    friend std::ostream& operator<<(std::ostream& os, const Columns& columns)
    {
        for (std::size_t i = 0, e = columns.data.size(); i != e; ++i)
            if (not columns.data[i].value)
                os << columns.data[i].str << ' ';

        return os;
    }

    container_type data;
};

struct ConditionsBackup
{
    ConditionsBackup(const vle::vpz::Vpz& vpz)
      : mconds(vpz.project().experiment().conditions())
    {
    }

    static bool hasSimulationEngine(const vle::vpz::Conditions& other)
    {
        if (other.exist("_cvle_cond")) {
            const vle::vpz::Condition& cond_cvle = other.get("_cvle_cond");
            for (auto port : cond_cvle) {
                if (port.first == "has_simulation_engine") {
                    const std::vector<std::shared_ptr<vle::value::Value>>&
                      vals = port.second;
                    if (vals.size() > 0) {
                        return (vals.at(0)->isBoolean() and
                                vals.at(0)->toBoolean().value());
                    }
                }
            }
        }
        return false;
    }

    void modify(vle::vpz::Vpz& toupdate, const vle::vpz::Conditions& other)
    {
        // check if one should modify simulation_engine
        bool modif_engine = false;
        if (other.exist("_cvle_cond")) {
            const vle::vpz::Condition& cond_cvle = other.get("_cvle_cond");
            for (auto port : cond_cvle) {
                if (port.first == "has_simulation_engine") {
                    const std::vector<std::shared_ptr<vle::value::Value>>&
                      vals = port.second;
                    modif_engine =
                      (vals.size() > 0) and (vals.at(0)->isBoolean() and
                                             vals.at(0)->toBoolean().value());
                }
            }
        }

        // modify
        vle::vpz::Conditions& toup_conds =
          toupdate.project().experiment().conditions();
        for (auto cond : other) {
            if (cond.first != "_cvle_cond" and
                (cond.first != "simulation_engine" or modif_engine)) {
                if (not mconds.exist(cond.first)) {
                    throw vle::utils::InternalError(_("Unknown condition"));
                }
                vle::vpz::Condition& toup_cond = toup_conds.get(cond.first);
                for (auto port : cond.second) {

                    vle::vpz::ConditionValues::iterator toup_port =
                      toup_cond.conditionvalues().find(port.first);
                    if (toup_port != toup_cond.conditionvalues().end()) {
                        toup_cond.conditionvalues().erase(toup_port);
                    }
                    if (port.second.size() == 0) {
                        throw vle::utils::InternalError(_("No value"));
                    }
                    toup_cond.addValueToPort(port.first, port.second.at(0));
                }
            }
        }
    }

    void restoreBackup(vle::vpz::Vpz& toupdate,
                       const vle::vpz::Conditions& other)
    {
        vle::vpz::Conditions& toup_conds =
          toupdate.project().experiment().conditions();
        for (auto cond : other) {
            if (cond.first != "_cvle_cond") {
                if (not mconds.exist(cond.first)) {
                    throw vle::utils::InternalError(_("Unknown condition"));
                }
                toup_conds.del(cond.first);
                toup_conds.add(mconds.get(cond.first));
            }
        }
    }

    std::string getId(const vle::vpz::Conditions& other) const
    {
        if (not other.exist("_cvle_cond")) {
            return "no_id";
        }
        const vle::vpz::Condition& cond_cvle = other.get("_cvle_cond");
        vle::vpz::ConditionValues::const_iterator it_id =
          cond_cvle.conditionvalues().find("id");
        if (it_id == cond_cvle.conditionvalues().end()) {
            return "no_id";
        }
        const std::vector<std::shared_ptr<vle::value::Value>>& vals =
          it_id->second;
        if (vals.size() == 0) {
            return "no_id";
        }
        if (not vals.at(0)->isString()) {
            return "no_id";
        }
        return vals.at(0)->toString().value();
    }

    vle::vpz::Conditions mconds;
};

std::ostream&
operator<<(std::ostream& os, const std::unique_ptr<vle::value::Matrix>& value)
{
    if (value->rows() <= 0 or value->columns() <= 0)
        return os;

    for (std::size_t i = 0; i < value->columns(); ++i) {
        if (value->get(i, value->rows() - 1)) {
            switch (value->get(i, value->rows() - 1)->getType()) {
            case vle::value::Value::BOOLEAN:
                os << (value->get(i, value->rows() - 1)->toBoolean().value() ==
                           true
                         ? "true"
                         : "false");
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
    std::chrono::milliseconds m_timeout;
    std::string m_packagename;
    std::string m_vpzfilename;
    std::unique_ptr<vle::manager::Simulation> m_simulator;
    VpzPtr m_vpz;
    std::unique_ptr<Columns> m_columns;             // used for simple values
    std::unique_ptr<ConditionsBackup> m_conditions; // used for complex values
    bool m_warnings;

    void simulate(std::ostream& os)
    {
        vle::manager::Error error;

        auto vpz = std::make_unique<vle::vpz::Vpz>(*m_vpz.get());

        auto result = m_simulator->run(std::move(vpz), &error);

        if (error.code) {
            if (m_warnings) {
                fprintf(stderr,
                        _("Simulation failed. %s [code: %d] in "),
                        error.message.c_str(),
                        error.code);
                if (m_columns) {
                    m_columns->printf(stderr);
                }
            } else {
                os << error.message << "\n";
            }
        } else if (result == NULL) {
            if (m_warnings) {
                fprintf(stderr,
                        _("Simulation without result (try storage as"
                          " output plug-in) in"));
                if (m_columns) {
                    m_columns->printf(stderr);
                }
            } else {
                os << "cvle worker error: no result "
                   << " (try storage as output plugin)\n";
            }
        } else {
            for (auto it = result->begin(), et = result->end(); it != et;
                 ++it) {
                if (it->second && it->second->isMatrix()) {
                    os << "view:" << it->first << "\n"
                       << vle::value::toMatrixValue(it->second);
                }
            }
        }
    }

public:
    Worker(const std::string& package,
           std::chrono::milliseconds timeout,
           const std::string& vpz,
           bool withoutspawn,
           bool warnings)
      : m_context(vle::utils::make_context())
      , m_timeout(timeout)
      , m_packagename(package)
      , m_simulator(nullptr)
      , m_warnings(warnings)
    {
        if (not withoutspawn) {
            m_simulator.reset(new vle::manager::Simulation(
              m_context,
              vle::manager::LOG_NONE,
              vle::manager::SIMULATION_NONE |
                vle::manager::SIMULATION_SPAWN_PROCESS,
              timeout,
              nullptr));
        } else {
            m_simulator.reset(
              new vle::manager::Simulation(m_context,
                                           vle::manager::LOG_NONE,
                                           vle::manager::SIMULATION_NONE,
                                           timeout,
                                           nullptr));
        }
        m_context->set_log_priority(3);
        vle::utils::Package pack(m_context);
        pack.select(m_packagename);
        m_vpz = std::make_unique<vle::vpz::Vpz>(
          pack.getExpFile(vpz, vle::utils::PKG_BINARY));
    }

    void init(const std::string& header)
    {
        if (header == "_cvle_complex_values") {
            m_conditions.reset(new ConditionsBackup(*m_vpz));
        } else {
            namespace ba = boost::algorithm;

            m_columns.reset(new Columns());
            std::vector<std::string> tokens;
            ba::split(tokens, header, ba::is_any_of(","));

            for (std::size_t i = 0, e = tokens.size(); i != e; ++i) {
                Access access(cleanup_token(tokens[i]));

                if (access.is_undefined_string()) {
                    m_columns->add(access.condition);
                } else {
                    m_columns->add(access.value(m_vpz));
                }
            }
        }
    }

    std::string run(const std::string& block)
    {
        std::ostringstream result;
        std::vector<std::string> output;
        std::string::size_type begin = 0u;
        std::string::size_type end;
        result.imbue(std::locale::classic());
        result << std::setprecision(std::floor(
                    std::numeric_limits<double>::digits * std::log10(2) + 2))
               << std::scientific;

        for (begin = 0, end = block.find('\n'); begin < block.size();
             begin = end + 1, end = block.find('\n', end + 1)) {
            std::string buffer(block, begin, end - begin);
            if (m_columns) { // use columns
                boost::algorithm::split(
                  output, buffer, boost::algorithm::is_any_of(","));

                for (std::size_t i = 0, e = output.size(); i != e; ++i) {
                    std::string current = cleanup_token(output[i]);
                    m_columns->update(i, current);
                }

                result << *m_columns << "\n";
                simulate(result);
                result << '\n';
            } else { // use vpz
                vle::vpz::Vpz temp;
                vle::vpz::SaxParser parser(temp);
                parser.parseMemory(buffer);
                vle::vpz::Conditions conds =
                  temp.project().experiment().conditions();
                std::vector<std::string> condNames = conds.conditionnames();
                m_conditions->modify(*m_vpz, conds);
                result << m_conditions->getId(conds) << "\n";
                simulate(result);
                m_conditions->restoreBackup(*m_vpz, conds);
                result << '\n';
            }
        }

        return result.str();
    }
};

template <typename T>
struct no_deleter
{
    void operator()(T*)
    {
    }
};

template <typename T>
std::shared_ptr<T>
open(const std::string& file)
{
    auto fs = std::make_shared<T>(file.c_str());

    if (!fs->is_open())
        throw vle::utils::FileError(_("Fail to open file %s"), file.c_str());

    return fs;
}

class Root
{
    std::shared_ptr<std::istream> m_is;
    std::shared_ptr<std::ostream> m_os;
    std::ofstream m_ofs;
    int m_blocksize;

public:
    Root(const std::string& input, const std::string& output, int blocksize)
      : m_is(&std::cin, no_deleter<std::istream>())
      , m_os(&std::cout, no_deleter<std::ostream>())
      , m_blocksize(blocksize)
    {
        if (!input.empty())
            m_is = open<std::ifstream>(input);

        if (!output.empty())
            m_os = open<std::ofstream>(output);
    }

    bool header(std::string& header)
    {
        std::getline(*m_is.get(), header);
        return m_is.get()->good();
    }

    bool read(std::string& block)
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

    void write(const std::string& block)
    {
        (*m_os.get()) << block;
        m_os->flush();
    }
};

enum CommunicationTag
{
    worker_block_todo_tag,
    worker_block_end_tag,
    worker_end_tag
};

int
run_as_master(const std::string& inputfile,
              const std::string& outputfile,
              int blocksize)
{
    int ret = EXIT_SUCCESS;
    int blockid = 0;

    try {
        Root r(inputfile, outputfile, blocksize);
        boost::mpi::communicator comm;
        std::vector<bool> workers(comm.size(), false);
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
                printf(
                  _("master sends block %d to %d\n"), blockid++, msg.source());
                comm.send(msg.source(), worker_block_todo_tag, block);
                workers[msg.source()] = true;
            }
        }

        while (std::find(workers.begin(), workers.end(), true) !=
               workers.end()) {
            boost::mpi::status msg = comm.probe();
            comm.recv(msg.source(), worker_block_end_tag, block);
            workers[msg.source()] = false;
            r.write(block);
        }

        for (int child = 1; child < comm.size(); ++child)
            comm.send(child, worker_end_tag);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "master fails: %s\n", e.what());
        ret = EXIT_SUCCESS;
    }

    return ret;
}

int
run_as_worker(const std::string& package,
              const std::string& vpz,
              std::chrono::milliseconds timeout,
              bool withoutspawn,
              bool warnings)
{
    int ret = EXIT_SUCCESS;

    try {
        boost::mpi::communicator comm;
        Worker w(package, timeout, vpz, withoutspawn, warnings);
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
    } catch (const std::exception& e) {
        std::fprintf(stderr, "worker fails: %s\n", e.what());
        ret = EXIT_FAILURE;
    }

    return ret;
}

void
show_help()
{
    printf(_("cvle [options...]\n\n"
             "  help,h                          Produce help message\n"
             "  timeout [int]                   Limit the simulation duration "
             "with a timeout in miliseconds.\n"
             "  package,P package file [file..] Set package name and files\n"
             "  input-file,i file               csv input file\n"
             "  output-file,o file              csv output file\n"
             "  withoutspawn                    Perform simulation into "
             "the worker process\n"
             "  warnings                        Show warnings in output\n"
             "  template,t file                 Generate a template csv input "
             "file\n"
             "  block-size,b size               Set number of lines to be sent"
             " [default 5000]\n"));
}

int
main(int argc, char* argv[])
{
    std::string package_name;
    std::string input_file, output_file, template_file;
    std::chrono::milliseconds timeout{ std::chrono::milliseconds::zero() };
    int withoutspawn = 0;
    int warnings = 0;
    int block_size = 5000;
    int ret = EXIT_SUCCESS;

    const char* const short_opts = "hP:i:o:t:b:";
    const struct option long_opts[] = {
        { "help", 0, nullptr, 'h' },
        { "timeout", 1, nullptr, 0 },
        { "package", 1, nullptr, 'P' },
        { "input-file", 1, nullptr, 'i' },
        { "output-file", 1, nullptr, 'o' },
        { "template", 1, nullptr, 't' },
        { "withoutspawn", 0, &withoutspawn, 1 },
        { "warnings", 0, &warnings, 1 },
        { "block-size", 1, nullptr, 'b' },
        { 0, 0, nullptr, 0 }
    };
    int opt_index;

    for (;;) {
        const auto opt =
          getopt_long(argc, argv, short_opts, long_opts, &opt_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0:
            if (not strcmp(long_opts[opt_index].name, "timeout")) {
                try {
                    long int t = std::stol(::optarg);
                    if (t <= 0)
                        throw std::exception();
                    timeout = std::chrono::milliseconds(t);
                } catch (const std::exception& /*e*/) {
                    fprintf(stderr,
                            _("Bad timeout: %s Assume no timeout\n"),
                            ::optarg);
                }
            }
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
            } catch (const std::exception& /* e */) {
                fprintf(stderr,
                        _("Bad block size: %s. "
                          "Assume block size=%d\n"),
                        ::optarg,
                        block_size);
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
        printf(
          _("Usage: cvle --package test tutu.vpz -i in.csv -o out.csv\n"));
        return ret;
    }

    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator comm;

    std::vector<std::string> vpz(argv + ::optind, argv + argc);
    if (vpz.size() > 1)
        fprintf(
          stderr, _("Use only the first vpz: %s\n"), vpz.front().c_str());

    if (comm.rank() == 0 and not template_file.empty())
        generate_template(template_file, package_name, vpz.front());

    if (comm.size() == 1) {
        fprintf(stderr, _("cvle needs two processors.\n"));
        return EXIT_FAILURE;
    }

    if (comm.rank() == 0) {
        printf(_("block size: %d\n"
                 "package   : %s\n"
                 "timeout   : %ld\n"
                 "input csv : %s\n"
                 "output csv: %s\n"
                 "vpz       :"),
               block_size,
               package_name.c_str(),
               timeout.count(),
               (input_file.empty()) ? "stdin" : input_file.c_str(),
               (output_file.empty()) ? "stdout" : output_file.c_str());

        for (const auto& elem : vpz)
            printf("%s ", elem.c_str());
        printf("\n");
        return run_as_master(input_file, output_file, block_size);
    }
    return run_as_worker(
      package_name, vpz.front(), timeout, withoutspawn, warnings);
}
