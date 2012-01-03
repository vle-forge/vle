/*
 * @file vle/extension/decision/KnowledgeBase.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/i18n.hpp>
#include <algorithm>
#include <cassert>

namespace vle { namespace extension { namespace decision {

void KnowledgeBase::setActivityDone(const std::string& name,
                                    const devs::Time& date)
{
    Activities::iterator it(mPlan.activities().get(name));

    if (not it->second.isInStartedState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is not started")) % name);
    }

    mPlan.activities().setFFAct(it);
    it->second.ff(date);
    it->second.acknowledge(name);
}

void KnowledgeBase::setActivityFailed(const std::string& name,
                                      const devs::Time& date)
{
    Activities::iterator it(mPlan.activities().get(name));

    if (it->second.isInDoneState()) {
        throw utils::ArgError(fmt(
                _("Decision: activity '%1%' is already finish")) % name);
    }

    if (not it->second.isInFailedState()) {
        mPlan.activities().setFailedAct(it);
        it->second.fail(date);
        it->second.acknowledge(name);
    }
}

devs::Time KnowledgeBase::duration(const devs::Time& time)
{
    devs::Time next = nextDate(time);

    if (next == devs::Time::negativeInfinity) {
        return 0.0;
    } else {
        assert(next >= time);
        return next - time;
    }
}

KnowledgeBase::Result KnowledgeBase::processChanges(const devs::Time& time)
{
    Activities::Result r = mPlan.activities().process(time);
    return std::make_pair(r.first, r.second);
}

//struct AddRuleToken
//{
    //KnowledgeBase& kb;

    //AddRuleToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const RuleToken& rule)
    //{
        //Rule& r = kb.addRule(rule.id);

        //for (Strings::const_iterator it = rule.predicates.begin(); it !=
             //rule.predicates.end(); ++it) {
            //r.add(kb.predicates()[*it]);
        //}
    //}
//};

//struct AddActivityToken
//{
    //KnowledgeBase& kb;

    //AddActivityToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const ActivityToken& activity)
    //{
        //decision::Activity& act = kb.addActivity(activity.id);

        //if (not activity.ack.empty()) {
            //act.addAcknowledgeFunction(
                //kb.acknowledgeFunctions()[activity.ack]);
        //}

        //for (Strings::const_iterator it = activity.rules.begin();
             //it != activity.rules.end(); ++it) {
            //act.addRule(*it, kb.rules().get(*it));
        //}

        //if (not activity.output.empty()) {
            //act.addOutputFunction(
                //kb.outputFunctions()[activity.output]);
        //}

        //switch (activity.temporal.type) {
        //case 1:
        //case 2:
            //act.initStartTimeFinishTime(activity.temporal.start,
                                        //activity.temporal.finish);
            //break;
        //case 3:
            //act.initStartTimeFinishRange(activity.temporal.start,
                                         //activity.temporal.minfinish,
                                         //activity.temporal.maxfinish);
            //break;
        //case 4:
        //case 5:
            //act.initStartRangeFinishTime(activity.temporal.minstart,
                                         //activity.temporal.maxstart,
                                         //activity.temporal.finish);
            //break;
        //case 6:
            //act.initStartRangeFinishRange(activity.temporal.minstart,
                                          //activity.temporal.maxstart,
                                          //activity.temporal.minfinish,
                                          //activity.temporal.maxfinish);
            //break;
        //default:
            //throw;
        //}
    //}
//};

//struct AddPrecedenceToken
//{
    //KnowledgeBase& kb;

    //AddPrecedenceToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const PrecedenceToken& pre)
    //{
        //switch (pre.type) {
        //case 1:
            //kb.addFinishToStartConstraint(pre.first, pre.second,
                                          //pre.mintimelag, pre.maxtimelag);
            //break;
        //case 2:
            //kb.addStartToStartConstraint(pre.first, pre.second,
                                         //pre.mintimelag, pre.maxtimelag);
            //break;
        //case 3:
            //kb.addFinishToFinishConstraint(pre.first, pre.second,
                                           //pre.mintimelag, pre.maxtimelag);
            //break;
        //default:
            //throw utils::ArgError(fmt(_(
                        //"Decision: unknow precedence type `%1%'")) %
                //pre.type);
        //}
    //}
//};

//void KnowledgeBase::instantiatePlan(const std::string& name)
//{
    //const Plan& plan = library()[name];

    //std::for_each(plan.rules.rules.begin(),
                  //plan.rules.rules.end(),
                  //AddRuleToken(*this));

    //std::for_each(plan.activities.activities.begin(),
                  //plan.activities.activities.end(),
                  //AddActivityToken(*this));

    //std::for_each(plan.precedences.precedences.begin(),
                  //plan.precedences.precedences.end(),
                  //AddPrecedenceToken(*this));
//}

}}} // namespace vle model decision
