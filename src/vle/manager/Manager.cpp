/**
 * @file vle/manager/Manager.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <cerrno>
#include <cstdio>
#include <glibmm/spawn.h>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Run.hpp>
#include <vle/manager/TotalExperimentGenerator.hpp>
#include <vle/manager/LinearExperimentGenerator.hpp>
#include <vle/oov/PluginFactory.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>

namespace vle { namespace manager {

ManagerRun::~ManagerRun()
{
    delete m_exp;
}

ExperimentGenerator* ManagerRun::getCombinationPlan(const vpz::Vpz& file,
                                                    std::ostream&   out)
{
    const vpz::Experiment& exp = file.project().experiment();

    if (exp.combination() == "linear") {
        return new LinearExperimentGenerator(file, out, m_storecomb,
                                             m_commonseed, m_rand);
    } else {
        return new TotalExperimentGenerator(file, out, m_storecomb,
                                            m_commonseed, m_rand);
    }
}

void ManagerRunMono::operator()(const vpz::Vpz& file)
{
    m_out << _("Manager: run experimental frames in one thread\n");

    initRandomGenerator(file);
    m_exp = getCombinationPlan(file, m_out);
    m_exp->saveVPZinstance(m_writefile);
    m_exp->build(&m_matrix);

    std::ostringstream ostr;

    while (not m_exp->vpzInstances().empty()) {
        vpz::Vpz* file = m_exp->vpzInstances().front();
        int instance = file->project().instance();
        int replica = file->project().replica();
        RunVerbose r(ostr);
        r.start(file);

        if (r.haveError()) {
            m_out << ostr.str() << std::endl;
        }

        m_matrix[replica][instance] = r.outputs();
        m_exp->vpzInstances().pop_front();
    }
}

void ManagerRunMono::operator()(const std::string& filename)
{
    m_filename.assign(filename);
    vpz::Vpz file(filename);
    operator()(file);
}

void ManagerRunThread::operator()(const vpz::Vpz& file)
{
    m_out << fmt(_("Manager: run experimental frames in %1% threads\n"))
        % m_process;

    if (m_finish) {
        throw utils::InternalError(
            _("ManagerRunThread cannot be started more than one time."));
    }

    initRandomGenerator(file);
    m_exp = getCombinationPlan(file, m_out);
    m_exp->saveVPZinstance(m_writefile);

    Glib::Thread* prod(Glib::Thread::create(
            sigc::mem_fun(*this, &ManagerRunThread::read), true));

    for (unsigned int i = 0; i < m_process; ++i) {
        m_pool.push(sigc::mem_fun(*this, &ManagerRunThread::run));
    }

    prod->join();
    m_pool.stop_unused_threads();
    m_pool.shutdown();

    if (utils::Trace::trace().haveWarning()) {
        m_out << fmt(_(
                "\n/!\\ Some warnings during simulation: See file %1%\n"))
            % utils::Trace::trace().getLogFile();
    }

    m_out << std::endl;
}

void ManagerRunThread::operator()(const std::string& filename)
{
    m_filename.assign(filename);
    vpz::Vpz file(filename);
    operator()(file);
}

void ManagerRunThread::read()
{
    m_exp->saveVPZinstance(m_writefile);
    m_exp->build(&m_mutex, &m_prodcond, &m_matrix);

    {
        Glib::Mutex::Lock lock(m_mutex);
        m_finish = true;
        m_prodcond.broadcast();
    }
}

void ManagerRunThread::run()
{
    vpz::Vpz* file;
    std::string filename;
    std::ostringstream ostr;
    int instance = 0;
    int replica = 0;
    oov::OutputMatrixViewList views;

    for (;;) {
        file = 0;
        {
            Glib::Mutex::Lock lock(m_mutex);

            while (m_exp->vpzInstances().empty() and not m_finish) {
                m_prodcond.wait(m_mutex);
            }
            if (not m_exp->vpzInstances().empty()) {
                file = m_exp->vpzInstances().front();
                m_exp->vpzInstances().pop_front();
            }
        }

        if (file) {
            filename.assign(file->filename());
            ostr.str("");
            instance = file->project().instance();
            replica = file->project().replica();
            RunVerbose r(ostr);
            r.start(file);

            if (r.haveError()) {
                m_out << ostr.str() << std::endl;
            }

            views = r.outputs();
        }

        {
            Glib::Mutex::Lock lock(m_mutex);
            if (file) {
                m_matrix[replica][instance] = views;
            }
            if (m_finish and m_exp->vpzInstances().empty()) {
                break;
            }
        }
    }
}

ManagerRunDistant::ManagerRunDistant(std::ostream& out, bool writefile,
                                     bool storecomb, bool commonseed, RandPtr rnd) :
    ManagerRun(out, writefile, storecomb, commonseed, rnd)
{
    try {
        mHost.read();
    }
    catch (const std::exception& e) {
        m_out << fmt(_("manager parsing host file error: %1%\n")) % e.what();
    }
}

ManagerRunDistant::ManagerRunDistant(std::ostream& out, bool writefile,
                                     RandPtr rnd) :
    ManagerRun(out, writefile, false, true, rnd)
{
    try {
        mHost.read();
    }
    catch (const std::exception& e) {
        m_out << fmt(_("manager parsing host file error: %1%\n")) % e.what();
    }
}

vpz::Vpz* ManagerRunDistant::getVpz()
{
    vpz::Vpz* file;

    for (;;) {
        file = 0;
        {
            Glib::Mutex::Lock lock(m_mutex);

            while (m_exp->vpzInstances().empty() and not m_finish) {
                m_prodcond.wait(m_mutex);
            }
            if (not m_exp->vpzInstances().empty()) {
                file = m_exp->vpzInstances().front();
                m_exp->vpzInstances().pop_front();
                return file;
            }
        }

        if (m_finish and m_exp->vpzInstances().empty()) {
            break;
        }
    }
    return 0;
}

unsigned int ManagerRunDistant::getVpzNumber()
{
    {
        Glib::Mutex::Lock lock(m_mutex);

        return m_exp->vpzInstances().size();
    }
}

void ManagerRunDistant::operator()(const std::string& filename)
{
    m_filename.assign(filename);
    vpz::Vpz file(filename);
    operator()(file);
}

void ManagerRunDistant::operator()(const vpz::Vpz& file)
{
    m_out << fmt(_("Manager: run experimental frames in distant\n"));
    initRandomGenerator(file);

    m_exp = getCombinationPlan(file, m_out);

    Glib::Thread* prod(Glib::Thread::create(
            sigc::mem_fun(*this, &ManagerRunDistant::read), true));

    Glib::Thread* cond(Glib::Thread::create(
            sigc::mem_fun(*this, &ManagerRunDistant::send), true));

    prod->join();
    cond->join();

    if (utils::Trace::trace().haveWarning()) {
        m_out << fmt(_(
                "\n/!\\ Some warnings during simulation: See file %1%\n"))
            % utils::Trace::trace().getLogFile();
    }
}

void ManagerRunDistant::read()
{
    m_exp->saveVPZinstance(m_writefile);
    m_exp->build(&m_mutex, &m_prodcond, &m_matrix);

    {
        Glib::Mutex::Lock lock(m_mutex);
        m_finish = true;
        m_prodcond.broadcast();
    }
}

void ManagerRunDistant::send()
{
    openConnectionWithSimulators();

    if (mClients.empty()) {
        throw utils::InternalError(
            _("Manager have no simulator connection."));
    }

    const utils::Hosts::SetHosts hosts = mHost.hosts();

    utils::Hosts::SetHosts::const_iterator ithost = hosts.begin();
    std::list < utils::net::Client* >::iterator itclient = mClients.begin();
    ithost = hosts.begin();
    itclient = mClients.begin();

    m_out << fmt(_("Manager: use %1% distant simulator\n")) % mClients.size();

    while (getVpzNumber() > 0) {
        if (ithost != hosts.end()) {
            int nbfiletosend = (*ithost).process() - getCurrentNumberVpzi(
                *(*itclient));

            getResult(*(*itclient));

            while (getVpzNumber() > 0 and nbfiletosend > 0) {
                vpz::Vpz* file(getVpz());
                sendVpzi(*(*itclient), file->writeToString());
                delete file;
                --nbfiletosend;
            }
            ++ithost;
            ++itclient;
        } else {
            ithost = hosts.begin();
            itclient = mClients.begin();
        }
    }

    closeConnectionWithSimulators();
}

void ManagerRunDistant::openConnectionWithSimulators()
{
    const utils::Hosts::SetHosts hosts = mHost.hosts();
    utils::Hosts::SetHosts::const_iterator ithost = hosts.begin();

    m_out << fmt(_(
            "Manager: try to open connection with %1% simulators\n"))
        % mHost.hosts().size();

    while (ithost != hosts.end()) {
        utils::net::Client* client = 0;
        try {
            m_out << fmt(_("Manager: try to open %1% on %2%\n"))
                % (*ithost).hostname() % (*ithost).port();

            client = new utils::net::Client((*ithost).hostname(),
                                            (*ithost).port());
            mClients.push_back(client);
        }
        catch (const std::exception& e) {
            m_out
                << fmt(_("Manager: Can not connect %1% simulator on port %2%: %3%\n"))
                % (*ithost).hostname() % (*ithost).port()
                % e.what();
        }
        ++ithost;
    }
}

void ManagerRunDistant::closeConnectionWithSimulators()
{
    std::list < utils::net::Client* >::iterator it = mClients.begin();
    while (it != mClients.end()) {
        (*it)->sendString("exit");
        getResult(*(*it));
        (*it)->recvString();
        delete (*it);
        ++it;
    }
    mClients.clear();
}

gint32 ManagerRunDistant::getMaxProcessor(utils::net::Client& cl)
{
    gint32 maxprocess = 0;

    try {
        cl.sendString("proc");
        maxprocess = cl.recvInteger();
    }
    catch (const std::exception& e) {
        m_out << fmt(_("Manager: error get max processor: %1%\n")) % e.what();
    }
    return maxprocess;
}

gint32 ManagerRunDistant::getCurrentNumberVpzi(utils::net::Client& cl)
{
    gint32 current = 0;

    try {
        cl.sendString("size");
        current = cl.recvInteger();
    }
    catch (const std::exception& e) {
        m_out << fmt(_("Manager: error get number vpz: %1%\n")) % e.what();
    }
    return current;
}

void ManagerRunDistant::getResult(utils::net::Client& cl)
{
    try {
        cl.sendString("fini");
        int nb = cl.recvInteger();

        for (int i = 0; i < nb; ++i) {
            cl.sendString("ok");
            int sz = cl.recvInteger();
            cl.sendString("ok");
            std::string result = cl.recvBuffer(sz);

            value::Set* vals = value::toSetValue(vpz::Vpz::parseValue(result));
            int nbview = value::toInteger(vals->get(0));
            int instance = value::toInteger(vals->get(1));
            int replica = value::toInteger(vals->get(2));
            cl.sendString("ok");

            for (int j = 0; j < nbview; ++j) {
                int sz = cl.recvInteger();
                cl.sendString("ok");
                result = cl.recvBuffer(sz);

                vals = value::toSetValue(vpz::Vpz::parseValue(result));
                cl.sendString("ok");

                std::string view = value::toString(vals->get(0));

                oov::OutputMatrix m;

                if (vals->get(1)) {
                    m.deserialize(*vals->get(1));
                }

                {
                    Glib::Mutex::Lock lock(m_mutex);
                    oov::OutputMatrixViewList& l(m_matrix[replica][instance]);
                    l[view] = m;
                }
            }
        }
    }
    catch (const std::exception& e) {
        m_out << fmt(_("Manager: error get result: %1%")) % e.what();
    }
}

void ManagerRunDistant::sendVpzi(utils::net::Client&  cl,
                                 const Glib::ustring& file)
{
    try {
        cl.sendString("file");
        cl.sendInteger(file.size());
        std::string tmp = cl.recvString();
        cl.sendBuffer(file);
    }
    catch (const std::exception& e) {
        m_out << fmt(_("Manager: error send vpz %1%: %2%\n")) % file % e.what();
    }
}

void ManagerRun::initRandomGenerator(const vpz::Vpz& file)
{
    if (m_rand.get() == 0) {
        m_out << _("Use the seed from vpz::Vpz replicas tags\n");
        m_rand = boost::shared_ptr < utils::Rand >(new utils::Rand());

        if (file.project().experiment().replicas().number() <= 0) {
            throw utils::ArgError(_("The replicas's tag does not defined"));
        }

        m_rand->seed(file.project().experiment().replicas().seed());
    }
}

}}  // namespace vle manager
