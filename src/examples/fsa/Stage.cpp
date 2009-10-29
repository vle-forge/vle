/**
 * @file examples/fsa/Stage.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
    ve::Statechart(init, events)
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

    ve::states(this) << INIT << PS << LV << IF << DF << DRG << FSLA
		     << MP << FIN;

    ve::activity(this, &Stage::activity1) >> PS;
    ve::activity(this, &Stage::activity2) >> LV;
    ve::activity(this, &Stage::activity3) >> IF;
    ve::activity(this, &Stage::activity4) >> DF;
    ve::activity(this, &Stage::activity5) >> DRG;
    ve::activity(this, &Stage::activity6) >> FSLA;
    ve::activity(this, &Stage::activity7) >> MP;

    ve::inAction(this, &Stage::action1) >> PS;
    ve::inAction(this, &Stage::action2) >> IF;
    ve::inAction(this, &Stage::action3) >> DF;
    ve::inAction(this, &Stage::action4) >> DRG;

    ve::transition(this, INIT, PS) << ve::when((double)date_sem);
    ve::transition(this, PS, LV) << ve::guard(&Stage::c2);
    ve::transition(this, LV, IF) << ve::guard(&Stage::c3);
    ve::transition(this, IF, DF) << ve::when((double)date_DF);
    ve::transition(this, DF, DRG) << ve::guard(&Stage::c5);
    ve::transition(this, DRG, FSLA) << ve::guard(&Stage::c6);
    ve::transition(this, FSLA, MP) << ve::guard(&Stage::c7);
    ve::transition(this, MP, FIN) << ve::when((double)date_rec);

    ve::eventInState(this, "Tmoy", &Stage::in) >> INIT >> PS >> LV >> IF >> DF
					       >> DRG >> FSLA >> MP >> FIN;

    initialState(INIT);

    timeStep(vle::utils::DateTime::aDay());

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    CumST = 0.0;
    STi = 0.0;
}

DECLARE_NAMED_DYNAMICS(Stage, Stage)

}}} // namespace vle examples fsa
