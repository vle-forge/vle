/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/devs/StreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/oov/CairoPlugin.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/version.hpp>

namespace vle { namespace devs {

oov::PluginPtr StreamWriter::plugin()
{
    if (not m_plugin) {
        throw utils::InternalError(
            _("Oov: The output plug-in is not initialized, "
              "may be the StreamWriter::open() function was nether called."));
    }

    return m_plugin;
}

void StreamWriter::open(const std::string& pluginname,
                        const std::string& package,
                        const std::string& location,
                        const std::string& file,
                        value::Value* parameters,
                        const devs::Time& time)
{
    void *symbol = 0;

    try {
        symbol = m_modulemgr.get(package, pluginname, utils::MODULE_OOV);
        oov::OovPluginSlot fct(utils::functionCast < oov::OovPluginSlot>(symbol));
        oov::PluginPtr ptr(fct(location));
        m_plugin = ptr;
    } catch(const std::exception& e) {
        throw utils::InternalError(
            fmt(_("Oov: Can not open the plug-in `%1%': %2%")) % pluginname %
            e.what());
    }

    /*
     * For cairo plug-ins, we build the cairo graphics context via the
     * CairoPlugin::init function.
     */
    if (plugin()->isCairo()) {
        oov::CairoPluginPtr plg = oov::toCairoPlugin(plugin());
        plg->init();
    }

    plugin()->onParameter(pluginname, location, file, parameters, time);
}

void StreamWriter::processNewObservable(Simulator* simulator,
                                        const std::string& portname,
                                        const devs::Time& time,
                                        const std::string& view)
{
    plugin()->onNewObservable(simulator->getName(),
                              simulator->getParent(),
                              portname, view, time);
}

void StreamWriter::processRemoveObservable(Simulator* simulator,
                                           const std::string& portname,
                                           const devs::Time& time,
                                           const std::string& view)
{
    plugin()->onDelObservable(simulator->getName(),
                              simulator->getParent(),
                              portname, view, time);
}

void StreamWriter::process(Simulator* simulator,
                           const std::string& portname,
                           const devs::Time& time,
                           const std::string& view,
                           value::Value* val)
{
    std::string name, parent;

    if (simulator) {
        name = simulator->getName();
        parent = simulator->getParent();
    }

    if (plugin()->isCairo()) {
        oov::CairoPluginPtr plg = oov::toCairoPlugin(plugin());
        plg->needCopy();
        plg->onValue(name, parent, portname, view, time, val);

        if (plg->isCopyDone()) {
            std::string file(
                utils::Path::buildFilename(
                    plg->location(), (fmt("img-%1$08d.png") %
                                      plg->getNextFrameNumber()).str()));

            try {
                plg->stored()->write_to_png(file);
            } catch(const std::exception& /*e*/) {
                throw utils::InternalError(
                    fmt(_("oov: cannot write image '%1%'")) % file);
            }
        }
    } else {
        plugin()->onValue(name, parent, portname, view, time, val);

    }
}

void StreamWriter::close(const devs::Time& time)
{
    plugin()->close(time);
}

value::Matrix * StreamWriter::matrix() const
{
    if (m_plugin) {
        return m_plugin->matrix();
    }

    return NULL;
}

}} // namespace vle devs
