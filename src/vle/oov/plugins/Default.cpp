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
}

Default::~Default()
{
    m_file.close();
}

void Default::onParameter(const std::string& plugin,
                         const std::string& location,
                         const std::string& file,
                         const std::string& parameters,
                         const double& time)
{
    m_filename = Glib::build_filename(location, file);
    m_file.open(m_filename.c_str());

    Assert(utils::ModellingError, m_file.is_open(),
           boost::format(
            "SimpleFile: cannot open file '%1%'") % m_filename);

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
        m_file << "value = " << value->writeToXml();
        delete value;
    }

    m_file << "\n";
}

void Default::close(const double& time)
{
    m_file << time << ": end" << "\n";
    m_file.close();
}

}}} // namespace vle oov plugin
