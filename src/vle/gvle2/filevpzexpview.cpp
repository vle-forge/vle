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
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/version.hpp>
#include <QtCore/qdebug.h>
#include <QMessageBox>
#include <QtXml>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include "vlepackage.h"
#include "filevpzexpview.h"
#include "ui_filevpzexpview.h"


/**
 * @brief Get the plug-in name of the filename provided.
 *
 * @code
 * // return "toto".
 * utils::Module::getPluginName("/home/foo/bar/libtoto.so");
 * @endcode
 *
 * @param path The path of the file to convert;
 * @return The plug-in name;
 *
 * TODO function from ModuleManager
 */
static std::string TMPgetLibraryName(const boost::filesystem::path& file)
{
    namespace fs = boost::filesystem;
    std::string library;

#if BOOST_VERSION > 104500
    if (file.filename().string().compare(0, 3, "lib") == 0) {
        library.append(file.filename().string(), 3, std::string::npos);
    }
#else
    if (file.filename().compare(0, 3, "lib") == 0) {
        library.append(file.filename(), 3, std::string::npos);
    }
#endif

#ifdef BOOST_WINDOWS
    if (fs::extension(file) == ".dll") {
        library.assign(library, 0, library.size() - 4);
    }
#else
    if (fs::extension(file) == ".so") {
        library.assign(library, 0, library.size() - 3);
    }
#endif

    return library;
}

namespace vle {
namespace gvle2 {

FileVpzExpView::FileVpzExpView(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzExpView),mVpm(0), mPlugin(0),
    currView("")
{
    ui->setupUi(this);
    ui->timeStep->setEnabled(false);
    ui->location->setEnabled(false);
    ui->viewTypes->setEditable(false);
    ui->viewTypes->setEnabled(false);
    ui->listVleOOV->setEditable(false);
    ui->listVleOOV->setEnabled(false);

    ui->vleViewList->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->vleViewList,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onViewListMenu(const QPoint&)));
}

FileVpzExpView::~FileVpzExpView()
{
    delete ui;
}

void FileVpzExpView::setVpm(vleVpm* vpm)
{
    mVpm = vpm;

    QObject::connect(ui->vleViewList, SIGNAL(itemPressed (QListWidgetItem *)),
                     this, SLOT(onViewSelected(QListWidgetItem *)));
    QObject::connect(ui->vleViewList, SIGNAL(itemChanged(QListWidgetItem *)),
                         this, SLOT(onItemChanged(QListWidgetItem *)));
    QObject::connect(ui->listVleOOV, SIGNAL(currentIndexChanged(const QString&)),
                     this, SLOT(onOutputSelected(const QString&)));
    QObject::connect(ui->viewTypes, SIGNAL(currentIndexChanged(const QString&)),
                     this, SLOT(onViewTypeSelected(const QString&)));
    QObject::connect(ui->timeStep, SIGNAL(valueChanged (double)),
                         this, SLOT(onTimeStepChanged(double)));
    QObject::connect(mVpm,
            SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
            this,
            SLOT(onUndoRedoVpm(QDomNode, QDomNode, QDomNode, QDomNode)));

    reload();
}

void
FileVpzExpView::reload()
{
    if (!mVpm) {
        throw vle::utils::InternalError(
                " gvle2: error in FileVpzExpView::reloadViews");
    }

    if (currView != "") {
        if (not mVpm->existViewFromDoc(currView)) {
            currView = "";
        }
    }

    //reload views
    std::vector<std::string> outputNames;
    mVpm->viewOutputNames(outputNames);
    bool oldBlock = ui->vleViewList->blockSignals(true);
    ui->vleViewList->clear();
    QListWidgetItem* itemSelected = 0;
    std::vector<std::string>::iterator itb = outputNames.begin();
    std::vector<std::string>::iterator ite = outputNames.end();
    for ( ; itb != ite; itb++) {
        QListWidgetItem* item = new QListWidgetItem(QString(itb->c_str()));
        item->setFlags (item->flags () | Qt::ItemIsEditable);
        ui->vleViewList->addItem(item);
        if (QString(itb->c_str()) == currView) {
            itemSelected = item;
        }

    }
    if (itemSelected) {
        ui->vleViewList->setItemSelected(itemSelected, true);
    }
    ui->vleViewList->blockSignals(oldBlock);

    //reload vle output plugins
    oldBlock = ui->listVleOOV->blockSignals(true);
    bool isEditable = ui->listVleOOV->isEditable();
    ui->listVleOOV->setEditable(true);
    ui->listVleOOV->clear();
    std::string repName;
    std::vector<std::string> pkglist;
    vle::utils::Path::path().fillBinaryPackagesList(pkglist);
    itb = pkglist.begin();
    ite = pkglist.end();
    for (; itb != ite; itb++) {
        vle::utils::Package pkg(*itb);
        vle::utils::PathList pathList = pkg.getPluginsOutput();
        vle::utils::PathList::const_iterator ipb = pathList.begin();
        vle::utils::PathList::const_iterator ipe = pathList.end();
        for (; ipb != ipe; ipb++) {
            repName.assign(*itb);
            repName += "/";
            repName += TMPgetLibraryName(*ipb);
            ui->listVleOOV->addItem(QString(repName.c_str()));
        }
    }
    if (currView != "") {
        QString plug = mVpm->getOutputPluginFromDoc(currView);
        ui->listVleOOV->setCurrentIndex(ui->listVleOOV->findText(plug));
        ui->listVleOOV->setEnabled(true);
    } else {
        ui->listVleOOV->setEnabled(false);
    }
    ui->listVleOOV->setEditable(isEditable);
    ui->listVleOOV->blockSignals(oldBlock);
    //reload view type
    oldBlock = ui->viewTypes->blockSignals(true);
    if (currView != "") {
        ui->viewTypes->setCurrentIndex(
                ui->viewTypes->findText(mVpm->viewTypeFromDoc(currView)));
        ui->viewTypes->setEnabled(true);
    } else {
        ui->viewTypes->setEnabled(false);
    }
    ui->viewTypes->blockSignals(oldBlock);
    //reload time step
    oldBlock = ui->timeStep->blockSignals(true);
    if (currView != "") {
        QString viewType = mVpm->viewTypeFromDoc(currView);
        if (viewType == "timed") {
            ui->timeStep->setEnabled(true);
            ui->timeStep->setValue(mVpm->timeStepFromDoc(currView));
        } else {
            ui->timeStep->setEnabled(false);
        }
    } else {
        ui->timeStep->setEnabled(false);
    }
    ui->timeStep->blockSignals(oldBlock);
    //reload gvle2 plugin
    updatePlugin();

}



