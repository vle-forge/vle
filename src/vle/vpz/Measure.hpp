/** 
 * @file vpz/Measure.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:25:06 +0100
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

#ifndef VLE_VPZ_MEASURE_HPP
#define VLE_VPZ_MEASURE_HPP

#include <list>
#include <string>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {


    class ObservableList : public std::map < std::string, Observable >
    {
    };

    /** 
     * @brief A measure made a link between a list of observation and an outut
     * plugin. This measure can be timed by a timestep  or completely event.
     */
    class Measure : public Base
    {
    public:
        enum Type { TIMED, EVENT };

        Measure();

        virtual ~Measure() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return MEASURE; }

        /** 
         * @brief Set the Measure with Event type and the specified name and
         * output.
         * 
         * @param output the output of the event Measure.
         *
         * @throw Exception::Internal if name or output are empty.
         */
        void setEventMeasure(const std::string& output);

        /** 
         * @brief Set the Measure with Timed type and the specified name, output
         * and timestep.
         * 
         * @param timestep the output of the timed Measure.
         * @param output the timestep of the timed Measure.
         *
         * @throw Exception::Internal if name or output are empty of timestep <=
         * 0.
         */
        void setTimedMeasure(double timestep,
                             const std::string& output);

        /** 
         * @brief Add an observable to the Measure.
         * 
         * @param o the observable to add.
         * @return a reference to the newly created observable.
         *
         * @throw Exception::Internal if observable already exist.
         */
        Observable& addObservable(const Observable& o);

        /** 
         * @brief Add an observable to the Measure.
         * 
         * @param name of the observable.
         * @param group if define, the group where observation is store.
         * @param index if define, the index of the group.
         * @return a reference to the newly created observable.
         */
        Observable& addObservable(const std::string& name,
                                  const std::string& group = std::string(),
                                  int index = -1);

        /** 
         * @brief Delete an observable from the list.
         * 
         * @param name model name to observe.
         */
        void delObservable(const std::string& name);

        /** 
         * @brief Get a reference to an Observable.
         * 
         * @param name name of the Observable.
         *
         * @throw Exception::Internal if Observable not found
         *
         * @return A reference to the founded Observable.
         */
        const Observable& getObservable(const std::string& name) const;

        /** 
         * @brief Test if an observable exist in list.
         * 
         * @param name name of the Observable.
         *
         * @return true if found, false otherwise.
         */
        bool existObservable(const std::string& name) const;

        void setName(const std::string& name) 
        { m_name.assign(name); }

        const std::string& name() const
        { return m_name; }

        const Type& type() const
        { return m_type; }

        std::string streamtype() const
        { return (m_type == TIMED ? "timed" : "event"); }

        double timestep() const
        { return m_timestep; }

        const std::string& output() const
        { return m_output; }

        /** 
         * @brief Return the list of all Observable.
         * 
         * @return A constant reference to the list of Observable.
         */
        const ObservableList& observables() const
        { return m_observables; }

        /** 
         * @brief Return the list of all Observable. You can use this function
         * to manipulate the observable.
         * 
         * @return A reference to the list of Observable.
         */
        ObservableList& observables()
        { return m_observables; }

        inline bool operator<(const Measure& measure)
        { return m_name < measure.name(); }

        inline bool operator==(const std::string& name)
        { return m_name == name; }


    private:
        std::string     m_name;
        Type            m_type;
        double          m_timestep;
        std::string     m_output;
        ObservableList  m_observables;
    };

}} // namespace vle vpz

#endif
