/**
 * @file src/vle/vpz/ParameterTrame.hpp
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

    /**
     * @brief The ParameterTrame is send at the beginning of the simulation when
     * creating the devs::View, oov::StreamReader, devs::StreamWriter.
     */
    class ParameterTrame : public Trame
    {
    public:
        /**
         * @brief Build a new ParameterTrame.
         * @param time the time of this event.
         * @param plugin the plugin of this view.
         * @param location the location of this view.
         */
        ParameterTrame(const std::string& time,
                       const std::string& plugin,
                       const std::string& location) :
            m_time(time),
            m_plugin(plugin),
            m_location(location)
        {}

        /**
         * @brief Build a new ParameterTrame.
         * @param time the time of this event.
         * @param data the data of the plugin.
         * @param plugin the plugin of this view.
         * @param location the location of this view.
         */
        ParameterTrame(const std::string& time,
                       const std::string& data,
                       const std::string& plugin,
                       const std::string& location) :
            m_time(time),
            m_data(data),
            m_plugin(plugin),
            m_location(location)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~ParameterTrame()
        { }

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <trame type="param" date="" plugin="" location="">
         *  [...]
         * </trame>
         * @endcode
         * @param out output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return TRAME.
         */
        virtual Base::type getType() const
        { return Base::TRAME; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Assign a data to the ParameterTrame.
         * @param data The new data.
         */
        inline void setData(const std::string& data)
        { m_data.assign(data); }

        /**
         * @brief The date of this event.
         * @return A date.
         */
        inline const std::string& time() const
        { return m_time; }

        /**
         * @brief The data of the plugin.
         * @return The CDATA of the output.
         */
        inline const std::string& data() const
        { return m_data; }

        /**
         * @brief The name of the plugin.
         * @return The plugin.
         */
        inline const std::string& plugin() const
        { return m_plugin; }

        /**
         * @brief The location of the plugin.
         * @return The location.
         */
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
