/*
 * @file vle/utils/ModuleManager.cpp
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
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Types.hpp>
#include <vle/version.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/checked_delete.hpp>
#include <boost/version.hpp>
#include <algorithm>

#ifdef BOOST_WINDOWS
#include <Winbase.h>
#include <windows.h>
#include <strsage.h>
#else
#include <dlfcn.h>
#endif

namespace vle { namespace utils {

/**
 * Get the plug-in name of the filename provided.
 *
 * @code
 * // return "toto".
 * utils::Module::getPluginName("/home/foo/bar/libtoto.so");
 * @endcode
 *
 * @param path The path of the file to convert;
 * @return The plug-in name;
 */
static std::string getLibraryName(const boost::filesystem::path& file)
{
    namespace fs = boost::filesystem;
    std::string library;

#if BOOST_VERSION > 104500
    if (file.filename().string().compare(0, 3, "lib") == 0) {
        library.append(file.filename().string(), 3, std::string::npos);
    }
#else
    if (file.filename().compare(0, 3, "lib") == 0) {
        library.append(file.filename(), 3, std::string::npos);
    }
#endif

#ifdef BOOST_WINDOWS
    if (fs::extension(file) == ".dll") {
        library.assign(library, 0, library.size() - 4);
    }
#else
    if (fs::extension(file) == ".so") {
        library.assign(library, 0, library.size() - 3);
    }
#endif

    return library;
}

static std::string getKeyName(const std::string& package,
                              const std::string& library)
{
    std::string result(package.size() + library.size() + 1, '/');

    result.replace(0, package.size(), package);
    result.replace(package.size() + 1, library.size(), library);

    return result;
}

class Module
{
public:
    /**
     * Build a new Module for a specified path build from the package name, the
     * library name and the type of the module. The shared library is not read.
     * To read the shared library and get symbol, use the Module::get()
     * function.
     *
     * @param package
     * @param library
     * @param type
     */
    Module(const std::string& package, const std::string& library,
          ModuleType type)
        : mPath(ModuleManager::buildModuleFilename(package, library, type)),
        mHandle(0), mFunction(0), mType(type), mIsGlobal(false)
    {
    }

    /**
     * Delete the instance of the shared library.
     */
    ~Module()
    {
        if (mHandle) {
#ifdef BOOST_WINDOWS
            FreeLibrary(mHandle);
#else
            dlclose(mHandle);
#endif
        }
    }

    /**
     * Get the symbol specified from the shared library. If the shared library
     * was nether opened, it was open.
     *
     * @throw utils::ArgError if the type argument specify many Type.
     * @throw utils::InternalError if the shared library does not exists, does
     * not have the specified symbol.
     *
     * @return A void pointer to the symbol found.
     */
    void* get()
    {
        if (not mHandle) {
            init();
            checkVersion();
        }

        if (mFunction) {
            return mFunction;
        } else {
            switch (mType) {
            case MODULE_DYNAMICS:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_dynamics"));
            case MODULE_DYNAMICS_WRAPPER:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_dynamics_wrapper"));
            case MODULE_EXECUTIVE:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_executive"));
            case MODULE_OOV:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_oov"));
            case MODULE_EOV:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_eov"));
            case MODULE_GVLE_MODELING:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_gvle_modeling"));
            case MODULE_GVLE_OUTPUT:
                return mFunction = reinterpret_cast < void* >(
                    getSymbol("vle_make_new_gvle_output"));
            default:
                throw utils::InternalError(fmt(
                        _("Module `%1%' have not the wanted symbol (%2%)")) %
                    mPath % static_cast < int >(mType));
            };
        }
    }

    ModuleType determine()
    {
        if (not mHandle) {
            init();
            checkVersion();
        }

        if (existSymbol("vle_make_new_dynamics")) {
            mType = MODULE_DYNAMICS;
        } else if (existSymbol("vle_make_new_dynamics_wrapper")) {
            mType = MODULE_DYNAMICS_WRAPPER;
        } else if (existSymbol("vle_make_new_executive")) {
            mType = MODULE_EXECUTIVE;
        } else if (existSymbol("vle_make_new_oov")) {
            mType = MODULE_OOV;
        } else if (existSymbol("vle_make_new_eov")) {
            mType = MODULE_EOV;
        } else if (existSymbol("vle_make_new_gvle_modeling")) {
            mType = MODULE_GVLE_MODELING;
        } else if (existSymbol("vle_make_new_gvle_output")) {
            mType = MODULE_GVLE_OUTPUT;
        }

        return mType;
    }

