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

#ifndef VLE_DLLDEFINES_HPP
#define VLE_DLLDEFINES_HPP

/*
 * See. http://gcc.gnu.org/wiki/Visibility
 */

#if defined _WIN32 || defined __CYGWIN__
#define VLE_HELPER_DLL_IMPORT __declspec(dllimport)
#define VLE_HELPER_DLL_EXPORT __declspec(dllexport)
#define VLE_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define VLE_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define VLE_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define VLE_HELPER_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define VLE_HELPER_DLL_IMPORT
#define VLE_HELPER_DLL_EXPORT
#define VLE_HELPER_DLL_LOCAL
#endif
#endif

/*
 * Now we use the generic helper definitions above to define VLE_API
 * and VLE_LOCAL. VLE_API is used for the public API symbols. It
 * either DLL imports or DLL exports (or does nothing for static
 * build) VLE_LOCAL is used for non-api symbols.
 */

#ifdef VLE_DLL
#ifdef vlelib_EXPORTS
#define VLE_API VLE_HELPER_DLL_EXPORT
#else
#define VLE_API VLE_HELPER_DLL_IMPORT
#endif
#define VLE_LOCAL VLE_HELPER_DLL_LOCAL
#define VLE_MODULE VLE_HELPER_DLL_EXPORT
#else
#define VLE_API
#define VLE_LOCAL
#define VLE_MODULE VLE_HELPER_DLL_EXPORT
#endif

#endif
