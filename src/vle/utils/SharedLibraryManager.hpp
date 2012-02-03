/*
 * @file vle/utils/SharedLibraryManager.hpp
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

#ifndef VLE_UTILS_SHAREDLIBRARYMANAGER_HPP
#define VLE_UTILS_SHAREDLIBRARYMANAGER_HPP

#include <vle/DllDefines.hpp>
#include <string>
#include <vector>

namespace vle { namespace utils {

/**
 * @brief Load or unload shared library of packages.
 *
 * Since VLE > 1.0, a package can provide shared library in addition to module.
 * These shared libraries are installed into the @c lib directory of the
 * package. The packages' modules are installed into the @c plugins directory.
 *
 * The @c SharedLibraryManager use the RAII (Resource Acquisition Is
 * Initialization) idiom. The constructor read and open shared library (using
 * @c dlopen or @c LoadLibrary function) and the destructor free the shared
 * library (using @c dlclose or @c FreeLibrary).
 *
 * @attention The @c SharedLibraryManager must be allocated before the @c
 * ModuleManager (see example below).
 *
 * The @c SharedLibraryManager is uncopyable and unassignable.
 *
 * The @c SharedLibraryManager use the Pimpl idiom.
 *
 * @code
 * void run_simulation()
 * {
 *   std::vector < std::string > pkgs;
 *   pkgs.push_back("decision-1.0");
 *   pkgs.push_back("difference-equatoin-1.0");
 *
 *   utils::SharedLibraryManager slm(pkgs);
 *   utils::ModuleManager mmg;
 *
 *   [...] // run the simulation.
 * }
 *  // when exiting scope, mmg unload plug-ins, slm unload shared libray.
 * @endcode
 */
class VLE_API SharedLibraryManager
{
public:
    /**
     * Build a @c SharedLibraryManager and browse all shared libraries.
     *
     * This constructor browses all packages with a @c lib directory and try to
     * load files with an extension @c dll or @c so. The load operation uses the
     * @c dlopen or @c LoadLibrary functions.
     */
    SharedLibraryManager() throw();

    /**
     * Build a @c SharedLibraryManager and browse specific shared libraries.
     *
     * This constructor browses the @c lib directory of the specified
     * @c packages and try to load files with an extension @c dll or @c so. The
     * load operation uses the @c dlopen or @c LoadLibrary functions.
     *
     * @param packages A list of shared library.
     */
    SharedLibraryManager(const std::vector < std::string >& packages) throw();

    /**
     * Unload all loaded shared library.
     */
    ~SharedLibraryManager() throw();

private:
    SharedLibraryManager(const SharedLibraryManager& other);
    SharedLibraryManager& operator=(const SharedLibraryManager& other);

    class Pimpl;
    Pimpl *mPimpl;
};

}} // namespace vle utils

#endif


