/*
 * @file vle/utils/Module.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/utils/Module.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/config.hpp>
#include <algorithm>

namespace vle { namespace utils {

ModuleCache* ModuleCache::mInstance = NULL;
boost::mutex ModuleCache::mMutex;

Module::Module(const std::string& path, const std::string& file)
{
    std::string filename = getPathName(path, file);

    mModule = ModulePtr(new Glib::Module(filename, Glib::MODULE_BIND_LOCAL));

    if (not (*mModule.get())) {
        throw utils::ArgError(fmt(
                _("Module: cannot load dynamic library `%1%' in directory"
                  " `%2%' using path `%3%' (%4%)")) % file % path % filename %
            Glib::Module::get_last_error());
    }
}

Module::Module(const std::string& filename)
{
    mModule = ModulePtr(new Glib::Module(filename, Glib::MODULE_BIND_LOCAL));

    if (not (*mModule.get())) {
        throw utils::ArgError(fmt(
                _("Module: cannot load dynamic library `%1%' (%2%)")) %
            filename % Glib::Module::get_last_error());
    }
}

void* Module::get(const std::string& symbol) const throw()
{
    boost::mutex::scoped_lock lock(ModuleCache::mutex());

    const_iterator it = mLst.find(symbol);

    if (it != end()) {
        return it->second;
    } else {
        void* sym = NULL;

        if (mModule->get_symbol(symbol, sym)) {
            mLst.insert(std::make_pair(symbol, sym));
        }

        return sym;
    }
}

std::string Module::getPluginName(const std::string& path)
{
    namespace fs = boost::filesystem;

    std::string filename = fs::basename(fs::path(path));

    if ((filename.size() > 3) and filename.compare(0, 3, "lib") == 0) {
        return filename.substr(3);
    }

    return filename;
}

std::string Module::getPathName(const std::string& path,
                                const std::string& name)
{
    namespace fs = boost::filesystem;

    fs::path result(path);
    std::string pluginname;

    if (((name.size() >= 3 and name.compare(0, 3, "lib")) or name.size() < 3)) {
        pluginname = "lib";
        pluginname += name;
    }

    fs::path file(name);

#ifdef BOOST_WINDOWS
    if (fs::extension(file) != ".dll") {
        pluginname += ".dll";
    }
#else
    if (fs::extension(file) != ".so") {
        pluginname += ".so";
    }
#endif

    result /= pluginname;

    return result.string();
}

std::vector < ModuleCache::iterator >
ModuleCache::browse(const std::string& path)
{
    std::vector < iterator > result;

    fill(result, path);

    return result;
}

std::vector < ModuleCache::iterator >
ModuleCache::browse(const std::vector < std::string >& paths)
{
    std::vector < iterator > result;

    for (std::vector < std::string >::const_iterator it = paths.begin();
         it != paths.end(); ++it) {
        fill(result, *it);
    }

    return result;
}

void ModuleCache::fill(std::vector < ModuleCache::iterator >& output,
                       const std::string& path)
{
    namespace fs = boost::filesystem;

    if (fs::is_directory(path)) {
        fs::directory_iterator it(path), end;
        for (; it != end; ++it) {
            if (fs::is_regular_file(it->status())) {
                try {
#ifdef BOOST_WINDOWS
                    if (fs::extension(*it) == ".dll")
#else
                    if (fs::extension(*it) == ".so")
#endif
                    {
                        boost::mutex::scoped_lock lock(ModuleCache::mutex());

                        std::pair < iterator, bool > tmp;

#if BOOST_VERSION > 104500
                        tmp = mLst.insert(Module(it->path().string()));
#else
                        tmp = mLst.insert(Module(it->path().file_string()));
#endif
                        if (tmp.second) {
                            output.push_back(tmp.first);
                        }
                    }
                } catch(const std::exception& /* e */) {
                }
            }
        }
    }
}

// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
//  / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

ModuleCache::iterator ModuleCache::load(const std::string& path,
                                        const std::string& file)
{
    std::string pathname = Module::getPathName(path, file);

    boost::mutex::scoped_lock lock(ModuleCache::mutex());

    iterator it = mLst.find(pathname);

    if (it != mLst.end()) {
        return it;
    } else {
        return mLst.insert(Module(pathname)).first;
    }
}

void ModuleCache::unload(const std::string& path, const std::string& file)
{
    std::string pathname = Module::getPathName(path, file);

    boost::mutex::scoped_lock lock(ModuleCache::mutex());

    iterator it = mLst.find(pathname);

    if (it != mLst.end()) {
        mLst.erase(it);
    }
}

void ModuleCache::unload(const std::string& filename)
{
    boost::mutex::scoped_lock lock(ModuleCache::mutex());

    iterator it = mLst.find(filename);

    if (it != mLst.end()) {
        mLst.erase(it);
    }
}

}} // namespace vle utils
