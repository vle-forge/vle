/**
 * @file devs/Stream.hpp
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
     * Base class of output stream of VLE. This class is the base class of
     * stream plugin declared with the function makeNewStream.
     *
     */
    class Stream
    {
    public:
        Stream();

        virtual ~Stream();

	///////////////////////////////////////////////////////////////////////

        /**
         * Initialise plugin with specified information.
         * @param outputPlugin the name of the instance of plugin loaded like,
         * text, sdml or net.
         * @param outputType type of output like, local, ip.
         * @param outputLocation where the plugin is launch.
         * @param parameters a string representation of parameters the XML
         * parameters.
         */
        virtual void init(const std::string& outputPlugin,
                          const std::string& outputType,
                          const std::string& outputLocation,
                          const std::string& parameters) =0;

        /**
         * Close the output stream.
         *
         */
        virtual void close() =0;

        /**
         * Get the current View.
         *
         * @return View attached or NULL if no attanchement.
         */
        inline devs::View* getView() const
        { return m_view; }

        /**
         * Set the current View.
         *
         * @param View the new View to attach.
         */
        inline void setView(devs::View* View)
        { m_view = View; }

	///////////////////////////////////////////////////////////////////////

        virtual void writeData() =0;

        virtual void writeHead(const std::vector < devs::Observable >&
			       variableNameList) =0;

        virtual void writeTail() =0;

        virtual void writeValue(const devs::Time& time,
                                value::Value value) =0;

	virtual void writeValues(const devs::Time& time,
				 const devs::StreamModelPortValue& valuelst,
				 const ObservableList& obslst) =0;
    private:
        devs::View*        m_view;
    };

}} // namespace vle devs

#define DECLARE_STREAM(x) \
extern "C" { vle::devs::Stream* makeNewStream(void) \
    { return new x(); } }

#endif
