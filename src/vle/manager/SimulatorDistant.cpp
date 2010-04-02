/**
 * @file vle/manager/SimulatorDistant.cpp
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


#include <vle/manager/SimulatorDistant.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <sstream>
#include <glibmm/spawn.h>
#include <glibmm/timer.h>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>



namespace vle { namespace manager {

SimulatorDistant::SimulatorDistant(std::ostream& out, size_t cpu, int port) :
    m_out(out),
    mNbCPU(cpu),
    mPort(port),
    mPool(cpu, false)
{
    mServer = new utils::net::Server(port);
}

SimulatorDistant::~SimulatorDistant()
{
    mPool.shutdown();
    delete mServer;
}

void SimulatorDistant::start()
{
    m_out << fmt(_("Simulator: CPU=%1% Port=%2%\n")) % mNbCPU % mPort;

    Glib::Thread* mWait = Glib::Thread::create(
        sigc::mem_fun(*this, &SimulatorDistant::wait), true);

    mWait->join();
    mPool.stop_unused_threads();
}

void SimulatorDistant::wait()
{
    for (;;) {
        mServer->acceptClient("manager");
        m_out << fmt(_("Simulator: connection sucess\n"));
        std::string msg;

        for (;;) {
            msg = mServer->recvBuffer("manager", 4);

            if  (msg == "proc") {
                daemonSendMaxProcessor();
            } else if (msg == "size") {
                daemonSendNumberVpzi();
            } else if (msg == "file") {
                daemonRecvFile();
            } else if (msg == "fini") {
                sendResult();
            } else if (msg == "exit") {
                daemonExit();
                break;
            }
        }
    }
}

void SimulatorDistant::daemonSendMaxProcessor()
{
    mServer->sendInteger("manager", mNbCPU);
}

void SimulatorDistant::daemonSendNumberVpzi()
{
    int nb = 0;

    {
        Glib::Mutex::Lock lock(mMutex);
        nb = mFileNames.size();
        mCondWait.signal();
    }

    mServer->sendInteger("manager", nb);
}

void SimulatorDistant::daemonRecvFile()
{
    boost::int32_t sz = mServer->recvInteger("manager");
    mServer->sendString("manager", "ok");
    std::string msg = mServer->recvBuffer("manager", sz);
    std::string filename(utils::Path::writeToTemp("vledae", msg));

    {
        Glib::Mutex::Lock lock(mMutex);
        mFileNames.push(filename);
        mPool.push(sigc::mem_fun(*this, &SimulatorDistant::run));
        mCondWait.signal();
    }
}

void SimulatorDistant::sendResult()
{
    {
        Glib::Mutex::Lock lock(mMutex);
        mServer->sendInteger("manager", mOutputs.size());

        for (OutputSimulationDistantList::iterator it = mOutputs.begin();
             it != mOutputs.end(); ++it) {

            std::string result = mServer->recvString("manager");

            value::Set* vals = value::Set::create();
            vals->add(value::Integer::create(it->outputs.size()));
            vals->add(value::Integer::create(it->instance));
            vals->add(value::Integer::create(it->replica));

            result = vals->writeToXml();

            mServer->sendInteger("manager", result.size());
            mServer->recvString("manager");
            mServer->sendString("manager", result);
            result = mServer->recvString("manager");

            for (oov::OutputMatrixViewList::iterator jt = it->outputs.begin();
                 jt != it->outputs.end(); ++jt) {

                vals = value::Set::create();
                vals->add(value::String::create(jt->first));
                vals->add(jt->second.serialize());

                result = vals->writeToXml();
                mServer->sendInteger("manager", result.size());
                mServer->recvString("manager");
                mServer->sendString("manager", result);
                result = mServer->recvString("manager");
            }
        }
        mOutputs.clear();
    }
}

void SimulatorDistant::daemonExit()
{
    for (;;) {
        {
            Glib::Mutex::Lock lock(mMutex);
            if (mFileNames.empty() and
                mPool.get_num_threads() == 0) {
                break;
            }
        }
        Glib::usleep(250000); // wait a 1/4 second to try if
                              // the simulations are finished.
    }
    sendResult();
    mServer->sendString("manager", "closed");
    mServer->closeClient("manager");
}

void SimulatorDistant::run()
{
    std::string filename;
    int instance;
    int replica;
    oov::OutputMatrixViewList views;

    {
        Glib::Mutex::Lock lock (mMutex);
        filename = mFileNames.front();
        mFileNames.pop();
        mCondRun.signal();
    }

    std::ostringstream ostr;
    RunVerbose r(ostr);
    vpz::Vpz* file = new vpz::Vpz(filename);
    instance = file->project().instance();
    replica = file->project().replica();

    m_out << fmt(_("Simulator: %1% %2% %3%\n")) % filename
        % instance % replica;

    r.start(file);

    if (r.haveError()) {
        m_out << fmt(_("/!\\ Error %1%\n")) % ostr.str();
    }

    views = r.outputs();
    {
        Glib::Mutex::Lock lock (mMutex);
        mOutputs.push_back(OutputSimulationDistant(instance, replica, views));
    }
}

}} // namespace vle manager
