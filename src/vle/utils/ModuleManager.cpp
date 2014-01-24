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


#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/version.hpp>

#ifdef BOOST_WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace fs = boost::filesystem;

namespace vle { namespace utils { namespace pimpl {

/**
 * @brief Get the plug-in name of the filename provided.
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

class Module
{
public:
    std::string mPath;
    std::string mPackage;
    std::string mLibrary;

#ifdef BOOST_WINDOWS
    HMODULE mHandle;
#else
    void *mHandle;
#endif

    void *mFunction;
    ModuleType mType;

    /**
     * @brief A Module store shared library and symbol.
     *
     * Build a new Module for a specified path build from the package name, the
     * library name and the type of the module. The shared library is not read.
     * To read the shared library and get symbol, use the Module::get()
     * function.
     *
     * @param package
     * @param library
     * @param type
     */
    Module(const std::string& path, const std::string& package, const
           std::string& library, ModuleType type)
        : mPath(path), mPackage(package), mLibrary(library), mHandle(0),
        mFunction(0), mType(type)
    {
    }

    /**
     * @brief Delete the instance of the shared library.
     */
    ~Module()
    {
        DTraceModel(vle::fmt("ModuleManager unload: %1%:%2%") % mHandle %
                mPath);

        if (mHandle) {
#ifdef BOOST_WINDOWS
            FreeLibrary(mHandle);
#else
            dlclose(mHandle);
#endif
        }
    }

