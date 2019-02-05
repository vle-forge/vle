/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2014-2018 INRA http://www.inra.fr
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

#include <memory>
#include <utility>
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

#include <boost/algorithm/string.hpp>

#define OMPI_SKIP_MPICXX
#include <mpi.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <stack>

#include <cassert>
#include <cerrno>
#include <cmath>
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

enum CommunicationTag
{
    worker_block_header_tag,
    worker_block_todo_tag,
    worker_block_end_tag,
    worker_end_tag
};

enum ErrorCode
{
    internal_failure_errorcode = -4,
    send_errorcode,
    recv_errorcode,
    message_order_errorcode,
};

static inline CommunicationTag
from_int(int tag)
{
    return static_cast<CommunicationTag>(tag);
}

static void
mpi_send_string(int target, CommunicationTag tag, const std::string& msg)
{
    if (MPI_Send(msg.data(),
                 static_cast<int>(msg.size()),
                 MPI_CHAR,
                 target,
                 tag,
                 MPI_COMM_WORLD) != MPI_SUCCESS) {
        fprintf(stderr,
                "Sending header (size: %d) to %d failed\n",
                static_cast<int>(msg.size()),
                target);
        MPI_Abort(MPI_COMM_WORLD, send_errorcode);
    }
}

static std::tuple<std::string, int, CommunicationTag>
mpi_recv_string()
{
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    int size;
    MPI_Get_count(&status, MPI_CHAR, &size);

    std::string ret(static_cast<std::string::size_type>(size), ' ');

    if (MPI_Recv(&ret[0],
                 static_cast<int>(ret.size()),
                 MPI_CHAR,
                 status.MPI_SOURCE,
                 status.MPI_TAG,
                 MPI_COMM_WORLD,
                 &status) != MPI_SUCCESS) {
        fprintf(stderr,
                "Receiving header (size: %d) from %d failed\n",
                size,
                status.MPI_SOURCE);
        MPI_Abort(MPI_COMM_WORLD, recv_errorcode);
    }

    return std::make_tuple(ret, status.MPI_SOURCE, from_int(status.MPI_TAG));
}

static void
mpi_send_block(int target,
               CommunicationTag tag,
               const std::string& block,
               int first,
               int last)
{
    std::array<int, 2> ids{ { first, last } };

    if (MPI_Send(block.data(),
                 static_cast<int>(block.size()),
                 MPI_CHAR,
                 target,
                 tag,
                 MPI_COMM_WORLD) != MPI_SUCCESS) {
        fprintf(stderr,
                "Sending block buffer (size: %d) to %d failed\n",
                static_cast<int>(block.size()),
                target);
        MPI_Abort(MPI_COMM_WORLD, send_errorcode);
    }

    if (MPI_Send(ids.data(),
                 static_cast<int>(ids.size()),
                 MPI_INT,
                 target,
                 tag,
                 MPI_COMM_WORLD) != MPI_SUCCESS) {
        fprintf(stderr,
                "Sending block rows identifier %d to %d to rank %d failed\n",
                first,
                last,
                target);
        MPI_Abort(MPI_COMM_WORLD, send_errorcode);
    }
}

static std::tuple<std::string, int, int, CommunicationTag>
mpi_worker()
{
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    int size;
    MPI_Get_count(&status, MPI_CHAR, &size);
    std::string ret(static_cast<std::string::size_type>(size), ' ');

    if (MPI_Recv(&ret[0],
                 static_cast<int>(ret.size()),
                 MPI_CHAR,
                 status.MPI_SOURCE,
                 status.MPI_TAG,
                 MPI_COMM_WORLD,
                 &status) != MPI_SUCCESS)
        fprintf(stderr,
                "Receiving block buffer (size: %d) from %d failed\n",
                size,
                status.MPI_SOURCE);

    if (status.MPI_TAG == worker_end_tag)
        return std::make_tuple(ret, -1, -1, worker_end_tag);

    std::array<int, 2> ids;
    if (MPI_Recv(ids.data(),
                 static_cast<int>(ids.size()),
                 MPI_INT,
                 status.MPI_SOURCE,
                 status.MPI_TAG,
                 MPI_COMM_WORLD,
                 &status) != MPI_SUCCESS)
        fprintf(stderr,
                "Receiving block rows identifier from %d failed\n",
                status.MPI_SOURCE);

    return std::make_tuple(ret, ids[0], ids[1], from_int(status.MPI_TAG));
}

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
     * \c value function tries to convert the \c Access object into a \c
     * vle::value::Value pointer by browsing the experimental condition of the
     * \e vpz arguement.
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
                long int val = strtol(params[i].c_str(), nullptr, 10);

                if (errno or val < 0 or
                    val >= static_cast<long int>(current->toSet().size()))
                    throw vle::utils::ArgError(
                      _("Fails to convert '%s.%s' parameter '%zu'"
                        " as correct set index"),
                      condition.c_str(),
                      port.c_str(),
                      i);

                current =
                  current->toSet()[static_cast<std::size_t>(val)].get();
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

