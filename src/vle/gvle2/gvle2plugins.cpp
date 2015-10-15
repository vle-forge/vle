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
#include "gvle2plugins.h"



namespace vle {
namespace gvle2 {

gvle2plug::gvle2plug(): package(), libPath()
{

}

gvle2plug::gvle2plug(QString pkg, QString path): package(pkg), libPath(path)
{

}


gvle2plugins::gvle2plugins()
{

}

gvle2plugins::~gvle2plugins()
{

}

void
gvle2plugins::loadPlugins()
{

    QString pathgvlec = "plugins/gvle2/condition";
    QString pathgvles = "plugins/gvle2/simulating";
    QString pathgvlem = "plugins/gvle2/modeling";
    QString pathgvleo = "plugins/gvle2/output";

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
                    continue;
                }
                PluginExpCond *expcond = qobject_cast<PluginExpCond *>(plugin);
                if (expcond) {
                    mCondPlugins.insert(expcond->getname(),
                            gvle2plug(it.fileName(), libName));
                }
                loader.unload();
            }
        }
    }
}

QStringList
gvle2plugins::getCondPluginsList()
{
    return mCondPlugins.keys();
}

QString
gvle2plugins::getCondPluginPath(QString name)
{
    return mCondPlugins.value(name).libPath;
}

QString
gvle2plugins::getCondPluginPackage(QString name)
{
    return mCondPlugins.value(name).package;
}

}} //namespaces
