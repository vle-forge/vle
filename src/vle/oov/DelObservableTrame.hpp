/** 
 * @file DelObservableTrame.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-15
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#ifndef VLE_OOV_DELOBSERVABLETRAME_HPP
#define VLE_OOV_DELOBSERVABLETRAME_HPP

#include <vle/oov/Trame.hpp>

namespace vle { namespace oov {

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

        virtual void print(std::ostream& out) const;

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

}} // namespace vle oov

#endif
