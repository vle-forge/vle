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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_library_test
#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>
#include <numeric>
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
#include <vle/vle.hpp>

#ifdef _WIN32
#  include <Windows.h>
#endif

using namespace vle;

namespace bs = boost::system;
namespace fs = boost::filesystem;

#if BOOST_VERSION <= 104500
/*
 * Build a temporary directory path name.
 */
static std::string get_temporary_path()
{
    const char *names[] = { "TMPDIR", "TMP", "TEMP" };
    const int names_size = sizeof(names) / sizeof(names[0]);

    for (int i = 0; i != names_size; ++i)
        if (::getenv(names[i]) && fs::exists(::getenv(names[i])) &&
                     fs::is_directory(::getenv(names[i])))
            return names[i];

#ifdef _WIN32
    return "c:/tmp";
#else
    return "/tmp";
#endif
}
#endif

/*
 * We need to ensure each file really installed.
 */
static void make_pause()
{
#ifdef _WIN32
    ::Sleep(1000);
#else
    ::sleep(1);
#endif
}

struct F
{
    vle::Init *a;
    fs::path current_path;

    F() throw()
        : a(0)
    {
        bs::error_code ec;

#if BOOST_VERSION > 104500
        current_path = fs::temp_directory_path(ec);
        current_path /= fs::unique_path("vle-%%%%-%%%%-%%%%-%%%%");
#else
	current_path = get_temporary_path();
	current_path /= "checkvle";
#endif

#if BOOST_VERSION > 104500
        fs::create_directory(current_path, ec);
#else
        fs::create_directory(current_path);
#endif

#if BOOST_VERSION > 104500
        if (fs::exists(current_path, ec) && fs::is_directory(current_path, ec)) {
#else
        if (fs::exists(current_path) && fs::is_directory(current_path)) {
#endif

#ifdef _WIN32
            ::_putenv((vle::fmt("VLE_HOME=%1%")
                    % current_path.string()).str().c_str());
#else
#  if BOOST_VERSION > 104500
            ::setenv("VLE_HOME", current_path.c_str(), 1);
#  else
            ::setenv("VLE_HOME", current_path.file_string().c_str(), 1);
#  endif
#endif
        }

#if BOOST_VERSION > 104500
        fs::current_path(current_path, ec);
#else
        fs::current_path(current_path);
#endif

        a = new vle::Init();
        vle::utils::Preferences prefs("vle.conf");
    }

    ~F() throw()
    {
#if BOOST_VERSION > 104500
        bs::error_code ec;

        if (fs::exists(current_path, ec) && fs::is_directory(current_path, ec)) {
            fs::remove_all(current_path, ec); /**< comment this line if you
                                      * want to conserve the temporary VLE_HOME
                                      * directory. */
        }
#else
        if (fs::exists(current_path) && fs::is_directory(current_path)) {
            fs::remove_all(current_path); /**< comment this line if you
                                      * want to conserve the temporary VLE_HOME
                                      * directory. */
        }
#endif

        delete a;
    }

private:
    F(const F&);
    F& operator=(const F&);
};

BOOST_FIXTURE_TEST_CASE(show_path, F)
{
    using vle::utils::Package;
    using vle::utils::Path;
    using vle::utils::PathList;

    Package pkg("show_path");
    pkg.create();
    pkg.configure();
    pkg.build();
    pkg.install();

    BOOST_REQUIRE_EQUAL((PathList::size_type)0,
                        Path::path().getSimulatorDirs().size());
}

BOOST_FIXTURE_TEST_CASE(show_package, F)
{
    using vle::utils::Path;
    using vle::utils::PathList;
    using vle::utils::Package;

    std::ostringstream out, err;

    Package pkg("show_package");
    pkg.create();
    pkg.configure();
    pkg.build();
    pkg.install();

    /* We need to ensure each file really installed. */
    make_pause();

    std::cout << "Installed packages:\n";
    PathList lst = Path::path().getBinaryPackages();
    std::copy(lst.begin(), lst.end(), std::ostream_iterator < std::string >(
                  std::cout, "\n"));

    if (not fs::exists(pkg.getExpDir(vle::utils::PKG_BINARY))) {
        std::cout << "Installed vpz  :\n";
        PathList vpz = pkg.getExperiments();
        std::copy(vpz.begin(), vpz.end(), std::ostream_iterator < std::string >(
                std::cout, "\n"));
    }
}

BOOST_FIXTURE_TEST_CASE(remote_package_check_package_tmp, F)
{
    utils::Package pkg_tmp("remote_package_check_package_tmp");
    pkg_tmp.create();
    pkg_tmp.configure();
    pkg_tmp.build();
    pkg_tmp.install();

    /* We need to ensure each file really installed. */
    make_pause();

    utils::RemoteManager rmt;
    utils::Packages results;

    {
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", NULL);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), true);
        BOOST_REQUIRE_EQUAL(results.size(), 0u);
    }

    {
        rmt.start(utils::REMOTE_MANAGER_LOCAL_SEARCH, ".*", NULL);
        rmt.join();
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 1u); /* We only build the tmp
                                                    package, not x. */

        BOOST_REQUIRE(results[0].name == "MyProject");
    }
}

