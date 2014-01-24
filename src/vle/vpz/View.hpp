/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_VPZ_VIEW_HPP
#define VLE_VPZ_VIEW_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <string>

namespace vle { namespace vpz {

    /**
     * @brief A View made a link between a list of Observation and an Output
     * plug-in. This View can be timed by a timestep, finish or completely
     * event and make link with Output by name.
     */
    class VLE_API View : public Base
    {
    public:
        /**
         * @brief Define the type of View.
         */
        enum Type { TIMED, EVENT, FINISH };

        /**
         * @brief Build a new event view with a specific name.
         * @param name The name of the View.
         */
        View(const std::string& name) :
            m_name(name),
            m_type(EVENT),
            m_timestep(0.0)
        {}

        /**
         * @brief Build a new View with a specific name, type and reference.
         * @param name The name of the View.
         * @param type The type of this View.
         * @param output The name of the Output.
         * @param timestep A timestep, necessary for type TIMED.
         * @throw utils::ArgError it the time step is not greater than 0 and the
         * Type is TIMED.
         */
        View(const std::string& name, View::Type type,
             const std::string& output, double timestep = 0.0);

        /**
         * @brief Nothing to delete.
         */
        virtual ~View()
        {}

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <view name="name" output="outout" type="event" />
         * <view name="name" output="output" type="finish" />
         * @endcode
         * @param out Output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return VIEW.
         */
        virtual Base::type getType() const
        { return VLE_VPZ_VIEW; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the name of this View.
         * @return The name.
         */
        inline const std::string& name() const
        { return m_name; }

        /**
          * @brief Set a new name for this View.
	  * @param name The new name for this View.
	  */
	inline void setName(std::string name)
        { m_name = name; }

        /**
         * @brief Get the type of this View.
         * @return The type, TIMED, FINISH or EVENT.
         */
        inline const Type& type() const
        { return m_type; }

        /**
         * @brief Set a new type for this View.
         * @param type The new type, TIMED, FINISH or EVENT.
         */
        inline void setType(Type type)
        { m_type = type; }

        /**
         * @brief Get a string representation of the current type.
         * @return "timed", "event" or "finish".
         */
        inline std::string streamtype() const
        {
            return m_type == TIMED ? "timed" : m_type == EVENT ? "event" :
                "finish";
        }

        /**
         * @brief Assign a new time step to the timed view.
         * @param time The new time.
         * @throw utils::ArgError if time is not greater thant 0.0.
         */
        void setTimestep(double time);

        /**
         * @brief Get the time step.
         * @return A time step.
         */
        inline double timestep() const
        { return m_timestep; }

        /**
         * @brief The string representation of the Output.
         * @return The Output.
         */
        inline const std::string& output() const
        { return m_output; }

        /**
         * @brief The CDATA attached to this View.
         * @return The CDATA.
         */
        inline const std::string& data() const
        { return m_data; }

        /**
         * @brief Assign a new CDATA.
         * @param data The new CDATA.
         */
        inline void setData(const std::string& data)
        { m_data = data; }

	/**
	 * @brief A operator to compare two Views
	 * @param view The View to compare
	 * @return True if the views are equals
	 */
	bool operator==(const View& view) const;

    private:
        std::string     m_name;
        Type            m_type;
        std::string     m_output;
        double          m_timestep;
        std::string     m_data;
    };

}} // namespace vle vpz

#endif
