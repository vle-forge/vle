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

#include <iostream>

#include <QMessageBox>
#include <QtCore/qdebug.h>
#include <QtXml>
#include <boost/unordered_map.hpp>
#include <boost/version.hpp>

#include "filevpzexpview.h"
#include "ui_filevpzexpview.h"
#include <vle/utils/Context.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Package.hpp>

namespace vle {
namespace gvle {

FileVpzExpView::FileVpzExpView(const utils::ContextPtr& ctx,
                               Logger* log,
                               QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::FileVpzExpView)
  , mVpz(0)
  , mPlugin(0)
  , currView("")
  , mCtx(ctx)
  , mLog(log)
{
    ui->setupUi(this);
    ui->timeStep->setEnabled(false);
    ui->listVleOOV->setEditable(false);
    ui->listVleOOV->setEnabled(false);

    ui->vleViewList->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->vleViewList,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,
                     SLOT(onViewListMenu(const QPoint&)));
}

FileVpzExpView::~FileVpzExpView()
{
    delete ui;
}

void
FileVpzExpView::setVpz(vleVpz* vpz)
{
    mVpz = vpz;

    QObject::connect(ui->vleViewList,
                     SIGNAL(itemPressed(QListWidgetItem*)),
                     this,
                     SLOT(onViewSelected(QListWidgetItem*)));
    QObject::connect(ui->vleViewList,
                     SIGNAL(itemChanged(QListWidgetItem*)),
                     this,
                     SLOT(onItemChanged(QListWidgetItem*)));
    QObject::connect(ui->listVleOOV,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(onOutputSelected(const QString&)));
    QObject::connect(ui->timeStep,
                     SIGNAL(valueChanged(double)),
                     this,
                     SLOT(onTimeStepChanged(double)));
    QObject::connect(ui->timedCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onTimedCheck(int)));
    QObject::connect(ui->outputCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onOutputCheck(int)));
    QObject::connect(ui->internalCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onInternalCheck(int)));
    QObject::connect(ui->externalCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onExternalCheck(int)));
    QObject::connect(ui->confluentCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onConfluentCheck(int)));
    QObject::connect(ui->finishCheck,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(onFinishCheck(int)));

    reload();
}

void
FileVpzExpView::reload()
{
    if (!mVpz) {
        throw vle::utils::InternalError(
          " gvle: error in FileVpzExpView::reloadViews");
    }

    if (currView != "") {
        if (not mVpz->existViewFromDoc(currView)) {
            currView = "";
        }
    }

    // reload views
    std::vector<std::string> outputNames;
    mVpz->viewOutputNames(outputNames);
    bool oldBlock = ui->vleViewList->blockSignals(true);
    ui->vleViewList->clear();
    QListWidgetItem* itemSelected = 0;
    QString viewName;
    std::vector<std::string>::iterator itb = outputNames.begin();
    std::vector<std::string>::iterator ite = outputNames.end();
    for (; itb != ite; itb++) {
        viewName = itb->c_str();
        QListWidgetItem* item = new QListWidgetItem(viewName);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole + 0, viewName);
        ui->vleViewList->addItem(item);
        if (viewName == currView) {
            itemSelected = item;
        }
    }
    if (itemSelected) {
        ui->vleViewList->setItemSelected(itemSelected, true);
    }
    ui->vleViewList->blockSignals(oldBlock);

    // reload vle output plugins
    oldBlock = ui->listVleOOV->blockSignals(true);
    bool isEditable = ui->listVleOOV->isEditable();
    ui->listVleOOV->setEditable(true);
    ui->listVleOOV->clear();
    std::string repName;
    std::vector<std::string> pkglist;
    auto pkgs = mCtx->getBinaryPackages();
    for (auto pkg : pkgs) {
        auto modules = mCtx->get_dynamic_libraries(
          pkg.filename(), utils::Context::ModuleType::MODULE_OOV);
        for (auto mod : modules) {
            repName.assign(pkg.filename());
            repName += "/";
            repName += mod.library;
            ui->listVleOOV->addItem(QString(repName.c_str()));
        }
    }

    if (currView != "") {
        QString plug = mVpz->getOutputPluginFromDoc(currView);
        ui->listVleOOV->setCurrentIndex(ui->listVleOOV->findText(plug));
        ui->listVleOOV->setEnabled(true);
    } else {
        ui->listVleOOV->setEnabled(false);
    }
    ui->listVleOOV->setEditable(isEditable);
    ui->listVleOOV->blockSignals(oldBlock);
    // reload view type
    updateViewType();
    // reload gvle plugin
    updatePlugin();
}