    const std::string& path() const
    {
        return mPath;
    }

private:
    std::string mPath;
#ifdef BOOST_WINDOWS
    HMODULE mHandle;
#else
    void *mHandle;
#endif

#ifdef BOOST_WINDOWS
    FARPROC mFunction;
#else
    void *mFunction;
#endif

    ModuleType mType;
    bool mIsGlobal;


    void init()
    {
        assert(not mHandle);

#ifdef BOOST_WINDOWS
        HMODULE handle = LoadLibrary(mPath.c_str());
#else
        void *handle = dlopen(mPath.c_str(), RTLD_LAZY);
#endif
        if (not handle) {
            std::string extra;
#ifdef BOOST_WINDOWS
            DWORD errorcode = ::GetLastError();
            LPVOID msg;

            ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_FROM_SYSTEM |
                            FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            errorcode,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR) &msg,
                            0, NULL);

            extra = msg;
            LocalFree(msg);
#else
            char *error = dlerror();
            if (error) {
                extra.assign(error);
            }
#endif
            throw utils::InternalError(fmt(
                    _("Module can not open shared library `%1%': %2%")) % mPath
                % extra);
        } else {
            mHandle = handle;
        }
    }

    void checkVersion()
    {
        typedef void(*versionFunction)(vle::uint32_t*, vle::uint32_t*,
                                       vle::uint32_t*);

#ifdef BOOST_WINDOWS
        FARPROC symbol;
#else
        void *symbol;
#endif

        uint32_t major, minor, patch;
        versionFunction fct;

        symbol = getSymbol("vle_api_level");
        fct = utils::functionCast < versionFunction >(symbol);
        fct(&major, &minor, &patch);

        if (major != VLE_MAJOR_VERSION or minor != VLE_MINOR_VERSION) {
            throw utils::InternalError(fmt(
                    _("Module: `%1%' was produced with VLE %1%.%2%.%3% and is"
                      " not API/ABI compatible with the current VLE"
                      " %1%.%2%.%3%")) % mPath % major % minor % patch %
                VLE_MAJOR_VERSION % VLE_MINOR_VERSION % VLE_PATCH_VERSION);
        } else if (patch != VLE_PATCH_VERSION) {
            utils::Trace::send(fmt(
                    _("Module: `%1%' was produced with VLE %1%.%2%.%3% and may"
                      " not be API/ABI compatible with the current VLE"
                      " %1%.%2%.%3%")) % mPath % major % minor % patch %
                VLE_MAJOR_VERSION % VLE_MINOR_VERSION % VLE_PATCH_VERSION);
        }
    }

#ifdef BOOST_WINDOWS
    FARPROC getSymbol(const char *dsymbol)
#else
    void *getSymbol(const char *symbol)
#endif
    {
        assert(mHandle);

#ifdef BOOST_WINDOWS
        FARPROC ptr = GetProcAddress(mHandle, symbol);
#else
        void *ptr = dlsym(mHandle, symbol);
#endif

        if (not ptr) {
            throw utils::InternalError(fmt(
                    _("Module `%1%' have not the desired symbol (%2%)")) %
                mPath % symbol);
        }

        return ptr;
    }

    bool existSymbol(const char *symbol) const
    {
        assert(mHandle);

#ifdef BOOST_WINDOWS
        FARPROC ptr = GetProcAddress(mHandle, symbol);
#else
        void *ptr = dlsym(mHandle, symbol);
#endif
        return ptr;
    }
};

class ModuleManager::Pimpl
{
public:
    typedef boost::unordered_map < std::string, Module* > ModuleTable;
    typedef ModuleTable::value_type value_type;
    typedef ModuleTable::const_iterator const_iterator;
    typedef ModuleTable::iterator iterator;

    Pimpl()
    {
    }

    struct DeleteModule
    {
        void operator()(value_type& tmp) {
            delete tmp.second;
            tmp.second = 0;
        }
    };

    struct GetPackageAndLibraryName
    {
        std::multimap < std::string, std::string > *modules;

        GetPackageAndLibraryName(
            std::multimap < std::string, std::string > *modules) :
            modules(modules)
        {}

