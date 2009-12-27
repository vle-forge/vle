/**
 * @file vle/manager/Run.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/manager/Run.hpp>
#include <glibmm/exception.h>

namespace vle { namespace manager {

void RunVerbose::operator()(vpz::Vpz* vpz)
{
    m_error = false;
    try {
        m_out << "[" << vpz->filename() << "]\n";

        m_out << _(" - Coordinator load models ......: ");
        m_root.load(*vpz);
        m_out << _("ok\n");

        m_out << _(" - Clean project file ...........: ");
        vpz->clear();
        delete vpz;
        m_out << _("ok\n");

        m_out << _(" - Coordinator initializing .....: ");
        m_root.init();
        m_out << _("ok\n");

        m_out << _(" - Simulation run................: ");
        while (m_root.run()) {}
        m_out << _("ok\n");

        m_out << _(" - Coordinator cleaning .........: ");
        m_root.finish();
        m_out << _("ok\n");
    } catch(const std::exception& e) {
        m_out << _("\n/!\\ vle error reported: ") <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_out << _("\n/!\\ vle Glib error reported: ") <<
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
            m_out << _(" - Open file.....................: ");
            vpz::Vpz vpz(filename);
            m_out << _("ok\n");

            m_out << _(" - Coordinator load models ......: ");
            m_root.load(vpz);
            m_out << _("ok\n");

            m_out << _(" - Clean project file ...........: ");
        }
        m_out << _("ok\n");

        m_out << _(" - Coordinator initializing .....: ");
        m_root.init();
        m_out << _("ok\n");

        m_out << _(" - Simulation run................: ");
        while (m_root.run()) {}
        m_out << _("ok\n");

        m_out << _(" - Coordinator cleaning .........: ");
        m_root.finish();
        m_out << _("ok\n");
    } catch(const std::exception& e) {
        m_out << _("\n/!\\ vle error reported: ") <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_out << _("\n/!\\ vle Glib error reported: ") <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    }
    m_out << std::endl;
}

void RunVerbose::operator()(const vpz::Vpz& vpz)
{
    m_error = false;
    try {
        {
            vpz::Vpz copy(vpz);
            m_out << "[" << copy.filename() << "]\n";

            m_out << _(" - Coordinator load models ......: ");
            m_root.load(copy);
            m_out << _("ok\n");

            m_out << _(" - Clean project file ...........: ");
        }
        m_out << _("ok\n");

        m_out << _(" - Coordinator initializing .....: ");
        m_root.init();
        m_out << _("ok\n");

        m_out << _(" - Simulation run................: ");
        while (m_root.run()) {}
        m_out << _("ok\n");

        m_out << _(" - Coordinator cleaning .........: ");
        m_root.finish();
        m_out << _("ok\n");
    } catch(const std::exception& e) {
        m_out << _("\n/!\\ vle error reported: ") <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_out << _("\n/!\\ vle Glib error reported: ") <<
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
        while (m_root.run()) {}
        m_root.finish();
    } catch(const std::exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle error reported: %1%\n")) % e.what()));
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle Glib error reported: %1%\n")) % e.what()));
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
        while (m_root.run()) {}
        m_root.finish();
    } catch(const std::exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle error reported: %1%\n")) % e.what()));
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle Glib error reported: %1%\n")) % e.what()));
        m_error = true;
    }
}

void RunQuiet::operator()(const vpz::Vpz& vpz)
{
    m_error = false;
    try {
        {
            vpz::Vpz copy(vpz);
            m_root.load(copy);
        }

        m_root.init();
        while (m_root.run()) {}
        m_root.finish();
    } catch(const std::exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle error reported: %1%\n")) % e.what()));
        m_error = true;
    } catch(const Glib::Exception& e) {
        m_stringerror.assign(boost::str(fmt(_(
                "/!\\ vle Glib error reported: %1%\n")) % e.what()));
        m_error = true;
    }
}

}} // namespace vle manager
