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
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/version.hpp>
#include <QtCore/qdebug.h>
#include <QMessageBox>
#include <QtXml>

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
    QWidget(parent), ui(new Ui::FileVpzExpView),mVpz(0), mPlugin(0),
    currView(""), currOutput("")
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

void FileVpzExpView::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
    reload();
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


}

void
FileVpzExpView::reload()
{
    reloadViews();
    reloadOov();
}

void
FileVpzExpView::reloadOov()
{
    bool isEditable = ui->listVleOOV->isEditable();
    ui->listVleOOV->setEditable(true);
    ui->listVleOOV->clear();
    std::string repName;
    std::vector<std::string> pkglist;
    vle::utils::Path::path().fillBinaryPackagesList(pkglist);
    std::vector<std::string>::const_iterator itb = pkglist.begin();
    std::vector<std::string>::const_iterator ite = pkglist.end();
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
    ui->listVleOOV->setEditable(isEditable);

}

void FileVpzExpView::reloadViews()
{
    if (!mVpz) {
        throw vle::utils::InternalError(
                " gvle2: error in FileVpzExpView::reloadViews");
    }
    // Initiate the view/port tree by adding the View list
    std::vector<std::string> outputNames;
    mVpz->viewOutputNames(outputNames);
    ui->vleViewList->clear();
    std::vector<std::string>::iterator itb = outputNames.begin();
    std::vector<std::string>::iterator ite = outputNames.end();
    for ( ; itb != ite; itb++)
    {
        QListWidgetItem* item = new QListWidgetItem(QString(itb->c_str()));
        item->setFlags (item->flags () | Qt::ItemIsEditable);
        ui->vleViewList->addItem(item);
    }
}

/**
 * @brief FileVpzExpView::condHidePlugin
 *        Hide the custom (plugin) widget, and restore default display
 */
void FileVpzExpView::outputHidePlugin()
{
    QWidget *ow = mPlugin->getWidget();
    int index = ui->pluginHere->indexOf(ow);
    if (index >= 0)
        ui->pluginHere->removeWidget(ow);
    mPlugin->delWidget();
    // Plugin aloc/desalloc is managed elseware, so we can just forget reference
    mPlugin = 0;
}


void FileVpzExpView::updatePlugin(const QString& plug)
{
    if ((getSelectedViewName() != currView) or
            (getSelectedOutputPlugin() != currOutput)) {
        currView = getSelectedViewName();
        currOutput = getSelectedOutputPlugin();
        if (mPlugin)
            outputHidePlugin();
        //TODO
        QString pluginName("");
        if (plug == "vle.output/storage") {
            pluginName = "gvle2.output/storage";
        }
        if (plug == "vle.output/file") {
            pluginName = "gvle2.output/file";
        }
        if (pluginName != "") {
            PluginOutput *plugin = mVpz->getPackage()->getOutputPlugin(pluginName);

            if (plugin == 0)
            {
                mVpz->logger()->log(QString("Output plugin cannot be loaded %1")
                        .arg(pluginName));
                return;
            }
            mPlugin = plugin;
            QWidget *w = mPlugin->getWidget();
            mPlugin->init(mVpz, getSelectedViewName());

            //    // Stay informed of changes made by plugin
            //    QObject::connect(w,    SIGNAL(valueChanged(vpzExpCond *)),
            //                     this, SLOT(onPluginChanges(vpzExpCond *)));

            int index = ui->pluginHere->addWidget(w);

            ui->pluginHere->setCurrentIndex(index);
        }
    }
}

QString
FileVpzExpView::getSelectedViewName()
{
    if (ui and ui->vleViewList and ui->vleViewList->currentItem()) {
        return ui->vleViewList->currentItem()->text();
    } else {
        return "";
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

void FileVpzExpView::onViewSelected(QListWidgetItem * item)
{
    //ui->viewTypes->setEditable(true);
    ui->viewTypes->setEnabled(true);
    //ui->listVleOOV->setEditable(true);
    ui->listVleOOV->setEnabled(true);
    QDomNode viewsNode = mVpz->viewsFromDoc();
    QString plug = mVpz->getOutputPluginFromViews(viewsNode,
            item->text());
    QDomNode viewNode = mVpz->viewFromViews(viewsNode, item->text());
    QString viewType = mVpz->viewTypeFromView(viewNode);
    if (viewType == "timed") {
        ui->timeStep->setEnabled(true);
        ui->timeStep->setValue(mVpz->timeStepFromView(viewNode));
    } else {
        ui->timeStep->setEnabled(false);
    }
    ui->viewTypes->setCurrentIndex(ui->viewTypes->findText(viewType));
    ui->listVleOOV->setCurrentIndex(ui->listVleOOV->findText(plug));
    updatePlugin(plug);

}

void
FileVpzExpView::onOutputSelected(const QString& item)
{
    if (item != "") {
        if (mVpz and ui->vleViewList and ui->vleViewList->currentItem()) {
            QDomNode viewsNode = mVpz->viewsFromDoc();
            QDomNode outputsNode = mVpz->outputsFromViews(viewsNode);
            QDomNode outputNode = mVpz->outputFromOutputs(outputsNode,
                    ui->vleViewList->currentItem()->text());
            mVpz->setOutputPlugin(outputNode, item);
            updatePlugin(item);
        }
    }
}

void
FileVpzExpView::onViewTypeSelected(const QString& item)
{
    if (item != "") {
        if (mVpz and ui->vleViewList and ui->vleViewList->currentItem()
                and ui->viewTypes and ui->viewTypes->currentText() != "") {
            QDomNode viewsNode = mVpz->viewsFromDoc();
            QDomNode viewNode = mVpz->viewFromViews(viewsNode,
                    ui->vleViewList->currentItem()->text());
             mVpz->setViewTypeFromView(viewNode, item);

             if (item == "timed") {
                 ui->timeStep->setEnabled(true);
                 ui->timeStep->setValue(mVpz->timeStepFromView(viewNode));
             } else {
                 ui->timeStep->setEnabled(false);
             }
        }
    }
}

void
FileVpzExpView::onTimeStepChanged(double v)
{
    if ((v > 0)  and mVpz and ui->vleViewList and ui->vleViewList->currentItem()
                and ui->viewTypes and ui->viewTypes->currentText() == "timed") {
        QDomNode viewsNode = mVpz->viewsFromDoc();
        QDomNode viewNode = mVpz->viewFromViews(viewsNode,
                getSelectedViewName());
        mVpz->setTimeStepFromView(viewNode, v);
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
            QString viewName =mVpz->newViewNameToDoc();
            mVpz->addViewToDoc(viewName);
            QListWidgetItem* item = new QListWidgetItem(viewName);
            item->setFlags (item->flags () | Qt::ItemIsEditable);
            ui->vleViewList->addItem(item);
        }
        if (actCode == FVEVM_remove_view) {
            if (item) {
                mVpz->rmViewToDoc(item->text());
                delete item;
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
        if (mVpz->existViewFromDoc(newName)) {
            bool save = ui->vleViewList->blockSignals(true);
            item->setText(oldViewName);
            ui->vleViewList->blockSignals(save);
        } else if (oldViewName != newName) {
            mVpz->renameViewToDoc(oldViewName, newName);
        }
        oldViewName = "";
    }

}

}}//namespaces
