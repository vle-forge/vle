/*
 * @file vle/utils/SharedLibraryManager.cpp
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

#include <vle/utils/DllDefines.hpp>
#include <vle/utils/SharedLibraryManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <algorithm>

#ifdef BOOST_WINDOWS
#include <Winbase.h>
#else
#include <dlfcn.h>
#endif

namespace fs = boost::filesystem;
namespace ss = boost::system;

namespace vle { namespace utils {

class SharedLibraryManager::Pimpl
{
    Pimpl(const Pimpl& other);
    Pimpl& operator=(const Pimpl& other);

#ifdef BOOST_WINDOWS
    typedef std::vector < HMODULE > Libraries;
#else
    typedef std::vector < void* > Libraries;
#endif

    Libraries mLibs; /* We sthore the handler for each libraries found. */

    struct Unload {
        void operator()(const Libraries::value_type& x) const {
            if (x) {
#ifdef BOOST_WINDOWS
                ::FreeLibrary(x);
#else
                ::dlclose(x);
#endif
            }
        }
    };

    void check(const fs::path& path)
    {
        if (fs::is_directory(path)) {
            fs::path lib = path;
            lib /= "lib";

#if BOOST_VERSION > 104500
            ss::error_code ec;
            if (fs::is_directory(lib, ec)) {
#else
            if (fs::is_directory(lib)) {
#endif
                fs::directory_iterator it(lib), end;
                for (; it != end; ++it) {
                    if (fs::is_regular_file(it->status())) {
#ifdef BOOST_WINDOWS
                        if (fs::extension(*it) == ".dll") {
                            load(*it);
                        }
#else
                        if (fs::extension(*it) == ".so") {
                            load(*it);
                        }
#endif
                    }
                }
            }
        }
    }

    void load(const fs::path& lib)
    {
#ifdef BOOST_WINDOWS
        HMODULE handle = LoadLibrary(lib.string().c_str());
#else
        void *handle = dlopen(lib.string().c_str(), RTLD_LAZY);
#endif

        if (handle) {
            mLibs.push_back(handle);
        }
    }

public:
    Pimpl()
    {
        fs::path path = utils::Path::path().getPackagesDir();
#if BOOST_VERSION > 104500
        ss::error_code ec;

        if (fs::is_directory(path, ec)) {
#else
        if (fs::is_directory(path)) {
#endif
            fs::directory_iterator it(path), end;

            for (; it != end; ++it) {
                if (fs::is_directory(it->status())) {
                    check(*it);
                }
            }
        }
    }

    Pimpl(const std::vector < std::string >& packages)
    {
        fs::path path = utils::Path::path().getPackagesDir();
        std::vector < std::string >::const_iterator it;

        for (it = packages.begin(); it != packages.end(); ++it) {
            fs::path tmp = path;
            tmp /= *it;

#if BOOST_VERSION > 104500
            ss::error_code ec;
            if (fs::is_directory(tmp, ec)) {
#else
            if (fs::is_directory(tmp)) {
#endif
                check(tmp);
            }
        }
    }

    ~Pimpl()
    {
        std::for_each(mLibs.begin(), mLibs.end(), Unload());
    }
};

SharedLibraryManager::SharedLibraryManager() throw()
    : mPimpl(new SharedLibraryManager::Pimpl())
{
}

SharedLibraryManager::SharedLibraryManager(
    const std::vector < std::string >& packages) throw()
    : mPimpl(new SharedLibraryManager::Pimpl(packages))
{
}

SharedLibraryManager::~SharedLibraryManager() throw()
{
    delete mPimpl;
}

}} // namespace vle utils
