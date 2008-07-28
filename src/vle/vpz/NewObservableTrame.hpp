/**
 * @file src/vle/vpz/NewObservableTrame.hpp
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

#ifndef VLE_VPZ_NEWOBSERVABLETRAME_HPP
#define VLE_VPZ_NEWOBSERVABLETRAME_HPP

#include <vle/vpz/Trame.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The NewObservableTrame is send when a model is created from the
     * simulation to the server oov.
     */
    class NewObservableTrame : public Trame
    {
    public:
        /**
         * @brief Build a new NewObservableTrame.
         * @param time the time of this event.
         * @param name the name of the model.
         * @param parent the parent of the model.
         * @param port the name of the port.
         * @param view the name of the view.
         */
        NewObservableTrame(const std::string& time,
                           const std::string& name,
                           const std::string& parent,
                           const std::string& port,
                           const std::string& view) :
            m_time(time),
            m_name(name),
            m_parent(parent),
            m_port(port),
            m_view(view)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~NewObservableTrame()
        {}

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <trame type="new" date="" name="" parent="" port="" view="" />
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
         * @brief The date of this event.
         * @return A date.
         */
        inline const std::string& time() const
        { return m_time; }

        /**
         * @brief The name of the simulator.
         * @return The simulator.
         */
        const std::string& name() const
        { return m_name; }

        /**
         * @brief The parent of the simulator.
         * @return The simulator.
         */
        const std::string& parent() const
        { return m_parent; }

        /**
         * @brief The port of the observation.
         * @return The name of the port.
         */
        const std::string& port() const
        { return m_port; }

        /**
         * @brief The view of the observation.
         * @return The name of the view.
         */
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
