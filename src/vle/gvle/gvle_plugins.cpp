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

#include <QDebug>
#include <QDirIterator>

#include "plugin_cond.h"
#include "plugin_mainpanel.h"
#include "plugin_output.h"
#include "plugin_simpanel.h"

#include "gvle_plugins.h"

namespace vle {
namespace gvle {

gvleplug::gvleplug()
  : package()
  , libPath()
  , loader(0)
{
}

gvleplug::gvleplug(QString pkg, QString path)
  : package(pkg)
  , libPath(path)
  , loader(0)
{
}

gvleplug::~gvleplug()
{
    if (loader) {
        loader->unload();
        delete loader;
    }
    loader = 0;
}

gvle_plugins::gvle_plugins(const utils::ContextPtr& ctx)
  : mOutputPlugins()
  , mCondPlugins()
  , mSimPanelPlugins()
  , mMainPanelPlugins()
  , mCtx(ctx)
{
}

gvle_plugins::~gvle_plugins()
{
}

void
gvle_plugins::registerPlugins()
{

    QString pathgvlec = "plugins/gvle/condition";
    QString pathgvleo = "plugins/gvle/output";
    QString pathgvlem = "plugins/gvle/modeling";
    QString pathgvleu = "plugins/gvle/out";
    QString pathgvles = "plugins/gvle/simulating";
    QString pathgvlev = "plugins/gvle/vpz";
    QString pathgvled = "plugins/gvle/data";

    for (auto binPkgDir : mCtx->getBinaryPackagesDir()) {

        QString packagesDir = binPkgDir.string().c_str();

        QDirIterator it(packagesDir, QDir::AllDirs);

        while (it.hasNext()) {
            it.next();

            if (QDir(it.filePath() + "/" + pathgvlec).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvlec,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load condition plugin: "
                                 << loader.errorString();

                        continue;
                    }
                    PluginExpCond* expcond =
                      qobject_cast<PluginExpCond*>(plugin);
                    if (expcond) {
                        mCondPlugins.insert(expcond->getname(),
                                            gvleplug(it.fileName(), libName));
                        expcond->deleteLater();
                    }
                }
            }
            if (QDir(it.filePath() + "/" + pathgvleo).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvleo,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load output plugin: "
                                 << loader.errorString();
                        continue;
                    }
                    PluginOutput* outputPlug =
                      qobject_cast<PluginOutput*>(plugin);
                    if (outputPlug) {
                        mOutputPlugins.insert(
                          outputPlug->getname(),
                          gvleplug(it.fileName(), libName));
                        outputPlug->deleteLater();
                    }
                }
            }

            if (QDir(it.filePath() + "/" + pathgvlem).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvlem,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load modeling plugin: "
                                 << loader.errorString();
                        continue;
                    }
                    PluginMainPanel* modeling =
                      qobject_cast<PluginMainPanel*>(plugin);
                    if (modeling) {
                        mMainPanelPlugins.insert(
                          modeling->getname(),
                          gvleplug(it.fileName(), libName));
                        modeling->deleteLater();
                    }
                }
            }
            if (QDir(it.filePath() + "/" + pathgvleu).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvleu,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load out plugin "
                                 << loader.errorString();
                        continue;
                    }
                    PluginMainPanel* modeling =
                      qobject_cast<PluginMainPanel*>(plugin);
                    if (modeling) {
                        mMainPanelOutPlugins.insert(
                          modeling->getname(),
                          gvleplug(it.fileName(), libName));
                        modeling->deleteLater();
                    }
                }
            }

            if (QDir(it.filePath() + "/" + pathgvles).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvles,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load simulating plugin "
                                 << loader.errorString();
                        continue;
                    }
                    PluginSimPanel* simulating =
                      qobject_cast<PluginSimPanel*>(plugin);
                    if (simulating) {
                        mSimPanelPlugins.insert(
                          simulating->getname(),
                          gvleplug(it.fileName(), libName));
                        simulating->deleteLater();
                    }
                }
            }

            if (QDir(it.filePath() + "/" + pathgvlev).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvlev,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load vpz plugin "
                                 << loader.errorString();
                        continue;
                    }

                    PluginMainPanel* vpz =
                      qobject_cast<PluginMainPanel*>(plugin);
                    if (vpz) {
                        mMainPanelVpzPlugins.insert(
                          vpz->getname(), gvleplug(it.fileName(), libName));
                        vpz->deleteLater();
                    }
                }
            }

            if (QDir(it.filePath() + "/" + pathgvled).exists()) {
                QDirIterator itbis(it.filePath() + "/" + pathgvled,
                                   QDir::Files);
                while (itbis.hasNext()) {
                    QString libName = itbis.next();
                    QPluginLoader loader(libName);
                    QObject* plugin = loader.instance();
                    if (!loader.isLoaded()) {
                        qDebug() << " WARNING cannot load data plugin "
                                 << loader.errorString();
                        continue;
                    }

                    PluginMainPanel* data =
                      qobject_cast<PluginMainPanel*>(plugin);
                    if (data) {
                        mMainPanelDataPlugins.insert(
                          data->getname(), gvleplug(it.fileName(), libName));
                    }
                }
            }
        }
    }
}