        void operator()(const value_type& tmp) const
        {
            std::string::size_type pos = tmp.second->path().find('/');

            if (pos == std::string::npos) {
                modules->insert(std::make_pair(
                        "",
                        tmp.second->path()));
            } else {
                modules->insert(std::make_pair(
                        tmp.second->path().substr(0, pos),
                        tmp.second->path().substr(pos)));
            }
        }
    };

    ~Pimpl()
    {
        std::for_each(mTableSimulator.begin(),
                      mTableSimulator.end(),
                      DeleteModule());
        std::for_each(mTableOov.begin(),
                      mTableOov.end(),
                      DeleteModule());
        std::for_each(mTableGvleModeling.begin(),
                      mTableGvleModeling.end(),
                      DeleteModule());
        std::for_each(mTableGvleOutput.begin(),
                      mTableGvleOutput.end(),
                      DeleteModule());

        std::for_each(mTablePackageSimulator.begin(),
                      mTablePackageSimulator.end(),
                      DeleteModule());
        std::for_each(mTablePackageOov.begin(),
                      mTablePackageOov.end(),
                      DeleteModule());
        std::for_each(mTablePackageGvleModeling.begin(),
                      mTablePackageGvleModeling.end(),
                      DeleteModule());
        std::for_each(mTablePackageGvleOutput.begin(),
                      mTablePackageGvleOutput.end(),
                      DeleteModule());
    }

    Module *getModule(const std::string& package,
                      const std::string& library,
                      ModuleType type)
    {
        switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_WRAPPER:
        case MODULE_EXECUTIVE:
            return mTablePackageSimulator.insert(
                std::make_pair < std::string, Module* >(
                    getKeyName(package, library),
                    new Module(package, library, type))).first->second;
        case MODULE_OOV:
        case MODULE_EOV:
            return mTablePackageOov.insert(
                std::make_pair < std::string, Module* >(
                    getKeyName(package, library),
                    new Module(package, library, type))).first->second;
        case MODULE_GVLE_MODELING:
            return mTablePackageGvleModeling.insert(
                std::make_pair < std::string, Module* >(
                    getKeyName(package, library),
                    new Module(package, library, type))).first->second;
        case MODULE_GVLE_OUTPUT:
            return mTablePackageGvleOutput.insert(
                std::make_pair < std::string, Module* >(
                    getKeyName(package, library),
                    new Module(package, library, type))).first->second;
        default:
            throw utils::InternalError();
        }
    }

    void fill(const boost::filesystem::path& package, ModuleType type)
    {
        namespace fs = boost::filesystem;
        namespace ss = boost::system;
        std::string path;

        switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_WRAPPER:
        case MODULE_EXECUTIVE:
            path = Path::path().getExternalPackagePluginSimulatorDir(
#if BOOST_VERSION > 104500
                package.filename().string());
#else
                package.filename());
#endif
            break;
        case MODULE_OOV:
        case MODULE_EOV:
            path = Path::path().getExternalPackagePluginOutputDir(
#if BOOST_VERSION > 104500
                package.filename().string());
#else
                package.filename());
#endif
            break;
        case MODULE_GVLE_MODELING:
            path = Path::path().getExternalPackagePluginGvleModelingDir(
#if BOOST_VERSION > 104500
                package.filename().string());
#else
                package.filename());
#endif
            break;
        case MODULE_GVLE_OUTPUT:
            path = Path::path().getExternalPackagePluginGvleOutputDir(
#if BOOST_VERSION > 104500
                package.filename().string());
#else
                package.filename());
#endif
            break;
        default:
            throw utils::InternalError();
        }