class Columns
{
public:
    struct KeepColumnDefinition
    {
        KeepColumnDefinition(std::string str_)
          : str(std::move(str_))
        {}

        std::string str;
    };

    /*
     * @c ValueColumnDefinition stores the clone of the value into a unique_ptr
     * and keep a reference to the pointer since this pointer come from a
     * shared_ptr (direct access to the condition port) or a unique_ptr (a
     * child in the value's tree).
     */
    struct ValueColumnDefinition
    {
        ValueColumnDefinition(vle::value::Value* value_)
          : default_value(value_->clone())
          , value(value_)
        {
            assert(value && "InternalError: nullptr value column");
        }

        std::unique_ptr<vle::value::Value> default_value;
        vle::value::Value* value;
    };

private:
    enum class column_type
    {
        keep_column = 0,
        value_column
    };

    bool m_more_output_details;
    std::vector<KeepColumnDefinition> keep_vector;
    std::vector<ValueColumnDefinition> value_vector;
    std::vector<std::pair<column_type, int>> indices;

    void check(std::size_t size) const
    {
        if (size > std::numeric_limits<int>::max()) {
            fprintf(
              stderr,
              "Column definition: cvle can not store more than %d columns.",
              std::numeric_limits<int>::max());

            throw vle::utils::ArgError(_("Too many column"));
        }
    }

public:
    Columns(bool more_output_details)
      : m_more_output_details(more_output_details)
      , keep_vector()
      , value_vector()
      , indices()
    {}

    bool more_output_details() const
    {
        return m_more_output_details;
    }

    void add(std::string str)
    {
        indices.emplace_back(column_type::keep_column,
                             static_cast<int>(keep_vector.size()));

        keep_vector.emplace_back(str);

        check(keep_vector.size());
    }

    void add(vle::value::Value* value)
    {
        indices.emplace_back(column_type::value_column,
                             static_cast<int>(value_vector.size()));

        value_vector.emplace_back(value);

        check(value_vector.size());
    }

    std::size_t size() const
    {
        return indices.size();
    }

    void update(std::size_t i, const std::string& str)
    {
        assert(i < indices.size() && "Too many column");

        int id = indices[i].second;

        if (indices[i].first == column_type::keep_column) {
            keep_vector[id].str = str;
        } else {
            // If @c str is empty, user want to use the default @c
            // vle::value::Value from the origin VPZ file. Otherwise, we
            // convert the value from the string.

            switch (value_vector[id].default_value->getType()) {
            case vle::value::Value::BOOLEAN:
                value_vector[id].value->toBoolean().value() =
                  str.empty()
                    ? value_vector[id].default_value->toBoolean().value()
                    : vle::utils::to<bool>(str);
                break;

            case vle::value::Value::INTEGER:
                value_vector[id].value->toInteger().value() =
                  str.empty()
                    ? value_vector[id].default_value->toInteger().value()
                    : vle::utils::to<std::int32_t>(str);
                break;

            case vle::value::Value::DOUBLE:
                value_vector[id].value->toDouble().value() =
                  str.empty()
                    ? value_vector[id].default_value->toDouble().value()
                    : vle::utils::to<double>(str);
                break;

            case vle::value::Value::STRING:
                value_vector[id].value->toString().value() =
                  str.empty()
                    ? value_vector[id].default_value->toString().value()
                    : str;
                break;

            default:
                break;
            }
        }
    }

    void printf(FILE* f) const
    {
        for (const auto& elem : indices)
            if (elem.first == column_type::keep_column)
                fprintf(f, "%s ", keep_vector[elem.second].str.c_str());
    }

