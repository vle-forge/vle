/**
 * @file vle/oov/plugins/Default.cpp
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


#include <vle/oov/plugins/Default.hpp>
#include <vle/utils/Debug.hpp>


namespace vle { namespace oov { namespace plugin {

Default::Default(const std::string& location) :
    Plugin(location)
{
    m_file.open(location.c_str());
    if (not m_file.is_open()) {
        Throw(utils::InternalError, (boost::format(
                    "Cannot open file '%1%'") % location));
    }
}

Default::~Default()
{
}

void Default::onParameter(const std::string& plugin,
                         const std::string& location,
                         const std::string& file,
                         const std::string& parameters,
                         const double& time)
{
    m_file << time << ": parameter "
        << " plugin: " << plugin
        << " location: " << location
        << " file: " << file
        << " parameters: " << parameters
        << "\n";
}

void Default::onNewObservable(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time)
{
    m_file << time << ": new observable "
        << " simulator: " << simulator
        << " parent: " << parent
        << " port: " << port
        << " view: " << view
        << "\n";
}

void Default::onDelObservable(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time)
{
    m_file << time << ": delete observable "
        << " simulator: " << simulator
        << " parent: " << parent
        << " port: " << port
        << " view: " << view
        << "\n";
}

void Default::onValue(const std::string& simulator,
                     const std::string& parent,
                     const std::string& port,
                     const std::string& view,
                     const double& time,
                     value::Value* value)
{
    m_file << time << ": value from "
        << " simulator: " << simulator
        << " parent: " << parent
        << " port: " << port
        << " view: " << view;

    if (value) {
        m_file << " = " << value;
        delete value;
    }

    m_file << "\n";
}

void Default::close(const double& time)
{
    m_file << time << ": end" << "\n";
    m_file.flush();
}

}}} // namespace vle oov plugin
