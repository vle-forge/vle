/**
 * @file vle/devs/StreamWriter.hpp
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


#ifndef DEVS_STREAM_HPP
#define DEVS_STREAM_HPP

#include <vle/devs/DllDefines.hpp>
#include <vle/devs/View.hpp>
#include <vle/devs/Time.hpp>
#include <vle/value/Value.hpp>
#include <vle/oov/Plugin.hpp>
#include <map>
#include <vector>

namespace vle { namespace devs {

    class View;
    class Observable;

    /**
     * Base class of the Stream Writer of the VLE DEVS simulator. This class is
     * the base of the MemoryStreamWriter and NetStreamWriter deployed as
     * plugins.
     *
     */
    class VLE_DEVS_EXPORT StreamWriter
    {
    public:
        StreamWriter() :
            m_view(0)
        {}

        virtual ~StreamWriter()
        {}

        ///
        ////
        ///

        /**
         * @brief Initialise plugin with specified information.
         * @param plugin the plugin's name.
         * @param package the plugin's package.
         * @param location where the plugin write data.
         * @param file name of the file.
         * @param parameters the value attached to the plug-in.
         * @param time the date when the plug-in was opened.
         */
        virtual void open(const std::string& plugin,
                          const std::string& package,
                          const std::string& location,
                          const std::string& file,
                          value::Value* parameters,
                          const devs::Time& time) = 0;

        virtual void processNewObservable(Simulator* simulator,
                                          const std::string& portname,
                                          const devs::Time& time,
                                          const std::string& view) = 0;

        virtual void processRemoveObservable(Simulator* simulator,
                                             const std::string& portname,
                                             const devs::Time& time,
                                             const std::string& view) = 0;

        /**
         * @brief Process the devs::ObservationEvent and write it to the Stream.
         * @param event the devs::ObservationEvent to write.
         */
        virtual void process(ObservationEvent& event) = 0;

        /**
         * Close the output stream.
         * @return A reference to the oov::Plugin if the plugin is serializable.
         */
        virtual oov::PluginPtr close(const devs::Time& time) = 0;

        /**
         * @brief Refresh the output to get a oov::PluginPtr during the
         * simulation.
         * @return A reference to the oov::Plugin if the plugin is serializable.
         */
        virtual oov::PluginPtr refreshPlugin() = 0;

        ///
        ////
        ///

        /**
         * @brief Get the current View.
         * @return View attached or NULL if no attanchement.
         */
        inline devs::View* getView() const
        { return m_view; }

        /**
         * @brief Set the current View.
         * @param View the new View to attach.
         */
        inline void setView(devs::View* View)
        { m_view = View; }

    private:
        devs::View*        m_view;
    };

}} // namespace vle devs

#define DECLARE_STREAMWRITER(x) \
extern "C" { vle::devs::StreamWriter* makeNewStreamWriter(void) \
    { return new x(); } }

#endif
