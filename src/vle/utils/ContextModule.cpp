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
#include <unordered_map>
#include <vle/utils/Algo.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace vle {
namespace utils {

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
static std::string
getLibraryName(const Path& file)
{
    std::string library;

    if (file.filename().compare(0, 3, "lib") == 0)
        library.append(file.filename(), 3, std::string::npos);

#if defined(_WIN32)
    if (file.extension() == ".dll")
        library.assign(library, 0, library.size() - 4);
#elif __APPLE__
    if (file.extension() == ".dylib")
        library.assign(library, 0, library.size() - 6);
#else
    if (file.extension() == ".so")
        library.assign(library, 0, library.size() - 3);
#endif

    return library;
}

struct Module
{
    Path mPath;
    std::string mPackage;
    std::string mLibrary;

#ifdef _WIN32
    HMODULE mHandle;
#else
    void* mHandle;
#endif

    void* mFunction;
    Context::ModuleType mType;

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
    Module(Path path,
           std::string package,
           std::string library,
           Context::ModuleType type)
      : mPath(std::move(path))
      , mPackage(std::move(package))
      , mLibrary(std::move(library))
      , mHandle(nullptr)
      , mFunction(nullptr)
      , mType(type)
    {
    }

    /**
     * @brief Delete the instance of the shared library.
     */
    ~Module() noexcept
    {
        if (mHandle) {
#ifdef _WIN32
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
            case Context::ModuleType::MODULE_DYNAMICS:
            case Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
            case Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
                if (!(mFunction = (getSymbol("vle_make_new_dynamics"))))
                    if (!(mFunction = (getSymbol("vle_make_new_executive"))))
                        if (!(mFunction =
                                (getSymbol("vle_make_new_dynamics_wrapper"))))
                            throw utils::InternalError(
                              (fmt(_("Module `%1%' is not a dynamic module"
                                     " (symbol vle_make_new_dynamics,"
                                     " vle_make_new_executive or"
                                     " vle_make_new_dynamics_wrapper are not"
                                     " found")) %
                               mPath.string())
                                .str());

                        else
                            mType =
                              Context::ModuleType::MODULE_DYNAMICS_WRAPPER;
                    else
                        mType = Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE;
                else
                    mType = Context::ModuleType::MODULE_DYNAMICS;
                break;
            case Context::ModuleType::MODULE_OOV:
                if (not(mFunction = (getSymbol("vle_make_new_oov"))))
                    throw utils::InternalError(
                      (fmt(_("Module `%1%' is not an oov module (symbol"
                             " vle_make_new_oov not found)")) %
                       mPath.string())
                        .str());
                break;
            default:
                throw utils::InternalError(_("Missing type"));
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

#ifdef _WIN32
        HMODULE handle = ::LoadLibrary(mPath.string().c_str());
#else
        void* handle =
          ::dlopen(mPath.string().c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif

        if (not handle) {
            std::string extra;
#ifdef _WIN32
            DWORD errorcode = ::GetLastError();
            LPVOID msg;

            ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            errorcode,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR)&msg,
                            0,
                            NULL);

            extra.assign((LPTSTR)msg);
            ::LocalFree(msg);
#else
            char* error = ::dlerror();
            if (error) {
                extra.assign(error);
            }
#endif
            throw utils::InternalError(
              (fmt(_("Module can not open shared library `%1%': %2%")) %
               mPath.string() % extra)
                .str());
        } else {
            mHandle = handle;
        }
    }

    void checkVersion()
    {
        typedef void (*versionFunction)(
          std::uint32_t*, std::uint32_t*, std::uint32_t*);
        void* symbol;

        auto version = vle::version();
        uint32_t major, minor, patch;

        versionFunction fct;

        symbol = getSymbol("vle_api_level");

        if (not symbol) {
            throw utils::InternalError(
              (fmt(_("Module `%1%' does not have a vle_api_level symbol. "
                     "This module seems to be too old.")) %
               mPath.string())
                .str());
        }

#ifdef _WIN32
        fct = (versionFunction)symbol;
#else
        fct = utils::functionCast<versionFunction>(symbol);
#endif
        fct(&major, &minor, &patch);

        if (major != static_cast<std::uint32_t>(std::get<0>(version)) or
            minor != static_cast<std::uint32_t>(std::get<1>(version))) {
            throw utils::InternalError(
              (fmt(_("Module: `%1%' was produced with VLE %2%.%3%.%4% and is"
                     " not API/ABI compatible with the current VLE"
                     " %5%.%6%.%7%")) %
               mPath.string() % major % minor % patch % std::get<0>(version) %
               std::get<1>(version) % std::get<2>(version))
                .str());
        } else if (patch != static_cast<std::uint32_t>(std::get<2>(version))) {
            // utils::Trace::send(
            //     (fmt(_("Module: `%1%' was produced with VLE %2%.%3%.%4% and
            //     may"
            //            " not be API/ABI compatible with the current VLE"
            //            " %5%.%6%.%7%")) % mPath % major % minor % patch %
            //      VLE_MAJOR_VERSION % VLE_MINOR_VERSION %
            //      VLE_PATCH_VERSION).str());
        }
    }

