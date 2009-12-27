/**
 * @file vle/manager/LinearExperimentGenerator.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/manager/LinearExperimentGenerator.hpp>
#include <vle/value/Set.hpp>


namespace vle { namespace manager {

void LinearExperimentGenerator::buildCombination(size_t& nb)
{
    for(size_t i=0; i < mCondition.size(); ++i)
        if (mCondition[i].sz != 1)
            mCondition[i].pos++;
    nb++;
}

size_t LinearExperimentGenerator::getCombinationNumber() const
{
    if (mCondition.empty()) {
        throw utils::InternalError(_(
                "Build a linear experimental frame with empty value?"));
    }

    size_t nb = 1;

    {
        std::vector < cond_t >::const_iterator it;
        for (it = mCondition.begin(); it != mCondition.end(); ++it) {
            if (it->sz != 1) {
                nb = it->sz;
                break;
            }
        }
    }

    size_t cndnb = 0;

    std::vector < cond_t >::const_iterator it;
    for (it = mCondition.begin(); it != mCondition.end(); ++it) {

        if (it->sz != nb and it->sz != 1) {
            throw utils::InternalError(fmt(_(
                        "Build a linear experimental frame with bad"
                        " number value for condition '%1%'")) % cndnb);
        }

        cndnb++;
    }

    return nb;
}

}} // namespace vle manager