void
FileVpzExpView::updateViewType()
{
    if (currView == "") {
        ui->timeStep->setEnabled(false);
        ui->timedCheck->setEnabled(false);
        ui->finishCheck->setEnabled(false);
        ui->outputCheck->setEnabled(false);
        ui->internalCheck->setEnabled(false);
        ui->externalCheck->setEnabled(false);
        ui->confluentCheck->setEnabled(false);
    } else {
        ui->timeStep->blockSignals(true);
        ui->timedCheck->blockSignals(true);
        ui->finishCheck->blockSignals(true);
        ui->outputCheck->blockSignals(true);
        ui->internalCheck->blockSignals(true);
        ui->externalCheck->blockSignals(true);
        ui->confluentCheck->blockSignals(true);

        ui->timeStep->setValue(1.0);
        ui->timedCheck->setCheckState(Qt::Unchecked);
        ui->finishCheck->setCheckState(Qt::Unchecked);
        ui->outputCheck->setCheckState(Qt::Unchecked);
        ui->internalCheck->setCheckState(Qt::Unchecked);
        ui->externalCheck->setCheckState(Qt::Unchecked);
        ui->confluentCheck->setCheckState(Qt::Unchecked);
        QStringList types = mVpz->getViewTypeFromDoc(currView);
        for (int i = 0; i < types.length(); i++) {
            QString type = types[i];
            if (type == "timed") {
                ui->timedCheck->setCheckState(Qt::Checked);
            } else if (type == "finish") {
                ui->finishCheck->setCheckState(Qt::Checked);
            } else if (type == "output") {
                ui->outputCheck->setCheckState(Qt::Checked);
            } else if (type == "internal") {
                ui->internalCheck->setCheckState(Qt::Checked);
            } else if (type == "external") {
                ui->externalCheck->setCheckState(Qt::Checked);
            } else if (type == "confluent") {
                ui->confluentCheck->setCheckState(Qt::Checked);
            }
        }
        if (ui->timedCheck->checkState() == Qt::Checked) {
            ui->timeStep->setValue(mVpz->timeStepFromDoc(currView));
            ui->timeStep->setEnabled(true);
            ui->timeStep->blockSignals(false);
        } else {
            ui->timeStep->setEnabled(false);
        }
        ui->timedCheck->setEnabled(true);
        ui->finishCheck->setEnabled(true);
        ui->outputCheck->setEnabled(true);
        ui->internalCheck->setEnabled(true);
        ui->externalCheck->setEnabled(true);
        ui->confluentCheck->setEnabled(true);

        ui->timedCheck->blockSignals(false);
        ui->finishCheck->blockSignals(false);
        ui->outputCheck->blockSignals(false);
        ui->internalCheck->blockSignals(false);
        ui->externalCheck->blockSignals(false);
        ui->confluentCheck->blockSignals(false);
    }
}

void
FileVpzExpView::updatePlugin()
{
    if (mPlugin) {
        QWidget* ow = mPlugin->getWidget();
        int index = ui->pluginHere->indexOf(ow);
        if (index >= 0)
            ui->pluginHere->removeWidget(ow);
        mPlugin->delWidget();
        // TODO is that true:
        // Plugin aloc/desalloc is managed elseware, so we can just forget
        // reference
        // delete mPlugin;//should we delete ?
        mPlugin = 0;
    }
    if (currView != "") {
        mPlugin = mVpz->provideOutputGUIplugin(currView);
        if (mPlugin) {
            QWidget* w = mPlugin->getWidget();
            int index = ui->pluginHere->addWidget(w);
            ui->pluginHere->setCurrentIndex(index);
        }
    }
}