QStringList
gvle_plugins::getOutputPluginsList()
{
    return mOutputPlugins.keys();
}

QString
gvle_plugins::getOutputPluginPath(QString name)
{
    return mOutputPlugins.value(name).libPath;
}

QString
gvle_plugins::getOutputPluginPackage(QString name)
{
    return mOutputPlugins.value(name).package;
}

PluginOutput*
gvle_plugins::provideOutputPlugin(QString name)
{
    if (not mOutputPlugins.contains(name)) {
        return 0;
    }
    gvleplug& plug = mOutputPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load OutputPlugin " << name;
            return 0;
        }
    }
    return qobject_cast<PluginOutput*>(plug.loader->instance());
}

QStringList
gvle_plugins::getCondPluginsList()
{
    return mCondPlugins.keys();
}

QString
gvle_plugins::getCondPluginPath(QString name)
{
    return mCondPlugins.value(name).libPath;
}

QString
gvle_plugins::getCondPluginPackage(QString name)
{
    return mCondPlugins.value(name).package;
}

PluginExpCond*
gvle_plugins::newInstanceCondPlugin(QString name)
{
    if (not mCondPlugins.contains(name)) {
        qDebug() << " Error no CondPlugins " << name;
        return 0;
    }
    gvleplug& plug = mCondPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load CondPlugins " << name;
            return 0;
        }
        return qobject_cast<PluginExpCond*>(plug.loader->instance());
    }
    return qobject_cast<PluginExpCond*>(plug.loader->instance())
      ->newInstance();
}

QStringList
gvle_plugins::getSimPanelPluginsList()
{
    return mSimPanelPlugins.keys();
}

QString
gvle_plugins::getSimPanelPluginPath(QString name)
{
    if (mSimPanelPlugins.contains(name)) {
        return mSimPanelPlugins.value(name).libPath;
    } else {
        return "";
    }
}

QString
gvle_plugins::getSimPanelPluginPackage(QString name)
{
    return mSimPanelPlugins.value(name).package;
}

PluginSimPanel*
gvle_plugins::newInstanceSimPanelPlugin(QString name)
{
    if (not mSimPanelPlugins.contains(name)) {
        qDebug() << " Error no SimPanelPlugin " << name;
        return 0;
    }
    gvleplug& plug = mSimPanelPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load SimPanelPlugin " << name;
            return 0;
        }
        return qobject_cast<PluginSimPanel*>(plug.loader->instance());
    }
    return qobject_cast<PluginSimPanel*>(plug.loader->instance())
      ->newInstance();
}

QStringList
gvle_plugins::getMainPanelPluginsList()
{
    return mMainPanelPlugins.keys();
}

QString
gvle_plugins::getMainPanelPluginPath(QString name)
{
    if (mMainPanelPlugins.contains(name)) {
        return mMainPanelPlugins.value(name).libPath;
    } else {
        return "";
    }
}

QString
gvle_plugins::getMainPanelPluginPackage(QString name)
{
    return mMainPanelPlugins.value(name).package;
}

