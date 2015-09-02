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

#include <vle/utils/ModuleManager.hpp>
#include <QMessageBox>
#include <QComboBox>
#include <QMenu>
#include <QUndoCommand>
#include "filevpzdynamics.h"
#include "ui_filevpzdynamics.h"

#include <QDebug>



AdhocCombo::AdhocCombo(QWidget *parent): QComboBox(parent)
{

}
AdhocCombo::~AdhocCombo()
{

}
void
AdhocCombo::setId(const QString& text)
{
    combo_id = text;
}

void
AdhocCombo::mousePressEvent(QMouseEvent * e)
{
    emit comboSelected(combo_id);
    //file_vpz_dyn->onComboSelected(combo_id);
    QComboBox::mousePressEvent(e);
}


AdhocQTextEdit::AdhocQTextEdit(QWidget *parent): QPlainTextEdit(parent)
{
    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    this->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
//    this->setTextInteractionFlags(Qt::TextEditorInteraction);
//    this->setTextInteractionFlags(Qt::TextEditable);
    this->setTextInteractionFlags(Qt::NoTextInteraction);
    this->setContextMenuPolicy(Qt::NoContextMenu);
}
AdhocQTextEdit::~AdhocQTextEdit()
{

}

void
AdhocQTextEdit::setTextEdition(bool val)
{
    if (val and this->textInteractionFlags() == Qt::NoTextInteraction) {
        saved_value = document()->toPlainText();
        this->setTextInteractionFlags(Qt::TextEditorInteraction);
        this->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
    } else {
        this->setTextInteractionFlags(Qt::NoTextInteraction);
        this->clearFocus();
        if (document()->toPlainText() != saved_value) {
            emit textUpdated(saved_value, document()->toPlainText());
        }
    }
}

void
AdhocQTextEdit::setText(const QString& text)
{
    this->document()->setPlainText(text);
    saved_value = text;
}

void
AdhocQTextEdit::focusOutEvent(QFocusEvent *e)
{
   setTextEdition(false);
}

/**
 * @brief FileVpzDynamics::FileVpzDynamics
 *        Default constructor for Dynamics tab of VPZ view
 *
 */
FileVpzDynamics::FileVpzDynamics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileVpzDynamics)
{
    mVpz = 0;
    mPackageLoaded = false;

    ui->setupUi(this);
    QStringList headers;
    headers.append("Dynamic name");
    headers.append("Package");
    headers.append("Library");
    ui->tabDynamics->setHorizontalHeaderLabels(headers);

//    ui->dynDetailBox->setVisible(false);

    QTableWidget* dynList = ui->tabDynamics;

    dynList->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    dynList->horizontalHeader()->setStretchLastSection(true);

    QObject::connect(ui->tabDynamics,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onDynamicsTableMenu(const QPoint&)));

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
void FileVpzDynamics::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
    QObject::connect(mVpz->undoStack, SIGNAL(undoRedoVpz(QDomNode, QDomNode)),
            this, SLOT(onUndoRedoVpz(QDomNode, QDomNode)));
}

/**
 * @brief FileVpzDynamics::reload
 *        Refresh the UI from VPZ datas
 *
 */
void FileVpzDynamics::reload()
{
    // Clear the current list content
    ui->tabDynamics->clear();
    ui->tabDynamics->setRowCount(0);

    // get list of known/installed packages
    QList <QString> pkgList;
    {
        vle::utils::PathList pathList;
        pathList = vle::utils::Path::path().getBinaryPackages();
        for (vle::utils::PathList::const_iterator i = pathList.begin(),
                e = pathList.end(); i != e; ++i) {
            std::string name = *i;
            QString pkgName = QString( name.c_str() );
            pkgList.append(pkgName);
        }
        qSort(pkgList.begin(), pkgList.end());
        pkgList.insert(0,"<None>");
    }

    // Get the dynamics list from VPZ
    QList<QString> ddynList;
    mVpz->fillWithDynamicsList(ddynList);
    for (int i = 0; i < ddynList.length(); i++) {
        const QString& dyn = ddynList.at(i);
        QString dynPkg = mVpz->getDynamicPackage(dyn);

        //get list of libs for the dynPkg package
        QList <QString> libList;
        {
            vle::utils::ModuleList lst;
            vle::utils::ModuleManager manager;
            std::string stdPackage = dynPkg.toLocal8Bit().constData();
            manager.browse();
            manager.fill(stdPackage, vle::utils::MODULE_DYNAMICS, &lst);
            vle::utils::ModuleList::iterator it;
            for (it = lst.begin(); it != lst.end(); ++it) {
                std::string name = it->library;
                QString libName = QString( name.c_str() );
                libList.append(libName);
            }
            qSort(libList.begin(), libList.end());
            libList.insert(0,"<None>");
        }


        QString dynLib = mVpz->getDynamicLibrary(dyn);
        ui->tabDynamics->insertRow(i);
        AdhocQTextEdit* dlabName = new AdhocQTextEdit(ui->tabDynamics);
        dlabName->setText(dyn);
        QObject::connect(dlabName,
                SIGNAL(textUpdated(const QString&, const QString&)),
                this, SLOT(onTextUpdated(const QString&, const QString&)));
        ui->tabDynamics->setCellWidget(i, 0, dlabName);

        AdhocCombo* dlabpkg = new AdhocCombo(ui->tabDynamics);
        dlabpkg->addItems(pkgList);
        dlabpkg->setCurrentIndex(dlabpkg->findText(dynPkg));
        dlabpkg->setId(dyn);
        ui->tabDynamics->setCellWidget(i, 1, dlabpkg);
        QObject::connect(dlabpkg,
                        SIGNAL(currentIndexChanged(const QString&)),
                        this, SLOT(onSelectPackage(const QString&)));
        QObject::connect(dlabpkg,
                        SIGNAL(comboSelected(const QString&)),
                        this, SLOT(onComboSelected(const QString&)));


        AdhocCombo* dlabLibrary = new AdhocCombo(ui->tabDynamics);
        dlabLibrary->addItems(libList);
        dlabLibrary->setCurrentIndex(dlabLibrary->findText(dynLib));
        dlabLibrary->setId(dyn);
        QObject::connect(dlabLibrary,
                        SIGNAL(currentIndexChanged(const QString&)),
                        this, SLOT(onSelectLibrary(const QString&)));
        QObject::connect(dlabLibrary,
                        SIGNAL(comboSelected(const QString&)),
                        this, SLOT(onComboSelected(const QString&)));
        ui->tabDynamics->setCellWidget(i, 2, dlabLibrary);
    }
    ui->tabDynamics->resizeColumnToContents(0);
}

