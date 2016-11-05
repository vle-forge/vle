/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
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

#ifndef FilePluginGUI_H
#define FilePluginGUI_H

#include <QObject>
#include <QtXml/qdom.h>
#include <vle/gvle/plugin_output.h>
#include "FilePluginGUItab.h"

namespace vle {
namespace gvle {

class FilePluginGUI : public PluginOutput
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginOutput")
    Q_INTERFACES(vle::gvle::PluginOutput)

public:
    FilePluginGUI();
    ~FilePluginGUI();
    QString getname() override;
    QWidget *getWidget() override;
    void     delWidget() override;
    void setLogger(Logger* logger) override;
    void init(vleVpz* vpz, const QString& viewName) override;

public slots:
    void onTabDeleted(QObject *obj);

private:
    Logger*           mLogger;
    FilePluginGUItab* mWidgetTab;
};

}} //namespaces

#endif
