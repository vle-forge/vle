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

#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Exception.hpp>
#include <ostream>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

namespace vle { namespace utils {

void Path::compress(const std::string& filepath,
                    const std::string& compressedfilepath)
{
    boost::filesystem::path path(filepath);
    if (not boost::filesystem::exists(path))
        throw utils::InternalError(
            (fmt(_("fail to compress '%1%': file or directory does not exist"))
             % filepath).str());

    vle::utils::Spawn spawn;

    std::vector<std::string> args {
        "-E", "tar", "jcf", compressedfilepath, path.string() };

    boost::filesystem::path pwd = boost::filesystem::current_path();

    std::string exe =
#ifdef _WIN32
        utils::Path::findProgram("cmake.exe");
#else
        utils::Path::findProgram("cmake");
#endif

    if (not spawn.start(exe, pwd.string(), args, 50000u))
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
}

void Path::decompress(const std::string& compressedfilepath,
                      const std::string& directorypath)
{
    boost::filesystem::path path(directorypath);
    if (not boost::filesystem::exists(path)
        or not boost::filesystem::is_directory(path))
        throw utils::InternalError(
            (fmt(_("fail to extract '%1%' in directory '%2%': "
                   " directory does not exist"))
             % compressedfilepath % directorypath).str());

    boost::filesystem::path file(compressedfilepath);
    if (not boost::filesystem::exists(path)
        or not boost::filesystem::is_regular(file))
        throw utils::InternalError(
            (fmt(_("fail to extract '%1%' in directory '%2%': "
                  "file does not exist"))
             % compressedfilepath % directorypath).str());

    vle::utils::Spawn spawn;

    std::vector<std::string> args {
        "-E", "tar", "jxf", file.string(), "." };

    std::string exe =
#ifdef _WIN32
        utils::Path::findProgram("cmake.exe");
#else
        utils::Path::findProgram("cmake");
#endif

    if (not spawn.start(exe, path.string(), args, 50000u))
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
}

}}
