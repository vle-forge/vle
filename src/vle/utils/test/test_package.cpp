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
#define BOOST_TEST_MODULE utils_library_test
#include <boost/test/included/unit_test.hpp>
#include <boost/version.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>
#include <numeric>
#include <memory>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/vle.hpp>

using namespace vle;

struct F
{
    std::unique_ptr<vle::Init> a;
    vle::utils::FSpath current_path;

    F()
    {
        current_path = vle::utils::FSpath::temp_directory_path();
        current_path /= vle::utils::FSpath::unique_path("vle-%%%%-%%%%-%%%%");
        current_path.create_directory();

        /* We need to ensure each file really installed. */
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (current_path.is_directory()) {
#ifdef _WIN32
            ::_putenv((vle::fmt("VLE_HOME=%1%")
                       % current_path.string()).str().c_str());
#else
            ::setenv("VLE_HOME", current_path.string().c_str(), 1);
#endif
        }

        vle::utils::FSpath::current_path(current_path);

        std::cout << "test start in " << current_path.string() << '\n';

        a = std::unique_ptr<vle::Init>(new vle::Init());
        vle::utils::Preferences prefs(false, "vle.conf");
    }

    ~F()
    {
        std::cout << "test finish in " << current_path.string() << '\n';
    }
};

BOOST_GLOBAL_FIXTURE(F);

BOOST_AUTO_TEST_CASE(show_package)
{
    using vle::utils::Path;
    using vle::utils::PathList;
    using vle::utils::Package;

    Package pkg("show_package");
    pkg.create();
    pkg.wait(std::cerr, std::cerr);
    pkg.configure();
    pkg.wait(std::cerr, std::cerr);
    pkg.build();
    pkg.wait(std::cerr, std::cerr);
    pkg.install();
    pkg.wait(std::cerr, std::cerr);

    /* We need to ensure each file really installed. */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    BOOST_REQUIRE_EQUAL(static_cast<PathList::size_type>(0),
                        Path::path().getSimulatorDirs().size());

    std::cout << "getBinaryPackagesDir: "
              << Path::path().getBinaryPackagesDir().string()
              << "\ngetBinaryPackages   : ";

    auto lst = Path::path().getBinaryPackages();
    for (const auto& elem : lst)
        std::cout << elem << ' ';

    BOOST_REQUIRE(Path::path().getBinaryPackages().size() == 1);
    vle::utils::FSpath p = pkg.getExpDir(vle::utils::PKG_BINARY);

    std::cout << "\ngetExpDir           : " << p.string()
              << "\ngetExperiments      :";
    auto vpz = pkg.getExperiments();

    for (const auto& elem : vpz)
        std::cout << elem.string() << ' ';
    
    BOOST_REQUIRE(vpz.size() == 1);
}

BOOST_AUTO_TEST_CASE(remote_package_check_package_tmp)
{
    utils::Package pkg_tmp("remote_package_check_package_tmp");
    pkg_tmp.create();
    pkg_tmp.wait(std::cerr, std::cerr);
    pkg_tmp.configure();
    pkg_tmp.wait(std::cerr, std::cerr);
    pkg_tmp.build();
    pkg_tmp.wait(std::cerr, std::cerr);
    pkg_tmp.install();
    pkg_tmp.wait(std::cerr, std::cerr);

    /* We need to ensure each file really installed. */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    utils::RemoteManager rmt;
    utils::Packages results;

    {
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", nullptr);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), true);
        BOOST_REQUIRE_EQUAL(results.size(), 0u);
    }

    {
        rmt.start(utils::REMOTE_MANAGER_LOCAL_SEARCH, ".*", nullptr);
        rmt.join();
        rmt.getResult(&results);

        //
        // results.size() == 2, remote_manager_local_search and
        // show_package (description.txt are the same, name is MyProject.
        //

        BOOST_REQUIRE(results.empty() == false);
        BOOST_REQUIRE(results.size() == 2);
        BOOST_REQUIRE(results[0].name == "MyProject" and
                      results[1].name == "MyProject");
    }
}

BOOST_AUTO_TEST_CASE(remote_package_local_remote)
{
    utils::PackageId pkg;

    pkg.size = 0;
    pkg.name = "name";
    pkg.distribution = "distribution";
    pkg.maintainer = "me";
    pkg.description = "too good";
    pkg.url = "http://www.vle-project.org";
    pkg.md5sum = "1234567890987654321";
    pkg.tags = { "a", "b", "c" };

    {
        utils::PackageLinkId dep = { "a", 1, 1, 1,
                                     utils::PACKAGE_OPERATOR_EQUAL };
        pkg.depends = utils::PackagesLinkId(1, dep);
    }

    {
        utils::PackageLinkId dep = { "a", 1, 1, 1,
                                     utils::PACKAGE_OPERATOR_EQUAL };
        pkg.builddepends = utils::PackagesLinkId(1, dep);
    }

    {
        utils::PackageLinkId dep = { "a", 1, 1, 1,
                                     utils::PACKAGE_OPERATOR_EQUAL };
        pkg.conflicts = utils::PackagesLinkId(1, dep);
    }

    pkg.major = 1;
    pkg.minor = 2;
    pkg.patch = 3;

    vle::utils::FSpath path = utils::RemoteManager::getRemotePackageFilename();

    {
        std::ofstream ofs(path.string());
        BOOST_REQUIRE(ofs.is_open());
        ofs << pkg << "\n";
        BOOST_REQUIRE(ofs.good());
    }

    /* We need to ensure each file really installed. */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    utils::RemoteManager rmt;

    {
        utils::Packages results;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", nullptr);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 1u);

        BOOST_REQUIRE(results[0].name == "name");
    }

    {
        utils::Packages results;
        rmt.start(utils::REMOTE_MANAGER_LOCAL_SEARCH, ".*", nullptr);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 2u);
    }
}

