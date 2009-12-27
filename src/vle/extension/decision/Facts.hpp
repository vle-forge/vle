/**
 * @file vle/extension/decision/Facts.hpp
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


#ifndef VLE_EXTENSION_DECISION_FACTS_HPP
#define VLE_EXTENSION_DECISION_FACTS_HPP

#include <vle/extension/decision/Rules.hpp>
#include <vle/value/Value.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension { namespace decision {

    typedef boost::function < void (const vle::value::Value&) > Fact;

    class Facts
    {
    public:
        typedef std::map < std::string, Fact > facts_t;
        typedef facts_t::const_iterator const_iterator;
        typedef facts_t::iterator iterator;
        typedef facts_t::size_type size_type;

        void add(const std::string& name, const Fact& pred);

        void apply(const std::string& name, const value::Value& value);

        iterator begin() { return m_lst.begin(); }
        const_iterator begin() const { return m_lst.begin(); }
        iterator end() { return m_lst.end(); }
        const_iterator end() const { return m_lst.end(); }
        size_type size() const { return m_lst.size(); }

    private:
        facts_t m_lst;
    };

    inline std::ostream& operator<<(std::ostream& s, const Facts& o)
    {
        s << "facts: ";
        for (Facts::const_iterator it = o.begin(); it != o.end(); ++it) {
            s << " (" << it->first << ")";
        }
        return s;
    }

}}} // namespace vle model decision

#endif