    /**
     * @brief Get the symbol specified from the shared library.
     *
     * If the shared library was nether opened, it was open.
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

        if (not mFunction) {
            switch (mType) {
            case MODULE_DYNAMICS:
                if (not (mFunction = (getSymbol("vle_make_new_dynamics")))) {
                    if (not (mFunction = (getSymbol("vle_make_new_executive")))) {
                        if (not (mFunction = (getSymbol("vle_make_new_dynamics_wrapper")))) {
                            throw utils::InternalError(fmt(
                                    _("Module `%1%' is not a dynamic module"
                                      " (symbol vle_make_new_dynamics,"
                                      " vle_make_new_executive or"
                                      " vle_make_new_dynamics_wrapper are not"
                                      " found")) % mPath);

                        } else {
                            mType = MODULE_DYNAMICS_WRAPPER;
                        }
                    } else {
                        mType = MODULE_DYNAMICS_EXECUTIVE;
                    }
                } else {
                    mType = MODULE_DYNAMICS;
                }
                break;
            case MODULE_DYNAMICS_EXECUTIVE:
                if (not (mFunction = (getSymbol("vle_make_new_executive")))) {
                    if (not (mFunction = (getSymbol("vle_make_new_dynamics")))) {
                        if (not (mFunction = (getSymbol("vle_make_new_dynamics_wrapper")))) {
                            throw utils::InternalError(fmt(
                                    _("Module `%1%' is not a dynamic module"
                                      " (symbol vle_make_new_dynamics,"
                                      " vle_make_new_executive or"
                                      " vle_make_new_dynamics_wrapper are not"
                                      " found")) % mPath);

                        } else {
                            mType = MODULE_DYNAMICS_WRAPPER;
                        }
                    } else {
                        mType = MODULE_DYNAMICS;
                    }
                } else {
                    mType = MODULE_DYNAMICS_EXECUTIVE;
                }
                break;
            case MODULE_DYNAMICS_WRAPPER:
                if (not (mFunction = (getSymbol("vle_make_new_dynamics_wrapper")))) {
                    if (not (mFunction = (getSymbol("vle_make_new_dynamics")))) {
                        if (not (mFunction = (getSymbol("vle_make_new_executive")))) {
                            throw utils::InternalError(fmt(
                                    _("Module `%1%' is not a dynamic module"
                                      " (symbol vle_make_new_dynamics,"
                                      " vle_make_new_executive or"
                                      " vle_make_new_dynamics_wrapper are not"
                                      " found")) % mPath);
                        } else {
                            mType = MODULE_DYNAMICS_EXECUTIVE;
                        }
                    } else {
                        mType = MODULE_DYNAMICS;
                    }
                } else {
                    mType = MODULE_DYNAMICS_WRAPPER;
                }
                break;
            case MODULE_OOV:
                if (not (mFunction = (getSymbol("vle_make_new_oov")))) {
                    throw utils::InternalError(fmt(
                            _("Module `%1%' is not an oov module (symbol"
                              " vle_make_new_oov not found)")) % mPath);
                }
                break;
            case MODULE_GVLE_GLOBAL:
                if (not (mFunction = (getSymbol("vle_make_new_gvle_global")))) {
                    throw utils::InternalError(fmt(
                            _("Module: `%1%' is not a GVLE global module"
                              " (symbol vle_make_new_gvle_global not found)"))
                        % mPath);
                }
                break;
            case MODULE_GVLE_MODELING:
                if (not (mFunction = (getSymbol("vle_make_new_gvle_modeling")))) {
                    throw utils::InternalError(fmt(
                            _("Module: `%1%' is not a GVLE modeling module"
                              " (symbol vle_make_new_gvle_modeling not found)"))
                        % mPath);
                }
                break;
            case MODULE_GVLE_OUTPUT:
                if (not (mFunction = (getSymbol("vle_make_new_gvle_output")))) {
                    throw utils::InternalError(fmt(
                            _("Module: `%1%' is not a GVLE output module"
                              " (symbol vle_make_new_gvle_output not found)"))
                        % mPath);
                }
                break;
            default:
                throw utils::InternalError();
            }
        }

        return mFunction;
    }

    bool operator==(const Module& other) const
    {
        return mPath == other.mPath;
    }

    bool operator<(const Module& other) const
    {
        return mPath < other.mPath;
    }

    void init()
    {
        assert(not mHandle);

#ifdef BOOST_WINDOWS
        HMODULE handle = ::LoadLibrary(mPath.c_str());
#else
        void *handle = ::dlopen(mPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif

        DTraceModel(vle::fmt("ModuleManager load: %1%:%2%") % handle % mPath);

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

            extra.assign((LPTSTR)msg);
            ::LocalFree(msg);
#else
            char *error = ::dlerror();
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
        void *symbol;

        uint32_t major, minor, patch;
        versionFunction fct;

        symbol = getSymbol("vle_api_level");

        if (not symbol) {
            throw utils::InternalError(fmt(
                    _("Module `%1%' does not have a vle_api_level symbol. "
                      "This module seems to be too old.")) % mPath);
        }

#ifdef BOOST_WINDOWS
        fct = (versionFunction)symbol;
#else
        fct = utils::functionCast < versionFunction >(symbol);
#endif
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

    /**
     * @brief Get a symbol in the opened shared library.
     *
     * @param dsymbol The symbol to get from the shared library.
     *
     * @return 0 if the symbol was not found, !0 otherwise.
     */
    void *getSymbol(const char *symbol)
    {
#ifdef BOOST_WINDOWS
        return (void*)::GetProcAddress(mHandle, symbol);
#else
        return ::dlsym(mHandle, symbol);
#endif
    }
};

class SymbolTable
{
public:
    typedef std::map < std::string, void* > Container;
    typedef std::map < std::string, void* >::const_iterator const_iterator;
    typedef std::map < std::string, void* >::iterator iterator;
    typedef std::map < std::string, void* >::size_type size_type;

    SymbolTable()
    {
    }

    ~SymbolTable()
    {
    }

    void *get(const std::string& symbol)
    {
        const_iterator it = mLst.find(symbol);

        if (it != mLst.end()) {
            return it->second;
        } else {
            void *result = 0;
#ifdef BOOST_WINDOWS
            FARPROC ptr = ::GetProcAddress(NULL, symbol.c_str());
            result = (void*)ptr;
#else
            result = ::dlsym(NULL, symbol.c_str());
#endif

            if (result) {
                mLst.insert(
                    std::make_pair < std::string, void* >(symbol, result));
                return result;
            } else {
                throw utils::InternalError(fmt(
                        _("Module: `%1%' not found in global space")) % symbol);
            }
        }
    }

    Container mLst;

private:
    SymbolTable(const SymbolTable&);
    SymbolTable& operator=(const SymbolTable&);
};

} // namespace pimpl

class ModuleManager::Pimpl
{
public:
    typedef boost::unordered_map < std::string, pimpl::Module* > ModuleTable;
    typedef ModuleTable::value_type value_type;
    typedef ModuleTable::const_iterator const_iterator;
    typedef ModuleTable::iterator iterator;

