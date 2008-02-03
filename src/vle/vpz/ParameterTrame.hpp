/**
 * @file vle/vpz/ParameterTrame.hpp
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


#ifndef VLE_VPZ_PARAMETERTRAME_HPP
#define VLE_VPZ_PARAMETERTRAME_HPP

#include <vle/vpz/Trame.hpp>



namespace vle { namespace vpz {

    class ParameterTrame : public Trame
    {
    public:
        ParameterTrame(const std::string& time,
                       const std::string& plugin,
                       const std::string& location) :
            m_time(time),
            m_plugin(plugin),
            m_location(location)
        { }

        ParameterTrame(const std::string& time,
                       const std::string& data,
                       const std::string& plugin,
                       const std::string& location) :
            m_time(time),
            m_data(data),
            m_plugin(plugin),
            m_location(location)
        { }

        virtual ~ParameterTrame()
        { }

        virtual Base::type getType() const
        { return Base::TRAME; }

        virtual void write(std::ostream& out) const;
        
        //
        ///
        /// Get/Set functions.
        ///
        //

        inline void setData(const std::string& data)
        { m_data.assign(data); }

        inline const std::string& time() const
        { return m_time; }

        inline const std::string& data() const
        { return m_data; }

        inline const std::string& plugin() const
        { return m_plugin; }

        inline const std::string& location() const
        { return m_location; }

    private:
        std::string     m_time;
        std::string     m_data;
        std::string     m_plugin;
        std::string     m_location;
    };

}} // namespace vle vpz

#endif
