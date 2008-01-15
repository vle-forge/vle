/**
 * @file devs/StreamWriter.hpp
 * @author The VLE Development Team.
 * @brief Base class of net visualisation plugin.
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

#ifndef DEVS_STREAM_HPP
#define DEVS_STREAM_HPP

#include <vle/devs/View.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/value/Value.hpp>
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
    class StreamWriter
    {
    public:
        StreamWriter() :
            m_view(0)
        { }

        virtual ~StreamWriter()
        { }

        //
        ///
        /// Virtual function to develop stream writer plugins.
        ///
        //

        /**
         * @brief Initialise plugin with specified information.
         * @param plugin the name of the instance of plugin loaded like,
         * text, sdml or net.
         * @param location where the plugin write data.
         * @param file name of the file.
         * @param parameters a string representation of parameters the XML
         * parameters.
         */
        virtual void open(const std::string& plugin,
                          const std::string& location,
                          const std::string& file,
                          const std::string& parameters,
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
        virtual void process(const ObservationEvent& event) = 0;

        /**
         * Close the output stream.
         */
        virtual void close(const devs::Time& time) = 0;

        //
        ///
        /// Get/Set functions
        ///
        //

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
