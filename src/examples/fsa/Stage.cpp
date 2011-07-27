/*
 * @file examples/fsa/Stage.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <Stage.hpp>
#include <boost/assign.hpp>
#include <boost/date_time.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

Stage::Stage(const vd::DynamicsInit& init,
	     const vd::InitEventList& events) :
    vf::Statechart(init, events)
{
    if (events.exist("datsemis")) {
	date_sem = boost::gregorian::date(
	    boost::gregorian::from_string(
		vle::value::toString(events.get("datsemis")))).julian_day();
    }
    if (events.exist("datDF")) {
	date_DF = boost::gregorian::date(
	    boost::gregorian::from_string(
		vle::value::toString(events.get("datDF")))).julian_day();
    }
    if (events.exist("datrec")) {
	date_rec = boost::gregorian::date(
	    boost::gregorian::from_string(
		vle::value::toString(events.get("datrec")))).julian_day();
    }

    if (events.exist("stlv"))
	stlv = vle::value::toDouble(events.get("stlv"));
    if (events.exist("stif"))
	stif = vle::value::toDouble(events.get("stif"));
    if (events.exist("ifjour"))
	ifjour = vle::value::toDouble(events.get("ifjour"));
    if (events.exist("stdf"))
	stdf = vle::value::toDouble(events.get("stdf"));
    if (events.exist("stdrg"))
	stdrg = vle::value::toDouble(events.get("stdrg"));
    if (events.exist("stmp"))
	stmp = vle::value::toDouble(events.get("stmp"));
    if (events.exist("vdrg"))
	vdrg = vle::value::toDouble(events.get("vdrg"));
    if (events.exist("NETpot"))
	NETpot = vle::value::toDouble(events.get("NETpot"));
    if (events.exist("Nstruct"))
	Nstruct = vle::value::toDouble(events.get("Nstruct"));
    if (events.exist("P1Gpot"))
	P1Gpot = vle::value::toDouble(events.get("P1Gpot"));

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    states(this) << INIT << PS << LV << IF << DF << DRG << FSLA
		     << MP << FIN;

    activity(this, &Stage::activity1) >> PS;
    activity(this, &Stage::activity2) >> LV;
    activity(this, &Stage::activity3) >> IF;
    activity(this, &Stage::activity4) >> DF;
    activity(this, &Stage::activity5) >> DRG;
    activity(this, &Stage::activity6) >> FSLA;
    activity(this, &Stage::activity7) >> MP;

    inAction(this, &Stage::action1) >> PS;
    inAction(this, &Stage::action2) >> IF;
    inAction(this, &Stage::action3) >> DF;
    inAction(this, &Stage::action4) >> DRG;

    transition(this, INIT, PS) << when((double)date_sem);
    transition(this, PS, LV) << guard(&Stage::c2);
    transition(this, LV, IF) << guard(&Stage::c3);
    transition(this, IF, DF) << when((double)date_DF);
    transition(this, DF, DRG) << guard(&Stage::c5);
    transition(this, DRG, FSLA) << guard(&Stage::c6);
    transition(this, FSLA, MP) << guard(&Stage::c7);
    transition(this, MP, FIN) << when((double)date_rec);

    eventInState(this, "Tmoy", &Stage::in) >> INIT >> PS >> LV >> IF >> DF
                                           >> DRG >> FSLA >> MP >> FIN;

    initialState(INIT);

    timeStep(utils::DateTime::aDay());

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    CumST = 0.0;
    STi = 0.0;
}

DECLARE_DYNAMICS(Stage)

}}} // namespace vle examples fsa
