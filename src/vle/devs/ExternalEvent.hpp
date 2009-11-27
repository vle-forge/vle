/**
 * @file vle/devs/ExternalEvent.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_DEVS_EXTERNALEVENT_HPP
#define VLE_DEVS_EXTERNALEVENT_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Event.hpp>
#include <string>

namespace vle { namespace devs {

    class Simulator;

    /**
     * @brief External event based on the devs::Event class and are build by
     * graph::Model when output function are called.
     *
     */
    class VLE_DEVS_EXPORT ExternalEvent : public Event
    {
    public:
        ExternalEvent(const std::string& sourcePortName) :
            Event(0),
            m_portName(sourcePortName),
            m_target(0)
        {}

        ExternalEvent(const std::string& sourcePortName,
                      Simulator* source) :
            Event(source),
            m_portName(sourcePortName),
            m_target(0)
        {}

	ExternalEvent(ExternalEvent* event,
		      Simulator* target,
                      const std::string& targetPortName,
                      bool needDeletion) :
	    Event(*event),
	    m_portName(targetPortName),
            m_target(target)
        {
            if (needDeletion) {
                deleter();
            }
        }

	virtual ~ExternalEvent()
        {}

	inline const std::string& getPortName() const
        { return m_portName; }

	inline Simulator* getTarget()
        { return m_target; }

        const std::string& getTargetModelName() const;

	inline bool onPort(const std::string& portName) const
        { return m_portName == portName; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        virtual bool isExternal() const
        { return true; }

        /**
         * @brief A new virtual function to specify if this event is an
         * InstantaneousEvent or single ExternalEvent.
         * @return true if this event is ExternalEvent, other return false.
         */
        virtual bool isRequest() const
        { return false; }

    protected:
	std::string   m_portName;
	Simulator*    m_target;
    };

    inline std::ostream& operator<<(std::ostream& o, const ExternalEvent& evt)
    {
        if (evt.getModel()) {
            o << "from: '" << evt.getSourceModelName();
        }

        return o << "' value: '"
            << (evt.haveAttributes() ? evt.getAttributes().writeToString() : "")
            << "' to port: '" << evt.getPortName()
            << "'";
    }

}} // namespace vle devs

#endif
