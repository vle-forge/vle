/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
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

#include "vlevpzdynamic.h"

/**
 * @brief vleVpzDynamic::vleVpzDynamic
 *        Default constructor
 *
 */
vleVpzDynamic::vleVpzDynamic(QString name, QString lib, QString package)
{
    mName    = name;
    mLibrary = lib;
    mPackage = package;

    mValidPackage = false;
    mValidLibrary = false;
    mIsAltered    = false;

    mPackageList.clear();
    mLibraryList.clear();
}

/**
 * @brief vleVpzDynamic::vleVpzDynamic
 *        Construct from an existing Dynamic (make clone)
 *
 */
vleVpzDynamic::vleVpzDynamic(vleVpzDynamic *dynamic)
{
    mName    = dynamic->getName();
    mLibrary = dynamic->getLibrary();
    mPackage = dynamic->getPackage();

    mValidPackage = false;
    mValidLibrary = false;
    mIsAltered    = false;

    mPackageList.clear();
    mLibraryList.clear();
}

/**
 * @brief vleVpzDynamic::getName
 *        Accessor for the name of the Dynamic
 *
 */
 QString vleVpzDynamic::getName()
{
    return mName;
}

/**
 * @brief vleVpzDynamic::setName
 *        Mutator for the name of the Dynamic
 *
 */
void vleVpzDynamic::setName(QString name)
{
    // Name of the Dynamic can't by nul
    if (name.isEmpty())
        // In case of an empty name, just ignore it
        return;

    // If a name has already been set
    if ( ! mName.isEmpty())
        // Mark the Dynamic as altered
        mIsAltered = true;

    mName = name;
}

/**
 * @brief vleVpzDynamic::getLibrary
 *        Accessor for the Library name
 *
 */
QString vleVpzDynamic::getLibrary()
{
    return mLibrary;
}

/**
 * @brief vleVpzDynamic::setLibrary
 *        Mutator for the Library name
 *
 */
void vleVpzDynamic::setLibrary(QString lib)
{
    // If a library has already been set
    if ( ! mLibrary.isEmpty())
        // Mark the Dynamic as altered
        mIsAltered = true;

    // Remove the tested flag
    mValidLibrary = false;

    mLibrary = lib;
}

/**
 * @brief vleVpzDynamic::getPackage
 *        Accessor for the Package name
 *
 */
QString vleVpzDynamic::getPackage()
{
    return mPackage;
}

/**
 * @brief vleVpzDynamic::setPackage
 *        Mutator for the Package name
 *
 */
void vleVpzDynamic::setPackage(QString pkg)
{
    // If a package has already been set
    if ( ! mPackage.isEmpty())
        // Mark the Dynamic as altered
        mIsAltered = true;

    // Remove the tested flag
    mValidPackage = false;

    mPackage = pkg;
}

/**
 * @brief vleVpzDynamic::isAltered
 *        Accessor for the altered flag
 *
 */
bool vleVpzDynamic::isAltered()
{
    return mIsAltered;
}

/**
 * @brief vleVpzDynamic::isValid
 *        Test and validate current Dynamic attributes
 *
 */
bool vleVpzDynamic::isValid(bool again)
{
    // If one of the mandatory attribute is missing ...
    if (mName.isEmpty() || mPackage.isEmpty() || mLibrary.isEmpty())
        // current dynamic is not valid
        return false;

    // If the "again" flag is set, force new check
    if (again)
    {
        // Reset the previously saved flags
        mValidPackage = false;
        mValidLibrary = false;
    }

    // If valid flags are set
    if (mValidPackage && mValidLibrary)
        // Dynamic has already been tested, return valid result
        return true;

    // Get the list of VLE packages
    getPackageList();
    // Search the specified package into the loaded list
    if ( ! mPackageList.contains(mPackage))
        // not found :( return invalid dynamic
        return false;
    // Save the result
    mValidPackage = true;
    // Clean (save) memory
    mPackageList.clear();

    // Get the list of VLE libraries for the package
    getLibraryList(mPackage);
    if ( ! mLibraryList.contains(mLibrary))
        return false;
    // Save the result
    mValidLibrary = true;
    // Clean (save) memory
    mLibraryList.clear();

    return true;
}

/**
 * @brief vleVpzDynamic::getPackageList
 *        Get the list of VLE packages
 *
 */
QList <QString> *vleVpzDynamic::getPackageList()
{
    // Get the package list from VLE
    vle::utils::PathList pathList;

    // Clean previously loaded values
    mPackageList.clear();

    pathList = vle::utils::Path::path().getBinaryPackages();
    for (vle::utils::PathList::const_iterator i = pathList.begin(), e = pathList.end();
             i != e; ++i)
    {
        std::string name = *i;
        QString pkgName = QString( name.c_str() );
        mPackageList.append(pkgName);
    }

    return &mPackageList;
}

/**
 * @brief vleVpzDynamic::getLibraryList
 *        Get the list of VLE libraries for the specified package
 *
 */
QList <QString> *vleVpzDynamic::getLibraryList(QString package)
{
    vle::utils::ModuleList lst;
    vle::utils::ModuleManager manager;

    // Clean previously loaded values
    mLibraryList.clear();

    // Convert package name argument to VLE string format
    std::string stdPackage = package.toLocal8Bit().constData();

    // Call VLE to get the Library list for the specified package
    manager.browse();
    manager.fill(stdPackage, vle::utils::MODULE_DYNAMICS, &lst);

    // Read and save results
    vle::utils::ModuleList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it)
    {
        std::string name = it->library;
        QString libName = QString( name.c_str() );
        mLibraryList.append(libName);
    }

    return &mLibraryList;
}
