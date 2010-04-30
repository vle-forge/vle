/**
 * @file vle/extension/decision/Parser.hpp
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


#ifndef VLE_EXTENSION_DECISION_PARSER_HPP
#define VLE_EXTENSION_DECISION_PARSER_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/Algo.hpp>
#include <limits>
#include <string>
#include <vector>
#include <istream>
#include <iterator>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT Parser
{
public:
    /**
     * @brief Build a boost::Spirit parser, fill the structure Plan from the
     * string an try to assign the Plan to the KnowledgeBase.
     *
     * @param content The content to transform into KnowledgeBase.
     *
     * @throw utils::ArgError if an error appear.
     */
    Parser(KnowledgeBase& kb, const std::string& content);

    /**
     * @brief Build a boost::Spirit parser, fill the structure Plan from the
     * stream an try to assign the Plan to the KnowledgeBase.
     *
     * @param stream An input stream to transform into KnowledgeBase.
     *
     * @throw utils::ArgError if an error appear.
     */
    Parser(KnowledgeBase& kb, std::istream& stream);

    struct Rule
    {
        std::string id;
        std::vector < std::string > predicates;
    };

    struct Rules
    {
        std::vector < Rule > rules;
    };

    struct Temporal
    {
        Temporal()
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

    struct Activity
    {
        std::string id;
        std::vector < std::string > rules;
        Temporal temporal;
        std::string ack;
        std::string output;
    };

    struct Activities
    {
        std::vector < Activity > activities;
    };

    struct Precedence
    {
        Precedence()
            : type(0), mintimelag(0), maxtimelag(devs::Time::infinity)
        {}

        int type;
        std::string first;
        std::string second;
        double mintimelag;
        double maxtimelag;
    };

    struct Precedences
    {
        std::vector < Precedence > precedences;
    };

    struct Plan
    {
        Rules rules;
        Activities activities;
        Precedences precedences;
    };

private:
    KnowledgeBase&  mKb;

    typedef std::vector < std::string > Strings;

    /**
     * @brief Fill the decision::KnowledgeBase from the plan reads from stream
     * or string.
     *
     * @param plan The plan source.
     *
     * @throw utils::ArgError if an error appear.
     */
    void fillKnowledegeBase(const Plan& plan);

    struct AddRule
    {
        KnowledgeBase& kb;

        AddRule(KnowledgeBase& kb) : kb(kb) {}

        void operator()(const Parser::Rule& rule)
        {
            decision::Rule& r = kb.addRule(rule.id);

            for (Strings::const_iterator it = rule.predicates.begin(); it !=
                 rule.predicates.end(); ++it) {
                r.add(kb.predicates()[*it]);
            }
        }
    };

    struct AddActivity
    {
        KnowledgeBase& kb;

        AddActivity(KnowledgeBase& kb) : kb(kb) {}

        void operator()(const Parser::Activity& activity)
        {
            decision::Activity& act = kb.addActivity(activity.id);

            if (not activity.ack.empty()) {
                act.addAcknowledgeFunction(
                    kb.acknowledgeFunctions()[activity.ack]);
            }

            for (Strings::const_iterator it = activity.rules.begin();
                 it != activity.rules.end(); ++it) {
                act.addRule(*it, kb.rules().get(*it));
            }

            if (not activity.output.empty()) {
                act.addOutputFunction(
                    kb.outputFunctions()[activity.output]);
            }

            switch (activity.temporal.type) {
            case 1:
            case 2:
                act.initStartTimeFinishTime(activity.temporal.start,
                                            activity.temporal.finish);
                break;
            case 3:
                act.initStartTimeFinishRange(activity.temporal.start,
                                             activity.temporal.minfinish,
                                             activity.temporal.maxfinish);
                break;
            case 4:
            case 5:
                act.initStartRangeFinishTime(activity.temporal.minstart,
                                             activity.temporal.maxstart,
                                             activity.temporal.finish);
                break;
            case 6:
                act.initStartRangeFinishRange(activity.temporal.minstart,
                                              activity.temporal.maxstart,
                                              activity.temporal.minfinish,
                                              activity.temporal.maxfinish);
                break;
            default:
                throw;
            }
        }
    };

    struct AddPrecedence
    {
        KnowledgeBase kb;

        AddPrecedence(KnowledgeBase& kb) : kb(kb) {}

        void operator()(const Parser::Precedence& pre)
        {
            switch (pre.type) {
            case 1:
                kb.addFinishToStartConstraint(pre.first, pre.second,
                                              pre.mintimelag, pre.maxtimelag);
                break;
            case 2:
                kb.addStartToStartConstraint(pre.first, pre.second,
                                             pre.mintimelag, pre.maxtimelag);
                break;
            case 3:
                kb.addFinishToFinishConstraint(pre.first, pre.second,
                                               pre.mintimelag, pre.maxtimelag);
                break;
            default:
                throw utils::ArgError(fmt(_(
                            "Decision: unknow precedence type `%1%'")) %
                    pre.type);
            }
        }
    };
};

std::ostream& operator<<(std::ostream& out, const Parser::Rule& rule)
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

std::ostream& operator<<(std::ostream& out, const Parser::Rules& rules)
{
    out << "rules {\n";

    std::copy(rules.rules.begin(), rules.rules.end(),
              std::ostream_iterator < Parser::Rule >(out));

    out << "}\n";

    return out;
}

std::ostream& operator<<(std::ostream& out, const Parser::Temporal& tmp)
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

std::ostream& operator<<(std::ostream& out, const Parser::Activity& act)
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

std::ostream& operator<<(std::ostream& out, const Parser::Activities& acts)
{
    out << "activities {\n";

    std::copy(acts.activities.begin(),
              acts.activities.end(),
              std::ostream_iterator < Parser::Activity >(out));

    return out << "}\n";
}

std::ostream& operator<<(std::ostream& out, const Parser::Precedence& pred)
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

std::ostream& operator<<(std::ostream& out, const Parser::Precedences& preds)
{
    out << "precedences {\n";

    std::copy(preds.precedences.begin(), preds.precedences.end(),
              std::ostream_iterator < Parser::Precedence >(out));

    out << "}\n";

    return out;
}

std::ostream& operator<<(std::ostream& out, const Parser::Plan& plan)
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