    typedef std::map < std::string, void* > MapSymbol;

    struct ModuleDeleter
    {
        void operator()(value_type& x) const
        {
            delete x.second;
            x.second = 0;
        }
    };

    /**
     * @brief An unary operator to convert pimpl::Module into Module.
     */
    struct ModuleConvert
    {
        Module operator()(const ModuleTable::value_type& module) const
        {
            return Module(module.second->mPackage, module.second->mLibrary,
                          module.second->mPath, module.second->mType);
        }
    };

    /**
     * @brief A predicate to check if a pimpl::Module is in specified package.
     */
    struct ModuleIsInPackage
    {
        const std::string& mPackage;

        ModuleIsInPackage(const std::string& package)
            : mPackage(package)
        {
        }

        bool operator()(const ModuleTable::value_type& module) const
        {
            return module.second->mPackage == mPackage;
        }
    };

    Pimpl()
    {
    }

    ~Pimpl()
    {
        std::for_each(mTableSimulator.begin(),
                      mTableSimulator.end(),
                      ModuleDeleter());
        std::for_each(mTableOov.begin(),
                      mTableOov.end(),
                      ModuleDeleter());
        std::for_each(mTableGvleGlobal.begin(),
                      mTableGvleGlobal.end(),
                      ModuleDeleter());
        std::for_each(mTableGvleModeling.begin(),
                      mTableGvleModeling.end(),
                      ModuleDeleter());
        std::for_each(mTableGvleOutput.begin(),
                      mTableGvleOutput.end(),
                      ModuleDeleter());
    }

    pimpl::Module *getModule(const std::string& package,
                             const std::string& library,
                             ModuleType type)
    {
        std::string path = buildModuleFilename(package, library, type);
        iterator it;

        switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_WRAPPER:
        case MODULE_DYNAMICS_EXECUTIVE:
            it = mTableSimulator.find(path);
            if (it != mTableSimulator.end()) {
                return it->second;
            } else {
                return mTableSimulator.insert(
                    std::make_pair < std::string, pimpl::Module* >(
                        path, new pimpl::Module(path, package, library,
                                                type))).first->second;
            }
            break;
        case MODULE_OOV:
            it = mTableOov.find(path);
            if (it != mTableOov.end()) {
                return it->second;
            } else {
                return mTableOov.insert(
                    std::make_pair < std::string, pimpl::Module* >(
                        path, new pimpl::Module(path, package, library,
                                                type))).first->second;
            }
            break;
        case MODULE_GVLE_GLOBAL:
            it = mTableGvleGlobal.find(path);
            if (it != mTableGvleGlobal.end()) {
                return it->second;
            } else {
                return mTableGvleGlobal.insert(
                    std::make_pair < std::string, pimpl::Module* >(
                        path, new pimpl::Module(path, package, library,
                                                type))).first->second;
            }
            break;
        case MODULE_GVLE_MODELING:
            it = mTableGvleModeling.find(path);
            if (it != mTableGvleModeling.end()) {
                return it->second;
            } else {
                return mTableGvleModeling.insert(
                    std::make_pair < std::string, pimpl::Module* >(
                        path, new pimpl::Module(path, package, library,
                                                type))).first->second;
            }
            break;
        case MODULE_GVLE_OUTPUT:
            it = mTableGvleOutput.find(path);
            if (it != mTableGvleOutput.end()) {
                return it->second;
            } else {
                return mTableGvleOutput.insert(
                    std::make_pair < std::string, pimpl::Module* >(
                        path, new pimpl::Module(path, package, library,
                                                type))).first->second;
            }
            break;
        default:
            throw utils::InternalError();
        }
        throw utils::InternalError();
    }

    void *getSymbol(const std::string& symbol)
    {
        return mTableSymbols.get(symbol);
    }

    /**
     * @brief Browse a directory of a package.
     *
     * @param package
     * @param package
     * @param type
     */
    void browse(const boost::filesystem::path& path,
                const std::string& package,
                ModuleType type)
    {
        if (fs::is_directory(path)) {
            fs::directory_iterator it(path), end;
            for (; it != end; ++it) {
                if (fs::is_regular(it->status())) {
                    std::string library = pimpl::getLibraryName(*it);

                    if (not library.empty()) {
                        getModule(package, library, type);
                    }
                }
            }
        }
    }

