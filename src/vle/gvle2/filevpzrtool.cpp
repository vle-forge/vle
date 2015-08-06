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
    ui(new Ui::fileVpzRtool), mWidgetTool(0), mVpz(0), mCurrScene(0),
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
FileVpzRtool::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
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
    if (not mVpz) {
        return ;
    }

    // Insert root model into tree widget
    ui->modelTree->insertTopLevelItem(0,
            treeInsertModel(mCurrScene->mOrigModel, 0));
}


vleVpz*
FileVpzRtool::vpz()
{
    //QString fileName = mVpz->getFilename();
    //mVleLibVpz = new vle::vpz::Vpz(fileName.toStdString());

    return mVpz;
}



QTreeWidgetItem*
FileVpzRtool::treeInsertModel(QDomNode model, QTreeWidgetItem *base)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    newItem->setText(0, mVpz->attributeValue(model, "name"));
    newItem->setData(0, Qt::UserRole+0, "Model");
//    newItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)model));
    newItem->setData(0, Qt::UserRole+2, QVariant::fromValue((void*)base));

    // If a root widget has been defined
    if (base) {
        // Insert the new item as a child of it
        base->addChild(newItem);
    }

    std::vector<QDomNode> subs = mVpz->submodelsFromModel(model);
    for (unsigned int i =0; i< subs.size() ; i++) {
        treeInsertModel(subs[i], newItem);
    }
    return newItem;
}

QWidget *FileVpzRtool::getTool()
{
    return mWidgetTool;
}


QString
FileVpzRtool::getFullPathModel(QTreeWidgetItem* base)
{
    if (base == 0) {
        return "";
    }
    QTreeWidgetItem* parent = base->parent();
    QString modelFullPath = base->text(0);
    while (parent) {
        modelFullPath = parent->text(0)+"."+modelFullPath;
        parent = parent->parent();
    }
    modelFullPath = mCurrScene->getClassOfOriginModel()+"/"+modelFullPath;
    return modelFullPath;
}

QTreeWidgetItem*
FileVpzRtool::getTreeWidgetItem(const QString& modelFullPath)
{
    QStringList pathList = modelFullPath.split("/").at(1).split(".");
    QTreeWidgetItem* item = ui->modelTree->topLevelItem(0);
    for (int i =0; i<pathList.size(); i++) {
        QString pathi = pathList.at(i);
        for (int j=0; j<item->childCount(); j++) {
            QTreeWidgetItem* ch = item->child(j);
            if (ch->text(0) == pathi) {
                item = ch;
            }
        }
    }
    return item;
}

void
FileVpzRtool::updateModelProperty()
{
    if (not mVpz) {
        return ;
    }
    if (ui->modelTree->selectedItems().isEmpty()) {
        return;
    }
    QTreeWidgetItem* item = ui->modelTree->currentItem();
    QString modelFullName = getFullPathModel(item);

    QDomNode nodeSel = mVpz->modelFromDoc(modelFullName);

    bool is_atomic = mVpz->attributeValue(nodeSel, "type") == "atomic";
    if (is_atomic) {
        QTableWidgetItem *newItem = new QTableWidgetItem(
                mVpz->attributeValue(nodeSel, "name"));
        newItem->setData(Qt::UserRole, ROW_NAME);
        //newItem->setData(Qt::UserRole+1, QVariant::fromValue((void*)model));
        ui->modelProperty->setItem(ROW_NAME, 1, newItem);
        // This row Name is selected by default
        ui->modelProperty->setCurrentCell(ROW_NAME, 0);

        WidgetVpzPropertyDynamics *wpd = new WidgetVpzPropertyDynamics();
        wpd->setModel(mVpz, modelFullName);
        ui->modelProperty->setCellWidget(ROW_DYN, 1, wpd);
        ui->modelProperty->resizeRowToContents(ROW_DYN);

        WidgetVpzPropertyExpCond *wpec = new WidgetVpzPropertyExpCond();
        wpec->setModel(mVpz, modelFullName);
        ui->modelProperty->setCellWidget(ROW_EXP, 1, wpec);
        ui->modelProperty->resizeRowToContents(ROW_EXP);

        WidgetVpzPropertyObservables *wpo = new WidgetVpzPropertyObservables();
        wpo->setModel(mVpz, modelFullName);
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


}


void
FileVpzRtool::onTreeModelSelected()
{
    if (ui->modelTree->selectedItems().isEmpty()) {
        return;
    }
    QTreeWidgetItem* item = ui->modelTree->currentItem();
    QString modelFullName = getFullPathModel(item);
    if (mCurrScene and not mExternalSelection
                   and not modelFullName.isEmpty()) {
        QDomNode nodeSel = mVpz->modelFromDoc(modelFullName);
        QDomNode parent = nodeSel.parentNode().parentNode();
        if (parent.nodeName() == "model") {
            mCurrScene->setFocus(parent);
            mCurrScene->selectSubModel(mVpz->attributeValue(nodeSel, "name"));
        } else {
            mCurrScene->setFocus(nodeSel);
        }
        mCurrScene->update();
    }
    updateModelProperty();
    mExternalSelection = false;
}

void
FileVpzRtool::onEnterCoupledModel(QDomNode node)
{
    QTreeWidgetItem* item = getTreeWidgetItem(mVpz->getFullPathModel(node));
    ui->modelTree->setCurrentItem(item);

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
    mCurrScene = scene;
    if (mVpz and scene and scene->mCoupled) {
        updateTree();
        QString theSelected = scene->getFullPathOfTheSelectedModel();
        if (theSelected.isEmpty() and
                not scene->mCoupled->mnode.parentNode().isNull()) { //TODO pas tres propre
            theSelected = mVpz->getFullPathModel(scene->mCoupled->mnode);
        }
        if (theSelected.isEmpty()) {
            ui->modelTree->expandToDepth(0);
        } else {
            ui->modelTree->setCurrentItem(getTreeWidgetItem(theSelected));
        }
        updateModelProperty();
    } else {
        clear();
    }
}

void
FileVpzRtool::onModelsUpdated()
{
    mExternalSelection = true;
    onInitializationDone(mCurrScene);
}

}} //namespaces
