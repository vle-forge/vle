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
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QUndoCommand>

#include "filevpzdynamics.h"
#include "ui_filevpzdynamics.h"
#include <vle/gvle/gvle_widgets.h>
#include <vle/utils/Context.hpp>

namespace vle {
namespace gvle {

/**
 * @brief FileVpzDynamics::FileVpzDynamics
 *        Default constructor for Dynamics tab of VPZ view
 *
 */
FileVpzDynamics::FileVpzDynamics(const utils::ContextPtr& ctx, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::FileVpzDynamics)
  , mVpz(0)
  , mCtx(ctx)

{
    ui->setupUi(this);
    QStringList headers;
    headers.append("Dynamic name");
    headers.append("Package");
    headers.append("Library");
    ui->tabDynamics->setHorizontalHeaderLabels(headers);

    QTableWidget* dynList = ui->tabDynamics;

    dynList->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
    dynList->horizontalHeader()->setStretchLastSection(true);

    QObject::connect(ui->tabDynamics,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,
                     SLOT(onDynamicsTableMenu(const QPoint&)));
}

/**
 * @brief FileVpzDynamics::~FileVpzDynamics
 *        Default destructor
 *
 */
FileVpzDynamics::~FileVpzDynamics()
{
    delete ui;
}

/**
 * @brief FileVpzDynamics::setVpz
 *        Set the VPZ object to use
 *
 */
void
FileVpzDynamics::setVpz(vleVpz* vpz)
{
    mVpz = vpz;
    QObject::connect(vpz, SIGNAL(dynamicsUpdated()), this, SLOT(reload()));
}

/**
 * @brief FileVpzDynamics::reload
 *        Refresh the UI from VPZ datas
 *
 */
void
FileVpzDynamics::reload()
{
    // Clear the current list content
    ui->tabDynamics->clear();
    ui->tabDynamics->setRowCount(0);

    // get list of known/installed packages
    QList<QString> pkgList;
    {
        auto pathList = mCtx->getBinaryPackages();
        for (auto i = pathList.cbegin(), e = pathList.cend(); i != e; ++i) {
            std::string name = i->filename();
            QString pkgName = QString(name.c_str());
            pkgList.append(pkgName);
        }
        qSort(pkgList.begin(), pkgList.end());
        pkgList.insert(0, "<None>");
    }

    // Get the dynamics list from VPZ
    QList<QString> ddynList;
    mVpz->fillWithDynamicsList(ddynList);
    for (int i = 0; i < ddynList.length(); i++) {
        const QString& dyn = ddynList.at(i);
        QString dynPkg = mVpz->getDynamicPackage(dyn);

        // get list of libs for the dynPkg package
        QList<QString> libList;
        {
            auto modules = mCtx->get_dynamic_libraries(
              dynPkg.toStdString(),
              utils::Context::ModuleType::MODULE_DYNAMICS);
            for (auto mod : modules) {
                std::string name = mod.library;
                QString libName = QString(name.c_str());
                libList.append(libName);
            }
            qSort(libList.begin(), libList.end());
            libList.insert(0, "<None>");
        }

        QString dynLib = mVpz->getDynamicLibrary(dyn);
        ui->tabDynamics->insertRow(i);
        VleLineEdit* dlabName =
          new VleLineEdit(ui->tabDynamics, dyn, "", false);
        QObject::connect(
          dlabName,
          SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
          this,
          SLOT(onTextUpdated(const QString&, const QString&, const QString&)));
        ui->tabDynamics->setCellWidget(i, 0, dlabName);

        VleCombo* dlabpkg = new VleCombo(ui->tabDynamics, dyn);
        dlabpkg->addItems(pkgList);
        dlabpkg->setCurrentIndex(dlabpkg->findText(dynPkg));
        ui->tabDynamics->setCellWidget(i, 1, dlabpkg);
        QObject::connect(
          dlabpkg,
          SIGNAL(valUpdated(const QString&, const QString&)),
          this,
          SLOT(onSelectPackage(const QString&, const QString&)));

        VleCombo* dlabLibrary =
          new VleCombo(ui->tabDynamics, QString("%1,%2").arg(dyn).arg(dynPkg));
        dlabLibrary->addItems(libList);
        dlabLibrary->setCurrentIndex(dlabLibrary->findText(dynLib));
        QObject::connect(
          dlabLibrary,
          SIGNAL(valUpdated(const QString&, const QString&)),
          this,
          SLOT(onSelectLibrary(const QString&, const QString&)));
        ui->tabDynamics->setCellWidget(i, 2, dlabLibrary);
    }
    ui->tabDynamics->resizeColumnToContents(0);
}

void
FileVpzDynamics::onSelectPackage(const QString& id, const QString& text)
{
    mVpz->configDynamicToDoc(id.split(",").at(0), text, "<Node>");
    reload();
}

void
FileVpzDynamics::onSelectLibrary(const QString& id, const QString& text)
{
    QStringList l = id.split(",");
    mVpz->configDynamicToDoc(l.at(0), l.at(1), text);
    reload();
}

void
FileVpzDynamics::onDynamicsTableMenu(const QPoint& pos)
{
    QPoint globalPos = ui->tabDynamics->viewport()->mapToGlobal(pos);
    QModelIndex index = ui->tabDynamics->indexAt(pos);
    QWidget* item = ui->tabDynamics->cellWidget(index.row(), index.column());

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add dynamic");
    action->setData(FVD_add_dynamic);
    action = menu.addAction("Remove dynamic");
    action->setData(FVD_remove_dynamic);
    action->setEnabled(item != 0 and index.column() == 0);

    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        FVD_MENU actCode = (FVD_MENU)selAction->data().toInt();
        switch (actCode) {
        case FVD_add_dynamic: {
            QString dynName = mVpz->newDynamicNameToDoc();
            mVpz->addDynamicToDoc(dynName, "<None>", "<None>");
            break;
        }
        case FVD_remove_dynamic: {
            mVpz->removeDyn((qobject_cast<VleLineEdit*>(item))->text());
            break;
        }
        }
    }
}

void FileVpzDynamics::onUndoRedoVpz(QDomNode /*oldValVpz*/,
                                    QDomNode /*newValVpz*/,
                                    QDomNode /*oldValVpm*/,
                                    QDomNode /*newValVpm*/)
{
    reload();
}

void
FileVpzDynamics::onTextUpdated(const QString& /*id*/,
                               const QString& old,
                               const QString& newval)
{
    mVpz->renameDynamicToDoc(old, newval);
    reload();
}

/**
 * @brief FileVpzDynamics::getSelected
 *        Search and return the Dynamic object of the current selection
 *
 */
QString
FileVpzDynamics::getSelected()
{
    int cr = ui->tabDynamics->currentRow();
    QWidget* widgetName = ui->tabDynamics->cellWidget(cr, 0);
    VleLineEdit* labelName = qobject_cast<VleLineEdit*>(widgetName);
    if (labelName == 0)
        return 0;
    return (labelName->text());
}

/**
 * @brief FileVpzDynamics::updatePackageList
 *        Refresh the package list widget
 *
 */
void FileVpzDynamics::updatePackageList(QString /*name*/)
{
    //    // Get list of known/installed packages
    //    vle::utils::PathList pathList;
    //    QList <QString> pkgList;
    //
    //    pathList = vle::utils::Path::path().getBinaryPackages();
    //    for (vle::utils::PathList::const_iterator i = pathList.begin(), e =
    //    pathList.end();
    //             i != e; ++i)
    //    {
    //        std::string name = *i;
    //        QString pkgName = QString( name.c_str() );
    //        pkgList.append(pkgName);
    //    }
    //
    //    // Sort results
    //    qSort(pkgList.begin(), pkgList.end());
    //
    //    // Clear current widget content
    //    ui->dynPackageList->clear();
    //
    //    for (int i=0; i < pkgList.count(); i++)
    //    {
    //        QString pkgName = pkgList.at(i);
    //
    //        QListWidgetItem *lwiName = new QListWidgetItem( pkgName );
    //        ui->dynPackageList->addItem(lwiName);
    //
    //        if (pkgName == name)
    //            ui->dynPackageList->setCurrentItem(lwiName);
    //    }
}

/**
 * @brief FileVpzDynamics::updateLibraryList
 *        Refresh the library list widget for the specific package
 *
 */
void FileVpzDynamics::updateLibraryList(QString /*package*/, QString /*name*/)
{

    //    vle::utils::ModuleList lst;
    //    vle::utils::ModuleManager manager;
    //    QList <QString> libList;
    //
    //    // Convert package name argument to VLE string format
    //    std::string stdPackage = package.toLocal8Bit().constData();
    //
    //    // Call VLE to get the Library list for the specified package
    //    manager.browse();
    //    manager.fill(stdPackage, vle::utils::MODULE_DYNAMICS, &lst);
    //
    //    // Read and save results
    //    vle::utils::ModuleList::iterator it;
    //    for (it = lst.begin(); it != lst.end(); ++it)
    //    {
    //        std::string name = it->library;
    //        QString libName = QString( name.c_str() );
    //        libList.append(libName);
    //    }
    //
    //    // Sort results
    //    qSort(libList.begin(), libList.end());
    //
    //    // Clear current widget content
    //    ui->dynLibraryList->clear();
    //
    //    // Parse the list
    //    for (int i=0; i < libList.count(); i++)
    //    {
    //        QString libName = libList.at(i);
    //
    //        QListWidgetItem *lwiName = new QListWidgetItem( libName );
    //        ui->dynLibraryList->addItem(lwiName);
    //        if (libName == name)
    //            ui->dynLibraryList->setCurrentItem(lwiName);
    //    }
}
}
} // namespaces
