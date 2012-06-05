/*
 * @file vle/utils/details/PackageParser.hpp
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

#ifndef VLE_UTILS_DETAILS_PACKAGEPARSER_HPP
#define VLE_UTILS_DETAILS_PACKAGEPARSER_HPP

#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/details/Package.hpp>
#include <string>
#include <iosfwd>


namespace vle { namespace utils {

/**
 * @c LoadPackages loads from stream and extracts all packages to fill
 * the set variable @e pkgs.
 *
 * @param[in] stream to read.
 * @param[in] distribution The distribution to assign for each
 * package.
 * @param[out] pkgs The @e PackagesIdSet to be filled.
 *
 * @throw Nothing
 *
 * @return True if success, false otherwise (and log with @c
 * utils::Trace subsytem).
 */
bool LoadPackages(std::istream&       stream,
                  const std::string&  distribution,
                  PackagesIdSet      *pkgs) throw();

/**
 * @c LoadPackages loads the specified file and extracts all packages
 * to fill the set variable @e pkgs.
 *
 * @param[in] filename The file to open and read.
 * @param[in] distribution The distribution to assign for each
 * package.
 * @param[out] pkgs The @e PackagesIdSet to be filled.
 *
 * @throw Nothing
 *
 * @return True if success, false otherwise (and log with @c
 * utils::Trace subsytem).
 */
bool LoadPackages(const std::string&  filename,
                  const std::string&  distribution,
                  PackagesIdSet      *pkgs) throw();

/**
 * @c LoadPackage loads from stream and extracts a package to the the
 * variable @e pkg.
 *
 * @param[in] stream to read.
 * @param[in] distribution The distribution to assign for package.
 * @param[out] pkg The @e PackageId to be filled.
 *
 * @throw Nothing
 *
 * @return true if success, false otherwise (and log with @c
 * utils::Trace subsystem).
 */
bool LoadPackage(std::istream&       stream,
                 const std::string&  distribution,
                 PackageId          *pkg) throw();

/**
 * @c LoadPackage loads the specified file and extracts a package to
 * the the variable @e pkg.
 *
 * @param[in] filename The file to open and read.
 * @param[in] distribution The distribution to assign for package.
 * @param[out] pkg The @e PackageId to be filled.
 *
 * @throw Nothing
 *
 * @return true if success, false otherwise (and log with @c
 * utils::Trace subsystem).
 */
bool LoadPackage(const std::string&  filename,
                 const std::string&  distribution,
                 PackageId          *pkg) throw();

}} // namespace vle utils

#endif /* VLE_UTILS_DETAILS_PACKAGEPARSER_HPP */
