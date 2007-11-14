/** 
 * @file manager/Simulator.cpp
 * @brief VLE in simulator mode execute the VPZ instances.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:05:37 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#include <vle/manager/Simulator.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <sstream>
#include <iostream>
#include <glibmm/spawn.h>



namespace vle { namespace manager {

Simulator::Simulator(size_t cpu, int port) :
    mNbCPU(cpu),
    mPort(port),
    mPool(cpu, false)
{
    mServer = new utils::net::Server(port);
}

Simulator::~Simulator()
{
    delete mServer;
}

void Simulator::start()
{
    std::cerr << boost::format("Simulator CPU: %1% Port: %2%\n") % mNbCPU % mPort;

    Glib::Thread* mWait = Glib::Thread::create(
        sigc::mem_fun(*this, &Simulator::wait), true);

    for (size_t i = 0; i < mNbCPU; ++i)
        mPool.push(sigc::mem_fun(*this, &Simulator::run));

    mWait->join();
    mPool.shutdown();
}

void Simulator::wait()
{
    for (;;) {
        mServer->accept_client("manager");
	std::cerr << "Connection success.\n";

        std::string msg;

        for (;;) {
            msg = mServer->recv_buffer("manager", 4);

	    std::cerr << "msg: " << msg << "\n";

            if  (msg == "proc")
                daemon_send_max_processor();
            else if (msg == "size")
                daemon_send_number_vpzi();
            else if (msg == "file")
                daemon_recv_file();
            else if (msg == "exit") {
                daemon_exit();
                break;
            }
        }
    }
}

void Simulator::daemon_send_max_processor()
{
    mServer->send_int("manager", mNbCPU);
}

void Simulator::daemon_send_number_vpzi()
{
    int nb = 0;

    {
        Glib::Mutex::Lock lock(mMutex);
        nb = mFileNames.size();
        mCondWait.signal();
    }

    mServer->send_int("manager", nb);

    std::cerr << "daemon_send_number_vpzi\n";
}

void Simulator::daemon_recv_file()
{
    std::cerr << "daemon_recv_file\n";
    gint32 sz = mServer->recv_int("manager");
    std::cerr << sz << "\n";
    std::string msg = mServer->recv_buffer("manager", sz);
    std::cerr << " buffer read: " << msg.size() << "\n";
    std::string filename(utils::write_to_temp("vledae", msg));
    std::cerr << "filename: " << filename << "\n";

    {
        Glib::Mutex::Lock lock(mMutex);
        mFileNames.push(filename);
        mCondWait.signal();
    }

    std::cerr << "daemon_recv_file2\n";
}

void Simulator::daemon_exit()
{
    mServer->close_client("manager");
}

void Simulator::run()
{
    std::string filename;

    std::cerr << "running ... \n";

    for(;;) {
        {
	    std::cerr << "mutex lock.\n";

            Glib::Mutex::Lock lock (mMutex);

	    std::cerr << "mutex cond.\n";

            while(mFileNames.empty())
                mCondWait.wait(mMutex);

	    std::cerr << "mutex new.\n";

            filename = mFileNames.front();
            mFileNames.pop();

	    std::cerr << "mutex sign.\n";
            mCondRun.signal();
        }
	std::cerr << boost::format("Simulation start on %1%.\n") % filename;
        Glib::spawn_command_line_sync((boost::format("vle -v %1% -j %2%\n") %
                                       utils::Trace::trace().getLevel() %
                                       filename).str());
    }
}

bool Simulator::run(const std::string& filename)
{
    bool result = true;
    try {
        std::cerr << "[" << filename << "]\n";
        std::cerr << " - Opening project file .........: ";
        vpz::Vpz* file = new vpz::Vpz(filename);
        std::cerr << "ok\n";

        std::cerr << " - Project file has translator ..: ";
        if (file->hasNoVLE()) {
            std::cerr << "yes\n    = Build complete hierarchy ..: ";
            file->expandTranslator();
            std::cerr << "ok\n";
        } else {
            std::cerr << "no\n";
        }

        std::cerr << " - Coordinator building .........: ";
        devs::RootCoordinator coordinator;
        std::cerr << "ok\n";

        std::cerr << " - Coordinator load models ......: ";
        coordinator.load(*file);
        std::cerr << "ok\n";

        std::cerr << " - Clean project file ...........: ";
        delete file;
        std::cerr << "ok\n";

        std::cerr << " - Coordinator initializing .....: "; 
        coordinator.init();
        std::cerr << "ok\n";

        std::cerr << " - Simulation start .............: ";
        while (coordinator.run());
        std::cerr << "ok\n";

        std::cerr << " - Coordinator cleaning .........: ";
        coordinator.finish();
        std::cerr << "ok\n";

        if (utils::Trace::trace().haveWarning()) {
            std::cerr << boost::format(
                "\n/!\\ Some warnings during simulation: See file %1%\n") %
                utils::Trace::trace().getLogFile();
        }

    } catch(const std::exception& e) {
        std::cerr << "\n/!\\ vle error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        result = false;
    } catch(const Glib::Exception& e) {
        std::cerr << "\n/!\\ vle Glib error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
        result = false;
    }
    std::cerr << std::endl;
    return result;
}

bool Simulator::run(const vpz::Vpz& file)
{
    bool result = true;
    try {
        //std::cerr << "[" << file.filename() << "]\n";
        //std::cerr << " - Already open project file ....: ok\n";

        //std::cerr << " - Coordinator building .........: ";
        devs::RootCoordinator coordinator;
        //std::cerr << "ok\n";

        //std::cerr << " - Coordinator load models ......: ";
        coordinator.load(file);
        //std::cerr << "ok\n";

        //std::cerr << " - Coordinator initializing .....: "; 
        coordinator.init();
        //std::cerr << "ok\n";

        //std::cerr << " - Simulation start .............: ";
        while (coordinator.run());
        //std::cerr << "ok\n";

        //std::cerr << " - Coordinator cleaning .........: ";
        coordinator.finish();
        //std::cerr << "ok\n";

        //if (utils::Trace::trace().haveWarning()) {
        //std::cerr << boost::format(
        //"\n/!\\ Some warnings during simulation: See file %1%\n") %
        //utils::Trace::trace().getLogFile();
        //}

    } catch(const std::exception& /*e*/) {
        //std::cerr << "\n/!\\ vle error reported: " <<
        //utils::demangle(typeid(e)) << "\n" << e.what();
        result = false;
    } catch(const Glib::Exception& /*e*/) {
        //std::cerr << "\n/!\\ vle Glib error reported: " <<
        //utils::demangle(typeid(e)) << "\n" << e.what();
        result = false;
    }
    std::cerr << std::endl;
    return result;
}

}} // namespace vle manager