    ModuleTable mTableSimulator;
    ModuleTable mTableOov;
    ModuleTable mTableGvleGlobal;
    ModuleTable mTableGvleModeling;
    ModuleTable mTableGvleOutput;

    pimpl::SymbolTable mTableSymbols;

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
                         ModuleType type,
                         ModuleType *newtype) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);


    if (not newtype) {
        return mPimpl->getModule(package, library, type)->get();
    } else {
        pimpl::Module *module = mPimpl->getModule(package, library, type);
        void *result = module->get();
        *newtype = module->mType;

        return result;
    }
}

void *ModuleManager::get(const std::string& symbol)
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    return mPimpl->getSymbol(symbol);
}

void ModuleManager::browse()
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    fs::path packages = utils::Path::path().getBinaryPackagesDir();

    fs::path pathsim = "plugins/simulator";
    fs::path pathoov = "plugins/output";
    fs::path pathgvleg = "plugins/gvle/global";
    fs::path pathgvlem = "plugins/gvle/modeling";
    fs::path pathgvleo = "plugins/gvle/output";

    if (fs::is_directory(packages)) {
        fs::directory_iterator it(packages), end;

        for (; it != end; ++it) { /* for each package */
            if (fs::is_directory(it->status())) {
#if BOOST_VERSION > 104500
                std::string package = it->path().filename().string();
#else
                std::string package = it->path().file_string();
#endif

                {
#if BOOST_VERSION > 104500
                    fs::path tmp = (*it) / pathsim;
#else
                    fs::path tmp = (*it);
                    tmp /= pathsim;
#endif
                    mPimpl->browse(tmp, package, MODULE_DYNAMICS);
                }

                {
#if BOOST_VERSION > 104500
                    fs::path tmp = (*it) / pathoov;
#else
                    fs::path tmp = (*it);
                    tmp /= pathoov;
#endif
                    mPimpl->browse(tmp, package, MODULE_OOV);
                }

                {
#if BOOST_VERSION > 104500
                    fs::path tmp = (*it) / pathgvleg;
#else
                    fs::path tmp = (*it);
                    tmp /= pathgvleg;
#endif
                    mPimpl->browse(tmp, package, MODULE_GVLE_GLOBAL);
                }

                {
#if BOOST_VERSION > 104500
                    fs::path tmp = (*it) / pathgvlem;
#else
                    fs::path tmp = (*it);
                    tmp /= pathgvlem;
#endif
                    mPimpl->browse(tmp, package, MODULE_GVLE_MODELING);
                }

                {
#if BOOST_VERSION > 104500
                    fs::path tmp = (*it) / pathgvleo;
#else
                    fs::path tmp = (*it);
                    tmp /= pathgvleo;
#endif
                    mPimpl->browse(tmp, package, MODULE_GVLE_OUTPUT);
                }
            }
        }
    }
}

void ModuleManager::browse(ModuleType type)
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    fs::path packages = utils::Path::path().getBinaryPackagesDir();

    fs::path pathtype;

    switch (type) {
    case MODULE_DYNAMICS:
    case MODULE_DYNAMICS_EXECUTIVE:
    case MODULE_DYNAMICS_WRAPPER:
        pathtype = "plugins/simulator";
        break;
    case MODULE_OOV:
        pathtype = "plugins/output";
        break;
    case MODULE_GVLE_GLOBAL:
        pathtype = "plugins/gvle/global";
        break;
    case MODULE_GVLE_MODELING:
        pathtype = "plugins/gvle/modeling";
        break;
    case MODULE_GVLE_OUTPUT:
        pathtype = "plugins/gvle/output";
        break;
    }

    if (fs::is_directory(packages)) {
        fs::directory_iterator it(packages), end;

        for (; it != end; ++it) { /* for each package */
            if (fs::is_directory(it->status())) {
                fs::path pkg = (*it);
                pkg /= pathtype;

#if BOOST_VERSION > 104500
                mPimpl->browse(pkg, it->path().filename().string(), type);
#else
                mPimpl->browse(pkg, it->path().file_string(), type);
#endif
            }
        }
    }
}