PluginMainPanel*
gvle_plugins::newInstanceMainPanelPlugin(QString name)
{
    if (not mMainPanelPlugins.contains(name)) {
        qDebug() << " Error no MainPanelPlugin " << name;
        return 0;
    }
    gvleplug& plug = mMainPanelPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load MainPanelPlugin " << name;
            return 0;
        }
        return qobject_cast<PluginMainPanel*>(plug.loader->instance());
    }
    // Tricky : the plugin instance is already used build a clone
    return qobject_cast<PluginMainPanel*>(plug.loader->instance())
      ->newInstance();
}

QStringList
gvle_plugins::getMainPanelOutPluginsList()
{
    return mMainPanelOutPlugins.keys();
}

QString
gvle_plugins::getMainPanelOutPluginPath(QString name)
{
    if (mMainPanelOutPlugins.contains(name)) {
        return mMainPanelOutPlugins.value(name).libPath;
    } else {
        return "";
    }
}

QString
gvle_plugins::getMainPanelOutPluginPackage(QString name)
{
    return mMainPanelOutPlugins.value(name).package;
}

PluginMainPanel*
gvle_plugins::newInstanceMainPanelOutPlugin(QString name)
{
    if (not mMainPanelOutPlugins.contains(name)) {
        qDebug() << " Error no MainPanelOutPlugin " << name;
        return 0;
    }
    gvleplug& plug = mMainPanelOutPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load MainPanelOutPlugin " << name;
            return 0;
        }
        return qobject_cast<PluginMainPanel*>(plug.loader->instance());
    }
    // Tricky : the plugin instance is already used build a clone
    return qobject_cast<PluginMainPanel*>(plug.loader->instance())
      ->newInstance();
}

QStringList
gvle_plugins::getMainPanelVpzPluginsList()
{
    return mMainPanelVpzPlugins.keys();
}

QString
gvle_plugins::getMainPanelVpzPluginPath(QString name)
{
    if (mMainPanelVpzPlugins.contains(name)) {
        return mMainPanelVpzPlugins.value(name).libPath;
    } else {
        return "";
    }
}

QString
gvle_plugins::getMainPanelVpzPluginPackage(QString name)
{
    return mMainPanelVpzPlugins.value(name).package;
}

PluginMainPanel*
gvle_plugins::newInstanceMainPanelVpzPlugin(QString name)
{
    if (not mMainPanelVpzPlugins.contains(name)) {
        qDebug() << " Error no MainPanelVpzPlugin " << name;
        return 0;
    }
    gvleplug& plug = mMainPanelVpzPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load MainPanelVpzPlugin " << name;
            return 0;
        }
        return qobject_cast<PluginMainPanel*>(plug.loader->instance());
    }
    // Tricky : the plugin instance is already used build a clone
    return qobject_cast<PluginMainPanel*>(plug.loader->instance())
      ->newInstance();
}

QStringList
gvle_plugins::getMainPanelDataPluginsList()
{
    return mMainPanelDataPlugins.keys();
}

QString
gvle_plugins::getMainPanelDataPluginPath(QString name)
{
    if (mMainPanelDataPlugins.contains(name)) {
        return mMainPanelDataPlugins.value(name).libPath;
    } else {
        return "";
    }
}

QString
gvle_plugins::getMainPanelDataPluginPackage(QString name)
{
    return mMainPanelDataPlugins.value(name).package;
}

PluginMainPanel*
gvle_plugins::newInstanceMainPanelDataPlugin(QString name)
{
    if (not mMainPanelDataPlugins.contains(name)) {
        qDebug() << " Error no MainPanelDataPlugin " << name;
        return 0;
    }
    gvleplug& plug = mMainPanelDataPlugins[name];
    if (not plug.loader) {
        plug.loader = new QPluginLoader(plug.libPath);
        if (not plug.loader->isLoaded()) {
            qDebug() << " Error cannot load MainPanelDataPlugin " << name;
            return 0;
        }
        return qobject_cast<PluginMainPanel*>(plug.loader->instance());
    }
    // Tricky : the plugin instance is already used build a clone
    return qobject_cast<PluginMainPanel*>(plug.loader->instance())
      ->newInstance();
}
}
} // namespaces
