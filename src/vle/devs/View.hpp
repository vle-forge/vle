/**
 * @file vle/devs/View.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_DEVS_VIEW_HPP
#define VLE_DEVS_VIEW_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/Observable.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Tools.hpp>
#include <string>
#include <vector>

namespace vle { namespace devs {

    class StreamWriter;
    class View;

    typedef std::map < std::string, devs::View* > ViewList;

    /**
     * @brief Represent a View on a devs::Simulator and a port name.
     *
     */
    class VLE_DEVS_EXPORT View
    {
    public:
        View(const std::string& name, StreamWriter* stream);

        virtual ~View();

        ObservationEvent* addObservable(Simulator* model,
                                  const std::string& portName,
                                  const Time& currenttime);

        void finish(const Time& time);

        virtual bool isEvent() const
        { return false; }

        virtual bool isTimed() const
        { return false; }

        virtual bool isFinish() const
        { return false; }

        virtual devs::ObservationEvent* processObservationEvent(
            devs::ObservationEvent* event) = 0;

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
        // Get/Set functions
        //

        inline const std::string& getName() const
        { return m_name; }

        inline const ObservableList& getObservableList() const
        { return m_observableList; }

        inline unsigned int getSize() const
        { return m_size; }

        inline vle::devs::StreamWriter * getStream() const
        { return m_stream; }

        /**
         * @brief Get the current reference of the plugin.
         * @return A reference to the plugin.
         */
        oov::PluginPtr plugin() const;

        /**
         * @brief Ask to the StreamWriter a new reference of the plugin and
         * affect it to the local reference. If the plugin receive from the
         * StreamWriter is null, then, the previous plugin is not modified.
         * @return A reference ot the plugin.
         */
        oov::PluginPtr updatePlugin();

        struct GetOovPlugin
        {
            oov::PluginPtr operator()(const ViewList::value_type& x)
            { return x.second->plugin(); }
        };

    protected:
        ObservableList      m_observableList;
        std::string         m_name;
        StreamWriter*       m_stream;
        size_t              m_size;
        oov::PluginPtr      m_plugin;
    };

}} // namespace vle devs

#endif
