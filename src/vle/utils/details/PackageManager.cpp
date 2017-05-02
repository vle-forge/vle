/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/details/PackageManager.hpp>
#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/i18n.hpp>

namespace {

bool
extract(vle::utils::ContextPtr ctx,
        vle::utils::Packages* out,
        const std::string& filepath) noexcept
{
    vle::utils::Path pkg(filepath);
    vle::utils::PackageParser parser;

    if (pkg.is_file()) {
        parser.extract(pkg.string(), std::string());
        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(), parser.end());
        return true;
    }

    vErr(ctx, _("Package: can not open file `%s'\n"), pkg.string().c_str());

    return false;
}

bool
rebuild(vle::utils::ContextPtr ctx, vle::utils::Packages* out) noexcept
{
    // We only use the user's binary package repository.
    auto pkgsdir = ctx->getBinaryPackagesDir()[0];
    vle::utils::PackageParser parser;

    if (not pkgsdir.is_directory()) {
        vErr(ctx,
             _("Remote: failed to open directory `%s'\n"),
             pkgsdir.string().c_str());
        return false;
    }

    for (vle::utils::DirectoryIterator it(pkgsdir), end; it != end; ++it) {
        if (it->is_directory()) {
            vle::utils::Path descfile = *it;
            descfile /= "Description.txt";

            if (descfile.is_file()) {
                parser.extract(descfile.string(), std::string());
            } else {
                vErr(ctx,
                     _("Remote: failed to open Description "
                       "from `%s'\n"),
                     descfile.string().c_str());
            }
        }
    }

    out->reserve(parser.size());
    out->insert(out->rbegin().base(), parser.begin(), parser.end());

    return true;
}

} // anonymous namespace

namespace vle {
namespace utils {

bool
LocalPackageManager::extract(ContextPtr ctx, Packages* out)
{
    try {
        return ::extract(ctx, out, ctx->getLocalPackageFilename().string());
    } catch (const std::exception& e) {
        vErr(ctx,
             _("Remote: internal error when reading local package:"
               " %s\n"),
             e.what());

        return false;
    }
}

bool
LocalPackageManager::rebuild(ContextPtr ctx, Packages* out)
{
    try {
        return ::rebuild(ctx, out);
    } catch (const std::exception& e) {
        vErr(ctx,
             _("Remote: failed to rebuild cache of installed"
               " package: %s\n"),
             e.what());

        return false;
    }
}

bool
RemotePackageManager::extract(ContextPtr ctx, Packages* out)
{
    try {
        return ::extract(ctx, out, ctx->getRemotePackageFilename().string());
    } catch (const std::exception& e) {
        vErr(ctx,
             _("Remote: internal error when reading remote"
               " package: %s\n"),
             e.what());

        return false;
    }
}
}
} // namespace vle utils
