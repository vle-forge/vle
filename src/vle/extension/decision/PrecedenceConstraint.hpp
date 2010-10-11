/*
 * @file vle/extension/decision/PrecedenceConstraint.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_DECISION_PRECEDENCECONTRAINT_HPP
#define VLE_EXTENSION_DECISION_PRECEDENCECONTRAINT_HPP

#include <vle/extension/decision/Activity.hpp>
#include <vle/extension/DllDefines.hpp>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT PrecedenceConstraint
{
public:
    typedef std::map < std::string, Activity >::const_iterator iterator;

    /**
     * @brief Defines the types of constraints.
     */
    enum Type {
        SS, /**< Start to Start relationship. */
        FS, /**< Finish to Start relationship. */
        FF /**< Finish to Finish relationship. */
    };

    /**
     * @brief Defines the types of the validity function.
     */
    enum ResultType {
        Inapplicable, /**< Constraint is inapplicable, for instance in FF
                        and A and B are in wait state. */
        Wait, /**< Constraint is not valid be can be valid in future. */
        Valid, /**< Constraint is valid. */
        Failed /**< Constraint is not valid and can not be valid in
                 future. */
    };

    /**
     * @brief Define the return of the isValid function. The attribute Type
     * defines the result of the validity, the attribute devs::Time the next
     * date to wake up the activity.
     */
    typedef std::pair < ResultType, devs::Time > Result;

    /* * * * * */

    PrecedenceConstraint(iterator first,
                         iterator second,
                         Type type,
                         const devs::Time& mintimelag,
                         const devs::Time& maxtimelag);

    bool isSS() const { return m_type == SS; }
    bool isFS() const { return m_type == FS; }
    bool isFF() const { return m_type == FF; }
    const Type& type() const { return m_type; }

    const devs::Time& mintimelag() const { return m_mintimelag; }
    const devs::Time& maxtimelag() const { return m_maxtimelag; }

    iterator first() const { return m_first; }
    iterator second() const { return m_second ; }

    Result isValid(const devs::Time& time) const;

private:
    Type       m_type;
    devs::Time m_mintimelag;
    devs::Time m_maxtimelag;
    iterator   m_first;
    iterator   m_second;
};

struct VLE_EXTENSION_EXPORT PrecedenceConstraintCompareFirst
{
    inline bool operator()(const PrecedenceConstraint& x,
                           const PrecedenceConstraint& y) const
    {
        return x.first()->first < y.first()->first;
    }
};

struct VLE_EXTENSION_EXPORT PrecedenceConstraintCompareSecond
{
    inline bool operator()(const PrecedenceConstraint& x,
                           const PrecedenceConstraint& y) const
    {
        return x.second()->first < y.second()->first;
    }
};

inline std::ostream&
operator<<(std::ostream& o, const PrecedenceConstraint::Type& t)
{
    switch (t) {
    case PrecedenceConstraint::SS: o << "SS"; break;
    case PrecedenceConstraint::FS: o << "FS"; break;
    case PrecedenceConstraint::FF: o << "FF"; break;
    }
    return o;
}

inline std::ostream&
operator<<(std::ostream& o, const PrecedenceConstraint& p)
{
    return o << "Precedence: "
        << "type: " << p.type()
        << " delta [" << p.mintimelag().toString() << ", "
        << p.maxtimelag().toString() << "] "
        << "(" << p.first()->first << ") - (" << p.second()->first << ")";
}

inline std::ostream&
operator<<(std::ostream& o, const PrecedenceConstraint::Result& p)
{
    switch (p.first) {
    case PrecedenceConstraint::Inapplicable: o << "Inapplicable"; break;
    case PrecedenceConstraint::Wait: o << "Wait"; break;
    case PrecedenceConstraint::Valid: o << "Valid"; break;
    case PrecedenceConstraint::Failed: o << "Failed"; break;
    }
    return o;
}

}}} // namespace vle model decision

#endif
