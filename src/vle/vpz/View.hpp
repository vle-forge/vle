/**
 * @file src/vle/vpz/View.hpp
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




#ifndef VLE_VPZ_View_HPP
#define VLE_VPZ_View_HPP

#include <list>
#include <string>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief A View made a link between a list of observation and an outut
     * plugin. This View can be timed by a timestep  or completely event.
     */
    class View : public Base
    {
    public:
        enum Type { TIMED, EVENT, FINISH };

        View(const std::string& name) :
            m_name(name),
            m_type(EVENT),
            m_timestep(0.0)
        { }

        virtual ~View() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return VIEW; }

        /** 
         * @brief Set the View with Event type on the specified output.
         * @param output the output of the event View.
         * @throw utils::SaxParserError if output is empty.
         */
        void setEventView(const std::string& output);

        /** 
         * @brief Set the View with Timed type on the specified output and
         * timestep.
         * @param timestep the output of the timed View.
         * @param output the timestep of the timed View.
         * @throw utils::SaxParserError if output is empty or timestep smaller
         * than 0.
         */
        void setTimedView(double timestep, const std::string& output);

        /** 
         * @brief Set the View with the Finish type on the specified output.
         * @param output the output of the Finish View.
         * @throw utils::SaxParserError if output is empty.
         */
        void setFinishView(const std::string& output);

        inline const std::string& name() const
        { return m_name; }

        inline const Type& type() const
        { return m_type; }

        inline void setType(Type type)
        { m_type = type; }

        inline std::string streamtype() const
        { return m_type == TIMED ? "timed" : 
                  m_type == EVENT ? "event" : "finish"; }

        inline void setTimestep(double time);

        inline double timestep() const
        { return m_timestep; }

        void setOutput(const std::string& output);

        inline const std::string& output() const
        { return m_output; }

        inline const std::string& data() const
        { return m_data; }

        inline void setData(const std::string& data)
        { m_data = data; }

    private:
        View()
        { }

        std::string     m_name;
        Type            m_type;
        double          m_timestep;
        std::string     m_output;
        std::string     m_data;
    };

}} // namespace vle vpz

#endif
