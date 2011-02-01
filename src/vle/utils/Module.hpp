/*
 * @file vle/utils/Module.hpp
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


#ifndef VLE_UTILS_MODULE_HPP
#define VLE_UTILS_MODULE_HPP 1

#include <ostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <vle/utils/DllDefines.hpp>
#include <vle/utils/Algo.hpp>
#include <glibmm/module.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace vle { namespace utils {

/**
 * @brief A vle::utils::Module is a boost::shared_ptr to a Glib::Module object
 * and stores the list of symbols found in this shared library.
 */
class VLE_UTILS_EXPORT Module
{
public:
    typedef boost::shared_ptr < Glib::Module > ModulePtr;

    typedef std::map < std::string, void* > SymbolList;
    typedef SymbolList::iterator iterator;
    typedef SymbolList::value_type value_type;
    typedef SymbolList::const_iterator const_iterator;
    typedef SymbolList::size_type size_type;

    /**
     * @brief Build a new Module based on a filename build from a path and a
     * plug-in name.
     * @code
     * // try to open /home/foo/bar/pkgs/simulators/libfile.so on Unix.
     * Module module("/home/foo/bar/pkgs/simulators", "file");
     * @endcode
     *
     * @param path The path of the dynamic library.
     * @param file The name of the plug-in.
     */
    Module(const std::string& path, const std::string& file);

    /**
     * @brief Build a new Module based on a filename provided by the user;
     * @code
     * Module module("/home/foo/bar/pkgs/simulators/libfile.so");
     * @endcode
     *
     * @param filename The file to open.
     */
    Module(const std::string& filename);

    Module(const Module& module)
        : mModule(module.mModule), mLst(module.mLst)
    {}

    Module& operator=(const Module& module)
    {
        if (this != &module) {
            Module tmp(module);
            tmp.swap(*this);
        }
        return *this;
    }

    /**
     * @brief The Module uses a boost::shared_ptr to manage the instance of the
     * Glib::Module.
     */
    ~Module() {}

    /**
     * @brief Swap data from throw Module.
     *
     * @param module The module to swap data.
     */
    void swap(Module& module)
    {
        mModule = module.mModule;
        mLst.swap(module.mLst);
    }

    /**
     * @brief Get a symbol from the shared library.
     * @param symbol The symbol to search from the shared library.
     * @throw Neither throw exception.
     * @return A pointer to the symbol founded or NULL otherwise.
     */
    void* get(const std::string& symbol) const throw();

    /**
     * @brief Get the name (unique identifiers) of the Glib::Module.
     * @return A constant reference to the name of the Glib::Module.
     */
    std::string path() const { return mModule->get_name(); }

    /**
     * @brief Get the list of symbols found into the Glib::Module.
     * @return A constant reference to the list of symbols.
     */
    const SymbolList& symbols() const { return mLst; }

    /**
     * @brief Compare the current Module path with an another.
     * @param other The other Module to check.
     * @return true if Module's path is lower than other's path.
     */
    bool operator<(const Module& other) const { return path() < other.path(); }

    /**
     * @brief Check if the two Module reference the same Module.
     * @param other The other Module to check.
     * @return true if Module's path is equal to other's path.
     */
    bool operator=(const Module& other) const { return path() == other.path(); }

    iterator find(const std::string& key) { return mLst.find(key); }
    const_iterator find(const std::string& key) const { return mLst.find(key); }
    size_type size() const { return mLst.size(); }
    iterator begin() { return mLst.begin(); }
    iterator end() { return mLst.end(); }
    const_iterator begin() const { return mLst.begin(); }
    const_iterator end() const { return mLst.end(); }

    /**
     * @brief Get the plug-in name of the filename provided.
     * @code
     * // return "toto".
     * utils::Module::getPluginName("/home/foo/bar/libtoto.so");
     * @endcode
     * @param path The path of the filename to convert;
     * @return The plug-in name;
     */
    static std::string getPluginName(const std::string& path);

    /**
     * @brief Get the plug-in path name from a path and a plug-in name.
     * @code
     * // return "/home/foo/bar/libtoto.so
     * utils::Module::getPathName("/home/foo/bar", "toto");
     *
     * // return "/home/foo/bar/libtoto.so
     * utils::Module::getPathName("/home/foo/bar", "libtoto.so");
     *
     * // return "/home/foo/bar/libtoto.so.so
     * utils::Module::getPathName("/home/foo/bar", "libtoto.so.so");
     * @endcode
     * @param path The path of the plug-in.
     * @param plugin The name of the plug-in.
     * @return The path of the plug-in.
     */
    static std::string getPathName(const std::string& path,
                                   const std::string& plugin);

private:
    ModulePtr mModule; /**< The Glib::Module is an object which wraps the
                         shared library and can not be NULL in this API. */

