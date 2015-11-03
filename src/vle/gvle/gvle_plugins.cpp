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

#include <QDirIterator>
#include <QDebug>
#include <vle/utils/Path.hpp>
#include "plugin_cond.h"
#include "plugin_output.h"
#include "gvle_plugins.h"



namespace vle {
namespace gvle {

gvleplug::gvleplug(): package(), libPath()
{

}

gvleplug::gvleplug(QString pkg, QString path): package(pkg), libPath(path)
{

}


gvle_plugins::gvle_plugins()
{

}

gvle_plugins::~gvle_plugins()
{

}

void
gvle_plugins::loadPlugins()
{

    QString pathgvlec = "plugins/gvle/condition";
    QString pathgvles = "plugins/gvle/simulating";
    QString pathgvlem = "plugins/gvle/modeling";
    QString pathgvleo = "plugins/gvle/output";

    QString packagesDir =
            vle::utils::Path::path().getBinaryPackagesDir().c_str();

    QDirIterator it(packagesDir, QDir::AllDirs);

    while (it.hasNext()) {
        it.next();

        if (QDir(it.filePath() + "/" + pathgvlec).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvlec, QDir::Files);
            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject *plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    qDebug() << " WARNING cannot load plugin " << libName;
                    continue;
                }
                PluginExpCond* expcond = qobject_cast<PluginExpCond*>(plugin);
                if (expcond) {
                    mCondPlugins.insert(expcond->getname(),
                            gvleplug(it.fileName(), libName));
                }

            }
        }
        if (QDir(it.filePath() + "/" + pathgvleo).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvleo, QDir::Files);
            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject* plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    qDebug() << " WARNING cannot load plugin " << libName;
                    continue;
                }
                PluginOutput* outputPlug = qobject_cast<PluginOutput*>(plugin);
                if (outputPlug) {
                    mOutputPlugins.insert(outputPlug->getname(),
                            gvleplug(it.fileName(), libName));
                }
            }
        }
    }
}

QStringList
gvle_plugins::getCondPluginsList()
{
    return mCondPlugins.keys();
}

QStringList
gvle_plugins::getOutputPluginsList()
{
    return mOutputPlugins.keys();
}

QString
gvle_plugins::getCondPluginPath(QString name)
{
    return mCondPlugins.value(name).libPath;
}

QString
gvle_plugins::getOutputPluginPath(QString name)
{
    return mOutputPlugins.value(name).libPath;
}

QString
gvle_plugins::getCondPluginPackage(QString name)
{
    return mCondPlugins.value(name).package;
}

QString
gvle_plugins::getOutputPluginPackage(QString name)
{
    return mOutputPlugins.value(name).package;
}

}} //namespaces
