/*
 * @file vle/utils/test/test1.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_library_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <boost/filesystem.hpp>
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
#include <vle/utils/Algo.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/vle.hpp>

using namespace vle;

struct F
{
    vle::Init *a;
    std::string oldname;

    F() : a(0), oldname()
    {
        printf("1\n");
        namespace fs = boost::filesystem;

        try {
            printf("2\n");
            fs::path tmp = fs::temp_directory_path();

            printf("3\n");
            printf("4\n");

            {
                char *env = ::getenv("VLE_HOME");
                printf("5\n");
                if (env) {
                    oldname.assign(env);
                }
                printf("6\n");
            }

            ::setenv("VLE_HOME", tmp.string().c_str(), 1);

        } catch (const std::exception&) {

        }

        a = new vle::Init();
    }

    ~F()
    {
        delete a;

        if (not oldname.empty()) {
            ::setenv("VLE_HOME", oldname.c_str(), 1);
        }
    }

private:
    F(const F&);
    F& operator=(const F&);
};

BOOST_GLOBAL_FIXTURE(F)

BOOST_AUTO_TEST_CASE(remote_package_read)
{
    const std::string data =                                    \
        "Package: toto\n"                                       \
        "Version: 12.21.30\n"                                   \
        "Depends: vle\n"                                        \
        "Build-Depends:\n"                                      \
        "Conflicts:\n"                                          \
        "Maintainer: quesnel@users.sourceforge.net\n"           \
        "Description: None\n"                                   \
        " .\n"                                                  \
        "Tags: test\n"                                          \
        "Url: http://www.vle-project.org/1.1/toto/toto.zip\n"   \
        "Size: 4096\n"                                          \
        "MD5sum: xxx\n"                                         \
        "Package: tata\n"                                       \
        "Version: 1.2.3\n"                                      \
        "Depends: vle\n"                                        \
        "Build-Depends:\n"                                      \
        "Conflicts:\n"                                          \
        "Maintainer: quesnel@users.sourceforge.net\n"           \
        "Description: None2\n"                                  \
        " .\n"                                                  \
        "Tags: test\n"                                          \
        "Url: http://www.vle-project.org/1.1/tata/tata.zip\n"   \
        "Size: 4096\n"                                          \
        "MD5sum: xxx\n";

    std::istringstream oss(data);
    vle::utils::RemoteManager rmt(oss);
    vle::utils::Packages pkgs;

    std::ostringstream output;
    rmt.start(vle::utils::REMOTE_MANAGER_SHOW, "toto", &output);
    rmt.join();

    rmt.getResult(&pkgs);

    BOOST_REQUIRE_EQUAL(pkgs.size(), 1u);

    BOOST_REQUIRE_EQUAL(pkgs[0].name, "toto");
    BOOST_REQUIRE_EQUAL(pkgs[0].size, 4096u);
    BOOST_REQUIRE_EQUAL(pkgs[0].md5sum, "xxx");
    BOOST_REQUIRE_EQUAL(pkgs[0].url,
                        "http://www.vle-project.org/1.1/toto/toto.zip");
    BOOST_REQUIRE_EQUAL(pkgs[0].maintainer, "quesnel@users.sourceforge.net");
    BOOST_REQUIRE_EQUAL(pkgs[0].major, 12);
    BOOST_REQUIRE_EQUAL(pkgs[0].minor, 21);
    BOOST_REQUIRE_EQUAL(pkgs[0].patch, 30);
    BOOST_REQUIRE_EQUAL(pkgs[0].description, "None");

    pkgs.clear();
    rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "t.*", &output);
    rmt.join();
    rmt.getResult(&pkgs);
    BOOST_REQUIRE_EQUAL(pkgs.size(), 2u);

    pkgs.clear();
    rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*ta", &output);
    rmt.join();
    rmt.getResult(&pkgs);
    BOOST_REQUIRE_EQUAL(pkgs.size(), 1u);

    pkgs.clear();
    rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, ".*", &output);
    rmt.join();
    rmt.getResult(&pkgs);
    BOOST_REQUIRE_EQUAL(pkgs.size(), 2u);

    pkgs.clear();
    rmt.start(vle::utils::REMOTE_MANAGER_SEARCH, "t[a-z]ta", &output);
    rmt.join();
    rmt.getResult(&pkgs);
    BOOST_REQUIRE_EQUAL(pkgs.size(), 1u);

    BOOST_REQUIRE_EQUAL(pkgs[0].name, "tata");
    BOOST_REQUIRE_EQUAL(pkgs[0].size, 4096u);
    BOOST_REQUIRE_EQUAL(pkgs[0].md5sum, "xxx");
    BOOST_REQUIRE_EQUAL(pkgs[0].url,
                        "http://www.vle-project.org/1.1/tata/tata.zip");
    BOOST_REQUIRE_EQUAL(pkgs[0].maintainer, "quesnel@users.sourceforge.net");
    BOOST_REQUIRE_EQUAL(pkgs[0].major, 1);
    BOOST_REQUIRE_EQUAL(pkgs[0].minor, 2);
    BOOST_REQUIRE_EQUAL(pkgs[0].patch, 3);
    BOOST_REQUIRE_EQUAL(pkgs[0].description, "None2");
}


BOOST_AUTO_TEST_CASE(test_compress_filepath)
{
    namespace fs = boost::filesystem;

    std::string filepath;
    std::string uniquepath;
    try {
        fs::path unique = fs::unique_path("%%%%-%%%%-%%%%-%%%%");

        vle::utils::Package::package().select(unique.string());
        vle::utils::Package::package().create();

        filepath = vle::utils::Path::path().getPackageDir();
        uniquepath = unique.string();
    } catch (...) {
        BOOST_REQUIRE(false);
    }

    BOOST_REQUIRE(not filepath.empty());

    fs::path tarfile(fs::temp_directory_path());
    tarfile /= "check.tar.bz2";


    fs::current_path(vle::utils::Path::path().getPackagesDir());

    BOOST_REQUIRE_NO_THROW(utils::Path::path().compress(uniquepath,
                                                        tarfile.string()));
    BOOST_REQUIRE(fs::exists(fs::path(tarfile)));

    fs::path tmpfile(fs::temp_directory_path());
    tmpfile /= "unique";

    fs::create_directory(tmpfile);
    fs::current_path(tmpfile);

    BOOST_REQUIRE_NO_THROW(utils::Path::path().decompress(tarfile.string(),
                                                          tmpfile.string()));
    BOOST_REQUIRE(fs::exists(fs::path(tmpfile)));

    tmpfile /= uniquepath;

    BOOST_REQUIRE(fs::exists(fs::path(tmpfile)));
}
