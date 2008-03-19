/**
 * @file src/vle/manager/JustRun.cpp
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




#include <vle/manager/JustRun.hpp>
#include <vle/manager/Run.hpp>

namespace vle { namespace manager {

void JustRunMono::operator()(const CmdArgs& args)
{
    m_lst.resize(args.size());

    m_out << boost::format(
        "JustRun: run %1% simples simulations in one thread\n") % args.size();

    if (m_output) {
        int i = 0;
        for (CmdArgs::const_iterator it = args.begin(); it != args.end(); ++it) {
            RunVerbose r(m_out);
            r.start(*it);
            m_names[*it] = i;
            m_lst[i] = r.outputs();
            ++i;
        }
    } else {
        int i = 0;
        for (CmdArgs::const_iterator it = args.begin(); it != args.end(); ++it) {
            RunQuiet r;
            r.start(*it);
            m_names[*it] = i;
            m_lst[i] = r.outputs();
            ++i;
        }
    }

    if (utils::Trace::trace().haveWarning()) {
        m_out << boost::format(
            "\n/!\\ Some warnings during simulation: See file %1%\n") %
            utils::Trace::trace().getLogFile();
    }
}

void JustRunThread::operator()(const CmdArgs& args)
{
    m_lst.resize(args.size());

    if (args.size() < m_process) {
        m_process = args.size();
    }

    m_out << boost::format(
        "JustRun: run %1% simples simulations in %2% threads\n") % args.size() %
        m_process;

    m_args = args;
    
    Glib::Thread* prod(Glib::Thread::create(
        sigc::mem_fun(*this, &JustRunThread::read), true));

    for (unsigned int i = 0; i < m_process; ++i) {
        m_pool.push(sigc::mem_fun(*this, &JustRunThread::run));
    }

    prod->join();
    m_pool.shutdown();

    for (std::map < std::string, std::string >::const_iterator it =
         m_outputs.begin(); it != m_outputs.end(); ++it) {
        m_out << it->second;
    }

    if (utils::Trace::trace().haveWarning()) {
        m_out << boost::format(
            "\n/!\\ Some warnings during simulation: See file %1%\n") %
            utils::Trace::trace().getLogFile();
    }

    m_out << std::endl;
}

void JustRunThread::read()
{
    int i = 0;
    for (CmdArgs::const_iterator it = m_args.begin(); it != m_args.end();
         ++it) {
        vpz::Vpz* file = new vpz::Vpz(*it);
        file->project().setInstance(i);
        {
            Glib::Mutex::Lock lock(m_mutex);
            m_vpzs.push_back(file);
            m_prodcond.signal();
        }
        ++i;
    }

    {
        Glib::Mutex::Lock lock(m_mutex);
        m_finish = true;
        m_prodcond.signal();
    }
}

void JustRunThread::run()
{
    vpz::Vpz* file;
    std::string filename;
    std::ostringstream ostr;
    int instance = 0;
    oov::OutputMatrixViewList views;

    for (;;) {
        file = 0;
        {
            Glib::Mutex::Lock lock(m_mutex);

            while (m_vpzs.empty() and not m_finish) {
                m_prodcond.wait(m_mutex);
            }

            if (not m_vpzs.empty()) {
                file  = m_vpzs.front();
                m_vpzs.pop_front();
            }
        }

        if (file) {
            filename.assign(file->filename());
            ostr.str("");
            instance = file->project().instance();
            RunVerbose r(ostr);
            r.start(file);
            views = r.outputs();
        }

        {
            Glib::Mutex::Lock lock(m_mutex);
            if (file) {
                m_outputs[filename] = ostr.str();
                m_names[filename] = instance;
                m_lst[instance] = views;
            }
            if (m_finish and m_vpzs.empty()) {
                break;
            }
        }
    }
}

}} // namespace vle manager
