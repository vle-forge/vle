/**
 * @file vle/eov/DllDefines.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef VLE_EOV_DLLDEFINES_HPP
#define VLE_EOV_DLLDEFINES_HPP

#if defined(__WIN32__)
  #if defined(vleeov_EXPORTS)
    #define VLE_EOV_EXPORT __declspec(dllexport)
  #else
    #define VLE_EOV_EXPORT __declspec(dllimport)
  #endif
#else
 #define VLE_EOV_EXPORT
#endif

#endif
