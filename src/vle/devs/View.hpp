/**
 * @file devs/View.hpp
 * @author The VLE Development Team.
 * @brief Represent an View on a devs::Simulator and a port name.
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

#ifndef VLE_DEVS_VIEW_HPP
#define VLE_DEVS_VIEW_HPP

#include <vle/devs/Observable.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Tools.hpp>
#include <string>
#include <vector>

namespace vle { namespace devs {

    class StreamWriter;

    /**
     * @brief Represent a View on a devs::Simulator and a port name.
     *
     */
    class View
    {
    public:
        View(const std::string& name, StreamWriter* stream);

        virtual ~View();

        StateEvent* addObservable(Simulator* model,
                                  const std::string& portName,
                                  const Time& currenttime);

        void finish(const Time& time);

        virtual bool isEvent() const
        { return false; }

        virtual bool isTimed() const
        { return false; }

        virtual bool isFinish() const
        { return false; }

        virtual devs::StateEvent* processStateEvent(
            devs::StateEvent* event) = 0;

	/**
	 * Delete an observable for a specified Simulator. If model does not
         * exist, nothing is produce otherwise, the stream receives a message
         * that a observable is dead.
	 * @param model delete observable attached to the specified
	 * Simulator.
	 */
	void removeObservable(Simulator* model);

        /** 
         * @brief Test if a simulator is already connected with the same port
         * to the View.
         * @param simulator the simulator to observe.
         * @param portname the port of the simulator to observe.
         * @return true if simulator is already connected with the same port.
         */
        bool exist(Simulator* simulator, const std::string& portname) const;

        /** 
         * @brief Test if a simulator is already connected to the View.
         * @param simulator the simulator to search.
         * @return true if simulator is already connected.
         */
        bool exist(Simulator* simulator) const;

        /** 
         * @brief Get the portnames of the Simulator.
         * @param simulator the simulator to search.
         * @return the list of portname of the value.
         * @throw utils::InternalError if simulator does not exist.
         */
        std::list < std::string > get(Simulator* simulator);

        //
        //
        // Get/Set functions
        //
        //

        inline const std::string& getName() const
        { return m_name; }

        inline const ObservableList& getObservableList() const
        { return m_observableList; }

        inline unsigned int getSize() const
        { return m_size; }

        inline vle::devs::StreamWriter * getStream() const
        { return m_stream; }

    protected:
        ObservableList      m_observableList;
        std::string         m_name;
        StreamWriter*       m_stream;
        size_t              m_size;
    };

}} // namespace vle devs

#endif
