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

#include <QMessageBox>
#include "filevpzdynamics.h"
#include "ui_filevpzdynamics.h"

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

    ui->dynDetailBox->setVisible(false);

    QTableWidget *dynList = ui->tabDynamics;

    dynList->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    dynList->horizontalHeader()->setStretchLastSection(true);

    QObject::connect(dynList, SIGNAL(currentCellChanged(int, int, int, int)),
                     this,    SLOT  (onSelected(int, int, int, int)));

    QObject::connect(ui->dynPackageList, SIGNAL(currentRowChanged(int)),
                     this,        SLOT  (onSelectPackage(int)) );
    QObject::connect(ui->dynLibraryList, SIGNAL(currentRowChanged(int)),
                     this,        SLOT  (onSelectLibrary(int)) );

    QObject::connect(ui->dynAdd ,   SIGNAL(clicked()),
                     this,          SLOT  (onAddButton()) );
    QObject::connect(ui->dynClone,  SIGNAL(clicked()),
                     this,          SLOT  (onCloneButton()) );
    QObject::connect(ui->dynRemove, SIGNAL(clicked()),
                     this,          SLOT  (onRemoveButton()) );
    QObject::connect(ui->dynSave,   SIGNAL(clicked()),
                     this,          SLOT  (onSaveButton()) );
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

    // Get the dynamics list from VPZ
    QList <vleVpzDynamic *> *dynList = mVpz->getDynamicsList();

    for (int i = 0; i < dynList->length(); i++)
    {
        vleVpzDynamic *dyn = dynList->at(i);
        ui->tabDynamics->insertRow(i);

        QLabel *dlabName = new QLabel(ui->tabDynamics);
        dlabName->setText( dyn->getName() );
        ui->tabDynamics->setCellWidget(i, 0, dlabName);

        QLabel *dlabpkg = new QLabel(ui->tabDynamics);
        dlabpkg->setText( dyn->getPackage() );
        ui->tabDynamics->setCellWidget(i, 1, dlabpkg);

        QLabel *dlabLibrary = new QLabel(ui->tabDynamics);
        dlabLibrary->setText( dyn->getLibrary() );
        ui->tabDynamics->setCellWidget(i, 2, dlabLibrary);
    }

}

/**
 * @brief FileVpzDynamics::onSelected (slot)
 *        Called when an existing dynamic is selected from list
 *
 */
void FileVpzDynamics::onSelected(int cr, int cc, int pr, int pc)
{
    (void) cc;
    (void) pc;

    // If another column of the same row is selected ... nothing to do
    if (cr == pr)
        return;

    if (pr >= 0)
    {
        QWidget *widgetOld;
        widgetOld = ui->tabDynamics->cellWidget(pr, 0);
        widgetOld->setStyleSheet("background-color: rgb(255, 255, 255);");
        widgetOld = ui->tabDynamics->cellWidget(pr, 1);
        widgetOld->setStyleSheet("background-color: rgb(255, 255, 255);");
        widgetOld = ui->tabDynamics->cellWidget(pr, 2);
        widgetOld->setStyleSheet("background-color: rgb(255, 255, 255);");
    }
    mPackageLoaded = false;

    ui->dynPackageList->clear();
    ui->dynLibraryList->clear();

    if (cr >= 0)
    {
        QWidget *widgetNew;
        widgetNew = ui->tabDynamics->cellWidget(cr, 0);
        QLabel *labelName = qobject_cast<QLabel *>(widgetNew);
        widgetNew->setStyleSheet("background-color: rgb(180, 250, 255);");

        widgetNew = ui->tabDynamics->cellWidget(cr, 1);
        QLabel *labelPackage = qobject_cast<QLabel *>(widgetNew);
        widgetNew->setStyleSheet("background-color: rgb(180, 250, 255);");

        widgetNew = ui->tabDynamics->cellWidget(cr, 2);
        QLabel *labelLibrary = qobject_cast<QLabel *>(widgetNew);
        widgetNew->setStyleSheet("background-color: rgb(180, 250, 255);");

        // Update the lineedit for the name field
        ui->dynNameEdit->setText(labelName->text());
        // Update the Package list
        updatePackageList(labelPackage->text());
        // Update the Library list
        updateLibraryList(labelPackage->text(), labelLibrary->text());

        mPackageLoaded = true;

        ui->dynDetailBox->setVisible(true);
        ui->dynClone->setEnabled(true);
        ui->dynRemove->setEnabled(true);
    }
    else
    {
        ui->dynDetailBox->setVisible(false);
        ui->dynClone->setEnabled(false);
        ui->dynRemove->setEnabled(false);
    }
}

/**
 * @brief FileVpzDynamics::onSelectPackage (slot)
 *        Called when a package is selected from widget
 *
 */
void FileVpzDynamics::onSelectPackage(int index)
{
    if ( ! mPackageLoaded)
        return;

    // Get the name of the selected package
    QString pkgName = ui->dynPackageList->item(index)->text();
    // Reload the Library list for this package
    updateLibraryList(pkgName, "");

    // Change the background color of the Package into Dynamics list
    int cr = ui->tabDynamics->currentRow();
    QWidget *widgetPackage = ui->tabDynamics->cellWidget(cr, 1);
    widgetPackage->setStyleSheet("background-color: rgb(255, 180, 180);");
}

/**
 * @brief FileVpzDynamics::onSelectLibrary (slot)
 *        Called when a library is selected from widget
 *
 */
void FileVpzDynamics::onSelectLibrary(int index)
{
    (void)index;

    if ( ! mPackageLoaded)
        return;

    // Change the background color of the Library into Dynamics list
    int cr = ui->tabDynamics->currentRow();
    QWidget *widgetPackage = ui->tabDynamics->cellWidget(cr, 2);
    widgetPackage->setStyleSheet("background-color: rgb(255, 180, 180);");
}