    mutable SymbolList mLst; /**< We attach for each Glib::Module a list of
                               symbols sorted by name. */
};

/**
 * @brief Store a list of Module sorted by unique identifiers. This class is a
 * multi thread safe singleton.
 */
class VLE_UTILS_EXPORT ModuleCache
{
public:
    typedef std::set < Module > ModuleList;
    typedef ModuleList::iterator iterator;
    typedef ModuleList::const_iterator const_iterator;
    typedef ModuleList::value_type value_type;
    typedef ModuleList::size_type size_type;

    /**
     * @brief Initialize the ModuleCache singleton system.
     */
    static void init() { instance(); }

    /**
     * @brief Delete the ModuleCache singleton system.
     */
    static void kill() { delete mInstance; mInstance = 0; }

    /**
     * @brief Get the reference to the ModuleCache instantiate in a static
     * member.
     * @return A reference to the singleton object.
     */
    static ModuleCache& instance()
    {
        if (not mInstance) {
            boost::mutex::scoped_lock lock(mMutex);

            if (not mInstance) {
                mInstance = new ModuleCache;
            }
        }

        return *mInstance;
    }

    /**
     * @brief Get a reference to the boost::mutex to build thread-safe
     * application.
     * @return A reference to the mutex.
     */
    static boost::mutex& mutex() { return mMutex; }

    /**
     * @brief Try to load the plug-in with the specified path and filename.
     *
     * @param path The directory of the plug-in.
     * @param file The plug-in name.
     *
     * @return An iterator to the cached or opened plug-in.
     */
    iterator load(const std::string& path, const std::string& file);

    /**
     * @brief Try to unload the plug-in.
     *
     * @param path The directory of the plug-in.
     * @param file The plug-in name.
     */
    void unload(const std::string& path, const std::string& file);

    /**
     * @brief Try to unload the plug-in.
     *
     * @param library The complete filename to delete.
     */
    void unload(const std::string& library);

    /**
     * @brief Browse the `path' and search all plug-in available and fill a
     * std::vector.
     * @param path The directory to browse.
     * @return A list of iterator to newly plug-ins founded.
     */
    std::vector < iterator > browse(const std::string& path);

    /**
     * @brief Browse all `paths' and search all plug-in available and fill a
     * std::vector.
     * @param paths The directories to browse.
     * @return A list of iterator to newly plug-ins founded.
     */
    std::vector < iterator > browse(const std::vector < std::string >& paths);

    /**
     * @brief Clear the Module cache.
     */
    void clear() { mLst.clear(); }

    /**
     * @brief Get a copy of all loaded plug-ins.
     * @return A new ModuleList.
     */
    ModuleList get() const { return mLst; }

private:
    /**
     * @brief Get the list of Module load in this ModuleCache.
     * @return A constant reference to the ModuleList.
     */
    const ModuleList& modules() const { return mLst; }

    size_type size() const { return mLst.size(); }
    iterator begin() { return mLst.begin(); }
    iterator end() { return mLst.end(); }

    ModuleList mLst; /**< Store a list of Module sorted by a unique identifiers
                       resolved by the name of the Glib::Module. */

    static ModuleCache* mInstance; /**< Store the instance of ModuleCache to
                                     develop the singleton design pattern. */

    static boost::mutex mMutex; /**< Store the instance of Mutex of ModuleCache
                                  to implement a multi-thread safe singleton
                                  design pattern. */

    ModuleCache() {}

    ~ModuleCache() {}

    ModuleCache(const ModuleCache& modules)
        : mLst(modules.mLst)
    {}

    ModuleCache& operator=(const ModuleCache& modules)
    {
        ModuleCache m(modules);
        m.swap(*this);
        return *this;
    }

    /**
     * @brief Swap ModuleCache data.
     * @param modules The ModuleCache to swap data.
     */
    void swap(ModuleCache& modules) { mLst.swap(modules.mLst); }

    void fill(std::vector < ModuleCache::iterator >& output,
              const std::string& path);
};

}} // namespace vle utils

#endif

