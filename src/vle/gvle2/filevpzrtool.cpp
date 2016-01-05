/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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

#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>
#include "filevpzrtool.h"
#include "ui_filevpzrtool.h"

namespace vle {
namespace gvle2 {


FileVpzRtool::FileVpzRtool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileVpzRtool), mWidgetTool(0), mVpm(0), mCurrScene(0),
    mExternalSelection(false)
{
    mWidgetTool = new QWidget();
    ui->setupUi(mWidgetTool);

    QObject::connect(ui->modelTree, SIGNAL(itemSelectionChanged()),
            this, SLOT (onTreeModelSelected()));
}

FileVpzRtool::~FileVpzRtool()
{
}


void
FileVpzRtool::setVpm(vleVpm* vpm)
{
    mVpm = vpm;
    mCurrScene = 0;
}

void
FileVpzRtool::clear()
{

    QTreeWidget *viewTree = ui->modelTree;
    viewTree->clearSelection();
    viewTree->clear();
    viewTree->setColumnCount(1);

}

void
FileVpzRtool::updateTree()
{
    clear();
    if (not mVpm) {
        return ;
    }
    // Insert root model into tree widget
    ui->modelTree->insertTopLevelItem(0, treeInsertModel(
            mVpm->baseModelFromModelQuery(
                    mVpm->vdo()->getXQuery(mCurrScene->mCoupled->mnode)), 0));
}


vleVpm*
FileVpzRtool::vpm()
{
    //QString fileName = mVpm->getFilename();
    //mVleLibVpz = new vle::vpz::Vpz(fileName.toStdString());

    return mVpm;
}



QTreeWidgetItem*
FileVpzRtool::treeInsertModel(QDomNode model, QTreeWidgetItem *base)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    newItem->setText(0, mVpm->vdo()->attributeValue(model, "name"));
    newItem->setData(0, Qt::UserRole+0, "Model");
//    newItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)model));
    newItem->setData(0, Qt::UserRole+2, QVariant::fromValue((void*)base));

    // If a root widget has been defined
    if (base) {
        // Insert the new item as a child of it
        base->addChild(newItem);
    }

    QList<QDomNode> subs = mVpm->submodelsFromModel(model);
    for (int i =0; i< subs.size() ; i++) {
        treeInsertModel(subs[i], newItem);
    }
    return newItem;
}

QWidget *FileVpzRtool::getTool()
{
    return mWidgetTool;
}


QString
FileVpzRtool::getModelQuery(QTreeWidgetItem* base)
{
    if (base == 0) {
        return "";
    }
    QString model_query = "model[@name=\""+base->text(0)+"\"]";
    QTreeWidgetItem* parent = base->parent();
    while (parent) {
        model_query = "model[@name=\""+parent->text(0)
                      +"\"]/submodels/"+model_query;
        parent = parent->parent();
    }
    QDomNode baseNode = mVpm->baseModelFromModelQuery(
            mVpm->vdo()->getXQuery(mCurrScene->mCoupled->mnode)).parentNode();
    model_query = mVpm->vdo()->getXQuery(baseNode)+"/"+model_query;
    return model_query;
}

QTreeWidgetItem*
FileVpzRtool::getTreeWidgetItem(const QString& model_query)
{
    QTreeWidgetItem* item = ui->modelTree->topLevelItem(0);
    if (item) {
        QStringList pathList = model_query.split("/");
        QString pathi ="";
        QTreeWidgetItem* ch = 0;
        for (int i =0; i<pathList.size(); i++) {
            pathi = pathList.at(i);
            if (pathi.startsWith("model")) {
                pathi = pathi.split("\"").at(1);
                for (int j=0; j<item->childCount(); j++) {
                    ch = item->child(j);
                    if (ch->text(0) == pathi) {
                        item = ch;
                    }
                }
            }
        }
    }
    return item;
}

