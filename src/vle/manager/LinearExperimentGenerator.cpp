/** 
 * @file manager/LinearExperimentGenerator.cpp
 * @brief A class to translate Experiment file into Instance of Experiment.
 * @author The vle Development Team
 * @date jeu, 04 avr 2007 11:19:02 +0100
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/manager/LinearExperimentGenerator.hpp>
#include <vle/value/Set.hpp>


namespace vle { namespace manager {

void LinearExperimentGenerator::build_combination(size_t& nb)
{
    for(size_t i=0; i < mCondition.size(); ++i)
	mCondition[i].pos++;
    nb++;
}

size_t LinearExperimentGenerator::get_combination_number() const
{
    const vpz::Conditions& cnds(mFile.project().experiment().conditions());

    Assert(utils::InternalError, not cnds.empty(),
           "Build a linear experimental frame with empty value?");

    const size_t nb = cnds.begin()->second.values().size();

    for (vpz::Conditions::const_iterator it = cnds.begin();
         it != cnds.end(); ++it) {

        const vpz::Condition::SetList& values(it->second.values());
        for (vpz::Condition::const_iterator jt = values.begin();
             jt != values.end(); ++jt) {

            Assert(utils::InternalError, nb == jt->second->size(),
                   boost::format("Build a linear experimental frame with bad"
                                 " number value for condition %1% port %2%") %
                   it->first % jt->first); 

        }
    }

    return nb;
}

}} // namespace vle manager
