/*
 * @file vle/eov/NetStreamReader.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_EOV_NETSTREAMREADER_HPP
#define VLE_EOV_NETSTREAMREADER_HPP 1

#include <vle/eov/DllDefines.hpp>
#include <vle/oov/NetStreamReader.hpp>
#include <vle/eov/Plugin.hpp>
#include <glibmm/thread.h>
#include <gtkmm/main.h>

namespace vle { namespace eov {

class Window;

class VLE_EOV_EXPORT NetStreamReader : public oov::NetStreamReader
{
public:
    NetStreamReader(int port, Window& main) :
        oov::NetStreamReader(port),
        m_main(main),
        m_finish(false)
    {}

    virtual ~NetStreamReader() {}

    inline Glib::Mutex& mutex()
    { return m_mutex; }

    /**
     * @brief Call the oov::NetStreamReader::process function. Close the
     * stream if the oov::NetStreamReader throws error.
     */
    void process();

    /**
     * @brief Return true if the NetStreamReader is closed by vle.
     * @return true if the connection is closed, false otherwise.
     */
    bool isFinish() const
    { return m_finish; }

    /**
     * @brief Return the error statement if the oov::NetStreamReader throws
     * error.
     * @return The statement error or an empty string.
     */
    const std::string& finishError() const
    { return m_finisherror; }

private:
    /**
     * @brief Call to initialise plug-in. Just before the Plugin
     * constructor. This function is used to initialise the Plugin with
     * parameter provided by the devs::StreamWritter class.
     */
    virtual void onParameter(const std::string& plugin,
                             const std::string& package,
                             const std::string& location,
                             const std::string& file,
                             value::Value* parameters,
                             const double& time);

    /**
     * @brief Call when a new observable (the devs::Simulator and port name)
     * is attached to a view.
     */
    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& portname,
                                 const std::string& view,
                                 const double& time);

    /**
     * @brief Call whe a observable (the devs::Simulator and port name) is
     * deleted from a view.
     */
    virtual void onDelObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& portname,
                                 const std::string& view,
                                 const double& time);

    /**
     * @brief Call when an observation event is send to the view.
     */
    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time,
                         value::Value* value);

    /**
     * @brief Call when the simulation is finished.
     */
    virtual void onClose(const double& time);

    void getGtkPlugin(const std::string& name);

    void getDefaultPlugin();

    void runWindow();

    Window&             m_main;
    PluginPtr           m_plugin;
    Glib::Mutex         m_mutex;
    std::string         m_newpluginname;
    bool		    m_finish;
    std::string         m_finisherror;
};

}} // namespace vle eov

#endif
