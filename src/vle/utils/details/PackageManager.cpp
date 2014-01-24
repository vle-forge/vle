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

#include <vle/utils/details/PackageManager.hpp>
#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/filesystem.hpp>

namespace vle { namespace utils {

static bool extract__(Packages *out, const std::string& filepath)
{
    namespace fs = boost::filesystem;

    fs::path pkg(filepath);
    PackageParser parser;

    if (fs::exists(pkg) and fs::is_regular_file(pkg)) {
        parser.extract(pkg.string(), std::string());
        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(), parser.end());

        return true;
    } else {
        TraceAlways(fmt(_("can not open file `%1%'")) % pkg.string());
    }

    return false;
}

static bool rebuild__(Packages *out)
{
    namespace fs = boost::filesystem;

    fs::path pkgsdir(utils::Path::path().getBinaryPackagesDir());
    PackageParser parser;

    if (fs::exists(pkgsdir) and fs::is_directory(pkgsdir)) {
        for (fs::directory_iterator it(pkgsdir), end; it != end; ++it) {
            if (fs::is_directory(it->status())) {
                fs::path descfile = *it;
                descfile /= "Description.txt";

                if (fs::exists(descfile)) {
                    parser.extract(descfile.string(), std::string());
                } else {
                    TraceAlways(
                        fmt(_("Remote manager: failed to open Description from"
                              " `%1%'")) % descfile.string());
                }
            }
        }

        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(),
                    parser.end());

        return true;
    } else {
        TraceAlways(fmt(_("failed to open directory `%1%'")) %
                    pkgsdir.string());

        return false;
    }
}

bool LocalPackageManager::extract(Packages *out)
{
    try {
        return extract__(out, RemoteManager::getLocalPackageFilename());
    } catch (const std::exception& e) {
        TraceAlways(
            fmt(_("Remote manager: internal error when reading local package:"
                  " %1%")) % e.what());

        return false;
    }
}

bool LocalPackageManager::rebuild(Packages *out)
{
    try {
        return rebuild__(out);
    } catch (const std::exception& e) {
        TraceAlways(
            fmt(_("Remote manager: failed to rebuild cache of installed"
                  " package: %1%")) % e.what());

        return false;
    }
}

bool RemotePackageManager::extract(Packages *out)
{
    try {
        return extract__(out, RemoteManager::getRemotePackageFilename());
    } catch (const std::exception& e) {
        TraceAlways(
            fmt(_("Remote manager: internal error when reading remote"
                  " package: %1%")) % e.what());

        return false;
    }
}

}}
