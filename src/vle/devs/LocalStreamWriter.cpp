/**
 * @file vle/devs/LocalStreamWriter.cpp
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


#include <vle/devs/LocalStreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/ValueTrame.hpp>



namespace vle { namespace devs {

LocalStreamWriter::LocalStreamWriter()
{
}

LocalStreamWriter::~LocalStreamWriter()
{
}

void LocalStreamWriter::open(const std::string& plugin,
                             const std::string& location,
                             const std::string& file,
                             const std::string& parameters,
                             const devs::Time& time)
{
    std::string filename(Glib::build_filename(location, file));
    m_reader.onParameter(vpz::ParameterTrame(utils::to_string(time),
                                             parameters, plugin, filename));
}

void LocalStreamWriter::processNewObservable(Simulator* simulator,
                                             const std::string& portname,
                                             const devs::Time& time,
                                             const std::string& view)
{
    m_reader.onNewObservable(vpz::NewObservableTrame(
            utils::to_string(time),
            simulator->getName(),
            simulator->getParent(),
            portname,
            view));
}

void LocalStreamWriter::processRemoveObservable(Simulator* simulator,
                                                const std::string& portname,
                                                const devs::Time& time,
                                                const std::string& view)
{
    m_reader.onDelObservable(vpz::DelObservableTrame(
            utils::to_string(time),
            simulator->getName(),
            simulator->getParent(),
            portname,
            view));
}

void LocalStreamWriter::process(const ObservationEvent& event)
{
    vpz::ValueTrame tr(utils::to_string(event.getTime()));
    tr.add(event.getModel()->getName(),
           event.getModel()->getParent(),
           event.getPortName(),
           event.getViewName());
    tr.add(&event.getAttributeValue(event.getPortName()));
    m_reader.onValue(tr);
}

oov::PluginPtr LocalStreamWriter::close(const devs::Time& time)
{
    m_reader.onClose(vpz::EndTrame(utils::to_string(time)));
    return m_reader.plugin();
}

oov::PluginPtr LocalStreamWriter::refreshPlugin()
{
    return m_reader.plugin();
}

}} // namespace vle devs
