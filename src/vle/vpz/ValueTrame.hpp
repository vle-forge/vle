/** 
 * @file ValueTrame.hpp
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

#ifndef VLE_VPZ_VALUETRAME_HPP
#define VLE_VPZ_VALUETRAME_HPP

#include <vle/vpz/Trame.hpp>
#include <vle/value/Value.hpp>



namespace vle { namespace vpz {

    class ModelTrame
    {
    public:
        ModelTrame(const std::string& simulator,
                   const std::string& parent,
                   const std::string& port,
                   const std::string& view) :
            m_simulator(simulator),
            m_parent(parent),
            m_port(port),
            m_view(view)
        { }

        void setValue(const value::Value& value)
        { m_value = value; }

        inline const std::string& simulator() const
        { return m_simulator; }

        inline const std::string& parent() const
        { return m_parent; }

        inline const std::string& port() const
        { return m_port; }

        inline const std::string& view() const
        { return m_view; }

        inline const value::Value& value() const
        { return m_value; }

    private:
        std::string     m_simulator;
        std::string     m_parent;
        std::string     m_port;
        std::string     m_view;
        value::Value    m_value;
    };

    typedef std::list < ModelTrame > ModelTrameList;

    class ValueTrame : public Trame
    {
    public:
        ValueTrame(const std::string& time) :
            m_time(time)
            
        { }

        virtual ~ValueTrame()
        { }

        void add(const std::string& simulator,
                 const std::string& parent,
                 const std::string& port,
                 const std::string& view);

        void add(const value::Value& value);

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return Base::MODELTRAME; }

        //
        ///
        /// Get/Set function.
        ///
        //

        const std::string& time() const
        { return m_time; }

        const ModelTrameList& trames() const
        { return m_list; }

    private:
        std::string     m_time;
        ModelTrameList  m_list;
    };

}} // namespace vle vpz

#endif
