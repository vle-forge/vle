/*
 * @file vle/utils/ModuleManager.hpp
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


#ifndef VLE_UTILS_MODULEMANAGER_HPP
#define VLE_UTILS_MODULEMANAGER_HPP 1

#include <vle/utils/DllDefines.hpp>
#include <string>
#include <map>

namespace vle { namespace utils {

enum ModuleType
{
    MODULE_UNKNOWN,
    MODULE_DYNAMICS,
    MODULE_DYNAMICS_WRAPPER,
    MODULE_EXECUTIVE,
    MODULE_OOV,
    MODULE_EOV,
    MODULE_GVLE_MODELING,
    MODULE_GVLE_OUTPUT
};

/**
 * ModuleManager permit to store a list of shared libraries or plug-ins and can
 * return for each plug-in a pointer to a function associated to a ModuleType.
 *
 * ModuleManager checks the symbols:
 * - "vle_api_level": A function which returns three \c uint32_t. This
 *   integers represent the version of VLE which build the plug-in.
 * - "vle_make_new_dynamics":
 * - "vle_make_new_dynamics_wrapper":
 * - "vle_make_new_executive":
 * - "vle_make_new_oov":
 * - "vle_make_new_eov":
 * - "vle_make_new_gvle_modeling":
 * - "vle_make_new_gvle_output":
 *
 * @code
 * vle::utils::ModuleManager mng;
 * void* mng.get("foo", "sim", vle::utils::MODULE_DYNAMICS);
 * @endcode
 */
class VLE_UTILS_EXPORT ModuleManager
{
public:
    ModuleManager();

    /**
     * Delete all shared libarry load into the ModuleManager.
     */
    ~ModuleManager();

    /**
     * Check the list of shared libraries already in the ModuleManager. If the
     * shared library exists, the symbol \c type is researched into the
     * library. Otherwise, the ModuleManager try to load the shared library
     * named from the combination of \c package, \c library and \type.
     *
     * @code
     * // try to load the shared library
     * // $VLE_HOME/.vle/pkgs/glue/lib/libcounter.so
     * ModuleManager manager;
     * manager.get("glue", "counter", MODULE_DYNAMICS);
     * @endcode
     *
     * @param package
     * @param library
     * @param type
     *
     * @throw utils::InternalError if shared library does not exists or if the
     * symbols wanted does not exist.
     *
     * @return
     */
    void *get(const std::string& package,
              const std::string& library,
              ModuleType type) const;

    /**
     * Determine the type of the plug-in in the patch builded from the
     * combination of \c package, \c library and \type.
     *
     * @param package
     * @param library
     * @param type
     *
     * @return
     */
    ModuleType determine(const std::string& package,
                         const std::string& library,
                         ModuleType type) const;

    /**
     * Browse all plug-ins directories, (global or from packages), for
     * all type of plug-in and fill the ModuleManager.
     */
    void fill();

    /*
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    /**
     * Fill the cache of module of the ModuleManager with all module of type \c
     * type readed from the packages distribution.
     *
     * @param type
     * @param output [out] return a map of modules availabled from the packages
     * of from global directory.
     */
    void browse(ModuleType type,
                std::multimap < std::string, std::string >* modules);

    /*
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     */

    /**
     * Build a path from the combination of \c package, \c library and \c type.
     * If type is MODULE_DYNAMICS, MODULE_DYNAMICS_WRAPPER or MODULE_EXECUTIVE,
     * the same filename is returned.
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

    class Pimpl;
    Pimpl *mPimpl;
};

}} // namespace vle utils

#endif

