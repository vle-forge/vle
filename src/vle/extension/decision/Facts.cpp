/**
 * @file vle/extension/decision/Facts.cpp
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


#include <vle/extension/decision/Facts.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace extension { namespace decision {

void Facts::add(const std::string& name, const Fact& fac)
{
    const_iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(fmt(
                _("Decision: fact '%1%' already exist")) % name);
    }

    m_lst.insert(std::make_pair < std::string, Fact >(name, fac));
}

void Facts::apply(const std::string& name, const value::Value& value)
{
    const_iterator it(m_lst.find(name));
    if (it == m_lst.end()) {
        throw utils::ArgError(fmt(
                _("Decision: fact '%1%' does not exist")) % name);
    }
    (*it).second(value);
}

}}} // namespace vle model decision
