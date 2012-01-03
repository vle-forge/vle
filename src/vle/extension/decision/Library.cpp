/**
 * @file vle/extension/decision/Library.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2012 INRA http://www.inra.fr
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


#include <vle/extension/decision/Library.hpp>
#include <vle/utils/Parser.hpp>
#include <sstream>

namespace vle { namespace extension { namespace decision {

void Library::add(const std::string& name, const std::string& content)
{
    Plan p(mKb, content);

    std::pair < iterator, bool > r;
    r = mLst.insert(std::make_pair(name, p));

    if (not r.second) {
        throw utils::ArgError(
            fmt(_("Decision: plan library, plan `%1%' already exists")) %
            name);
    }
}

void Library::add(const std::string& name, std::istream& stream)
{
    Plan p(mKb, stream);

    std::pair < iterator, bool > r;
    r = mLst.insert(std::make_pair(name, p));

    if (not r.second) {
        throw utils::ArgError(
            fmt(_("Decision: plan library, plan `%1%' already exists")) %
            name);
    }
}

}}} // namespace vle model decision