#if BOOST_VERSION > 104500
        ss::error_code ec;
        if (fs::is_directory(path, ec)) {
#else
        if (fs::is_directory(path)) {
#endif
            fs::directory_iterator it(path), end;

            for (; it != end; ++it) {
#if BOOST_VERSION > 103600
                if (fs::is_regular_file(it->status())) {
                    std::string library = getLibraryName(*it);

                    if (not library.empty()) {
                        getModule(package.filename().string(), library, type);
                    }
                }
#else
                if (fs::is_regular(it->status())) {
                    std::string library = getLibraryName(*it);

                    if (not library.empty()) {
                        getModule(package.filename(), library, type);
                    }
                }
#endif
            }
        }
    }

    void fill()
    {
        namespace fs = boost::filesystem;

        {
            fs::path packages = Path::path().getPackagesDir();

            if (fs::is_directory(packages)) {
                fs::directory_iterator it(packages), end;

                for (; it != end; ++it) {
                    if (fs::is_directory(it->status())) {
                        fill(it->path(), MODULE_DYNAMICS);
                        fill(it->path(), MODULE_OOV);
                        fill(it->path(), MODULE_GVLE_MODELING);
                        fill(it->path(), MODULE_GVLE_OUTPUT);
                    }
                }
            }
        }
    }

    void browse(ModuleType type,
                std::multimap < std::string, std::string >* modules)
    {
        switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_WRAPPER:
        case MODULE_EXECUTIVE:
            std::for_each(mTableSimulator.begin(),
                          mTableSimulator.end(),
                          GetPackageAndLibraryName(modules));
            std::for_each(mTablePackageSimulator.begin(),
                          mTablePackageSimulator.end(),
                          GetPackageAndLibraryName(modules));
            break;

        case MODULE_OOV:
        case MODULE_EOV:
            std::for_each(mTableOov.begin(),
                          mTableOov.end(),
                          GetPackageAndLibraryName(modules));
            std::for_each(mTablePackageOov.begin(),
                          mTablePackageOov.end(),
                          GetPackageAndLibraryName(modules));
            break;

        case MODULE_GVLE_MODELING:
            std::for_each(mTableGvleModeling.begin(),
                          mTableGvleModeling.end(),
                          GetPackageAndLibraryName(modules));
            std::for_each(mTablePackageGvleModeling.begin(),
                          mTablePackageGvleModeling.end(),
                          GetPackageAndLibraryName(modules));
            break;

        case MODULE_GVLE_OUTPUT:
            std::for_each(mTableGvleOutput.begin(),
                          mTableGvleOutput.end(),
                          GetPackageAndLibraryName(modules));
            std::for_each(mTablePackageGvleOutput.begin(),
                          mTablePackageGvleOutput.end(),
                          GetPackageAndLibraryName(modules));
            break;

        default:
            break;
        }
    }

    ModuleTable mTableSimulator;
    ModuleTable mTableOov;
    ModuleTable mTableGvleModeling;
    ModuleTable mTableGvleOutput;
    ModuleTable mTablePackageSimulator;
    ModuleTable mTablePackageOov;
    ModuleTable mTablePackageGvleModeling;
    ModuleTable mTablePackageGvleOutput;

    boost::mutex mMutex;
};

ModuleManager::ModuleManager()
    : mPimpl(new ModuleManager::Pimpl())
{
}

ModuleManager::~ModuleManager()
{
    delete mPimpl;
}

void *ModuleManager::get(const std::string& package,
                         const std::string& library,
                         ModuleType type) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    return mPimpl->getModule(package, library, type)->get();
}

ModuleType ModuleManager::determine(const std::string& package,
                                    const std::string& library,
                                    ModuleType type) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    return mPimpl->getModule(package, library, type)->determine();
}


void ModuleManager::fill()
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    mPimpl->fill();
}

void ModuleManager::browse(ModuleType type,
                           std::multimap < std::string, std::string > *modules)
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    mPimpl->browse(type, modules);
}

std::string ModuleManager::buildModuleFilename(const std::string& package,
                                               const std::string& library,
                                               ModuleType type)
{
    namespace fs = boost::filesystem;

    fs::path current;

    switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_WRAPPER:
        case MODULE_EXECUTIVE:
            current = Path::path().getExternalPackagePluginSimulatorDir(
                package);
            break;
        case MODULE_OOV:
        case MODULE_EOV:
            current = Path::path().getExternalPackagePluginOutputDir(
                package);
            break;
        case MODULE_GVLE_MODELING:
            current = Path::path().getExternalPackagePluginGvleModelingDir(
                package);
            break;
        case MODULE_GVLE_OUTPUT:
            current = Path::path().getExternalPackagePluginGvleOutputDir(
                package);
            break;
        default:
            throw utils::InternalError();
    }

#ifdef BOOST_WINDOWS
    std::string filename = "lib" + library + ".dll";
#else
    std::string filename = "lib" + library + ".so";
#endif

    current /= filename;

    return current.string();
}

}} // namespace vle utils
