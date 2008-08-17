/**
 * @file src/vle/utils/Path.cpp
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

#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include <list>

#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <config.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

std::string Path::getHomeDir() const
{
    std::list < std::string > arb;
    arb.push_back(Glib::get_home_dir());
    arb.push_back(".vle");
    return Glib::build_path(G_DIR_SEPARATOR_S, arb);
}

std::string Path::buildUserPath(const std::string& dir)
{
    if (dir.empty()) {
        std::list < std::string > arb;
        arb.push_back(Glib::get_home_dir());
        arb.push_back(".vle");
        return Glib::build_path(G_DIR_SEPARATOR_S, arb);
    } else {
        std::list < std::string > arb;
        arb.push_back(Glib::get_home_dir());
        arb.push_back(".vle");
        arb.push_back(dir);
        return Glib::build_path(G_DIR_SEPARATOR_S, arb);
    }
}

bool Path::initPath()
{
    m_prefix.assign(VLE_PREFIX_DIR);

    readEnv("VLE_SIMULATOR_PATH", m_simulator);
    addSimulatorDir(buildPrefixLibrariesPath(m_prefix, "simulator"));
    addSimulatorDir(buildUserPath("simulator"));
    addSimulatorDir("./simulator");
    addSimulatorDir(".");

    readEnv("VLE_OOV_PATH", m_stream);
    addStreamDir(buildPrefixLibrariesPath(m_prefix, "stream"));
    addStreamDir(buildUserPath("stream"));
    addStreamDir("./stream");
    addStreamDir(".");

    readEnv("VLE_MODEL_PATH", m_model);
    addModelDir(buildPrefixLibrariesPath(m_prefix, "model"));
    addModelDir(buildUserPath("model"));
    addModelDir("./model");
    addModelDir(".");

    return true;
}

}} // namespace vle utils
