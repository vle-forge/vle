/** 
 * @file manager/Manager.cpp
 * @brief VLE in manager mode distribute the VPZ instances to VLE simulators.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:03:13 +0100
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

#include <iostream>
#include <cerrno>
#include <cstdio>
#include <glibmm/spawn.h>
#include <vle/manager/Manager.hpp>
#include <vle/manager/TotalExperimentGenerator.hpp>
#include <vle/manager/LinearExperimentGenerator.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>



namespace vle { namespace manager {

Manager::Manager(bool savevpz) :
    mSaveVPZ(savevpz)
{
    try {
        mHost.read_file();
    } catch(const std::exception& e) {
        std::cerr << "manager parsing host file error.\n";
    }
}

Manager::~Manager()
{
    close_connection_with_simulators();
}

void Manager::run_all_in_localhost(const std::string& filename)
{
    run_all_in_localhost(vpz::Vpz(filename));
}

void Manager::run_all_in_localhost(const vpz::Vpz& vpz)
{
    mFile = vpz;
    if (mFile.hasNoVLE()) {
	mFile.expandTranslator();
    }    


    ExperimentGenerator* expgen = get_combination_plan();
    expgen->saveVPZinstance(mSaveVPZ);

    expgen->build();

    std::list < std::string > lst = expgen->get_instances_files();
    std::list < std::string >::const_iterator it = lst.begin();

    while (lst.empty() == false) {
        std::cerr << " - Simulator start in another process\n";
        Glib::spawn_command_line_sync((boost::format("vle -v %1% %2%\n") %
                                       utils::Trace::trace().getLevel() %
                                       lst.front()).str());
        if (std::remove(lst.front().c_str()) == -1) {
            std::cerr << boost::format("Delete file %1% error: %2%\n") % 
                lst.front() % strerror(errno);
        }
        std::cerr << "\n";
        lst.pop_front();
    }
    delete expgen;
}

void Manager::run_localhost(const std::string& filename)
{
    run_localhost(vpz::Vpz(filename));
}

void Manager::run_localhost(const vpz::Vpz& file)
{
    mFile = file;
    if (mFile.hasNoVLE()) {
	mFile.expandTranslator();
    }

    scheduller();
}

void Manager::run_daemon(int port)
{
    utils::buildDaemon();

    utils::net::Server* server = 0;
    try {
        server = new utils::net::Server(port);
    } catch (const std::exception& e) {
        std::cerr << boost::format("Error opening server: %1%\n") % e.what();
        return;
    }

    for (;;) {
        try {
            server->accept_client("client");
            gint32 sz = server->recv_int("client");
            std::string file(server->recv_buffer("client", sz));
            mFile.parseFile(utils::write_to_temp("vleman", file));
	    if (mFile.hasNoVLE()) {
		mFile.expandTranslator();
	    }

            scheduller();

            server->close_client("client");
        } catch (const std::exception& e) {
            std::cerr << boost::format("Error with client: %1%\n") % e.what();
        }
    }
    delete server;
}

void Manager::scheduller()
{
    open_connection_with_simulators();
    Assert(utils::InternalError, not mClients.empty(),
           "Manager have no simulator connection.");

    const utils::Hosts::SetHosts hosts = mHost.hosts();

    ExperimentGenerator* expgen = get_combination_plan();
    expgen->saveVPZinstance(mSaveVPZ);
    expgen->build();

    std::list < std::string > lst = expgen->get_instances_files();
    std::list < std::string >::const_iterator it = lst.begin();

    std::cerr << "hosts number:" << hosts.size()
        << "\nfiles number: " << lst.size()
        << "\nclients: " << mClients.size() << "\n";

    utils::Hosts::SetHosts::const_iterator ithost = hosts.begin();
    std::list < utils::net::Client*>::iterator itclient = mClients.begin();
    ithost = hosts.begin();
    itclient = mClients.begin();
    while (not lst.empty()) {
        if (ithost != hosts.end()) {
            int nbfiletosend = (*ithost).process() -
                get_current_number_vpzi(*(*itclient));

            while (not lst.empty() and nbfiletosend > 0) {
                send_vpzi(*(*itclient), lst.front());
                lst.pop_front();
                --nbfiletosend;
            }
            ++ithost;
            ++itclient;
        } else {
            ithost = hosts.begin();
            itclient = mClients.begin();
        }
    }
    delete expgen;
    close_connection_with_simulators();
}

ExperimentGenerator* Manager::get_combination_plan() const
{
    ExperimentGenerator* result = 0;
    const vpz::Experiment& exp = mFile.project().experiment();
    if (exp.combination() == "linear") {
        result = new LinearExperimentGenerator(mFile);
    } else {
        result = new TotalExperimentGenerator(mFile);
    }
    return result;
}

void Manager::open_connection_with_simulators()
{
    const utils::Hosts::SetHosts hosts = mHost.hosts();
    utils::Hosts::SetHosts::const_iterator ithost = hosts.begin();

    while (ithost != hosts.end()) {
        utils::net::Client* client = 0;
        try {
            client = new utils::net::Client((*ithost).hostname(),
                                            (*ithost).port());
            mClients.push_back(client);
        } catch (const std::exception& e) {
            std::cerr << boost::format(
                "Error connection with the %1% simulator on port %2%: %3%\n") %
                (*ithost).hostname() % (*ithost).port() % e.what();
        }
        ++ithost;
    }
}

void Manager::close_connection_with_simulators()
{
    std::list < utils::net::Client* >::iterator it = mClients.begin();
    while (it != mClients.end()) {
        (*it)->send("exit", 4);
        delete (*it);
        ++it;
    }
    mClients.clear();
}

gint32 Manager::get_max_processor(utils::net::Client& cl)
{
    gint32 maxprocess = 0;
    try {
        cl.send("proc", 4);
        maxprocess = cl.recv_int();
    } catch (const std::exception& e) {
        std::cerr << boost::format(
            "Error get max processor: %1%\n") % e.what();
    }
    return maxprocess;
}

gint32 Manager::get_current_number_vpzi(utils::net::Client& cl)
{
    gint32 current = 0;
    try {
        cl.send("size", 4);
        current = cl.recv_int();
    } catch (const std::exception& e) {
        std::cerr << boost::format(
            "Error get current number of VPZi: %1%\n") % e.what();
    }
    return current;
}

void Manager::send_vpzi(utils::net::Client& cl, const Glib::ustring& filename)
{
    try {
        cl.send("file", 4);
        std::string file = Glib::file_get_contents(filename);
        cl.send_int(file.size());
        cl.send_buffer(file);
    } catch (const std::exception& e) {
        std::cerr << boost::format("Error sendind VPZi %1%: %2%\n") %
            filename.c_str() % e.what();
    }
}

}} // namespace vle manager
