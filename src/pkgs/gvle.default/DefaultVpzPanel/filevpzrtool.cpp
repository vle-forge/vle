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

#include "filevpzrtool.h"
#include "ui_filevpzrtool.h"
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>

namespace vle {
namespace gvle {

FileVpzRtool::FileVpzRtool(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::fileVpzRtool)
  , mVpz(0)
  , mCurrScene(0)
{
    ui->setupUi(this);

    ui->modelProperty->setAutoScroll(false);

    QObject::connect(ui->modelTree,
                     SIGNAL(itemSelectionChanged()),
                     this,
                     SLOT(onTreeModelSelected()));
}

FileVpzRtool::~FileVpzRtool()
{
    delete ui;
    mVpz = 0;
    mCurrScene = 0;
}

void
FileVpzRtool::setVpz(vleVpz* vpz)
{
    mVpz = vpz;
    mCurrScene = 0;
    QObject::connect(vpz, SIGNAL(modelsUpdated()), this, SLOT(onDataUpdate()));
    QObject::connect(
      vpz, SIGNAL(dynamicsUpdated()), this, SLOT(onDataUpdate()));
    QObject::connect(
      vpz,
      SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
      this,
      SLOT(onUndoRedoVpz(QDomNode, QDomNode, QDomNode, QDomNode)));
}

void
FileVpzRtool::clear()
{

    QTreeWidget* viewTree = ui->modelTree;
    bool oldblock = viewTree->blockSignals(true);
    viewTree->clearSelection();
    viewTree->clear();
    viewTree->setColumnCount(1);
    viewTree->blockSignals(oldblock);
}

void
FileVpzRtool::updateTree()
{
    clear();
    if (not mVpz) {
        return;
    }
    // Insert root model into tree widget
    ui->modelTree->insertTopLevelItem(
      0,
      treeInsertModel(mVpz->baseModelFromModelQuery(
                        mVpz->vdo()->getXQuery(mCurrScene->mCoupled->mnode)),
                      0));
}

vleVpz*
FileVpzRtool::vpz()
{
    return mVpz;
}

QTreeWidgetItem*
FileVpzRtool::treeInsertModel(QDomNode model, QTreeWidgetItem* base)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(0, DomFunctions::attributeValue(model, "name"));
    QString model_query = mVpz->vdo()->getXQuery(model);
    newItem->setData(0, Qt::UserRole + 0, model_query);
    if (base) {
        base->addChild(newItem);
    }
    QList<QDomNode> subs = mVpz->submodelsFromModel(model);
    for (int i = 0; i < subs.size(); i++) {
        treeInsertModel(subs[i], newItem);
    }
    return newItem;
}

QString
FileVpzRtool::getModelQuery(QTreeWidgetItem* item)
{
    return item->data(0, Qt::UserRole + 0).toString();
}

QTreeWidgetItem*
FileVpzRtool::getTreeWidgetItem(const QString& model_query,
                                QTreeWidgetItem* base)
{
    QTreeWidgetItem* item = base;
    if (not item) {
        item = ui->modelTree->topLevelItem(0);
    }
    if (not item) {
        return 0;
    }
    if (getModelQuery(item) == model_query) {
        return item;
    }
    QTreeWidgetItem* foundItem = 0;
    for (int j = 0; j < item->childCount(); j++) {
        foundItem = getTreeWidgetItem(model_query, item->child(j));
        if (foundItem) {
            return foundItem;
        }
    }
    return 0;
}

void
FileVpzRtool::updateModelProperty(const QString& model_query)
{
    bool oldBlockModeProperty = ui->modelProperty->blockSignals(true);
    bool oldBlockStackProperty = ui->stackProperty->blockSignals(true);

    if (not mVpz) {
        ui->stackProperty->setCurrentIndex(0);
        return;
    }
    if (ui->modelTree->selectedItems().isEmpty()) {
        ui->stackProperty->setCurrentIndex(0);
        return;
    }
    if (model_query.isEmpty()) {
        ui->stackProperty->setCurrentIndex(0);
        return;
    }

    QDomNode nodeSel = mVpz->vdo()->getNodeFromXQuery(model_query);
    bool is_atomic = DomFunctions::attributeValue(nodeSel, "type") == "atomic";
    if (is_atomic) {
        QTableWidgetItem* newItem =
          new QTableWidgetItem(DomFunctions::attributeValue(nodeSel, "name"));
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        ui->modelProperty->setItem(ROW_NAME, 1, newItem);
        // This row Name is selected by default
        ui->modelProperty->setCurrentCell(ROW_NAME, 0);

        WidgetVpzPropertyDynamics* wpd = new WidgetVpzPropertyDynamics();
        wpd->selectModel(mVpz, model_query);
        ui->modelProperty->setCellWidget(ROW_DYN, 1, wpd);

        WidgetVpzPropertyObservables* wpo = new WidgetVpzPropertyObservables();
        wpo->selectModel(mVpz, model_query);
        ui->modelProperty->setCellWidget(ROW_OBS, 1, wpo);

        WidgetVpzPropertyExpCond* wpec = new WidgetVpzPropertyExpCond();
        wpec->selectModel(mVpz, model_query);
        ui->modelProperty->setCellWidget(ROW_EXP, 1, wpec);

        ui->modelProperty->resizeRowToContents(ROW_EXP);
        ui->modelProperty->resizeColumnsToContents();
        ui->stackProperty->setCurrentIndex(1);
    } else {
        ui->modelProperty->setCellWidget(ROW_DYN, 1, 0);
        ui->modelProperty->setCellWidget(ROW_EXP, 1, 0);
        ui->modelProperty->setCellWidget(ROW_OBS, 1, 0);
        ui->stackProperty->setCurrentIndex(0);
    }
    ui->modelProperty->blockSignals(oldBlockModeProperty);
    ui->stackProperty->blockSignals(oldBlockStackProperty);
}

void
FileVpzRtool::updateModelLabel()
{
    // update model label
    if (mCurrScene) {
        if (mCurrScene->getClass() == "") {
            ui->labelModel->setText("Main model");
        } else {
            ui->labelModel->setText(
              QString("Class model: %1").arg(mCurrScene->getClass()));
        }
    } else {
        ui->labelModel->setText("No model");
    }
}

void
FileVpzRtool::onTreeModelSelected()
{
    if (ui->modelTree->selectedItems().isEmpty()) {
        return;
    }
    QTreeWidgetItem* item = ui->modelTree->currentItem();
    QString model_query = getModelQuery(item);
    if (mCurrScene and not model_query.isEmpty()) {
        QDomNode nodeSel = mVpz->vdo()->getNodeFromXQuery(model_query);
        QDomNode parent = nodeSel.parentNode().parentNode();
        if (parent.nodeName() == "model") {
            mCurrScene->setFocus(parent);
            mCurrScene->selectSubModel(
              DomFunctions::attributeValue(nodeSel, "name"));
        } else {
            mCurrScene->setFocus(nodeSel);
        }
    }
    updateModelProperty(model_query);
}

void
FileVpzRtool::onEnterCoupledModel(QDomNode node)
{
    QTreeWidgetItem* item = getTreeWidgetItem(mVpz->vdo()->getXQuery(node));
    if (item) {
        bool oldBlock = ui->modelTree->blockSignals(true);
        ui->modelTree->setCurrentItem(item);
        ui->modelTree->blockSignals(oldBlock);
    }
}

void
FileVpzRtool::onSelectionChanged()
{
    QStringList sels = mCurrScene->getSelectedModels();
    if (sels.size() == 1) {

        bool oldBlock = ui->modelTree->blockSignals(true);
        ui->modelTree->setCurrentItem(getTreeWidgetItem(sels.at(0)));
        QTreeWidgetItem* item = ui->modelTree->currentItem();
        QString model_query = getModelQuery(item);
        updateModelProperty(model_query);
        ui->modelTree->blockSignals(oldBlock);
    }
}

void
FileVpzRtool::onInitializationDone(VpzDiagScene* scene)
{
    bool oldBlock = ui->modelTree->blockSignals(true);
    mCurrScene = scene;
    QString theSelected = "";
    if (mVpz and scene and scene->mCoupled) {
        updateTree();
        theSelected = scene->getXQueryOfTheSelectedModel();
        if (theSelected.isEmpty() and
            not scene->mCoupled->mnode.parentNode()
                  .isNull()) { // TODO pas tres propre
            theSelected = mVpz->vdo()->getXQuery(scene->mCoupled->mnode);
        }
        if (theSelected.isEmpty()) {
            ui->modelTree->expandToDepth(0);
        } else {
            ui->modelTree->setCurrentItem(getTreeWidgetItem(theSelected));
        }

    } else {
        clear();
    }
    updateModelLabel();
    updateModelProperty(theSelected);
    ui->modelTree->blockSignals(oldBlock);
}

void
FileVpzRtool::onDataUpdate()
{
    onInitializationDone(mCurrScene);
}

void FileVpzRtool::onUndoRedoVpz(QDomNode /*oldValVpz*/,
                                 QDomNode /*newValVpz*/,
                                 QDomNode /*oldValVpm*/,
                                 QDomNode /*newValVpm*/)
{
    onInitializationDone(mCurrScene);
}
}
} // namespaces