QString
FileVpzExpView::getSelectedOutputPlugin()
{
    if (ui and ui->listVleOOV) {
        return ui->listVleOOV->currentText();
    } else {
        return "";
    }
}

// Slots

void
FileVpzExpView::onViewSelected(QListWidgetItem* item)
{
    if (item->text() != currView) {
        currView = item->text();
        reload();
    }
}

void
FileVpzExpView::onOutputSelected(const QString& item)
{
    if (item != "") {
        mVpz->setOutputPluginToDoc(currView, item);
        reload();
    }
}

void
FileVpzExpView::onTimeStepChanged(double v)
{
    if (v > 0) {
        mVpz->setTimeStepToDoc(currView, v);
    }
}

void
FileVpzExpView::onTimedCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "timed");
        } else {
            mVpz->rmViewTypeToDoc(currView, "timed");
        }
    }
    updateViewType();
}

void
FileVpzExpView::onOutputCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "output");
        } else {
            mVpz->rmViewTypeToDoc(currView, "output");
        }
    }
    updateViewType();
}
void
FileVpzExpView::onInternalCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "internal");
        } else {
            mVpz->rmViewTypeToDoc(currView, "internal");
        }
    }
    updateViewType();
}
void
FileVpzExpView::onExternalCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "external");
        } else {
            mVpz->rmViewTypeToDoc(currView, "external");
        }
    }
    updateViewType();
}
void
FileVpzExpView::onConfluentCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "confluent");
        } else {
            mVpz->rmViewTypeToDoc(currView, "confluent");
        }
    }
    updateViewType();
}
void
FileVpzExpView::onFinishCheck(int v)
{
    if (currView != "") {
        if (v) {
            mVpz->addViewTypeToDoc(currView, "finish");
        } else {
            mVpz->rmViewTypeToDoc(currView, "finish");
        }
    }
    updateViewType();
}

void
FileVpzExpView::onViewListMenu(const QPoint& pos)
{
    QPoint globalPos = ui->vleViewList->mapToGlobal(pos);
    // QModelIndex index = ui->vleViewList->indexAt(pos);
    QListWidgetItem* item = ui->vleViewList->itemAt(pos);

    bool enable_view = true;
    if (item and not mVpz->enabledViewFromDoc(item->text())) {
        enable_view = false;
    }
    QAction* action;
    QMenu menu;
    action = menu.addAction("Add view");
    action->setData(FVEVM_add_view);
    action->setEnabled(true);
    action = menu.addAction("Remove view");
    action->setData(FVEVM_remove_view);
    action->setEnabled(item);
    menu.addSeparator();
    action = menu.addAction("Disable view");
    action->setData(FVEVM_disable_view);
    action->setEnabled(item);
    action->setCheckable(true);
    action->setChecked(not enable_view); // TODO if disabled

    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == FVEVM_add_view) {
            QString viewName = mVpz->newViewNameToDoc();
            mVpz->addViewToDoc(viewName);
            currView = viewName;
            reload();
        } else if (actCode == FVEVM_remove_view) {
            if (item) {
                mVpz->rmViewToDoc(item->text());
                currView = "";
                reload();
            }
        } else if (actCode == FVEVM_disable_view) {
            if (item) {
                mVpz->enableViewToDoc(item->text(), not enable_view);
            }
        }
    }
}

void
FileVpzExpView::onItemChanged(QListWidgetItem* item)
{
    QString oldViewName = item->data(Qt::UserRole + 0).toString();
    QString newName = item->text();
    if (mVpz->existViewFromDoc(newName)) {
        bool oldBlock = ui->vleViewList->blockSignals(true);
        item->setText(oldViewName);
        ui->vleViewList->blockSignals(oldBlock);
    } else if (oldViewName != newName) {
        mVpz->renameViewToDoc(oldViewName, newName);
    }
    reload();
}

void FileVpzExpView::onUndoRedoVpz(QDomNode /*oldVpz*/,
                                   QDomNode /*newVpz*/,
                                   QDomNode /*oldVpm*/,
                                   QDomNode /*newVpm*/)
{
    reload();
}
}
} // namespaces