void FileVpzExpView::updatePlugin()
{
    if (mPlugin) {
        QWidget* ow = mPlugin->getWidget();
        int index = ui->pluginHere->indexOf(ow);
        if (index >= 0)
            ui->pluginHere->removeWidget(ow);
        mPlugin->delWidget();
        // TODO is that true:
        //Plugin aloc/desalloc is managed elseware, so we can just forget reference
        //delete mPlugin;//should we delete ?
        mPlugin = 0;
    }
    if (currView != "") {
        mPlugin = mVpm->provideOutputGUIplugin(currView);
        if (mPlugin) {
            QWidget *w = mPlugin->getWidget();
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

//Slots

void FileVpzExpView::onViewSelected(QListWidgetItem* item)
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
        mVpm->setOutputPluginToDoc(currView, item);
        reload();
    }
}

void
FileVpzExpView::onViewTypeSelected(const QString& item)
{
    if (item != "") {
        mVpm->setViewTypeToDoc(currView, item);
        reload();
    }
}

void
FileVpzExpView::onTimeStepChanged(double v)
{
    if (v > 0) {
        mVpm->setTimeStepToDoc(currView, v);
    }
}

void
FileVpzExpView::onViewListMenu(const QPoint& pos)
{
    QPoint globalPos = ui->vleViewList->mapToGlobal(pos);
    //QModelIndex index = ui->vleViewList->indexAt(pos);
    QListWidgetItem* item = ui->vleViewList->itemAt(pos);

    QAction* action;
    QMenu myMenu;
    action = myMenu.addAction("Add view");
    action->setData(FVEVM_add_view);
    action->setEnabled(true);
    action = myMenu.addAction("Remove view");
    action->setData(FVEVM_remove_view);
    action->setEnabled(item);
    action = myMenu.addAction("Rename view");
    action->setData(FVEVM_rename_view);
    action->setEnabled(item);


    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == FVEVM_add_view) {
            QString viewName = mVpm->newViewNameToDoc();
            mVpm->addViewToDoc(viewName);
            currView = viewName;
            reload();
//            QListWidgetItem* item = new QListWidgetItem(viewName);
//            item->setFlags (item->flags () | Qt::ItemIsEditable);
//            ui->vleViewList->addItem(item);
        }
        if (actCode == FVEVM_remove_view) {
            if (item) {
                mVpm->rmViewToDoc(item->text());
                currView = "";
                reload();
            }
        }
        if (actCode == FVEVM_rename_view) {
            if (item) {
                oldViewName = item->text();
                ui->vleViewList->editItem(item);
            }
        }

    }
}

void
FileVpzExpView::onItemChanged(QListWidgetItem * item)
{
    if (not oldViewName.isEmpty())  {
        QString newName = item->text();
        if (mVpm->existViewFromDoc(newName)) {
            bool save = ui->vleViewList->blockSignals(true);
            item->setText(oldViewName);
            ui->vleViewList->blockSignals(save);
        } else if (oldViewName != newName) {
            mVpm->renameViewToDoc(oldViewName, newName);
        }
        oldViewName = "";
        reload();
    }

}

void
FileVpzExpView::onUndoRedoVpm(QDomNode /*oldVpz*/, QDomNode /*newVpz*/,
        QDomNode /*oldVpm*/, QDomNode /*newVpm*/)
{
    reload();
}

}}//namespaces
