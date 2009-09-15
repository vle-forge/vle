/**
 * @file vle/extension/decision/Rules.cpp
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


#include <vle/extension/decision/Rules.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace extension { namespace decision {

Rule& Rules::add(const std::string& name, const Rule& rule)
{
    const_iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError("Decision: rule already exist");
    }

    return (*m_lst.insert(std::make_pair < std::string, Rule >(
                name, rule)).first).second;
}

Rule& Rules::add(const std::string& name, const Predicate& pred)
{
    iterator it(m_lst.find(name));

    if (it == m_lst.end()) {
        Rule& r(add(name));
        r.add(pred);
        return r;
    } else {
        it->second.add(pred);
        return it->second;
    }
}

Rules::result_t Rules::apply() const
{
    result_t result;
    const_iterator it(m_lst.begin());

    while (it != m_lst.end()) {
        if (it->second.isAvailable()) {
            result.push_back(it);
        }
        ++it;
    }
    return result;
}

}}} // namespace vle model decision