BOOST_FIXTURE_TEST_CASE(remote_package_local_remote, F)
{
    using namespace boost::assign;

    utils::PackageId pkg;

    pkg.size = 0;
    pkg.name = "name";
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

     fs::path path = utils::RemoteManager::getRemotePackageFilename();

    {
        std::ofstream ofs(path.string().c_str());
        ofs << pkg << "\n";
    }

    /* We need to ensure each file really installed. */
    make_pause();

    utils::RemoteManager rmt;

    {
        utils::Packages results;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", NULL);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 1u);

        BOOST_REQUIRE(results[0].name == "name");
    }

    {
        utils::Packages results;
        rmt.start(utils::REMOTE_MANAGER_LOCAL_SEARCH, ".*", NULL);
        rmt.join();
        rmt.getResult(&results);
        BOOST_REQUIRE_EQUAL(results.empty(), true);
        BOOST_REQUIRE_EQUAL(results.size(), 0u);
    }
}

BOOST_FIXTURE_TEST_CASE(remote_package_read_write, F)
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
    make_pause();

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", NULL);
        rmt.join();

        utils::Packages results;
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 10u);
    }

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", NULL);
        rmt.join();

        utils::Packages results;
        rmt.getResult(&results);

        BOOST_REQUIRE_EQUAL(results.empty(), false);
        BOOST_REQUIRE_EQUAL(results.size(), 10u);
    }

    {
        utils::RemoteManager rmt;
        rmt.start(utils::REMOTE_MANAGER_SEARCH, ".*", NULL);
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
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "name.*", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 10u);
    }

    {
        vle::utils::Packages pkgs;
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*0", &output);
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
        rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*[1|2]", &output);
        rmt.join();
        rmt.getResult(&pkgs);
        BOOST_REQUIRE_EQUAL(pkgs.size(), 2u);
    }
}

BOOST_FIXTURE_TEST_CASE(test_compress_filepath, F)
{
    std::string filepath;
    std::string uniquepath;
    try {
#if BOOST_VERSION > 104500
        fs::path unique = fs::unique_path("%%%%-%%%%-%%%%-%%%%");
#else
        fs::path unique = "check";
#endif

        vle::utils::Package pkg(unique.string());
        pkg.create();

        filepath = pkg.getSrcDir(vle::utils::PKG_SOURCE);

        pkg.configure();
        pkg.build();
        pkg.install();
        uniquepath = unique.string();
    } catch (...) {
        BOOST_REQUIRE(false);
    }

    /* We need to ensure each file really installed. */
    make_pause();

    BOOST_REQUIRE(not filepath.empty());

#if BOOST_VERSION > 104500
    fs::path tarfile(fs::temp_directory_path());
#else
    fs::path tarfile(get_temporary_path());
#endif
    tarfile /= "check.tar.bz2";

    fs::current_path(vle::utils::Path::path().getBinaryPackagesDir());

    BOOST_REQUIRE_NO_THROW(utils::Path::path().compress(uniquepath,
                                                        tarfile.string()));
    BOOST_REQUIRE(fs::exists(fs::path(tarfile)));

#if BOOST_VERSION > 104500
    fs::path tmpfile(fs::temp_directory_path());
#else
    fs::path tmpfile = get_temporary_path();
#endif
    tmpfile /= "unique";

    fs::create_directory(tmpfile);
    fs::current_path(tmpfile);

    BOOST_REQUIRE_NO_THROW(utils::Path::path().decompress(tarfile.string(),
                                                          tmpfile.string()));
    BOOST_REQUIRE(fs::exists(fs::path(tmpfile)));

    tmpfile /= uniquepath;

    BOOST_REQUIRE(fs::exists(fs::path(tmpfile)));
}
