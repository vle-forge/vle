/** 
 * @file Run.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2008-01-16
 */

/*
 * Copyright (C) 2008 - The vle Development Team
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

#include <vle/manager/Run.hpp>

namespace vle { namespace manager {

void RunVerbose::operator()(vpz::Vpz* vpz)
{
    m_error = false;
    try {
        m_out << "[" << vpz->filename() << "]\n";

        m_out << " - Coordinator load models ......: ";
        m_root.load(*vpz);
        m_out << "ok\n";

        m_out << " - Clean project file ...........: ";
        vpz->clear();
        delete vpz;
        m_out << "ok\n";

        m_out << " - Coordinator initializing .....: "; 
        m_root.init();
        m_out << "ok\n";

        m_out << " - Simulation run................: ";
        while (m_root.run());
        m_out << "ok\n";

        m_out << " - Coordinator cleaning .........: ";
        m_root.finish();
        m_out << "ok\n";
    } catch(const std::exception& e) {
        m_out << "\n/!\\ vle error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_out << "\n/!\\ vle Glib error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    }
    m_out << std::endl;
}

void RunVerbose::operator()(const std::string& filename)
{
    m_error = false;
    try {
        m_out << "[" << filename << "]\n";
        {
            m_out << " - Open file.....................: ";
            vpz::Vpz vpz(filename);
            m_out << "ok\n";

            m_out << " - Coordinator load models ......: ";
            m_root.load(vpz);
            m_out << "ok\n";

            m_out << " - Clean project file ...........: ";
        }
        m_out << "ok\n";

        m_out << " - Coordinator initializing .....: "; 
        m_root.init();
        m_out << "ok\n";

        m_out << " - Simulation run................: ";
        while (m_root.run());
        m_out << "ok\n";

        m_out << " - Coordinator cleaning .........: ";
        m_root.finish();
        m_out << "ok\n";
    } catch(const std::exception& e) {
        m_out << "\n/!\\ vle error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_out << "\n/!\\ vle Glib error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    }
    m_out << std::endl;
}

void RunQuiet::operator()(vpz::Vpz* vpz)
{
    m_error = false;
    try {
        m_root.load(*vpz);
        vpz->clear();
        delete vpz;

        m_root.init();
        while (m_root.run());
        m_root.finish();
    } catch(const std::exception& e) {
        m_stringerror.assign(boost::str(boost::format(
                "/!\\ vle error reported: %1%\n") % e.what()));
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_stringerror.assign(boost::str(boost::format(
                "/!\\ vle Glib error reported: %1%\n") % e.what()));
        m_error = true;
    }
}

void RunQuiet::operator()(const std::string& filename)
{
    m_error = false;
    try {
        {
            vpz::Vpz vpz(filename);
            m_root.load(vpz);
        }

        m_root.init();
        while (m_root.run());
        m_root.finish();
    } catch(const std::exception& e) {
        m_stringerror.assign(boost::str(boost::format(
                "/!\\ vle error reported: %1%\n") % e.what()));
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_stringerror.assign(boost::str(boost::format(
                "/!\\ vle Glib error reported: %1%\n") % e.what()));
        m_error = true;
    }
}

}} // namespace vle manager