    /**
     * @brief Get a symbol in the opened shared library.
     *
     * @param dsymbol The symbol to get from the shared library.
     *
     * @return 0 if the symbol was not found, !0 otherwise.
     */
    void* getSymbol(const char* symbol)
    {
#ifdef _WIN32
        return (void*)::GetProcAddress(mHandle, symbol);
#else
        return ::dlsym(mHandle, symbol);
#endif
    }
};

struct SymbolTable
{
public:
    using container = std::unordered_map<std::string, void*>;
    using const_iterator = container::const_iterator;
    using iterator = container::iterator;
    using size_type = container::size_type;

    container mLst;

    SymbolTable() = default;
    ~SymbolTable() = default;

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    void* get(const std::string& symbol)
    {
        const_iterator it = mLst.find(symbol);

        // Already in cache, returns the symbol.
        if (it != mLst.end())
            return it->second;

        // Otherwise, try to found symbol into the current process symbol
        // table.

        void* result = nullptr;

#ifdef _WIN32
        FARPROC ptr = ::GetProcAddress(NULL, symbol.c_str());
        result = (void*)ptr;
#else
        result = ::dlsym(nullptr, symbol.c_str());
#endif

        if (not result)
            throw utils::InternalError(
              (fmt(_("Module: `%1%' not found in global space")) % symbol)
                .str());

        auto ret = mLst.emplace(symbol, result);

        if (not ret.second)
            assert(false && "emplace failure but find returns true");

        return result;
    }
};

struct ModuleManager
{
    using ModuleTable =
      std::unordered_map<std::string, std::unique_ptr<Module>>;
    using value_type = ModuleTable::value_type;
    using const_iterator = ModuleTable::const_iterator;
    using iterator = ModuleTable::iterator;

    Context* mContext;
    ModuleTable mTableSimulator;
    ModuleTable mTableOov;
    SymbolTable mTableSymbols;

    ModuleManager(Context* ctx)
      : mContext(ctx)
    {
        assert(ctx);
    }

    ~ModuleManager() noexcept
    {
        if (mContext)
            return;

        for (const auto& elem : mTableSimulator)
            if (elem.second) {
                vDbg(mContext,
                     _("ModuleManager: simulator unload %s (%s %s %s)\n"),
                     elem.first.c_str(),
                     elem.second->mPath.string().c_str(),
                     elem.second->mPackage.c_str(),
                     elem.second->mLibrary.c_str());
            } else {
                vDbg(mContext,
                     _("ModuleManager: simulator unload %s (not loaded)\n"),
                     elem.first.c_str());
            }

        for (const auto& elem : mTableOov)
            if (elem.second) {
                vDbg(mContext,
                     _("ModuleManager: output unload %s (%s %s %s)\n"),
                     elem.first.c_str(),
                     elem.second->mPath.string().c_str(),
                     elem.second->mPackage.c_str(),
                     elem.second->mLibrary.c_str());
            } else {
                vDbg(mContext,
                     _("ModuleManager: output unload %s (not loaded)\n"),
                     elem.first.c_str());
            }
    }

