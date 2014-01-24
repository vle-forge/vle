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


#ifndef VLE_UTILS_MODULEMANAGER_HPP
#define VLE_UTILS_MODULEMANAGER_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/utils/Types.hpp>
#include <string>
#include <vector>

namespace vle { namespace utils {

/**
 * @brief A @e Module must have a type which determine the symbol to load.
 */
enum ModuleType
{
    MODULE_DYNAMICS, /**< The @e Module is a simulator and it needs to have the
                       following symbol: @c vle_make_new_dynamics. */
    MODULE_DYNAMICS_WRAPPER, /**< The @e Module is a simulator and it needs to
                               have the following symbols: @c
                               vle_make_new_dynamics_wrapper. */
    MODULE_DYNAMICS_EXECUTIVE, /**< The @e Module is a simulator and it needs
                                 to have the following symbols: @c
                                 vle_make_new_executive. */
    MODULE_OOV, /**< The @e Module is an output (from oov library) and it needs
                  to have the following symbol: @c vle_make_new_oov. */
    MODULE_GVLE_GLOBAL, /**< The @e Module is a global module (from the vle
                            library) and it needs to have the following symbol:
                            @c vle_make_new_gvle_global. */
    MODULE_GVLE_MODELING, /**< The @e Module is a modeling module (from the vle
                            library) and it needs to have the following symbol:
                            @c vle_make_new_gvle_modeling. */
    MODULE_GVLE_OUTPUT /**< The @e Module is a modeling output module (from the
                         gvle library) and it needs to have the following
                         symbol: @c vle_make_new_gvle_output. */
};

/**
 * @brief A @e Module describes a dynamic loaded shared library.
 *
 * @e Module is a public representation of @e ModuleManager's internal Module
 * which store an handle to the shared library and a pointer to the symbol
 * referenced by the @e ModuleType.
 */
struct VLE_API Module
{
    Module(const std::string& package,
           const std::string& library,
           const std::string& path,
           ModuleType type)
        : package(package), library(library), path(path), type(type)
    {
    }

    Module()
    {
    }

    std::string package;
    std::string library;
    std::string path;
    ModuleType type;
};

/**
 * @brief A @e ModuleList stored list of @e Module in public API.
 */
typedef std::vector < Module > ModuleList;

/**
 * @brief ModuleManager permit to store a list of shared libraries or modules.
 *
 * ModuleManager stores shared libraries of VLE (simulators, output, gvle's
 * output and gvle's modeling modules). For each module, an handle is stored,
 * a pointer to the associated function, a type and a version.
 *
 * ModuleManager checks the symbols:
 * - "vle_api_level": A function which returns three @c uint32_t. This
 *   integers represent the version of VLE which build the module.
 * - "vle_make_new_dynamics" (MODULE_DYNAMICS).
 * - "vle_make_new_dynamics_wrapper" (MODULE_DYNAMICS).
 * - "vle_make_new_executive" (MODULE_DYNAMICS).
 * - "vle_make_new_oov" (MODULE_OOV).
 * - "vle_make_new_gvle_global" (MODULE_GVLE_GLOBAL).
 * - "vle_make_new_gvle_modeling" (MODULE_GVLE_MODELING).
 * - "vle_make_new_gvle_output" (MODULE_GVLE_OUTPUT).
 *
 * @code
 * vle::utils::ModuleManager mng;
 * void* mng.get("foo", "sim", vle::utils::MODULE_DYNAMICS);
 * @endcode
 */
class VLE_API ModuleManager
{
public:
    ModuleManager();

    /**
     * Delete all shared library load into the ModuleManager.
     */
    ~ModuleManager();

