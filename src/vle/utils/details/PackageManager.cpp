/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace utils {

static bool extract__(Packages *out, const std::string& filepath)
{
    Path pkg(filepath);
    PackageParser parser;

    if (pkg.is_file()) {
        parser.extract(pkg.string(), std::string());
        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(), parser.end());
        return true;
    }

    TraceAlways(_("Package: can not open file `%s'"),
                pkg.string().c_str());

    return false;
}

static bool rebuild__(ContextPtr ctx, Packages *out)
{
    Path pkgsdir(ctx->getBinaryPackagesDir());
    PackageParser parser;

    if (pkgsdir.is_directory()) {
        for (DirectoryIterator it(pkgsdir), end; it != end; ++it) {
            if (it->is_directory()) {
                Path descfile = *it;
                descfile /= "Description.txt";

                if (descfile.is_file()) {
                    parser.extract(descfile.string(), std::string());

                } else {
                    TraceAlways(
                        _("Remote: failed to open Description "
                          "from `%s'"), descfile.string().c_str());
                }
            }
        }

        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(),
                    parser.end());

        return true;
    } else {
        TraceAlways(_("Remote: failed to open directory `%s'"),
                     pkgsdir.string().c_str());

        return false;
    }
}

bool LocalPackageManager::extract(ContextPtr ctx, Packages *out)
{
    try {
        return extract__(out, ctx->getLocalPackageFilename().string());
    } catch (const std::exception& e) {
        TraceAlways(_("Remote: internal error when reading local package:"
                      " %s"), e.what());

        return false;
    }
}

bool LocalPackageManager::rebuild(ContextPtr ctx, Packages *out)
{
    try {
        return rebuild__(ctx, out);
    } catch (const std::exception& e) {
        TraceAlways(_("Remote: failed to rebuild cache of installed"
                      " package: %s"), e.what());

        return false;
    }
}

bool RemotePackageManager::extract(ContextPtr ctx, Packages *out)
{
    try {
        return extract__(out, ctx->getRemotePackageFilename().string());
    } catch (const std::exception& e) {
        TraceAlways(_("Remote: internal error when reading remote"
                      " package: %s"), e.what());

        return false;
    }
}

}}
