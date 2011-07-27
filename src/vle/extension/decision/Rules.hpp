/*
 * @file vle/extension/decision/Rules.hpp
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


#ifndef VLE_EXTENSION_DECISION_RULES_HPP
#define VLE_EXTENSION_DECISION_RULES_HPP

#include <vle/extension/decision/Rule.hpp>
#include <vle/extension/DllDefines.hpp>
#include <map>
#include <string>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT Rules
{
public:
    typedef std::map < std::string, Rule > rules_t;
    typedef rules_t::const_iterator const_iterator;
    typedef rules_t::iterator iterator;
    typedef rules_t::value_type value_type;
    typedef rules_t::size_type size_type;
    typedef std::vector < const_iterator > result_t;

    Rule& add(const std::string& name, const Rule& rule = Rule());

    Rule& add(const std::string& name, const Predicate& pred);

    result_t apply() const;

    const Rule& get(const std::string& name) const;

    iterator begin() { return m_lst.begin(); }
    const_iterator begin() const { return m_lst.begin(); }
    iterator end() { return m_lst.end(); }
    const_iterator end() const { return m_lst.end(); }
    size_type size() const { return m_lst.size(); }
    bool empty() const { return m_lst.empty(); }

private:
    rules_t m_lst;
};

inline std::ostream& operator<<(std::ostream& s, const Rules& o)
{
    std::ios_base::fmtflags fl = s.flags();
    s << std::boolalpha << "rules:";
    for (Rules::const_iterator it = o.begin(); it != o.end(); ++it) {
        s << " (" << it->first << "," << it->second.isAvailable() << ")";
    }
    s.flags(fl);
    return s;
}

}}} // namespace vle model decision

#endif