    /**
     * @brief Get a symbol from a shared library.
     *
     * Check the list of shared libraries already in the ModuleManager. If the
     * shared library exists, the symbol @c type is researched into the
     * library. Otherwise, the ModuleManager try to load the shared library
     * named from the combination of @c package, @c library and @e type.
     *
     * This function return is @e newtype parameter the type of the module
     * read. The @e type is MODULE_DYNAMICS, MODULE_DYNAMICS_EXECUTIVE or
     * MODULE_DYNAMICS_WRAPPER the @e newtype check the type and if it
     * does not corresponds it check the two other simulators (MODULE_DYNAMICS,
     * MODULE_DYNAMICS_EXECUTIVE or MODULE_DYNAMICS_EXECUTIVE). If @e type is
     * not MODULE_DYNAMICS, MODULE_DYNAMICS_WRAPPER,
     * MODULE_DYNAMICS_EXECUTIVE , the @e newtype will be affected by the
     * founded type.
     *
     * @code
     * // try to load the shared library
     * // $VLE_HOME/.vle/pkgs/glue/lib/libcounter.so
     * ModuleManager manager;
     * ModuleType type;
     * manager.get("glue", "counter", MODULE_DYNAMICS, &type);
     * assert(type == MODULE_DYNAMICS || type == MODULE_DYNAMICS_WRAPPER ||
     *        type == MODULE_DYNAMICS_WRAPPER);
     *
     * manager.get("glue", "counter", MODULE_DYNAMICS_WRAPPER, &type);
     * assert(type == MODULE_DYNAMICS || type == MODULE_DYNAMICS_WRAPPER ||
     *        type == MODULE_DYNAMICS_WRAPPER);
     *
     * manager.get("glue", "counter", MODULE_OOV, &type);
     * assert(type == MODULE_OOV);
     * @endcode
     *
     * @param package
     * @param library
     * @param type
     * @param[out] newtype If @e newtype is null, newtype will not be affected.
     *
     * @throw utils::InternalError if shared library does not exists or if the
     * symbols wanted does not exist.
     *
     * @return
     */
    void *get(const std::string& package, const std::string& library,
              ModuleType type, ModuleType *newtype = 0) const;


    /**
     * @brief Get a symbol directly in the executable.
     *
     * Try to get a symbol from the executable if it was never loaded. This
     * function allows to build executable where we can store simulators,
     * oov's modules, gvle's modules for instance in unit test.
     *
     * @code
     * // try to get the symbol:
     * ModuleManager manager;
     * manager.get(utils::
     * @endcode
     *
     * @param symbol The name of the symbol (be careful, it must be demangled).
     *
     * @throw utils::InternalError if the executable does not have a symbol of
     * this name.
     *
     * @return A pointer to the founded symbol.
     */
    void *get(const std::string& symbol);

    /*
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    void browse();

    void browse(ModuleType type);

    void browse(const std::string& package);

    void browse(const std::string& package, ModuleType type);

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
    void fill(ModuleList *lst) const;

    /**
     * @brief Retrieve the list of modules of the specified type.
     *
     * @param type The type of module to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(ModuleType type, ModuleList *lst) const;

    /**
     * @brief Retrieve the list of modules of the specified package.
     *
     * @param package The name of the package to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(const std::string& package, ModuleList *lst) const;

    /**
     * @brief Retrieve the list of modules of the specified package and type.
     *
     * @param package The name of the package to by retrieve.
     * @param type The type of module to by retrieve.
     * @param[out] lst An output parameter, all modules are pushed backward.
     */
    void fill(const std::string& package, ModuleType type,
              ModuleList *lst) const;

    /*
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    /**
     * @brief Build a path for a module.
     *
     * Build a path from the combination of @c package, @c library and @c type
     * parameters.
     *
     * @code
     * std::string path = buildModuleFilename("foo", "bar", MODULE_DYNAMICS);
     *
     * assert(path == "/home/user/.vle/pkgs/foo/modules/simulators/libbar.so");
     * @endcode
     *
     * @param package
     * @param library
     * @param type
     *
     * @return
     */
    static std::string buildModuleFilename(const std::string& package,
                                           const std::string& library,
                                           ModuleType type);

private:
    ModuleManager(const ModuleManager& other);
    ModuleManager& operator=(const ModuleManager& other);

    class Pimpl; /**< We hide the implementation detail of this class. */
    Pimpl *mPimpl;
};

}} // namespace vle utils

#endif

