/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/version.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Filesystem.hpp>
#include <ostream>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

namespace vle { namespace utils {

void Path::compress(const std::string& filepath,
                    const std::string& compressedfilepath)
{
    FSpath path(filepath);
    if (not path.exists())
        throw utils::InternalError(
            (fmt(_("fail to compress '%1%': file or directory does not exist"))
             % filepath).str());

    FSpath pwd = FSpath::current_path();
    std::string command;

    try {
        {
            utils::Preferences prefs(true);
            prefs.get("vle.command.tar", &command);
        }

        command = (vle::fmt(command) % compressedfilepath
                   % path.string()).str();

        std::vector<std::string> argv = Spawn::splitCommandLine(command);
        std::string exe = std::move(argv.front());
        argv.erase(argv.begin());

        utils::Spawn spawn;
        if (not spawn.start(exe, pwd.string(), argv, 50000u))
            throw utils::InternalError(_("fail to start cmake command"));

        std::string output, error;
        while (not spawn.isfinish()) {
            if (spawn.get(&output, &error)) {
                std::cout << output;
                std::cerr << error;

                output.clear();
                error.clear();

                std::this_thread::sleep_for(std::chrono::microseconds(200));
            } else
                break;
        }

        spawn.wait();

        std::string message;
        bool success;
        spawn.status(&message, &success);

        if (not message.empty())
            std::cerr << message << '\n';
    } catch (const std::exception& e) {
        TraceAlways(_("Compress: unable to compress '%s' "
                      "in '%s' with the '%s' command"),
                    compressedfilepath.c_str(),
                    pwd.string().c_str(),
                    command.c_str());
    }
}

void Path::decompress(const std::string& compressedfilepath,
                      const std::string& directorypath)
{
    FSpath path(directorypath);
    if (not path.is_directory())
        throw utils::InternalError(
            (fmt(_("fail to extract '%1%' in directory '%2%': "
                   " directory does not exist"))
             % compressedfilepath % directorypath).str());

    FSpath file(compressedfilepath);
    if (not file.is_file())
        throw utils::InternalError(
            (fmt(_("fail to extract '%1%' in directory '%2%': "
                   "file does not exist"))
             % compressedfilepath % directorypath).str());

    FSpath pwd = FSpath::current_path();

    std::string command;
    try {
        {
            utils::Preferences prefs(true);
            prefs.get("vle.command.untar", &command);
        }

        command = (vle::fmt(command) % file.string()).str();

        std::vector<std::string> argv = Spawn::splitCommandLine(command);
        std::string exe = std::move(argv.front());
        argv.erase(argv.begin());

        utils::Spawn spawn;
        if (not spawn.start(exe, path.string(), argv, 50000u))
            throw utils::InternalError(_("fail to start cmake command"));

        std::string output, error;
        while (not spawn.isfinish()) {
            if (spawn.get(&output, &error)) {
                std::cout << output;
                std::cerr << error;

                output.clear();
                error.clear();

                std::this_thread::sleep_for(std::chrono::microseconds(200));
            } else
                break;
        }

        spawn.wait();

        std::string message;
        bool success;
        spawn.status(&message, &success);

        if (not message.empty())
            std::cerr << message << '\n';
    } catch (const std::exception& e) {
        TraceAlways(_("Decompress: unable to decompress '%s' "
                      "in '%s' with the '%s' command"),
                    compressedfilepath.c_str(),
                    pwd.string().c_str(),
                    command.c_str());
    }
}

}}
