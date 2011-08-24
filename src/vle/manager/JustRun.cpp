/*
 * @file vle/manager/JustRun.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/manager/JustRun.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/SharedLibraryManager.hpp>

namespace vle { namespace manager {

bool JustRunMono::operator()(const CmdArgs& args)
{
    m_lst.resize(args.size());

    m_out << fmt(_("JustRun: run %1% simples simulations in one thread\n")) %
        args.size();

    int i = 0;
    bool success = true;

    utils::SharedLibraryManager slm;
    utils::ModuleManager modulemgr;

    if (m_output) {
        for (CmdArgs::const_iterator it = args.begin(); it != args.end();
             ++it) {
            RunVerbose r(modulemgr, m_out);
            r.start(*it);
            m_names[*it] = i;
            m_lst[i] = r.outputs();
            success = (success and r.haveError()) ? false : success;
            ++i;
        }
    } else {
        for (CmdArgs::const_iterator it = args.begin();
             it != args.end(); ++it) {
            RunQuiet r(modulemgr);
            r.start(*it);
            m_names[*it] = i;
            m_lst[i] = r.outputs();
            success = (success and r.haveError()) ? false : success;
            ++i;
        }
    }

    return success;
}

bool JustRunThread::operator()(const CmdArgs& args)
{
    m_lst.resize(args.size());

    if (args.size() < m_process) {
        m_process = args.size();
    }

    m_out << fmt(_("JustRun: run %1% simples simulations in %2% threads\n")) %
        args.size() % m_process;

    m_args = args;

    Glib::Thread* prod(Glib::Thread::create(
        sigc::mem_fun(*this, &JustRunThread::read), true));

    for (unsigned int i = 0; i < m_process; ++i) {
        m_pool.push(sigc::mem_fun(*this, &JustRunThread::run));
    }

    prod->join();
    m_pool.stop_unused_threads();
    m_pool.shutdown();

    for (std::map < std::string, std::string >::const_iterator it =
         m_outputs.begin(); it != m_outputs.end(); ++it) {
        m_out << it->second;
    }

    m_out << std::endl;

    return m_success;
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

    utils::SharedLibraryManager slm;
    utils::ModuleManager modulemgr;

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
            RunVerbose r(modulemgr, ostr);
            r.start(file);
            m_success = (m_success and r.haveError()) ? false : m_success;
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