void
FileVpzDynamics::onComboSelected(const QString& dyn)
{
    curr_dyn = dyn;
}

void
FileVpzDynamics::onSelectPackage(const QString& text)
{

    mVpz->configDynamicToDoc(curr_dyn, text, "<Node>");
    reload();

//    if ( ! mPackageLoaded)
//        return;
//
//    // Get the name of the selected package
//    QString pkgName = ui->dynPackageList->item(index)->text();
//    // Reload the Library list for this package
//    updateLibraryList(pkgName, "");
//
//    // Change the background color of the Package into Dynamics list
//    int cr = ui->tabDynamics->currentRow();
//    QWidget *widgetPackage = ui->tabDynamics->cellWidget(cr, 1);
//    widgetPackage->setStyleSheet("background-color: rgb(255, 180, 180);");
}

/**
 * @brief FileVpzDynamics::onSelectLibrary (slot)
 *        Called when a library is selected from widget
 *
 */
void FileVpzDynamics::onSelectLibrary(const QString& text)
{

    QString pkgName = "";
    int i=0;
    bool found = false;
    for (; (i<ui->tabDynamics->rowCount() and not found);i++) {
        QString dyni = (qobject_cast<AdhocQTextEdit*>(
                ui->tabDynamics->cellWidget(i,0)))->toPlainText();
        if (dyni == curr_dyn) {
            pkgName = (qobject_cast<AdhocCombo*>(
                    ui->tabDynamics->cellWidget(i,1)))->currentText();
        }

    }
    mVpz->configDynamicToDoc(curr_dyn, pkgName, text);
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
    action = menu.addAction("Edit name");
    action->setData(FVD_rename_dynamic);
    action->setEnabled(item != 0);
    action = menu.addAction("Remove");
    action->setData(FVD_remove_dynamic);
    action->setEnabled(item != 0);


    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        FVD_MENU actCode = (FVD_MENU) selAction->data().toInt();
        switch(actCode){
        case FVD_add_dynamic: {
            QString dynName = mVpz->newDynamicNameToDoc();
            mVpz->addDynamicToDoc(dynName, "<None>", "<None>");
            reload();
            break;
        } case FVD_rename_dynamic: {
            AdhocQTextEdit* itemText = qobject_cast<AdhocQTextEdit*>(item);
            itemText->setTextEdition(true);
            break;
        } case FVD_remove_dynamic: {
            mVpz->removeDyn((qobject_cast<AdhocQTextEdit*>(item))->toPlainText());
            reload();
            break;
        }}
    }
}

void
FileVpzDynamics::onUndoRedoVpz(QDomNode oldVal, QDomNode newVal)
{
    reload();
}

void
FileVpzDynamics::onTextUpdated(const QString& old, const QString& newval)
{
    mVpz->renameDynamicToDoc(old, newval);
    reload();
}

/**
 * @brief FileVpzDynamics::getSelected
 *        Search and return the Dynamic object of the current selection
 *
 */
QString FileVpzDynamics::getSelected()
{
    int cr = ui->tabDynamics->currentRow();
    QWidget *widgetName  = ui->tabDynamics->cellWidget(cr, 0);
    AdhocQTextEdit  *labelName   = qobject_cast<AdhocQTextEdit *>(widgetName);
    if (labelName == 0)
        return 0;
    return (labelName->toPlainText());
}

/**
 * @brief FileVpzDynamics::updatePackageList
 *        Refresh the package list widget
 *
 */
void FileVpzDynamics::updatePackageList(QString name)
{
//    // Get list of known/installed packages
//    vle::utils::PathList pathList;
//    QList <QString> pkgList;
//
//    pathList = vle::utils::Path::path().getBinaryPackages();
//    for (vle::utils::PathList::const_iterator i = pathList.begin(), e = pathList.end();
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
void FileVpzDynamics::updateLibraryList(QString package, QString name)
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
