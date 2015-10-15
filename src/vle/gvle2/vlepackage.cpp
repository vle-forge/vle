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
#include <QPluginLoader>
#include <QtCore/qdebug.h>
#include "vlepackage.h"
#include "plugin_modeler.h"
#include "plugin_cond.h"
#include "sourcecpp.h"

namespace vle {
namespace gvle2 {

/**
 * @brief vlePackage::vlePackage
 *        Default constructor for a VLE package
 *
 */
vlePackage::vlePackage()
{
    mStdPackage = 0;
    mSourceCpp.clear();
    mModelerClass.clear();
    mModelerLoader.clear();
}

/**
 * @brief vlePackage::vlePackage
 *        Constructor for a VLE package, with specified base path
 *
 */
vlePackage::vlePackage(QString path)
{
    mStdPackage = 0;
    mSourceCpp.clear();
    mModelerClass.clear();
    mModelerLoader.clear();

    mDir.setPath(path);

    QDir dir(mDir);
    dir.cdUp();
    QDir::setCurrent( dir.path() );

    if (mDir.exists("src"))
        refreshModelerClass();
}

/**
 * @brief vlePackage::getName
 *        Getter for the package name
 */
QString vlePackage::getName()
{
    return mDir.dirName();
}

/**
 * @brief vlePackage::getSrcFilePath
 *        Get the path name for a source file into the package
 */
QString vlePackage::getSrcFilePath(QString name)
{
    QDir dir(mDir);
    dir.cd("src");
    return dir.filePath(name);
}

/**
 * @brief vlePackage::setStdPackage
 *        Set vle-api package
 */
void vlePackage::setStdPackage(vle::utils::Package *pkg)
{
    mStdPackage = pkg;

    mStdPackage->select( mDir.dirName().toStdString() );
}

vle::utils::Package*
vlePackage::getStdPackage()
{
    return mStdPackage;
}

/* ------------------------ Sources files Functions ------------------------ */

sourceCpp *vlePackage::openSourceCpp(QString filename)
{
    //qDebug() << "openSourceCpp() " << filename;
    QFileInfo fi = QFileInfo(filename);

    sourceCpp *s;
    if ( mSourceCpp.contains(fi.fileName()) )
    {
        s = mSourceCpp.value(fi.fileName());
        mSourceCppUsage[s] ++;
        //qDebug() << "  - Found " << fi.fileName() << " obj=" << s << " " << mSourceCppUsage[s];
    }
    else
    {
        s = new sourceCpp(filename, this);
        //qDebug() << "  - New " << fi.fileName() << " obj=" << s;
        mSourceCpp.insert(fi.fileName(), s);
        mSourceCppUsage.insert(s, 1);
        //QObject::connect(s,    SIGNAL(destroyed(QObject*)),
        //                 this, SLOT  (sourceDestroyed(QObject*)) );
    }
    return s;
}

void vlePackage::closeSourceCpp(sourceCpp *src)
{
    //qDebug() << "vlePackage::closeSourceCpp() " << mSourceCppUsage[src];
}

void vlePackage::sourceDestroyed(QObject *obj)
{
    //qDebug() << "vlePackage::sourceDestroyed() " << obj;
}

/* ----------------------- Modeler Plugins Functions ----------------------- */

QStringList
vlePackage::getListOfCondPlugins()
{
    return mCondPlugins.keys();
}

void vlePackage::addModeler(QString filename)
{
    if (mModelers.key(filename).isEmpty())
    {
        QPluginLoader *loader = new QPluginLoader(filename);
        QObject *plugin = loader->instance();
        if ( ! loader->isLoaded())
        {
            delete loader;
            return;
        }

        PluginModeler *modeler = qobject_cast<PluginModeler *>(plugin);
        if (modeler)
        {
            mModelers.insert(modeler->getname(), filename);
        }
        delete loader;
    }
}

PluginModeler *vlePackage::getModelerByClass(QString className)
{
    // Search the plugin that own the specified class
    QString pluginName = getModelerClassPlugin(className);

    return getModelerByPlugin(pluginName);
}

PluginModeler *vlePackage::getModelerByPlugin(QString pluginName)
{
    // Search the library file of the plugin
    QString pluginFile = mModelers.value(pluginName);

    QPluginLoader *loader;
    if (mModelerLoader.contains(pluginFile))
        loader = mModelerLoader.value(pluginFile);
    else
    {
        loader = new QPluginLoader(pluginFile);
        mModelerLoader.insert(pluginFile, loader);
    }
    QObject *plugin = loader->instance();

    PluginModeler *modeler = qobject_cast<PluginModeler *>(plugin);

    return modeler;
}

int  vlePackage::getModelerCount()
{
    return mModelers.count();
}

QString vlePackage::getModelerName(int pos)
{
    QList<QString> k = mModelers.keys();
    return k.at(pos);
}

int  vlePackage::getModelerClassCount()
{
    return mModelerClass.count();
}

QString vlePackage::getModelerClass(int pos)
{
    QList<QString> k = mModelerClass.keys();

    return k.at(pos);
}

QString vlePackage::getModelerClassPlugin(QString className)
{
    return mModelerClass.value(className);
}

void vlePackage::refreshModelerClass()
{
    QDir srcDir(mDir);
    srcDir.cd("src");

    QFileInfoList list = srcDir.entryInfoList();
    QListIterator<QFileInfo> files( list );
    while( files.hasNext() )
    {
        QFileInfo fileInfo = files.next();
        if ( ! fileInfo.isFile())
            continue;
        sourceCpp srcFile( fileInfo.filePath() );
        if (srcFile.getTemplate()->isValid())
        {
            QString className  = srcFile.getTemplate()->getTagValue("class");
            QString pluginName = srcFile.getTemplate()->getPluginName();
            mModelerClass.insert(className, pluginName);
        }
    }
}


/* --------------- Experimental Conditions Plugins Functions --------------- */

void
vlePackage::addExpPlugin(QString name, QString fileName)
{
    if (mCondPlugins.key(name).isEmpty())
    {
        mCondPlugins.insert(name.toLower(), fileName);
    }
}

int
vlePackage::expPluginCount()
{
    return mCondPlugins.count();
}

PluginExpCond*
vlePackage::getCondPlugin(QString name)
{
    // Search the library file of the plugin
    QString pluginFile = mCondPlugins.value(name.toLower());
    // Load the QT plugin library
    QPluginLoader *loader = new QPluginLoader(pluginFile);
    // Get an instance of the GVLE2 plugin
    QObject *plugin = loader->instance();

    PluginExpCond *expPlugin = 0;
    if (plugin)
        expPlugin = qobject_cast<PluginExpCond *>(plugin);

    delete loader;

    return expPlugin;
}

QString
vlePackage::getExpPluginName(int index)
{
    QList<QString> names = mCondPlugins.keys();
    return names.at(index);
}


/* --------------- Output Plugins Functions --------------- */

void vlePackage::addOutputPlugin(QString name, QString fileName)
{
    if (mOutputPlugins.key(name).isEmpty())
    {
        mOutputPlugins.insert(name.toLower(), fileName);
    }
}

PluginOutput *vlePackage::getOutputPlugin(QString name)
{
    // Search the library file of the plugin
    QString pluginFile = mOutputPlugins.value(name.toLower());
    // Load the QT plugin library
    QPluginLoader *loader = new QPluginLoader(pluginFile);
    // Get an instance of the GVLE2 plugin
    QObject *plugin = loader->instance();

    PluginOutput *outPlugin = 0;
    if (plugin)
        outPlugin = qobject_cast<PluginOutput *>(plugin);

    delete loader;
    return outPlugin;
}

}} //namespaces
