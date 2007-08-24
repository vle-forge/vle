/** 
 * @file LocalStreamWriter.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-21
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    m_reader.init(plugin, filename);
    m_reader.onParameter(vpz::ParameterTrame(utils::to_string(time),
                                             parameters, plugin, location));
}

void LocalStreamWriter::processNewObservable(Simulator* simulator,
                                             const std::string& portname,
                                             const devs::Time& time,
                                             const std::string& view)
{
    m_reader.onNewObservable(vpz::NewObservableTrame(
            utils::to_string(time),
            simulator->getStructure()->getName(),
            std::string(),
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
            simulator->getStructure()->getName(),
            std::string(),
            portname,
            view));
}

void LocalStreamWriter::process(const StateEvent& event)
{
    vpz::ValueTrame tr(utils::to_string(event.getTime()));
    tr.add(event.getModel()->getStructure()->getName(),
           std::string(),
           event.getPortName(),
           event.getViewName());
    tr.add(event.getAttributeValue(event.getPortName()));
    m_reader.onValue(tr);
}

void LocalStreamWriter::close()
{
    m_reader.onClose();
}

}} // namespace vle devs
