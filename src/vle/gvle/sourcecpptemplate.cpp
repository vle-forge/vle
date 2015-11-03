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

#include <QStringList>

#include "sourcecpp.h"
#include "sourcecpptemplate.h"

namespace vle {
namespace gvle {


/**
 * @brief sourceCppTemplate::sourceCppTemplate
 *        Default constructor
 *
 */
sourceCppTemplate::sourceCppTemplate(sourceCpp *parent)
{
    mSource = parent;
    mTagLoaded = false;
}

bool sourceCppTemplate::isValid()
{
    if (mSource == 0)
        return false;

    // Try to load tag
    if (! mTagLoaded)
        tagLoad();
    if (! mTagLoaded)
        return false;

    return true;
}

QString sourceCppTemplate::getPluginName()
{
    if ( ! mTagLoaded)
        tagLoad();

    return mTagPlugin;
}

/**
 * @brief sourceCppTemplate::getTagValue
 *        Return the value of a config key (read from main tag)
 *
 */
QString sourceCppTemplate::getTagValue(QString name)
{
    if (! mTagLoaded)
        tagLoad();

    return mTagConf.value(name);
}

/**
 * @brief sourceCppTemplate::tagArrayLoad
 *        Init an array from config key (before read or write it)
 *
 */
bool sourceCppTemplate::tagArrayLoad(QString name)
{
    if (! mTagLoaded)
        tagLoad();

    mTagConfValue.clear();

    if (mTagConf.contains(name) == false)
        return false;

    QString entryValue = mTagConf.value(name);

    QStringList entryArray = entryValue.split("|");
    for (int i = 0; i < entryArray.count(); i++)
    {
        QStringList fields = entryArray.at(i).split(",");
        if (fields.count() < 2)
            continue;
        mTagConfValue.insert(fields.at(0), fields.at(1));
    }

    return true;
}

/**
 * @brief sourceCppTemplate::getTagArrayCount
 *        Return the size of the array loaded by tagArrayLoad()
 *
 */
int sourceCppTemplate::getTagArrayCount()
{
    return mTagConfValue.count();
}

/**
 * @brief sourceCppTemplate::getTagArrayName
 *        Return the name of the specified array item
 *
 */
QString sourceCppTemplate::getTagArrayName(int pos)
{
    QList<QString> keys = mTagConfValue.keys();
    return keys.at(pos);
}

/**
 * @brief sourceCppTemplate::getTagArrayValue
 *        Return the value of the specified array item
 *
 */
QString sourceCppTemplate::getTagArrayValue(int pos)
{
    QList<QString> values = mTagConfValue.values();
    return values.at(pos);
}

/**
 * @brief sourceCppTemplate::tagLoad
 *        Use VLE api to parse source file and load tag
 *
 */
void sourceCppTemplate::tagLoad()
{
    vle::utils::Template tpl;
    try {
        std::string tplPlugin, tplPackage, tplConf;

        tpl.open(mSource->getFilename().toStdString());
        tpl.tag(tplPlugin, tplPackage, tplConf);

        // Save the plugin name
        mTagPlugin = QString(tplPlugin.c_str());
        // Save the Package name
        mTagPackage = QString(tplPackage.c_str());

        mTagConf.clear();
        QString tagConf = QString( tplConf.c_str() );
        QStringList confEntries = tagConf.split(";");
        for (int i = 0; i < confEntries.count(); i++)
        {
            QStringList entry = confEntries.at(i).split(":");
            mTagConf.insert(entry.at(0), entry.at(1));
        }
        mTagLoaded = true;
    } catch(...) {
        // ToDo : handle error
    }
}

}}//namespaces
