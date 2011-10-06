/**
 * @file vle/extension/decision/Plan.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2011 INRA http://www.inra.fr
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


#include <vle/extension/decision/Plan.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/i18n.hpp>
#include <string>
#include <istream>

namespace vle { namespace extension { namespace decision {

typedef utils::Block UB;
typedef utils::Block::Blocks UBB;
typedef utils::Block::Strings UBS;
typedef utils::Block::Reals UBR;

Plan::Plan(KnowledgeBase& kb, const std::string& buffer)
    : mKb(kb)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root());
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

Plan::Plan(KnowledgeBase& kb, std::istream& stream)
    : mKb(kb)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root());
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root());
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root());
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const utils::Block& root)
{
    utils::Block::BlocksResult mainrules, mainactivities, mainprecedences;

    mainrules = root.blocks.equal_range("rules");
    mainactivities = root.blocks.equal_range("activities");
    mainprecedences = root.blocks.equal_range("precedences");

    utils::Block::Blocks::const_iterator it;

    for (it = mainrules.first; it != mainrules.second; ++it) {
        utils::Block::BlocksResult rules;
        rules = it->second.blocks.equal_range("rule");
        fillRules(rules);
    }

    for (it = mainactivities.first; it != mainactivities.second; ++it) {
        utils::Block::BlocksResult activities;
        activities = it->second.blocks.equal_range("activity");
        fillActivities(activities);
    }

    for (it = mainprecedences.first; it != mainprecedences.second; ++it) {
        utils::Block::BlocksResult precedences;
        precedences = it->second.blocks.equal_range("precedence");
        fillPrecedences(precedences);
    }
}

void Plan::fillRules(const utils::Block::BlocksResult& rules)
{
    for (UBB::const_iterator it = rules.first; it != rules.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second) {
            throw utils::ArgError(_("Decision: rule needs id"));
        }

        Rule& rule = mRules.add(id.first->second);
        UB::StringsResult preds = block.strings.equal_range("predicates");

        for (UB::Strings::const_iterator jt = preds.first;
             jt != preds.second; ++jt) {
            rule.add((mKb.predicates().get(jt->second))->second);
        }
    }
}

void Plan::fillActivities(const utils::Block::BlocksResult& acts)
{
    for (UBB::const_iterator it = acts.first; it != acts.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second) {
            throw utils::ArgError(_("Decision: activity needs id"));
        }

        Activity& act = mActivities.add(id.first->second, Activity());

        UB::StringsResult rules = block.strings.equal_range("rules");
        for (UBS::const_iterator jt = rules.first; jt != rules.second; ++jt) {
            act.addRule(jt->second, mRules.get(jt->second));
        }

        UB::StringsResult ack = block.strings.equal_range("ack");
        if (ack.first != ack.second) {
            act.addAcknowledgeFunction((mKb.acknowledgeFunctions().get(
                        ack.first->second))->second);
        }

        UB::StringsResult out = block.strings.equal_range("output");
        if (out .first != out.second) {
            act.addOutputFunction((mKb.outputFunctions().get(
                        out.first->second))->second);
        }

        UB::StringsResult upd = block.strings.equal_range("update");
        if (upd.first != upd.second) {
            act.addUpdateFunction((mKb.updateFunctions().get(
                        upd.first->second))->second);
        }

        UB::BlocksResult temporal = block.blocks.equal_range("temporal");
        if (temporal.first != temporal.second) {
            fillTemporal(temporal, act);
        }
    }
}

void Plan::fillTemporal(const utils::Block::BlocksResult& temps,
                        Activity& activity)
{
    for (UBB::const_iterator it = temps.first; it != temps.second; ++it) {
        const utils::Block& block = it->second;

        UB::RealsResult start = block.reals.equal_range("start");
        UB::RealsResult mins = block.reals.equal_range("minstart");
        UB::RealsResult maxs = block.reals.equal_range("maxstart");
        UB::RealsResult finish = block.reals.equal_range("finish");
        UB::RealsResult minf = block.reals.equal_range("minfinish");
        UB::RealsResult maxf = block.reals.equal_range("maxfinish");

        if (start.first != start.second) {
            if (finish.first != finish.second) {
                activity.initStartTimeFinishTime(
                    start.first->second, finish.first->second);
            } else {
                double vmin, vmax;
                if (minf.first != minf.second) {
                    if (maxf.first != maxf.second) {
                        vmin = minf.first->second;
                        vmax = maxf.first->second;
                    } else {
                        vmin = minf.first->second;
                        vmax = devs::Time::infinity;
                    }
                } else {
                    if (maxf.first != maxf.second) {
                        vmin = 0;
                        vmax = maxf.first->second;
                    } else {
                        vmin = 0;
                        vmax = devs::Time::infinity;
                    }
                }
                activity.initStartTimeFinishRange(start.first->second, vmin,
                                                  vmax);
            }
        } else {
            double vmin, vmax;
            if (mins.first != mins.second) {
                vmin = mins.first->second;
            } else {
                vmin = devs::Time::negativeInfinity;
            }

            if (maxs.first != maxs.second) {
                vmax = maxs.first->second;
            } else {
                vmax = devs::Time::infinity;
            }

            if (finish.first != finish.second) {
                activity.initStartRangeFinishTime(vmin, vmax,
                                                  finish.first->second);
            } else {
                double vminf, vmaxf;
                if (minf.first != minf.second) {
                    if (maxf.first != maxf.second) {
                        vminf = minf.first->second;
                        vmaxf = maxf.first->second;
                    } else {
                        vminf = minf.first->second;
                        vmaxf = devs::Time::infinity;
                    }
                } else {
                    if (maxf.first != maxf.second) {
                        vminf = 0;
                        vmaxf = maxf.first->second;
                    } else {
                        vminf = 0;
                        vmaxf = devs::Time::infinity;
                    }
                }
                activity.initStartRangeFinishRange(
                    vmin, vmax, vminf, vmaxf);
            }
        }
    }
}

void Plan::fillPrecedences(const utils::Block::BlocksResult& preds)
{
    for (UBB::const_iterator it = preds.first; it != preds.second; ++it) {
        const utils::Block& block = it->second;

        std::string valuefirst, valuesecond;
        double valuemintl = 0.0;
        double valuemaxtl = devs::Time::infinity;

        UB::StringsResult first = block.strings.equal_range("first");
        if (first.first != first.second) {
            valuefirst = first.first->second;
        }

        UB::StringsResult second = block.strings.equal_range("second");
        if (second.first != second.second) {
            valuesecond = second.first->second;
        }

        UB::RealsResult mintl = block.reals.equal_range("mintimelag");
        if (mintl.first != mintl.second) {
            valuemintl = mintl.first->second;
        }

        UB::RealsResult maxtl = block.reals.equal_range("maxtimelag");
        if (maxtl.first != maxtl.second) {
            valuemaxtl = maxtl.first->second;
        }

        UB::StringsResult type = block.strings.equal_range("type");
        if (type.first != type.second) {
            if (type.first->second == "SS") {
                mActivities.addStartToStartConstraint(valuefirst, valuesecond,
                                                      valuemintl, valuemaxtl);
            } else if (type.first->second == "FS") {
                mActivities.addFinishToStartConstraint(valuefirst, valuesecond,
                                                       valuemintl, valuemaxtl);
            } else if (type.first->second == "FF") {
                mActivities.addFinishToFinishConstraint(valuefirst,
                                                        valuesecond,
                                                        valuemintl, valuemaxtl);
            } else {
                throw utils::ArgError(fmt(
                        _("Decision: precendence type `%1%' unknown")) %
                    type.first->second);
            }
        } else {
            throw utils::ArgError(_("Decision: precedences type unknown"));
        }
    }
}

}}} // namespace vle extension decision
