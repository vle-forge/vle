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

#ifndef gvle_FILE_VPZ_SIM_H
#define gvle_FILE_VPZ_SIM_H

#include <QGraphicsScene>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>
#include <QUndoView>
//
#include "ui_filevpzrtool.h"
#include "vlevpz.h"
#include "widgetvpzproperty.h"
#include "vpzDiagScene.h"
#include "plugin_simpanel.h"

#define ROW_NAME 0
#define ROW_DYN  1
#define ROW_EXP  2
#define ROW_OBS  3

namespace Ui {
class FileVpzSim;
}

namespace vle {
namespace gvle {

class FileVpzSim : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzSim(vle::utils::Package* pkg, gvle_plugins* plugs,
            QWidget *parent = 0);
    ~FileVpzSim();
    void setVpm(vleVpm* v);
    void setSimLeftWidget(QWidget* leftWidget);
    QWidget* rightWidget();

signals:
    void rightWidgetChanged();
public slots:
    void onPluginChanged(const QString& text);


public:
    Ui::FileVpzSim*  ui;
private:
    vleVpm*          mVpm;
    gvle_plugins*    mGvlePlugins;
    PluginSimPanel*  mPluginSimPanel;
    vle::utils::Package*     mPackage;
};

}} //namespaces

#endif
