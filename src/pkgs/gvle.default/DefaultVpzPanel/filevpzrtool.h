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

#ifndef gvle_FILE_VPZ_RTOOL_H
#define gvle_FILE_VPZ_RTOOL_H

#include <QGraphicsScene>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>
#include <QUndoView>
#include <vle/gvle/vlevpz.hpp>
#include "ui_filevpzrtool.h"
#include "widgetvpzproperty.h"
#include "vpzDiagScene.h"

//#ifndef Q_MOC_RUN
//#include <vle/vpz/Vpz.hpp>
//#endif

#define ROW_NAME 0
#define ROW_DYN  1
#define ROW_OBS  2
#define ROW_EXP  3


namespace Ui {
class fileVpzRTool;
}

namespace vle {
namespace gvle {

class FileVpzRtool : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzRtool(QWidget *parent = 0);
    ~FileVpzRtool();
    void setVpz(vleVpz* v);
    void clear();
    void updateTree();
    vleVpz* vpz();
    QString getModelQuery(QTreeWidgetItem* item);
    QTreeWidgetItem* getTreeWidgetItem(const QString& model_query,
            QTreeWidgetItem* base = 0);
    void updateModelProperty(const QString& model_query);
    void updateModelLabel();
    QTreeWidgetItem* treeInsertModel(QDomNode mode, QTreeWidgetItem *base);
//    void diagSelectModel(vleVpzModel *base, bool force = FALSE);
//    void treeUpdateModel(vleVpzModel *model, QString oldName, QString newName);

public slots:
    void onTreeModelSelected();
    void onEnterCoupledModel(QDomNode node);
    void onSelectionChanged();
    void onInitializationDone(VpzDiagScene* scene);
    void onDataUpdate();
    void onUndoRedoVpz(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);
public:
    Ui::fileVpzRtool*  ui;
private:
    vleVpz*          mVpz;
    VpzDiagScene*    mCurrScene;
};

}} //namespaces

#endif
