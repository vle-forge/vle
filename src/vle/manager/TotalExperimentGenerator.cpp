/** 
 * @file manager/TotalExperimentGenerator.cpp
 * @brief A class to translate Experiement file into Instance of Experiment.
 * @author The vle Development Team
 * @date jeu, 02 nov 2006 11:19:02 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#include <vle/manager/TotalExperimentGenerator.hpp>
#include <vle/value/Set.hpp>



namespace vle { namespace manager {

void TotalExperimentGenerator::build_combination(size_t& nb)
{
    size_t sz = mCondition.size() - 1;

    if (mCondition[sz].pos != mCondition[sz].sz - 1) {
        mCondition[sz].pos++;
    } else {
        int i = sz;

        while (i >= 0) {
            if (mCondition[i].pos == mCondition[i].sz - 1) {
                mCondition[i].pos = 0;
                i--;
            } else {
                mCondition[i].pos++;
                break;
            }
        }
    }
    nb++;
}

size_t TotalExperimentGenerator::get_combination_number() const
{
    size_t nb = 1;
    const vpz::Conditions::ConditionList& conds(
        mFile.project().experiment().conditions().conditions());

    for (vpz::Conditions::const_iterator it = conds.begin();
         it != conds.end(); ++it) {

        const vpz::Condition::ValueList& values(it->second.values());
        for (vpz::Condition::const_iterator jt = values.begin();
             jt != values.end(); ++jt) {
            nb *= jt->second->size();
        }
    }

    return nb;
}

}} // namespace vle manager
