/*
 * @file vle/manager/Run.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/manager/Run.hpp>
#include <glibmm/exception.h>
#include <boost/progress.hpp>
#include <cmath>

namespace vle { namespace manager {

void RunVerbose::operator()(vpz::Vpz* vpz)
{
    m_error = false;
    boost::timer timer;
    try {
        const double duration = vpz->project().experiment().duration();
        const double begin = vpz->project().experiment().begin();

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
        boost::progress_display display(100, m_out, "\n   ", "   ", "   ");
        long previous = 0;
        while (m_root.run()) {
            long pc = std::floor(100. * (m_root.getCurrentTime() - begin) /
                                 duration);

            display += pc - previous;
            previous = pc;
        }
        display += 100 - previous;

        m_out << _(" - Coordinator cleaning .........: ");
        m_root.finish();
        m_out << _("ok\n");
        m_out << _(" - Time spent in kernel .........: ") << timer.elapsed()
            << "s\n";
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
    boost::timer timer;
    try {
        double duration, begin;

        m_out << "[" << filename << "]\n";
        {
            m_out << _(" - Open file.....................: ");
            vpz::Vpz vpz(filename);
            m_out << _("ok\n");

            m_out << _(" - Coordinator load models ......: ");
            m_root.load(vpz);
            m_out << _("ok\n");

            m_out << _(" - Clean project file ...........: ");
            duration = vpz.project().experiment().duration();
            begin = vpz.project().experiment().begin();
        }
        m_out << _("ok\n");

        m_out << _(" - Coordinator initializing .....: ");
        m_root.init();
        m_out << _("ok\n");

        m_out << _(" - Simulation run................: ");
        boost::progress_display display(100, m_out, "\n   ", "   ", "   ");
        long previous = 0;
        while (m_root.run()) {
            long pc = std::floor(100. * (m_root.getCurrentTime() - begin) /
                                 duration);

            display += pc - previous;
            previous = pc;
        }
        display += 100 - previous;

        m_out << _(" - Coordinator cleaning .........: ");
        m_root.finish();
        m_out << _("ok\n");
        m_out << _(" - Time spent in kernel .........: ") << timer.elapsed()
            << "s\n";
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
