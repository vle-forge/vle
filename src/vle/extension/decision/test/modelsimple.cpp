/**
 * @file vle/extension/decision/test/modelsimple.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/extension/decision/Agent.hpp>


namespace vmd = vle::extension::decision;

namespace ex {

    class SimpleAgent: public vmd::Agent
    {
    public:
        SimpleAgent(const vle::devs::DynamicsInit& mdl,
                    const vle::devs::InitEventList& evts)
            : vmd::Agent(mdl, evts), today(0), yesterday(0)
        {
            addFact("today", boost::bind(&SimpleAgent::date, this, _1));

            vmd::Rule& r1 = addRule("Rule 1");
            r1.add(boost::bind(&ex::SimpleAgent::haveGoodTemp, this));
            r1.add(boost::bind(&ex::SimpleAgent::isAlwaysTrue, this));

            vmd::Rule& r2 = addRule("Rule 2");
            r2.add(boost::bind(&ex::SimpleAgent::haveGoodTemp, this));

            vmd::Activity& act1 = addActivity("act1");
            act1.addRule("Rule 1", r1);

            vmd::Activity& act2 = addActivity("act2");
            act2.addRule("Rule 2", r2);
            act2.initStartTimeFinishTime(1.5, 2.5);

            vmd::Activity& act3 = addActivity("act3");
            act3.addRule("Rule 2", r2);
            act3.initStartTimeFinishTime(3.5, 4.5);
        }

        /*
         * list of facts
         */

        void date(const vle::value::Value& val)
        {
            yesterday = today;
            today = val.toDouble().value();
        }

        /*
         * list of predicates
         */

        bool isAlwaysTrue() const
        {
            return true;
        }

        bool haveGoodTemp() const
        {
            return yesterday > 15.0 and today > 20.0;
        };

        double today, yesterday;
    };


} // namespace ex
