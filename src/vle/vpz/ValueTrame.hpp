/**
 * @file vle/vpz/ValueTrame.hpp
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


#ifndef VLE_VPZ_VALUETRAME_HPP
#define VLE_VPZ_VALUETRAME_HPP

#include <vle/vpz/Trame.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The ModelTrame stores the result of observation for a
     * devs::Simulator models and the value::Value attached to the observation.
     */
    class ModelTrame
    {
    public:
        /**
         * @brief Build an empty ModelTrame.
         * @param simulator The name of the devs::Simulator.
         * @param parent The parent of the devs::Simulator.
         * @param port The port of observation.
         * @param view The view of observation.
         */
        ModelTrame(const std::string& simulator,
                   const std::string& parent,
                   const std::string& port,
                   const std::string& view) :
            m_simulator(simulator),
            m_parent(parent),
            m_port(port),
            m_view(view)
        {}

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Assign a new value.
         * @param value The new value.
         */
        void setValue(const value::Value& value)
        { m_value = value; }

        /**
         * @brief Get the name of the devs::Simulator.
         * @return name of the model.
         */
        inline const std::string& simulator() const
        { return m_simulator; }

        /**
         * @brief Get the parent of the devs::Simulator.
         * @return name of the parent.
         */
        inline const std::string& parent() const
        { return m_parent; }

        /**
         * @brief Get the name of the observation port.
         * @return name of the observation port.
         */
        inline const std::string& port() const
        { return m_port; }

        /**
         * @brief Get the name of the view.
         * @return name of the view.
         */
        inline const std::string& view() const
        { return m_view; }

        /**
         * @brief Get a constant reference to the value.
         * @return The value.
         */
        inline const value::Value& value() const
        { return m_value; }

        /**
         * @brief Show the XML representation of this class.
         * @param o output stream.
         * @param m the ModelTrame to show.
         * @return the output stream.
         */
        friend std::ostream& operator<<(std::ostream& o, const ModelTrame& m)
        {
            o << "<modeltrame"
                << " name=\"" << m.simulator() << "\" "
                << " parent=\"" << m.parent() << "\" "
                << " port=\"" << m.port() << "\" "
                << " view=\"" << m.view() << "\" >"
                << m.value()->toXML();
            return o << "</modeltrame>";
        }

    private:
        std::string     m_simulator;
        std::string     m_parent;
        std::string     m_port;
        std::string     m_view;
        value::Value    m_value;
    };

    /**
     * @brief Define a list of ModelTrame.
     */
    typedef std::list < ModelTrame > ModelTrameList;

    /**
     * @brief The ValueTrame is send when simulator send data to the
     * oov::StreamReader. The ValueTrame wraps a ModelTrameList to store, for
     * each devs::Simulator the observation.
     */
    class ValueTrame : public Trame
    {
    public:
        /**
         * @brief Build an empty ValueTrame with a specific date.
         * @param time The time of this observation.
         */
        ValueTrame(const std::string& time) :
            m_time(time)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~ValueTrame()
        {}

        /**
         * @brief Write an XML representation of this class.
         * @code
         * <trame type="value" date="1.25">
         *  <modeltrame name="" parent="" port="" view="">
         *   [value]
         *  </modeltrame>
         * </trame>
         * @endcode
         * @param out the output stream parameter.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return MODELTRAME.
         */
        virtual Base::type getType() const
        { return Base::MODELTRAME; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the ModelTrameList.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add a ModelTrame into the list.
         * @param simulator The name of the devs::Simulator.
         * @param parent The parent of the devs::Simulator.
         * @param port The observation port.
         * @param view The view attached.
         */
        void add(const std::string& simulator,
                 const std::string& parent,
                 const std::string& port,
                 const std::string& view);

        /**
         * @brief Add a value::Value to the latest ModelTrame into the
         * ModelTrameList.
         * @param value The value to attach.
         * @throw utils::ArgError if list of ModelTrameList is empty.
         */
        void add(const value::Value& value);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief The date of this event.
         * @return A date.
         */
        const std::string& time() const
        { return m_time; }

        /**
         * @brief The set of trames.
         * @return The trames.
         */
        const ModelTrameList& trames() const
        { return m_list; }

    private:
        std::string     m_time;
        ModelTrameList  m_list;
    };

}} // namespace vle vpz

#endif
