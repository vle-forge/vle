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

#include "sourcecpp.h"

namespace vle {
namespace gvle2 {

/**
 * @brief sourceCpp::sourceCpp
 *        Default constructor
 *
 */
sourceCpp::sourceCpp() :
    QObject(0)
{
    mIsNew    = true;
    mModeler  = 0;
    mPackage  = 0;
    mTemplate = 0;
}

/**
 * @brief sourceCpp::sourceCpp
 *        Constructor with a source filename specified
 *
 */
sourceCpp::sourceCpp(QString filename, vlePackage *package) :
    QObject(0)
{
    mFileName = filename;
    mModeler  = 0;
    mPackage  = package;
    mTemplate = 0;

    QFile testFile(mFileName);
    mIsNew = (testFile.exists() == false);
}

/**
 * @brief sourceCpp::~sourceCpp
 *        Default destructor
 *
 */
sourceCpp::~sourceCpp()
{
    if (mTemplate)
        delete mTemplate;
}

/**
 * @brief sourceCpp::getFilename
 *        Getter for the filename property
 *
 */
QString sourceCpp::getFilename()
{
    return mFileName;
}

bool sourceCpp::isNew()
{
    return mIsNew;
}

void sourceCpp::load()
{
    QFile file(mFileName);
    QTextStream stream(&file);
    // Read the whole file using readAll, this assume that source files are small
    mContent = stream.readAll();
    file.close();
}

void sourceCpp::write(QString data)
{
    QFile file(mFileName);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << data;
    file.close();

    if (mTemplate)
    {
        delete mTemplate;
        mTemplate = 0;
        getTemplate();
    }
}

/**
 * @brief sourceCpp::getTemplate
 *        Get a template object for this source file
 *
 */
sourceCppTemplate *sourceCpp::getTemplate()
{
    if (mTemplate == 0)
    {
        mTemplate = new sourceCppTemplate(this);
    }
    return mTemplate;
}

/**
 * @brief sourceCpp::getModeler
 *        Get the PluginModeler used by the current source
 *
 */
PluginModeler *sourceCpp::getModeler()
{
    if (mModeler)
        return mModeler;

    if (mPackage == 0)
        return 0;

    QString name = getModelerName();

    PluginModeler *modeler = mPackage->getModelerByPlugin(name);

    mModeler = modeler;

    return modeler;
}

/**
 * @brief sourceCpp::getModelerName
 *        Get the name of the modeler (if any)
 *
 */
QString sourceCpp::getModelerName()
{
    sourceCppTemplate *tpl = getTemplate();
    if (tpl == 0)
        return "";

    return tpl->getPluginName();
}
/* EOF */

}}//namespaces
