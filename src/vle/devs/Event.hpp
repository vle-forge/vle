/**
 * @file Event.hpp
 * @author The VLE Development Team.
 * @brief The Event base class. Define the base for internal, initialisation,
 * state and external events.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_EVENT_HPP
#define DEVS_EVENT_HPP

#include <vle/devs/Time.hpp>
#include <vle/devs/Attribute.hpp>
#include <vle/value/Map.hpp>
#include <string>
#include <iostream>

namespace vle { namespace devs {

    class sAtomicModel;

    /**
     * @brief The Event base class. Define the base for internal,
     * initialisation, state and external events.
     *
     */
    class Event
    {
    public:
        /** Define an enumeration to type event in conflict function.
         */
        enum EventType { INTERNAL, EXTERNAL, INSTANTANEOUS };
        
        /**
	 * Generate a new Event with and model source name.
	 *
         * @param time arrived time.
         * @param model model source name.
         */
        Event(const Time& time, sAtomicModel* model);

        /**
	 * Copy constructor.
	 */
        Event(const Event& event);

        /**
	 * Destructor delete all Value.
	 */
        virtual ~Event();

	/**
	 * Get the source model for internal, state and external events.
	 *
	 *
	 * @return a reference to the model.
	 */
	inline sAtomicModel* getModel() { return m_source; }

        /**
	 * Get the name of source model.
	 *
	 * @return model source name.
	 */
        const std::string& getSourceModelName() const;

        /**
	 * @return arrived time.
	 */
        inline const Time& getTime() const
        { return m_time; }

        /**
	 * Invalidate Event, don't use it.
	 */
        inline void invalidate()
        { m_valid = false; }

        /**
	 * @return true if Event is valid.
	 */
        inline bool isValid() const
        { return m_valid; }

        /**
         * @return true if Event is an External event.
	 */
        virtual bool isExternal() const = 0;

        /**
	 * @return true if Event is an Init event.
	 */
        virtual bool isInit() const = 0;

        /**
         * @return true if Event is an Internal event.
         */
        virtual bool isInternal() const = 0;

        /**
         * @return true if Event is an State event.
         */
        virtual bool isState() const = 0;

        /**
	 * Inferior comparator use Time as key.
         *
	 * @param event Event to test, no test on validity.
         * @return true if this Event is inferior to event.
         */
        inline virtual bool operator<(const Event * event) const
        { return m_time < event->m_time; }

        /**
         * Superior comparator use Time as key.
         *
         * @param event Event to test, no test on validity.
         *
         * @return true if this Event is superior to event.
         */
        inline virtual bool operator>(const Event * event) const
        { return m_time > event->m_time; }

        /**
         * Equality comparator use Time as key.
         *
         * @param event Event to test, no test on validity.
         *
         * @return true if this Event is equal to  event.
         */
        inline virtual bool operator==(const Event * event) const
        { return m_time == event->m_time; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        void putAttribute(vle::value::Map* map);

        /**
         * Put an attribute on this Event.
         *
         * @param name std::string name of Value to add.
         *
         * @param value Value to add.
         */
        inline void putAttribute(const std::string& name,
                                 vle::value::Value* value)
        { m_attributes.addValue(name, value); }

        /**
         * Put an attribute on an event. The goal is to simplify building event.
         * <h4>Example:</h4>
         * <table class="mdTable" border="0"><tr><td>
         * <pre>
         * ExternalEvent* evt = new ExternalEvent(
         *                              "free?", currenttime, getModel();
         * evt << attribute("x", 5) << attribute("y", 7.0)
         *     << attribute("msg", "hello world");
         * </pre>
         * </td></tr></table>
         *
         * @param event the event to put the attribute.
         * @param attr the attribute to put into event.
         *
         * @return the current event.
         */
        inline friend Event* operator<<(Event* event, const Attribute& attr)
        { event->putAttribute(attr.first, attr.second); return event;}
        
        /**
         * Put an attribute on an event. The goal is to simplify building event.
         * <h4>Example:</h4>
         * <table class="mdTable" border="0"><tr><td>
         * <pre>
         * ExternalEvent evt("free?", currenttime, getModel();
         * evt << attribute("x", 5) << attribute("y", 7.0)
         *     << attribute("msg", "hello world");
         * </pre>
         * </td></tr></table>
         *
         * @param event the event to put the attribute.
         * @param attr the attribute to put into event.
         *
         * @return the current event.
         */
        inline friend Event& operator<<(Event& event, const Attribute& attr)
        { event.putAttribute(attr.first, attr.second); return event;}
        
        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** 
         * @brief Test if the map have a Value with specified name.
         * 
         * @param name the name of value to find.
         * 
         * @return true if Value exist, false otherwise.
         */
        inline bool existAttributeValue(const std::string& name) const
        { return m_attributes.existValue(name); }

        /**
         * Get an attribute from this Event.
         *
         * @param name std::string name of Value to get.
         *
         * @return a reference to Value.
         */
        inline vle::value::Value* getAttributeValue(
            const std::string& name) const
        { return m_attributes.getValue(name); }

        /**
         * Get a const pointer attribute from this Event.
         *
         * @param name std::string name of const pointer to get.
         *
         * @return a const void *
         */
        const void* getConstPointerAttributeValue(
            const std::string& name) const;

        /**
         * Get a double attribute from this Event.
         *
         * @param name std::string name of double to get.
         *
         * @return a double.
         */
        double getDoubleAttributeValue(const std::string& name) const;

        /**
         * Get an integer attribute from this Event.
         *
         * @param name std::string name of integer to get.
         *
         * @return an integer.
         */
        long getIntegerAttributeValue(const std::string& name) const;

        /**
         * Get a boolean attribute from this Event.
         *
         * @param name std::string name of boolean to get.
         *
         * @return a boolean.
         */
        bool getBooleanAttributeValue(const std::string& name) const;

        /**
         * Get a string attribute from this Event.
         *
         * @param name std::string name of string to get.
         *
         * @return a string.
         */
        const std::string& getStringAttributeValue(
                    const std::string& name) const;

        /**
         * Return the map attached to the event.
         *
         * @return a reference to the attached map.
         */
        inline const vle::value::Map& getAttributes() const
        { return m_attributes; }

    private:
        Time            m_time;
        sAtomicModel*   m_source;
        bool            m_valid;
        vle::value::Map m_attributes;
    };

}} // namespace vle devs

#endif
