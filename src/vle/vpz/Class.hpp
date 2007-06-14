/** 
 * @file vpz/Class.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 17 fév 2006 11:53:22 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VPZ_CLASS_HPP
#define VLE_VPZ_CLASS_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>

namespace vle { namespace vpz {

    class Class : public Base
    {
    public:
        Class(const std::string& name) :
            m_name(name)
        { }

        virtual ~Class()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const 
        { return CLASS; }

        inline const std::string& name() const
        { return m_name; }

        inline const Model& model() const
        { return m_model; }

        inline Model& model()
        { return m_model; }

        inline const Dynamics& dynamics() const
        { return m_dynamics; }

        inline Dynamics& dynamics()
        { return m_dynamics; }

        inline const Experiment& experiments() const
        { return m_experiement; }

        inline Experiment& experiments()
        { return m_experiement; }

    private:
        Class()
        { }

        std::string     m_name;
        Model           m_model;
        Dynamics        m_dynamics;
        Experiment      m_experiement;
    };

}} // namespace vle vpz

#endif