void ModuleManager::browse(const std::string& package)
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    fs::path pkg = utils::Path::path().getBinaryPackagesDir();
    pkg /= package;
    pkg /= "plugins";

    {
        fs::path tmp = pkg;
        tmp /= "simulator";
        mPimpl->browse(tmp, package, MODULE_DYNAMICS);
    }

    {
        fs::path tmp = pkg;
        tmp /= "output";
        mPimpl->browse(tmp, package, MODULE_OOV);
    }

    {
        fs::path tmp = pkg;
        tmp /= "gvle/global";
        mPimpl->browse(tmp, package, MODULE_GVLE_GLOBAL);
    }

    {
        fs::path tmp = pkg;
        tmp /= "gvle/modeling";
        mPimpl->browse(tmp, package, MODULE_GVLE_MODELING);
    }

    {
        fs::path tmp = pkg;
        tmp /= "gvle/output";
        mPimpl->browse(tmp, package, MODULE_GVLE_OUTPUT);
    }

}

void ModuleManager::browse(const std::string& package, ModuleType type)
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    fs::path pkg = utils::Path::path().getBinaryPackagesDir();
    pkg /= package;
    pkg /= "plugins";

    switch (type) {
    case MODULE_DYNAMICS:
    case MODULE_DYNAMICS_EXECUTIVE:
    case MODULE_DYNAMICS_WRAPPER:
        pkg /= "simulator";
        break;
    case MODULE_OOV:
        pkg /= "output";
        break;
    case MODULE_GVLE_GLOBAL:
        pkg /= "gvle/global";
        break;
    case MODULE_GVLE_MODELING:
        pkg /= "gvle/modeling";
        break;
    case MODULE_GVLE_OUTPUT:
        pkg /= "gvle/output";
        break;
    }

    mPimpl->browse(pkg, package, type);
}

void ModuleManager::fill(ModuleList *lst) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    std::transform(mPimpl->mTableSimulator.begin(),
                   mPimpl->mTableSimulator.end(),
                   std::back_inserter(*lst),
                   ModuleManager::Pimpl::ModuleConvert());

    std::transform(mPimpl->mTableOov.begin(),
                   mPimpl->mTableOov.end(),
                   std::back_inserter(*lst),
                   ModuleManager::Pimpl::ModuleConvert());

    std::transform(mPimpl->mTableGvleGlobal.begin(),
                   mPimpl->mTableGvleGlobal.end(),
                   std::back_inserter(*lst),
                   ModuleManager::Pimpl::ModuleConvert());

    std::transform(mPimpl->mTableGvleModeling.begin(),
                   mPimpl->mTableGvleModeling.end(),
                   std::back_inserter(*lst),
                   ModuleManager::Pimpl::ModuleConvert());

    std::transform(mPimpl->mTableGvleOutput.begin(),
                   mPimpl->mTableGvleOutput.end(),
                   std::back_inserter(*lst),
                   ModuleManager::Pimpl::ModuleConvert());
}