/**
 * @brief FileVpzDynamics::onAddButton (slot)
 *        Called when the "Add" button is clicked
 *
 */
void FileVpzDynamics::onAddButton()
{
    // Create a new Dynamic
    vleVpzDynamic *dyn = new vleVpzDynamic();
    // Set a default name, but no default package/library
    dyn->setName(tr("A_New_Dynamic"));
    // Insert it to the current VPZ
    mVpz->addDynamic(dyn);

    // Reload the Dynamics list to show the newly created dynamic
    reload();
}

/**
 * @brief FileVpzDynamics::onCloneButton (slot)
 *        Called when the "Clone" button is clicked
 *
 */
void FileVpzDynamics::onCloneButton()
{
    vleVpzDynamic *dynamic = getSelected();

    // Make a new Dynamic with the same values
    vleVpzDynamic *newDynamic = new vleVpzDynamic(dynamic);
    // Update his name
    newDynamic->setName( QString(dynamic->getName()).append("Clone") );
    // Insert it to the current VPZ
    mVpz->addDynamic(newDynamic);

    // Reload the Dynamics list to show the newly created dynamic
    reload();
}

/**
 * @brief FileVpzDynamics::onRemoveButton (slot)
 *        Called when the "Remove" button is clicked
 *
 */
void FileVpzDynamics::onRemoveButton()
{
    vleVpzDynamic *dynamic = getSelected();
    mVpz->removeDynamic(dynamic);

    // Reload the Dynamics list to show the newly created dynamic
    reload();
}

/**
 * @brief FileVpzDynamics::onSaveButton
 *        Called when a clieck event occurs from the "Save" button
 *
 */
void FileVpzDynamics::onSaveButton()
{
    bool altered = false;

    int cr = ui->tabDynamics->currentRow();
    QWidget *widgetName  = ui->tabDynamics->cellWidget(cr, 0);
    QLabel  *labelName   = qobject_cast<QLabel *>(widgetName);
    QString  selName     = labelName->text();

    vleVpzDynamic *dynamic = mVpz->getDynamic(selName);
    if (dynamic == 0)
        throw tr("Save on an unknown dynamic");

    // Get the dynamic name from lineedit widget
    QString newName = ui->dynNameEdit->text();
    // Test name validity
    if (newName.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Name of a Dynamic can't be empty"));
        msgBox.exec();
        return;
    }

    QString pkgName;
    if (ui->dynPackageList->currentItem())
        pkgName = ui->dynPackageList->currentItem()->text();
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please select a valid Package first"));
        msgBox.exec();
        return;
    }

    QString libName;
    if (ui->dynLibraryList->currentItem())
        libName = ui->dynLibraryList->currentItem()->text();
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please select a valid Library first"));
        msgBox.exec();
        return;
    }

    if (newName != dynamic->getName())
    {
        dynamic->setName(newName);
        altered = true;
    }

    if (pkgName != dynamic->getPackage())
    {
        dynamic->setPackage(pkgName);
        altered = true;
    }

    if (libName != dynamic->getLibrary())
    {
        dynamic->setLibrary(libName);
        altered = true;
    }

    if (altered)
        reload();
}

/**
 * @brief FileVpzDynamics::getSelected
 *        Search and return the Dynamic object of the current selection
 *
 */
vleVpzDynamic *FileVpzDynamics::getSelected()
{
    int cr = ui->tabDynamics->currentRow();
    QWidget *widgetName  = ui->tabDynamics->cellWidget(cr, 0);
    QLabel  *labelName   = qobject_cast<QLabel *>(widgetName);
    if (labelName == 0)
        return 0;
    QString  selName     = labelName->text();

    vleVpzDynamic *dynamic = mVpz->getDynamic(selName);

    return dynamic;
}

/**
 * @brief FileVpzDynamics::updatePackageList
 *        Refresh the package list widget
 *
 */
void FileVpzDynamics::updatePackageList(QString name)
{
    vleVpzDynamic *vpzDynamic = new vleVpzDynamic();

    // Get list of known/installed packages
    QList<QString> *pkgList = vpzDynamic->getPackageList();
    // Sort results
    qSort(pkgList->begin(), pkgList->end());

    // Clear current widget content
    ui->dynPackageList->clear();

    for (int i=0; i < pkgList->count(); i++)
    {
        QString pkgName = pkgList->at(i);

        QListWidgetItem *lwiName = new QListWidgetItem( pkgName );
        ui->dynPackageList->addItem(lwiName);

        if (pkgName == name)
            ui->dynPackageList->setCurrentItem(lwiName);
    }

    delete vpzDynamic;
}

/**
 * @brief FileVpzDynamics::updateLibraryList
 *        Refresh the library list widget for the specific package
 *
 */
void FileVpzDynamics::updateLibraryList(QString package, QString name)
{
    vleVpzDynamic *vpzDynamic = new vleVpzDynamic();

    // Get list of libraries available into the specified package
    QList<QString> *libList = vpzDynamic->getLibraryList(package);
    // Sort results
    qSort(libList->begin(), libList->end());

    // Clear current widget content
    ui->dynLibraryList->clear();

    // Parse the list
    for (int i=0; i < libList->count(); i++)
    {
        QString libName = libList->at(i);

        QListWidgetItem *lwiName = new QListWidgetItem( libName );
        ui->dynLibraryList->addItem(lwiName);
        if (libName == name)
            ui->dynLibraryList->setCurrentItem(lwiName);
    }

    delete vpzDynamic;
}
