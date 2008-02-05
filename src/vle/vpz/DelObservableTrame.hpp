/**
 * @file src/vle/vpz/DelObservableTrame.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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




#ifndef VLE_VPZ_DELOBSERVABLETRAME_HPP
#define VLE_VPZ_DELOBSERVABLETRAME_HPP

#include <vle/vpz/Trame.hpp>

namespace vle { namespace vpz {

    class DelObservableTrame : public Trame
    {
    public:
        DelObservableTrame(const std::string& time,
                           const std::string& name,
                           const std::string& parent,
                           const std::string& port,
                           const std::string& view) :
            m_time(time),
            m_name(name),
            m_parent(parent),
            m_port(port),
            m_view(view)
        { }

        virtual ~DelObservableTrame()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return Base::TRAME; }
        //
        ///
        /// Get/Set functions.
        ///
        //

        inline const std::string& time() const
        { return m_time; }

        const std::string& name() const
        { return m_name; }

        const std::string& parent() const
        { return m_parent; }

        const std::string& port() const
        { return m_port; }

        const std::string& view() const
        { return m_view; }

    private:
        std::string     m_time;
        std::string     m_name;
        std::string     m_parent;
        std::string     m_port;
        std::string     m_view;
    };

}} // namespace vle vpz

#endif
