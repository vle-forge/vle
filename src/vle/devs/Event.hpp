/**
 * @file src/vle/devs/Event.hpp
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




#ifndef DEVS_EVENT_HPP
#define DEVS_EVENT_HPP

#include <vle/devs/Time.hpp>
#include <vle/devs/Attribute.hpp>
#include <vle/value/Map.hpp>
#include <string>
#include <iostream>

namespace vle { namespace devs {

    class Simulator;

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
        enum EventType { INTERNAL, EXTERNAL };
        
        /**
	 * Generate a new Event with and model source name.
	 *
         * @param model model source name.
         */
        Event(Simulator* model);

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
	 * @return a reference to the model.
	 */
        inline Simulator* getModel()
        { return m_source; }

	/**
	 * Get the source model for internal, state and external events.
	 *
	 * @return a reference to the model.
	 */
        inline Simulator* getModel() const
        { return m_source; }

        /**
         * Assign a simulator to the event.
         *
         * @param sim the simulator.
         */
        inline void setModel(Simulator* sim)
        { m_source = sim; }

        /**
	 * Get the name of source model.
	 *
	 * @return model source name.
	 */
        const std::string getSourceModelName() const;

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
        virtual bool isExternal() const
        { return false; }

        /**
         * @return true if Event is an Internal event.
         */
        virtual bool isInternal() const
        { return false; }

        /**
         * @return true if Event is an State event.
         */
        virtual bool isObservation() const
        { return false; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        void putAttribute(vle::value::Map map);

        /**
         * Put an attribute on this Event.
         *
         * @param name std::string name of Value to add.
         * @param value Value to add, not clone.
         */
        inline void putAttribute(const std::string& name, value::Value value)
        { m_attributes->addValue(name, value); }

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
        { return m_attributes->existValue(name); }

        /**
         * Get an attribute from this Event.
         *
         * @param name std::string name of Value to get.
         *
         * @return a reference to Value.
         */
        inline vle::value::Value getAttributeValue(
            const std::string& name) const
        { return m_attributes->getValue(name); }

        /**
         * Get a double attribute from this Event.
         *
         * @param name std::string name of double to get.
         *
         * @return a double.
         */
        inline double getDoubleAttributeValue(const std::string& name) const
        { return m_attributes->getDoubleValue(name); }

        /**
         * Get an integer attribute from this Event.
         *
         * @param name std::string name of integer to get.
         *
         * @return an integer.
         */
        inline int getIntegerAttributeValue(const std::string& name) const
        { return m_attributes->getIntValue(name); }

        /**
         * Get an long attribute from this Event.
         *
         * @param name std::string name of integer to get.
         *
         * @return an integer.
         */
        inline int getLongAttributeValue(const std::string& name) const
        { return m_attributes->getLongValue(name); }

        /**
         * Get a boolean attribute from this Event.
         *
         * @param name std::string name of boolean to get.
         *
         * @return a boolean.
         */
        bool getBooleanAttributeValue(const std::string& name) const
        { return m_attributes->getBooleanValue(name); }

        /**
         * Get a string attribute from this Event.
         *
         * @param name std::string name of string to get.
         *
         * @return a string.
         */
        inline const std::string& getStringAttributeValue(
                    const std::string& name) const
        { return m_attributes->getStringValue(name); }

        /** 
         * @brief Get a Set attribute from this event.
         * 
         * @param name std::string name of Set to get.
         *
         * @return a Set
         */
        inline value::Set getSetAttributeValue(const std::string& name) const
        { return m_attributes->getSetValue(name); }

        /** 
         * @brief Get a Map attribute from this event.
         * 
         * @param name std::string name of Map to get.
         * 
         * @return a Map.
         */
        inline value::Map getMapAttributeValue(const std::string& name) const
        { return m_attributes->getMapValue(name); }

        /**
         * Return the map attached to the event.
         *
         * @return a reference to the attached map.
         */
        inline const vle::value::Map& getAttributes() const
        { return m_attributes; }

    private:
        Event();

        Simulator*      m_source;
        bool            m_valid;
        value::Map      m_attributes;
    };

}} // namespace vle devs

#endif
