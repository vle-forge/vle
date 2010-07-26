/**
 * @file vle/extension/decision/Plan.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_EXTENSION_DECISION_PLAN_HPP
#define VLE_EXTENSION_DECISION_PLAN_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/devs/Time.hpp>
#include <string>
#include <map>
#include <limits>
#include <string>
#include <vector>
#include <istream>
#include <iterator>

namespace vle { namespace extension { namespace decision {

struct PlanToken;

typedef PlanToken Plan;
typedef std::vector < std::string > Strings;

struct VLE_EXTENSION_EXPORT RuleToken
{
    std::string id;
    Strings predicates;
};

struct VLE_EXTENSION_EXPORT RulesToken
{
    std::vector < RuleToken > rules;
};

struct VLE_EXTENSION_EXPORT TemporalToken
{
    TemporalToken()
        : start(devs::Time::negativeInfinity),
        finish(devs::Time::infinity),
        minstart(devs::Time::negativeInfinity),
        maxstart(devs::Time::infinity),
        minfinish(devs::Time::negativeInfinity),
        maxfinish(devs::Time::infinity),
        type(0)
    {}

    double start;
    double finish;
    double minstart;
    double maxstart;
    double minfinish;
    double maxfinish;
    int type;
};

struct VLE_EXTENSION_EXPORT ActivityToken
{
    std::string id;
    Strings rules;
    TemporalToken temporal;
    std::string ack;
    std::string output;
};

struct VLE_EXTENSION_EXPORT ActivitiesToken
{
    std::vector < ActivityToken > activities;
};

struct VLE_EXTENSION_EXPORT PrecedenceToken
{
    PrecedenceToken()
        : type(0), mintimelag(0), maxtimelag(devs::Time::infinity)
    {}

    int type;
    std::string first;
    std::string second;
    double mintimelag;
    double maxtimelag;
};

struct VLE_EXTENSION_EXPORT PrecedencesToken
{
    std::vector < PrecedenceToken > precedences;
};

struct VLE_EXTENSION_EXPORT PlanToken
{
    RulesToken rules;
    ActivitiesToken activities;
    PrecedencesToken precedences;
};

inline std::ostream& operator<<(std::ostream& out, const RuleToken& rule)
{
    out << "    rule {\n"
        << "        id = \"" << rule.id << "\";\n";

    if (not rule.predicates.empty()) {
        out << "        predicates = ";

        utils::formatCopy(rule.predicates.begin(), rule.predicates.end(),
                          out, " ", ";", "\"");

        out << "\n";
    }

    out << "    }\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const RulesToken& rules)
{
    out << "rules {\n";

    std::copy(rules.rules.begin(), rules.rules.end(),
              std::ostream_iterator < RuleToken >(out));

    out << "}\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const TemporalToken& tmp)
{
    out << "        temporal {\n";

    if (tmp.start > devs::Time::negativeInfinity) {
        out << "            start = " << tmp.start << ";\n";
        if (tmp.finish < devs::Time::infinity) {
            out << "            finish = " << tmp.finish << ";\n";
        } else {
            out << "            minfinish = " << tmp.minfinish << ";\n"
                << "            maxfinish = " << tmp.maxfinish << ";\n";
        }
    } else {
        out << "            minstart = " << tmp.minstart << ";\n"
            << "            maxstart = " << tmp.maxstart << ";\n";
        if (tmp.finish < devs::Time::infinity) {
            out << "            finish = " << tmp.finish << ";\n";
        } else {
            out << "            minfinish = " << tmp.minfinish << ";\n"
                << "            maxfinish = " << tmp.maxfinish << ";\n";
        }
    }

    out << "        }\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const ActivityToken& act)
{
    out << "    activity {\n"
        << "        id = \"" << act.id << "\";\n"
        << "        rules = ";

    utils::formatCopy(act.rules.begin(), act.rules.end(),
                      out, " ", ";", "\"");

    out << "\n"
        << act.temporal
        << "        ack = \"" << act.ack << "\";\n"
        << "        output = \"" << act.output << "\";\n"
        << "    }\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const ActivitiesToken& acts)
{
    out << "activities {\n";

    std::copy(acts.activities.begin(),
              acts.activities.end(),
              std::ostream_iterator < ActivityToken >(out));

    return out << "}\n";
}

inline std::ostream& operator<<(std::ostream& out, const PrecedenceToken& pred)
{
    out << "    precedence {\n"
        << "        type = " << pred.type << ";\n"
        << "        first = \"" << pred.first << "\";\n"
        << "        second = \"" << pred.second << "\";\n";

    if (pred.mintimelag != devs::Time::negativeInfinity) {
        out << "        mintimelag = " << pred.mintimelag << ";\n";
    }

    if (pred.maxtimelag != devs::Time::infinity) {
        out << "        maxtimelag = " << pred.maxtimelag << ";\n";
    }

    out << "    }\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out,
                                const PrecedencesToken& preds)
{
    out << "precedences {\n";

    std::copy(preds.precedences.begin(), preds.precedences.end(),
              std::ostream_iterator < PrecedenceToken >(out));

    out << "}\n";

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const PlanToken& plan)
{
    out << plan.rules
        << "\n"
        << plan.activities
        << "\n"
        << plan.precedences;

    return out;
}

}}} // namespace vle model decision

#endif
