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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE utils_library_test
#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/vle.hpp>
#include <memory>

using namespace vle;

struct F
{
    vle::Init a;

    F()
        : a()
    {}

    ~F() noexcept = default;
};

BOOST_FIXTURE_TEST_CASE(download_dtd, F)
{
    vle::utils::DownloadManager dm;

    vle::utils::FSpath p(vle::utils::FSpath::temp_directory_path());
    p /= "vle-1.1.0.dtd";

    dm.start("www.vle-project.org/vle-1.0.0.dtd", p.string());
    dm.join();

    BOOST_CHECK(dm.isFinish());
    BOOST_CHECK(not dm.hasError());

    vle::utils::FSpath dowloaded(dm.filename());
    BOOST_CHECK(dowloaded.is_file());
}


BOOST_FIXTURE_TEST_CASE(download_package, F)
{
    vle::utils::DownloadManager dm;

    vle::utils::FSpath p(vle::utils::FSpath::temp_directory_path());
    p /= "packages.pkg";

    dm.start("http://www.vle-project.org/pub/1.1/packages.pkg", p.string());
    dm.join();

    BOOST_CHECK(dm.isFinish());
    BOOST_CHECK(not dm.hasError());

    vle::utils::FSpath d(dm.filename());
    BOOST_CHECK(d.is_file());
}

BOOST_FIXTURE_TEST_CASE(download_package_bis, F)
{
    vle::utils::DownloadManager dm;

    vle::utils::FSpath p(vle::utils::FSpath::temp_directory_path());
    p /= "packages.pkg";

    dm.start("http://www.vle-project.org/pub/1.1/packages.pkg", p.string());
    dm.join();

    BOOST_CHECK(dm.isFinish());
    BOOST_CHECK(not dm.hasError());

    vle::utils::FSpath d(dm.filename());
    BOOST_CHECK(d.is_file());
}