void ModuleManager::fill(ModuleType type, ModuleList *lst) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    switch (type) {
    case MODULE_DYNAMICS:
    case MODULE_DYNAMICS_EXECUTIVE:
    case MODULE_DYNAMICS_WRAPPER:
        std::transform(mPimpl->mTableSimulator.begin(),
                       mPimpl->mTableSimulator.end(),
                       std::back_inserter(*lst),
                       ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_OOV:
        std::transform(mPimpl->mTableOov.begin(),
                       mPimpl->mTableOov.end(),
                       std::back_inserter(*lst),
                       ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_GLOBAL:
        std::transform(mPimpl->mTableGvleGlobal.begin(),
                       mPimpl->mTableGvleGlobal.end(),
                       std::back_inserter(*lst),
                       ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_MODELING:
        std::transform(mPimpl->mTableGvleModeling.begin(),
                       mPimpl->mTableGvleModeling.end(),
                       std::back_inserter(*lst),
                       ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_OUTPUT:
        std::transform(mPimpl->mTableGvleOutput.begin(),
                       mPimpl->mTableGvleOutput.end(),
                       std::back_inserter(*lst),
                       ModuleManager::Pimpl::ModuleConvert());
        break;
    }
}

void ModuleManager::fill(const std::string& package, ModuleList *lst) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    transformIf(mPimpl->mTableSimulator.begin(),
                mPimpl->mTableSimulator.end(),
                std::back_inserter(*lst),
                ModuleManager::Pimpl::ModuleIsInPackage(package),
                ModuleManager::Pimpl::ModuleConvert());

    transformIf(mPimpl->mTableOov.begin(),
                mPimpl->mTableOov.end(),
                std::back_inserter(*lst),
                ModuleManager::Pimpl::ModuleIsInPackage(package),
                ModuleManager::Pimpl::ModuleConvert());

    transformIf(mPimpl->mTableGvleGlobal.begin(),
                mPimpl->mTableGvleGlobal.end(),
                std::back_inserter(*lst),
                ModuleManager::Pimpl::ModuleIsInPackage(package),
                ModuleManager::Pimpl::ModuleConvert());

    transformIf(mPimpl->mTableGvleModeling.begin(),
                mPimpl->mTableGvleModeling.end(),
                std::back_inserter(*lst),
                ModuleManager::Pimpl::ModuleIsInPackage(package),
                ModuleManager::Pimpl::ModuleConvert());

    transformIf(mPimpl->mTableGvleOutput.begin(),
                mPimpl->mTableGvleOutput.end(),
                std::back_inserter(*lst),
                ModuleManager::Pimpl::ModuleIsInPackage(package),
                ModuleManager::Pimpl::ModuleConvert());
}

void ModuleManager::fill(const std::string& package, ModuleType type,
                         ModuleList *lst) const
{
    boost::mutex::scoped_lock lock(mPimpl->mMutex);

    switch (type) {
    case MODULE_DYNAMICS:
    case MODULE_DYNAMICS_EXECUTIVE:
    case MODULE_DYNAMICS_WRAPPER:
        transformIf(mPimpl->mTableSimulator.begin(),
                    mPimpl->mTableSimulator.end(),
                    std::back_inserter(*lst),
                    ModuleManager::Pimpl::ModuleIsInPackage(package),
                    ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_OOV:
        transformIf(mPimpl->mTableOov.begin(),
                    mPimpl->mTableOov.end(),
                    std::back_inserter(*lst),
                    ModuleManager::Pimpl::ModuleIsInPackage(package),
                    ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_GLOBAL:
        transformIf(mPimpl->mTableGvleGlobal.begin(),
                    mPimpl->mTableGvleGlobal.end(),
                    std::back_inserter(*lst),
                    ModuleManager::Pimpl::ModuleIsInPackage(package),
                    ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_MODELING:
        transformIf(mPimpl->mTableGvleModeling.begin(),
                    mPimpl->mTableGvleModeling.end(),
                    std::back_inserter(*lst),
                    ModuleManager::Pimpl::ModuleIsInPackage(package),
                    ModuleManager::Pimpl::ModuleConvert());
        break;
    case MODULE_GVLE_OUTPUT:
        transformIf(mPimpl->mTableGvleOutput.begin(),
                    mPimpl->mTableGvleOutput.end(),
                    std::back_inserter(*lst),
                    ModuleManager::Pimpl::ModuleIsInPackage(package),
                    ModuleManager::Pimpl::ModuleConvert());
        break;
    }
}

std::string ModuleManager::buildModuleFilename(const std::string& package,
                                               const std::string& library,
                                               ModuleType type)
{
    fs::path current;
    vle::utils::Package pkg(package);

    switch (type) {
        case MODULE_DYNAMICS:
        case MODULE_DYNAMICS_EXECUTIVE:
        case MODULE_DYNAMICS_WRAPPER:
            current = pkg.getPluginSimulatorDir(PKG_BINARY);
            break;
        case MODULE_OOV:
            current = pkg.getPluginOutputDir(PKG_BINARY);
            break;
        case MODULE_GVLE_GLOBAL:
            current = pkg.getPluginGvleGlobalDir(PKG_BINARY);
            break;
        case MODULE_GVLE_MODELING:
            current = pkg.getPluginGvleModelingDir(PKG_BINARY);
            break;
        case MODULE_GVLE_OUTPUT:
            current = pkg.getPluginGvleOutputDir(PKG_BINARY);
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