void
FileVpzRtool::updateModelProperty(const QString& model_query)
{
    bool oldBlockModeProperty = ui->modelProperty->blockSignals(true);
    bool oldBlockStackProperty = ui->stackProperty->blockSignals(true);
    if (not mVpm) {
        return ;
    }
    if (ui->modelTree->selectedItems().isEmpty()) {
        return;
    }
    if (model_query.isEmpty()) {
        return;
    }

    QDomNode nodeSel = mVpm->vdo()->getNodeFromXQuery(model_query);
    bool is_atomic = mVpm->vdo()->attributeValue(nodeSel, "type") == "atomic";
    if (is_atomic) {
        QTableWidgetItem *newItem = new QTableWidgetItem(
                mVpm->vdo()->attributeValue(nodeSel, "name"));
        newItem->setData(Qt::UserRole, ROW_NAME);
        //newItem->setData(Qt::UserRole+1, QVariant::fromValue((void*)model));
        ui->modelProperty->setItem(ROW_NAME, 1, newItem);
        // This row Name is selected by default
        ui->modelProperty->setCurrentCell(ROW_NAME, 0);

        WidgetVpzPropertyDynamics *wpd = new WidgetVpzPropertyDynamics();
        wpd->setModel(mVpm, model_query);
        ui->modelProperty->setCellWidget(ROW_DYN, 1, wpd);
        ui->modelProperty->resizeRowToContents(ROW_DYN);

        WidgetVpzPropertyExpCond *wpec = new WidgetVpzPropertyExpCond();
        wpec->setModel(mVpm, model_query);
        ui->modelProperty->setCellWidget(ROW_EXP, 1, wpec);
        ui->modelProperty->resizeRowToContents(ROW_EXP);

        WidgetVpzPropertyObservables *wpo = new WidgetVpzPropertyObservables();
        wpo->setModel(mVpm, model_query);
        ui->modelProperty->setCellWidget(ROW_OBS, 1, wpo);
        ui->modelProperty->resizeRowToContents(ROW_OBS);
        ui->modelProperty->resizeColumnToContents(1);
//        ui->modelProperty->resizeColumnToContents(0);
//        ui->modelProperty->resizeColumnToContents(1);

//            ui->modelProperty->resizeRowsToContents();
//            ui->modelProperty->resizeColumnsToContents();

//        ui->modelProperty->setVisible(false);
//        ui->modelProperty->resizeRowsToContents();
//        ui->modelProperty->resizeColumnsToContents();
//        ui->modelProperty->setVisible(true);

            ui->stackProperty->setCurrentIndex(1);


        //        ui->modelProperty->resizeRowsToContents();
        //        ui->modelProperty->resizeColumnsToContents();



    } else {
        ui->modelProperty->setCellWidget(ROW_DYN, 1, 0);
        ui->modelProperty->setCellWidget(ROW_EXP, 1, 0);
        ui->modelProperty->setCellWidget(ROW_OBS, 1, 0);
//        ui->modelProperty->resizeRowsToContents();
//        ui->modelProperty->resizeColumnsToContents();
        ui->stackProperty->setCurrentIndex(0);
    }
    ui->modelProperty->blockSignals(oldBlockModeProperty);
    ui->stackProperty->blockSignals(oldBlockStackProperty);


}


void
FileVpzRtool::onTreeModelSelected()
{
    if (ui->modelTree->selectedItems().isEmpty()) {
        return;
    }
    QTreeWidgetItem* item = ui->modelTree->currentItem();
    QString model_query = getModelQuery(item);
    if (mCurrScene and not mExternalSelection
                   and not model_query.isEmpty()) {
        QDomNode nodeSel = mVpm->vdo()->getNodeFromXQuery(model_query);
        QDomNode parent = nodeSel.parentNode().parentNode();
        if (parent.nodeName() == "model") {
            mCurrScene->setFocus(parent);
            mCurrScene->selectSubModel(
                    mVpm->vdo()->attributeValue(nodeSel, "name"));
        } else {
            mCurrScene->setFocus(nodeSel);
        }
        mCurrScene->update();
    }
    updateModelProperty(model_query);
    mExternalSelection = false;
}

void
FileVpzRtool::onEnterCoupledModel(QDomNode node)
{
    QTreeWidgetItem* item = getTreeWidgetItem(mVpm->vdo()->getXQuery(node));
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
        mExternalSelection = true;
        ui->modelTree->setCurrentItem(getTreeWidgetItem(sels.at(0)));
    }
}

void
FileVpzRtool::onInitializationDone(VpzDiagScene* scene)
{
    bool oldBlock = ui->modelTree->blockSignals(true);
    mCurrScene = scene;
    if (mVpm and scene and scene->mCoupled) {
        updateTree();
        QString theSelected = scene->getXQueryOfTheSelectedModel();
        if (theSelected.isEmpty() and
                not scene->mCoupled->mnode.parentNode().isNull()) { //TODO pas tres propre
            theSelected = mVpm->vdo()->getXQuery(scene->mCoupled->mnode);
        }
        if (theSelected.isEmpty()) {
            ui->modelTree->expandToDepth(0);
        } else {
            ui->modelTree->setCurrentItem(getTreeWidgetItem(theSelected));
        }
        updateModelProperty(theSelected);
    } else {
        clear();
    }
    ui->modelTree->blockSignals(oldBlock);
}

void
FileVpzRtool::onModelsUpdated()
{
    mExternalSelection = true;
    onInitializationDone(mCurrScene);
}


}} //namespaces
