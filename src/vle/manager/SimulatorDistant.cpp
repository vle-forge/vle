/**
 * @file src/vle/manager/SimulatorDistant.cpp
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




#include <vle/manager/SimulatorDistant.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <sstream>
#include <iostream>
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
    delete mServer;
}

void SimulatorDistant::start()
{
    m_out << boost::format("Simulator: CPU=%1% Port=%2%\n") % mNbCPU % mPort;

    Glib::Thread* mWait = Glib::Thread::create(
        sigc::mem_fun(*this, &SimulatorDistant::wait), true);

    mWait->join();
    mPool.shutdown();
}

void SimulatorDistant::wait()
{
    for (;;) {
        mServer->accept_client("manager");
        m_out << boost::format("Simulator: connection sucess\n");
        std::string msg;

        for (;;) {
            msg = mServer->recv_buffer("manager", 4);

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
    mServer->send_int("manager", mNbCPU);
}

void SimulatorDistant::daemonSendNumberVpzi()
{
    int nb = 0;

    {
        Glib::Mutex::Lock lock(mMutex);
        nb = mFileNames.size();
        mCondWait.signal();
    }

    mServer->send_int("manager", nb);
}

void SimulatorDistant::daemonRecvFile()
{
    gint32 sz = mServer->recv_int("manager");
    mServer->send_string("manager", "ok");
    std::string msg = mServer->recv_buffer("manager", sz);
    std::string filename(utils::write_to_temp("vledae", msg));

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
        mServer->send_int("manager", mOutputs.size());

        for (OutputSimulationDistantList::iterator it = mOutputs.begin();
             it != mOutputs.end(); ++it) {

            std::string result = mServer->recv_string("manager");

            value::Set vals = value::SetFactory::create();
            vals->addValue(value::IntegerFactory::create(it->outputs.size()));
            vals->addValue(value::IntegerFactory::create(it->instance));
            vals->addValue(value::IntegerFactory::create(it->replica));

            result = vals->toXML();

            mServer->send_int("manager", result.size());
            mServer->recv_string("manager");
            mServer->send_string("manager", result);
            result = mServer->recv_string("manager");

            for (oov::OutputMatrixViewList::iterator jt = it->outputs.begin();
                 jt != it->outputs.end(); ++jt) {

                vals = value::SetFactory::create();
                vals->addValue(value::StringFactory::create(jt->first));
                vals->addValue(jt->second.serialize());

                result = vals->toXML();
                mServer->send_int("manager", result.size());
                mServer->recv_string("manager");
                mServer->send_string("manager", result);
                result = mServer->recv_string("manager");
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
    mServer->send_string("manager", "closed");
    mServer->close_client("manager");
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

    m_out << boost::format("Simulator: %1% %2% %3%\n") % filename
        % instance % replica;

    r.start(file);

    if (r.haveError()) {
        m_out << boost::format("/!\\ Error %1%\n") % ostr.str();
    }

    views = r.outputs();
    {
        Glib::Mutex::Lock lock (mMutex);
        mOutputs.push_back(OutputSimulationDistant(instance, replica, views));
    }
}

}} // namespace vle manager