    friend std::ostream& operator<<(std::ostream& os, const Columns& columns)
    {
        for (const auto& elem : columns.indices)
            if (elem.first == column_type::keep_column) {
                if (columns.more_output_details()) {
                    os << columns.keep_vector[elem.second].str.c_str() << ' ';
                } else {
                    os << columns.keep_vector[elem.second].str.c_str() << ',';
                }
            }

        return os;
    }
};

struct ConditionsBackup
{
    ConditionsBackup(const vle::vpz::Vpz& vpz)
      : mconds(vpz.project().experiment().conditions())
    {}

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

                    auto toup_port =
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
        auto it_id = cond_cvle.conditionvalues().find("id");
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
    bool m_more_output_details;

    void simulate(std::ostream& os, int row_id)
    {
        vle::manager::Error error;

        auto vpz = std::make_unique<vle::vpz::Vpz>(*m_vpz.get());
        vpz->project().setInstance(row_id);

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
        } else if (result == nullptr) {
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
            for (auto& it : *result) {
                if (it.second && it.second->isMatrix()) {
                    if (m_more_output_details) {
                        os << "view:" << it.first << "\n"
                           << vle::value::toMatrixValue(it.second);
                    } else {
                        it.second->writeFile(os);
                    }
                }
            }
        }
    }

public:
    Worker(std::string package,
           std::chrono::milliseconds timeout,
           const std::string& vpz,
           bool withoutspawn,
           bool warnings,
           bool more_output_details)
      : m_context(vle::utils::make_context())
      , m_timeout(timeout)
      , m_packagename(std::move(package))
      , m_simulator(nullptr)
      , m_warnings(warnings)
      , m_more_output_details(more_output_details)
    {
        if (not withoutspawn) {
            m_simulator = std::make_unique<vle::manager::Simulation>(
              m_context,
              vle::manager::LOG_NONE,
              vle::manager::SIMULATION_NONE |
                vle::manager::SIMULATION_SPAWN_PROCESS,
              m_timeout,
              nullptr);
        } else {
            m_simulator = std::make_unique<vle::manager::Simulation>(
              m_context,
              vle::manager::LOG_NONE,
              vle::manager::SIMULATION_NONE,
              m_timeout,
              nullptr);
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
            m_conditions = std::make_unique<ConditionsBackup>(*m_vpz);
        } else {
            namespace ba = boost::algorithm;

            m_columns = std::make_unique<Columns>(m_more_output_details);
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

    std::string run(const std::string& block, int first, int last)
    {
        fprintf(stdout, "worker run row %d to %d\n", first, last);

        std::ostringstream result;
        std::vector<std::string> output;
        std::string::size_type begin = 0u;
        std::string::size_type end;
        result.imbue(std::locale::classic());
        result << std::setprecision(static_cast<int>(std::floor(
                    std::numeric_limits<double>::digits * std::log10(2) + 2)))
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

                result << *m_columns;
                simulate(result, first++);
            } else { // use vpz
                vle::vpz::Vpz temp;
                temp.parseMemory(buffer);
                vle::vpz::Conditions conds =
                  temp.project().experiment().conditions();
                std::vector<std::string> condNames = conds.conditionnames();
                m_conditions->modify(*m_vpz, conds);
                result << m_conditions->getId(conds) << "\n";
                simulate(result, first++);
                m_conditions->restoreBackup(*m_vpz, conds);
                result << '\n';
            }
        }

        fprintf(stdout, "worker finishes at %d (%d)\n", first, last);

        return result.str();
    }
};

template<typename T>
struct no_deleter
{
    void operator()(T*)
    {}
};

template<typename T>
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
    int m_first_id;
    int m_last_id;
    int m_blocksize;

public:
    Root(const std::string& input, const std::string& output, int blocksize)
      : m_is(&std::cin, no_deleter<std::istream>())
      , m_os(&std::cout, no_deleter<std::ostream>())
      , m_first_id(0)
      , m_last_id(0)
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

    bool read(std::string& block, int& first, int& last)
    {
        m_first_id = m_last_id;
        int i = 0;
        block.clear();

        if (m_is->fail())
            return false;

        while (m_is->good() && i < m_blocksize) {
            std::string tmp;
            std::getline(*m_is.get(), tmp);

            if (not tmp.empty()) {
                ++m_last_id;
                block += tmp;
                block += '\n';
                ++i;
            } else {
                first = m_first_id;
                last = m_last_id;

                return not block.empty();
            }
        }
        first = m_first_id;
        last = m_last_id;

        return true;
    }

    void write(const std::string& block)
    {
        (*m_os.get()) << block;
        m_os->flush();
    }
};