    /**
     * @brief An unary operator to convert pimpl::Module into Module.
     */
    struct ModuleConvert
    {
        Context::Module operator()(const ModuleTable::value_type& module) const
        {
            return { module.second->mPackage,
                     module.second->mLibrary,
                     module.second->mPath,
                     module.second->mType };
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

    bool exists(const std::string& filepath) const
    {
        auto it = mTableSimulator.find(filepath);
        if (it != mTableSimulator.cend())
            return true;

        return mTableOov.find(filepath) != mTableOov.cend();
    }

    const std::unique_ptr<Module>& getModule(const std::string& package,
                                             const std::string& library,
                                             Context::ModuleType type)
    {
        Path path = buildModuleFilename(package, library, type);
        std::string strpath = path.string();

        iterator it;

        switch (type) {
        case Context::ModuleType::MODULE_DYNAMICS:
        case Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
        case Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
            it = mTableSimulator.find(strpath);
            if (it != mTableSimulator.end())
                return it->second;

            return mTableSimulator
              .emplace(strpath,
                       std::make_unique<Module>(path, package, library, type))
              .first->second;
            break;
        case Context::ModuleType::MODULE_OOV:
            it = mTableOov.find(strpath);
            if (it != mTableOov.end())
                return it->second;

            return mTableOov
              .emplace(strpath,
                       std::make_unique<Module>(path, package, library, type))
              .first->second;
            break;
        default:
            break;
        }

        throw utils::InternalError(_("Missing type"));
    }

    void* getSymbol(const std::string& symbol)
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
    void browse(const Path& path,
                const std::string& package,
                Context::ModuleType type)
    {
        if (not path.is_directory())
            return;

        DirectoryIterator it(path), end;
        for (; it != end; ++it) {
            if (it->is_file()) {
                std::string library = getLibraryName(*it);

                if (not library.empty()) {
                    getModule(package, library, type);
                }
            }
        }
    }

    Path buildModuleFilename(const std::string& package,
                             const std::string& library,
                             Context::ModuleType type)
    {
        const auto& paths = mContext->getBinaryPackagesDir();
        for (const auto& elem : paths) {
            Path current = elem / package;

            if (not current.is_directory()) // if package does not exists
                continue;                   // in repository test the next.

            switch (type) {
            case Context::ModuleType::MODULE_DYNAMICS:
            case Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
            case Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
                current /= "plugins";
                current /= "simulator";
                break;
            case Context::ModuleType::MODULE_OOV:
                current /= "plugins";
                current /= "output";
                break;
            default:
                throw utils::InternalError(_("Missing type"));
            }

#if defined(_WIN32)
            std::string filename = "lib" + library + ".dll";
#elif defined(__APPLE__)
            std::string filename = "lib" + library + ".dylib";
#else
            std::string filename = "lib" + library + ".so";
#endif

            current /= filename;

            if (not current.is_file()) {
                vDbg(mContext,
                     _("ModuleManager: library %s is missing"
                       " in binary package %s in %s\n"),
                     library.c_str(),
                     package.c_str(),
                     current.string().c_str());
                continue;
            }

            return current;
        }

        throw utils::FileError(
          (fmt(_("ModuleManager: library %1% package %2% not found"
                 "in binary repositories")) %
           library % package)
            .str());
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    void browse()
    {
        const auto& paths = mContext->getBinaryPackagesDir();

        for (const auto& elem : paths) {
            Path packages = elem;
            Path pathsim = "plugins/simulator";
            Path pathoov = "plugins/output";

            if (not packages.is_directory()) {
                vErr(mContext,
                     _("ModuleManager: %s is not a packages "
                       "directory\n"),
                     elem.string().c_str());
                continue;
            }

            DirectoryIterator it(elem), end;
            for (; it != end; ++it) {
                if (not it->is_directory())
                    continue;

                std::string package = it->path().filename();
                if (exists(it->path().string())) {
                    vInfo(mContext,
                          _("ModuleManager: package `%s' already"
                            " exists. Forget this path: `%s'\n"),
                          package.c_str(),
                          it->path().string().c_str());
                    continue;
                }

                {
                    Path tmp = (*it) / "plugins" / "simulator";
                    browse(tmp, package, Context::ModuleType::MODULE_DYNAMICS);
                }

                {
                    Path tmp = (*it) / "plugins" / "output";
                    browse(tmp, package, Context::ModuleType::MODULE_OOV);
                }
            }
        }
    }

    /*
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    /**
     * @brief Retrieve the list of all modules.
     *
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(std::vector<Context::Module>* lst) const
    {
        std::transform(mTableSimulator.begin(),
                       mTableSimulator.end(),
                       std::back_inserter(*lst),
                       ModuleManager::ModuleConvert());

        std::transform(mTableOov.begin(),
                       mTableOov.end(),
                       std::back_inserter(*lst),
                       ModuleManager::ModuleConvert());
    }

    /**
     * @brief Retrieve the list of modules of the specified type.
     *
     * @param type The type of module to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(Context::ModuleType type,
              std::vector<Context::Module>* lst) const
    {
        switch (type) {
        case Context::ModuleType::MODULE_DYNAMICS:
        case Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
        case Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
            std::transform(mTableSimulator.begin(),
                           mTableSimulator.end(),
                           std::back_inserter(*lst),
                           ModuleManager::ModuleConvert());
            break;
        case Context::ModuleType::MODULE_OOV:
            std::transform(mTableOov.begin(),
                           mTableOov.end(),
                           std::back_inserter(*lst),
                           ModuleManager::ModuleConvert());
            break;
        default:
            break;
        };
    }

    /**
     * @brief Retrieve the list of modules of the specified package.
     *
     * @param package The name of the package to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(const std::string& package,
              std::vector<Context::Module>* lst) const
    {
        transformIf(mTableSimulator.begin(),
                    mTableSimulator.end(),
                    std::back_inserter(*lst),
                    ModuleManager::ModuleIsInPackage(package),
                    ModuleManager::ModuleConvert());

        transformIf(mTableOov.begin(),
                    mTableOov.end(),
                    std::back_inserter(*lst),
                    ModuleManager::ModuleIsInPackage(package),
                    ModuleManager::ModuleConvert());
    }

    /**
     * @brief Retrieve the list of modules of the specified package and type.
     *
     * @param package The name of the package to by retrieve.
     * @param type The type of module to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(const std::string& package,
              Context::ModuleType type,
              std::vector<Context::Module>* lst) const
    {
        switch (type) {
        case Context::ModuleType::MODULE_DYNAMICS:
        case Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
        case Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
            transformIf(mTableSimulator.begin(),
                        mTableSimulator.end(),
                        std::back_inserter(*lst),
                        ModuleManager::ModuleIsInPackage(package),
                        ModuleManager::ModuleConvert());
            break;
        case Context::ModuleType::MODULE_OOV:
            transformIf(mTableOov.begin(),
                        mTableOov.end(),
                        std::back_inserter(*lst),
                        ModuleManager::ModuleIsInPackage(package),
                        ModuleManager::ModuleConvert());
            break;
        default:
            break;
        }
    }
};

void*
Context::get_symbol(const std::string& package,
                    const std::string& library,
                    Context::ModuleType type,
                    Context::ModuleType* newtype)
{
    if (not m_pimpl->modules)
        m_pimpl->modules = std::make_shared<ModuleManager>(this);

    if (not newtype)
        return m_pimpl->modules->getModule(package, library, type)->get();

    const auto& module = m_pimpl->modules->getModule(package, library, type);
    auto* result = module->get();
    *newtype = module->mType;

    return result;
}

void*
Context::get_symbol(const std::string& pluginname)
{
    if (not m_pimpl->modules)
        m_pimpl->modules = std::make_shared<ModuleManager>(this);

    return m_pimpl->modules->getSymbol(pluginname);
}

void
Context::unload_dynamic_libraries() noexcept
{
    m_pimpl.reset(nullptr);
}

std::vector<Context::Module>
Context::get_dynamic_libraries(const std::string& package,
                               ModuleType type) const
{
    std::vector<Context::Module> ret;
    const auto& paths = getBinaryPackagesDir();
    for (auto elem : paths) {
        if (not elem.is_directory())
            continue;

        elem /= package;
        if (not elem.is_directory())
            continue;

        if (type == ModuleType::MODULE_DYNAMICS or
            type == ModuleType::MODULE_DYNAMICS_WRAPPER or
            type == ModuleType::MODULE_DYNAMICS_EXECUTIVE) {
            Path modules = elem;
            modules /= "plugins";
            modules /= "simulator";
            if (modules.is_directory()) {
                for (DirectoryIterator it(modules), end; it != end; ++it) {
                    if (not it->is_file())
                        continue;

                    ret.push_back({ package,
                                    getLibraryName(it->path()),
                                    it->path(),
                                    ModuleType::MODULE_DYNAMICS });
                }
            }
        }

        if (type == Context::ModuleType::MODULE_OOV) {
            Path modules = elem;
            modules /= "plugins";
            modules /= "output";
            if (modules.is_directory()) {
                for (DirectoryIterator it(modules), end; it != end; ++it) {
                    if (not it->is_file())
                        continue;

                    ret.push_back({ package,
                                    getLibraryName(it->path()),
                                    it->path(),
                                    ModuleType::MODULE_OOV });
                }
            }
        }

        break; // We stop when we have found the first package in the
               // first binary package repositories.
    }

    return ret;
}
}
} // namespace vle utils
