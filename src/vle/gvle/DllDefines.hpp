/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_DLLDEFINES_HPP
#define VLE_GVLE_DLLDEFINES_HPP

/*
 * See. http://gcc.gnu.org/wiki/Visibility
 */

#if defined _WIN32 || defined __CYGWIN__
#  define GVLE_HELPER_DLL_IMPORT __declspec(dllimport)
#  define GVLE_HELPER_DLL_EXPORT __declspec(dllexport)
#  define GVLE_HELPER_DLL_LOCAL
#else
#  if __GNUC__ >= 4
#    define GVLE_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#    define GVLE_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#    define GVLE_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#    define GVLE_HELPER_DLL_IMPORT
#    define GVLE_HELPER_DLL_EXPORT
#    define GVLE_HELPER_DLL_LOCAL
#  endif
#endif

/*
 * Now we use the generic helper definitions above to define GVLE_API
 * and GVLE_LOCAL. GVLE_API is used for the public API symbols. It
 * either DLL imports or DLL exports (or does nothing for static
 * build) GVLE_LOCAL is used for non-api symbols.
 */

#ifdef GVLE_DLL
#  ifdef gvlelib_EXPORTS
#    define GVLE_API GVLE_HELPER_DLL_EXPORT
#  else
#    define GVLE_API GVLE_HELPER_DLL_IMPORT
#  endif
#  define GVLE_LOCAL GVLE_HELPER_DLL_LOCAL
#  define GVLE_MODULE GVLE_HELPER_DLL_EXPORT
#else
#  define GVLE_API
#  define GVLE_LOCAL
#  define GVLE_MODULE GVLE_HELPER_DLL_EXPORT
#endif

#endif