BOOST_AUTO_TEST_CASE(remote_package_read_write)
{
    using namespace boost::assign;

    {
        std::ofstream OX("/tmp/X.dat");
        std::ofstream ofs(
            utils::RemoteManager::getRemotePackageFilename().c_str());

        for (int i = 0; i < 10; ++i) {
            utils::PackageId pkg;

            pkg.size = i;
            pkg.name = (fmt("name-%1%") % i).str();
            pkg.distribution = "distribution";
            pkg.maintainer = "me";
            pkg.description = "too good";
            pkg.url = "http://www.vle-project.org";
            pkg.md5sum = "1234567890987654321";
            pkg.tags += "a", "b", "c";

            {
                utils::PackageLinkId dep = { "a", 1, 1, 1,
                                             utils::PACKAGE_OPERATOR_EQUAL };
                pkg.depends = utils::PackagesLinkId(1, dep);
            }

            {
                utils::PackageLinkId dep = { "a", 1, 1, 1,
                                             utils::PACKAGE_OPERATOR_EQUAL };
                pkg.builddepends = utils::PackagesLinkId(1, dep);
            }

            {
                utils::PackageLinkId dep = { "a", 1, 1, 1,
                                             utils::PACKAGE_OPERATOR_EQUAL };
                pkg.conflicts = utils::PackagesLinkId(1, dep);
            }

            pkg.major = 1;
            pkg.minor = 2;
            pkg.patch = 3;

            ofs << pkg;
            OX << pkg;
        }
    }

    /* We need to ensure each file really installed. */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", nullptr);
        rmt.join();

        utils::Packages results;
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 10u);
    }

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", nullptr);
        rmt.join();

        utils::Packages results;
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 10u);
    }

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", nullptr);
        rmt.join();

        utils::Packages results;
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 10u);
    }

    vle::utils::RemoteManager rmt;

    std::ostringstream output;
    rmt.start(vle::utils::REMOTE_MANAGER_SHOW, "name-8", &output);
    rmt.join();

    {
        vle::utils::Packages pkgs;
        rmt.getResult(&pkgs);

        BOOST_REQUIRE_EQUAL(pkgs.size(), 1u);

        BOOST_REQUIRE_EQUAL(pkgs[0].name, "name-8");
        BOOST_REQUIRE_EQUAL(pkgs[0].size, 8u);
        BOOST_REQUIRE_EQUAL(pkgs[0].md5sum, "1234567890987654321");
        BOOST_REQUIRE_EQUAL(pkgs[0].url, "http://www.vle-project.org");
        BOOST_REQUIRE_EQUAL(pkgs[0].maintainer, "me");
        BOOST_REQUIRE_EQUAL(pkgs[0].major, 1);
        BOOST_REQUIRE_EQUAL(pkgs[0].minor, 2);
        BOOST_REQUIRE_EQUAL(pkgs[0].patch, 3);
        BOOST_REQUIRE_EQUAL(pkgs[0].description, "too good");
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 10u);
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "name.*", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 10u);
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "0$", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 1u);
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 10u);
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "[1|2]$", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 2u);
    }
}

BOOST_AUTO_TEST_CASE(test_compress_filepath)
{
    std::string filepath;
    std::string uniquepath;

    try {
        utils::FSpath unique = utils::FSpath::unique_path("%%%%-%%%%-%%%%-%%%%");
        vle::utils::Package pkg(unique.string());
        pkg.create();
        pkg.wait(std::cerr, std::cerr);
        pkg.configure();
        pkg.wait(std::cerr, std::cerr);
        pkg.build();
        pkg.wait(std::cerr, std::cerr);
        pkg.install();
        filepath = pkg.getSrcDir(vle::utils::PKG_SOURCE);
        uniquepath = unique.string();
    } catch (...) {
        BOOST_REQUIRE(false);
    }

    /* We need to ensure each file really installed. */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    BOOST_REQUIRE(not filepath.empty());

    utils::FSpath tarfile(utils::FSpath::temp_directory_path());
    tarfile /= "check.tar.bz2";

    utils::FSpath::current_path(vle::utils::Path::path().getBinaryPackagesDir());

    BOOST_REQUIRE_NO_THROW(utils::Path::path().compress(uniquepath,
                                                        tarfile.string()));

    utils::FSpath t { tarfile };
    BOOST_REQUIRE(t.exists());

    utils::FSpath tmpfile(utils::FSpath::temp_directory_path());
    tmpfile /= "unique";

    tmpfile.create_directory();
    
    utils::FSpath::current_path(tmpfile);

    BOOST_REQUIRE_NO_THROW(utils::Path::path().decompress(tarfile.string(),
                                                          tmpfile.string()));
    utils::FSpath t2 { tmpfile };
    BOOST_REQUIRE(t2.exists());

    t2 /= uniquepath;

    BOOST_REQUIRE(t2.exists());
}
