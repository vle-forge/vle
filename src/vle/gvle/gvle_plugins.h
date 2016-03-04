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

#ifndef gvle_gvle_plugins_H
#define gvle_gvle_plugins_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QPluginLoader>



namespace vle {
namespace gvle {

class PluginOutput;
class PluginExpCond;
class PluginSimPanel;
class PluginMainPanel;



struct gvleplug
{
    gvleplug();
    gvleplug(QString pkg, QString libPath);
    virtual ~gvleplug();

    QString        package;
    QString        libPath;
    QPluginLoader* loader;
};

class gvle_plugins : public QObject
{
    Q_OBJECT

public:
    gvle_plugins();
    ~gvle_plugins();

    void             registerPlugins();

    QStringList      getOutputPluginsList();
    QString          getOutputPluginPath(QString name);
    QString          getOutputPluginPackage(QString name);
    PluginOutput*    provideOutputPlugin(QString name);

    QStringList      getCondPluginsList();
    QString          getCondPluginPath(QString name);
    QString          getCondPluginPackage(QString name);
    PluginExpCond*   provideCondPlugin(QString name);

    QStringList      getSimPanelPluginsList();
    QString          getSimPanelPluginPath(QString name);
    QString          getSimPanelPluginPackage(QString name);
    PluginSimPanel*  newInstanceSimPanelPlugin(QString name);

    QStringList      getMainPanelPluginsList();
    QString          getMainPanelPluginPath(QString name);
    QString          getMainPanelPluginPackage(QString name);
    PluginMainPanel* newInstanceMainPanelPlugin(QString name);

    QStringList      getMainPanelOutPluginsList();
    QString          getMainPanelOutPluginPath(QString name);
    QString          getMainPanelOutPluginPackage(QString name);
    PluginMainPanel* newInstanceMainPanelOutPlugin(QString name);

private:
    QMap<QString,gvleplug>      mOutputPlugins;
    QMap<QString,gvleplug>      mCondPlugins;
    QMap<QString,gvleplug>      mSimPanelPlugins;
    QMap<QString,gvleplug>      mMainPanelPlugins;
    QMap<QString,gvleplug>      mMainPanelOutPlugins;
};

}}//namespaces

#endif // gvle_win_H
