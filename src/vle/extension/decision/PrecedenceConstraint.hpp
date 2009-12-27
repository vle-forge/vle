/**
 * @file vle/extension/decision/PrecedenceConstraint.hpp
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


#ifndef VLE_EXTENSION_DECISION_PRECEDENCECONTRAINT_HPP
#define VLE_EXTENSION_DECISION_PRECEDENCECONTRAINT_HPP

#include <vle/extension/decision/Activity.hpp>

namespace vle { namespace extension { namespace decision {

    class PrecedenceConstraint
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
        enum Result {
            Inapplicable, /**< Constraint is inapplicable, for instance in FF
                            and A and B are in wait state. */
            Wait, /**< Constraint is not valid be can be valid in future. */
            Valid, /**< Constraint is valid. */
            Failed /**< Constraint is not valid and can not be valid in
                     future. */
        };

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

        const devs::Time& timelag() const { return m_mintimelag; }
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

    struct PrecedenceConstraintCompareFirst
    {
        inline bool operator()(const PrecedenceConstraint& x,
                               const PrecedenceConstraint& y) const
        {
            return x.first()->first < y.first()->first;
        }
    };

    struct PrecedenceConstraintCompareSecond
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
            << " delta [" << p.mintimelag() << ", " << p.maxtimelag() << "] "
            << "(" << p.first()->first << ") - (" << p.second()->first << ")";
    }

    inline std::ostream&
        operator<<(std::ostream& o, const PrecedenceConstraint::Result& p)
    {
        switch (p) {
        case PrecedenceConstraint::Inapplicable: o << "Inapplicable"; break;
        case PrecedenceConstraint::Wait: o << "Wait"; break;
        case PrecedenceConstraint::Valid: o << "Valid"; break;
        case PrecedenceConstraint::Failed: o << "Failed"; break;
        }
        return o;
    }

}}} // namespace vle model decision

#endif