int
run_as_master(const std::string& inputfile,
              const std::string& outputfile,
              int blocksize)
{
    int ret = EXIT_SUCCESS;
    int blockid = 0;
    int first, last;

    try {
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        Root r(inputfile, outputfile, blocksize);
        std::vector<bool> workers(world_size, false);
        std::string block, header;
        r.header(header);

        for (int rank = 1; rank != world_size; ++rank)
            mpi_send_string(rank, worker_block_header_tag, header);

        for (int rank = 1; rank < world_size; ++rank) {
            if (r.read(block, first, last)) {
                mpi_send_block(
                  rank, worker_block_todo_tag, block, first, last);
                workers[rank] = true;
            } else
                break;
        }

        bool end = false; // Stop when all buffer are sent.
        int from;
        CommunicationTag tag;
        while (end == false) {
            std::tie(block, from, tag) = mpi_recv_string();

            assert(tag == worker_block_end_tag);

            workers[from] = false;
            r.write(block);
            end = !r.read(block, first, last);

            if (not block.empty()) {
                printf(_("master sends block %d to %d\n"), blockid++, from);
                mpi_send_block(
                  from, worker_block_todo_tag, block, first, last);
                workers[from] = true;
            }
        }

        while (std::find(workers.begin(), workers.end(), true) !=
               workers.end()) {
            std::tie(block, from, tag) = mpi_recv_string();
            assert(tag == worker_block_end_tag);
            workers[from] = false;
            r.write(block);
        }

        for (int rank = 1; rank < world_size; ++rank)
            mpi_send_string(rank, worker_end_tag, "ok");
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
              bool warnings,
              bool more_output_details)
{
    try {
        Worker w(
          package, timeout, vpz, withoutspawn, warnings, more_output_details);
        std::string block;
        int from;
        int first, last;
        CommunicationTag tag;

        //
        // Worker requires the header from the master mpi process.
        //

        std::tie(block, from, tag) = mpi_recv_string();

        if (from != 0 or tag != worker_block_header_tag) {
            fprintf(stderr, "Worker fails to receive the header\n");
            MPI_Abort(MPI_COMM_WORLD, message_order_errorcode);
            return EXIT_FAILURE;
        }

        w.init(block);

        //
        // Worker receives block of data to simulate or a end message to stop
        // the current process.
        //

        bool end = false;
        while (not end) {
            std::tie(block, first, last, tag) = mpi_worker();

            switch (tag) {
            case worker_end_tag:
                end = true;
                break;

            case worker_block_todo_tag:
                block = w.run(block, first, last);
                mpi_send_string(0, worker_block_end_tag, block);
                break;

            default:
                fprintf(stderr, "Internal error in MPI message order\n");
                MPI_Abort(MPI_COMM_WORLD, message_order_errorcode);
            }
        }
    } catch (const std::exception& e) {
        fprintf(stderr, "worker fails: %s\n", e.what());
        MPI_Abort(MPI_COMM_WORLD, internal_failure_errorcode);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
    int more_output_details = 0;
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
        { "more-output-details", 0, &more_output_details, 1 },
        { nullptr, 0, nullptr, 0 }
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

    std::vector<std::string> vpz(argv + ::optind, argv + argc);
    if (vpz.size() > 1)
        fprintf(
          stderr, _("Use only the first vpz: %s\n"), vpz.front().c_str());

    MPI_Init(&argc, &argv);
    int rank = 0, world_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0 and not template_file.empty())
        generate_template(template_file, package_name, vpz.front());

    if (world_size == 1) {
        fprintf(stderr, _("cvle needs two processors.\n"));
        return EXIT_FAILURE;
    }

    int status;
    if (rank == 0) {
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

        status = run_as_master(input_file, output_file, block_size);
    } else {
        status = run_as_worker(package_name,
                               vpz.front(),
                               timeout,
                               withoutspawn,
                               warnings,
                               more_output_details);
    }

    MPI_Finalize();

    return status;
}
