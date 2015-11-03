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

struct gvleplug
{
    gvleplug();
    gvleplug(QString pkg, QString libPath);

    QString package;
    QString libPath;
};

class gvle_plugins : public QObject
{
    Q_OBJECT

public:
    gvle_plugins();
    ~gvle_plugins();

    void loadPlugins();
    QStringList getCondPluginsList();
    QString getCondPluginPath(QString name);
    QString getCondPluginPackage(QString name);
    QStringList getOutputPluginsList();
    QString getOutputPluginPath(QString name);
    QString getOutputPluginPackage(QString name);

private:
    QList<QPluginLoader*> mModelerPlugins;
    QMap<QString,QString> mSimulatorPlugins;
    QMap<QString,gvleplug>      mCondPlugins;
    QMap<QString,gvleplug>      mOutputPlugins;
};

}}//namespaces

#endif // gvle_win_H
