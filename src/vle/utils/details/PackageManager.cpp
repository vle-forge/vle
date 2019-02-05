/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/RemoteManager.hpp>

#include "utils/ContextPrivate.hpp"
#include "utils/details/PackageManager.hpp"
#include "utils/details/PackageParser.hpp"
#include "utils/i18n.hpp"

namespace {

void
extract(vle::utils::ContextPtr ctx,
        vle::utils::Packages* out,
        const std::string& filepath)
{
    vle::utils::Path pkg(filepath);
    vle::utils::PackageParser parser;

    if (not pkg.is_file()) {
        ctx->error(_("Package: can not open file `%s'\n"),
                   pkg.string().c_str());
    } else {
        parser.extract(pkg.string(), std::string());
        out->reserve(parser.size());
        out->insert(out->rbegin().base(), parser.begin(), parser.end());
    }
}

void
rebuild(vle::utils::ContextPtr ctx, vle::utils::Packages* out)
{
    // We only use the user's binary package repository.
    auto pkgsdir = ctx->getBinaryPackagesDir()[0];
    vle::utils::PackageParser parser;

    if (not pkgsdir.is_directory()) {
        ctx->error(_("Remote: failed to open directory `%s'\n"),
                   pkgsdir.string().c_str());
        return;
    }

    for (vle::utils::DirectoryIterator it(pkgsdir), end; it != end; ++it) {
        if (it->is_directory()) {
            vle::utils::Path descfile = *it;
            descfile /= "Description.txt";

            try {
                if (descfile.is_file()) {
                    parser.extract(descfile.string(), std::string());
                } else {
                    ctx->error(_("Remote: failed to open Description.txt "
                                 "from `%s'\n"),
                               descfile.string().c_str());
                }
            } catch (const std::exception& e) {
                ctx->error(_("Remote: error reading file `%s': %s\n"),
                           descfile.string().c_str(),
                           e.what());
            }
        }
    }

    out->reserve(parser.size());
    out->insert(out->rbegin().base(), parser.begin(), parser.end());
}

} // anonymous namespace

namespace vle {
namespace utils {

bool
LocalPackageManager::extract(ContextPtr ctx, Packages* out)
{
    try {
        ::extract(ctx, out, ctx->getLocalPackageFilename().string());
    } catch (const std::exception& e) {
        ctx->error(_("Remote: internal error when reading local package:"
                     " %s\n"),
                   e.what());

        return false;
    }

    return true;
}

bool
LocalPackageManager::rebuild(ContextPtr ctx, Packages* out)
{
    try {
        ::rebuild(ctx, out);
    } catch (const std::exception& e) {
        ctx->error(_("Remote: failed to rebuild cache of installed"
                     " package: %s\n"),
                   e.what());

        return false;
    }

    return true;
}

bool
RemotePackageManager::extract(ContextPtr ctx, Packages* out)
{
    try {
        ::extract(ctx, out, ctx->getRemotePackageFilename().string());
    } catch (const std::exception& e) {
        ctx->error(_("Remote: internal error when reading remote"
                     " package: %s\n"),
                   e.what());

        return false;
    }

    return true;
}
}
} // namespace vle utils
