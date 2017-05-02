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

#include <boost/version.hpp>
#include <memory>
#include <vle/utils/Context.hpp>
#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vle.hpp>

using namespace vle;

void
download_dtd()
{
    auto ctx = vle::utils::make_context();
    vle::utils::DownloadManager dm(ctx);

    vle::utils::Path p(vle::utils::Path::temp_directory_path());
    p /= "vle-1.1.0.dtd";

    dm.start("www.vle-project.org/vle-1.0.0.dtd", p.string());
    dm.join();

    Ensures(dm.isFinish());
    Ensures(not dm.hasError());

    vle::utils::Path dowloaded(dm.filename());
    Ensures(dowloaded.is_file());
}

void
download_package()
{
    auto ctx = vle::utils::make_context();
    vle::utils::DownloadManager dm(ctx);

    vle::utils::Path p(vle::utils::Path::temp_directory_path());
    p /= "packages.pkg";

    dm.start("http://www.vle-project.org/pub/1.1/packages.pkg", p.string());
    dm.join();

    Ensures(dm.isFinish());
    Ensures(not dm.hasError());

    vle::utils::Path d(dm.filename());
    Ensures(d.is_file());
}

void
download_package_bis()
{
    auto ctx = vle::utils::make_context();
    vle::utils::DownloadManager dm(ctx);

    vle::utils::Path p(vle::utils::Path::temp_directory_path());
    p /= "packages.pkg";

    dm.start("http://www.vle-project.org/pub/1.1/packages.pkg", p.string());
    dm.join();

    Ensures(dm.isFinish());
    Ensures(not dm.hasError());

    vle::utils::Path d(dm.filename());
    Ensures(d.is_file());
}

int
main()
{
    vle::Init app;

    download_dtd();
    download_package();
    download_package_bis();

    return unit_test::report_errors();
}
