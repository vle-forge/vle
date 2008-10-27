/**
 * @file vle/devs/Event.hpp
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
#include <vle/utils/Pool.hpp>
#include <string>

namespace vle { namespace devs {

    class Simulator;
    class Event;

    class Pools
    {
    public:
        /**
         * @brief Thread-Safe access to the Pools singleton.
         * @return A reference to the boost::pools.
         */
        static utils::Pools < Event >& pools();

        /**
         * @brief Initialize the singleton.
         */
        static void init()
        { pools(); }

        /**
         * @brief Kill the singleton.
         */
        static void kill()
        { delete m_pool; m_pool = 0; }

    private:
        /**
         * @brief The singleton object.
         */
        static Pools* m_pool;

        /**
         * @brief The set of boost::pool for Event class.
         */
        utils::Pools < Event > m_pools;

        ///
        ////
        ///

        /**
         * @brief Build a new utils::Pools with a default size defined to the
         * max size of the Event (InternalEvent, ExternalEvent,
         * RequestEvent or ObservationEvent).
         */
        Pools();

        /**
         * @brief Private copy constructor.
         * @param other
         */
        Pools(const Pools& other);

        /**
         * @brief Private assign operator.
         * @param other
         * @return
         */
        Pools& operator=(const Pools& /* other */) { return *this; }
    };

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
	 * @brief Generate a new Event with and model source name.
         * @param model model source name.
         */
        Event(Simulator* model) :
            m_source(model),
            m_valid(true),
            m_delete(false),
            m_attributes(0)
        {}

        ///
        ////
        ///

        /**
         * @brief Override the new operator to use the boost::pool allocator.
         * See the class Pools.
         * @param size The size of the object to allocate.
         * @return A pointer to the newly area.
         */
        inline static void* operator new(size_t size)
        { return Pools::pools().allocate(size); }

        /**
         * @brief Override the delete operator to use the boost::pool allocator.
         * See the class Pools.
         * @param deletable The pointer to delete.
         * @param size The size of the memory.
         */
        inline static void operator delete(void* deletable, size_t size)
        { Pools::pools().deallocate(deletable, size); }

        ///
        ////
        ///

        /**
	 * Copy constructor.
	 */
        Event(const Event& event) :
            m_source(event.m_source),
            m_valid(event.m_valid),
            m_delete(false),
            m_attributes(event.m_attributes)
        {}

        /**
	 * Destructor delete all Value.
	 */
        virtual ~Event();

	/**
	 * Get the source model for internal, state and external events.
	 * @return a reference to the model.
	 */
        inline Simulator* getModel()
        { return m_source; }

	/**
	 * Get the source model for internal, state and external events.
	 * @return a reference to the model.
	 */
        inline Simulator* getModel() const
        { return m_source; }

        /**
         * Assign a simulator to the event.
         * @param sim the simulator.
         */
        inline void setModel(Simulator* sim)
        { m_source = sim; }

        /**
	 * Get the name of source model.
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
         * @brief This function activate the deletion of all value::Value
         * attached.
         */
        inline void deleter()
        { m_delete = true; }

        /**
         * @brief True if the deletion of all value::Value attached is need.
         * @return true if deletion is activated, false otherwise.
         */
        inline bool needDelete() const
        { return m_delete; }

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

        void putAttributes(const value::Map& map);

        /**
         * Put an attribute on this Event.
         * @param name std::string name of Value to add.
         * @param value Value to add, not clone.
         */
        inline void putAttribute(const std::string& name, value::Value* value)
        { attributes().add(name, value); }

        /**
         * Put an attribute on an event. The goal is to simplify building event.
         * @code
         * ExternalEvent* evt = new ExternalEvent(
         *                              "free?", currenttime, getModel();
         * evt << attribute("x", 5) << attribute("y", 7.0)
         *     << attribute("msg", "hello world");
         * @endcode
         * @param event the event to put the attribute.
         * @param attr the attribute to put into event.
         * @return the current event.
         */
        inline friend Event* operator<<(Event* event, const Attribute& attr)
        { event->putAttribute(attr.first, attr.second); return event;}

        /**
         * Put an attribute on an event. The goal is to simplify building event.
         * @code
         * ExternalEvent evt("free?", currenttime, getModel();
         * evt << attribute("x", 5) << attribute("y", 7.0)
         *     << attribute("msg", "hello world");
         * @endcode
         * @param event the event to put the attribute.
         * @param attr the attribute to put into event.
         * @return the current event.
         */
        inline friend Event& operator<<(Event& event, const Attribute& attr)
        { event.putAttribute(attr.first, attr.second); return event;}

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Test if the map have a Value with specified name.
         * @param name the name of value to find.
         * @return true if Value exist, false otherwise.
         */
        inline bool existAttributeValue(const std::string& name) const
        { return attributes().existValue(name); }

        /**
         * Get an attribute from this Event.
         * @param name std::string name of Value to get.
         * @return a reference to Value.
         */
        inline const value::Value& getAttributeValue(
            const std::string& name) const
        { return attributes().get(name); }

        /**
         * Get a double attribute from this Event.
         * @param name std::string name of double to get.
         * @return a double.
         */
        inline double getDoubleAttributeValue(const std::string& name) const
        { return attributes().getDouble(name); }

        /**
         * Get an integer attribute from this Event.
         * @param name std::string name of integer to get.
         * @return an integer.
         */
        inline int getIntegerAttributeValue(const std::string& name) const
        { return attributes().getInt(name); }

        /**
         * Get an long attribute from this Event.
         * @param name std::string name of integer to get.
         * @return an integer.
         */
        inline long getLongAttributeValue(const std::string& name) const
        { return attributes().getLong(name); }

        /**
         * Get a boolean attribute from this Event.
         * @param name std::string name of boolean to get.
         * @return a boolean.
         */
        bool getBooleanAttributeValue(const std::string& name) const
        { return attributes().getBoolean(name); }

        /**
         * Get a string attribute from this Event.
         * @param name std::string name of string to get.
         * @return a string.
         */
        inline const std::string& getStringAttributeValue(
                    const std::string& name) const
        { return attributes().getString(name); }

        /**
         * @brief Get a Set attribute from this event.
         * @param name std::string name of Set to get.
         * @return a Set
         */
        inline const value::Set& getSetAttributeValue(
            const std::string& name) const
        { return attributes().getSet(name); }

        /**
         * @brief Get a Map attribute from this event.
         * @param name std::string name of Map to get.
         * @return a Map.
         */
        inline const value::Map& getMapAttributeValue(
            const std::string& name) const
        { return attributes().getMap(name); }

        /**
         * @brief Return the map attached to the event.
         * @return a reference to the attached map.
         */
        inline const value::Map& getAttributes() const
        { return attributes(); }

        /**
         * @brief Check if attributes is present in the attributes lists.
         * @return True if the attributes lists exists, false otherwise.
         */
        inline bool haveAttributes() const
        { return m_attributes; }

    private:
        Event() :
            m_source(0),
            m_valid(true),
            m_delete(false),
            m_attributes(0)
        {
            throw utils::InternalError();
        }

        value::Map& attributes()
        {
            if (m_attributes == 0) {
                m_attributes = value::Map::create();
            }
            return *m_attributes;
        }

        const value::Map& attributes() const
        {
            if (m_attributes == 0) {
                throw utils::ArgError("No attribute in this event");
            }
            return *m_attributes;
        }

        Simulator*      m_source;
        bool            m_valid;
        bool            m_delete;
        value::Map*     m_attributes;
    };

    inline utils::Pools < Event >& Pools::pools()
    {
        if (not m_pool) {
            m_pool = new Pools();
        }
        return m_pool->m_pools;
    }

    /**
     * @brief Initialize the Pools singleton.
     */
    void init();

    /**
     * @brief Kill the Pools singleton.
     */
    void finalize();

}} // namespace vle devs

#endif
