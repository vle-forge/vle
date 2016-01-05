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

#include <QFileInfo>
#include "pluginmodelersummary.h"
#include "ui_pluginmodelersummary.h"

/**
 * @brief pluginModelerSummary::pluginModelerSummary
 *        Default constructor
 *
 */
pluginModelerSummary::pluginModelerSummary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pluginModelerSummary)
{
    ui->setupUi(this);
    QObject::connect(ui->classList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
                     this,          SLOT  (onSelectClass(QListWidgetItem *)));
    QObject::connect(ui->buttonNew, SIGNAL(clicked()),
                     this,          SLOT(onNewClass()));
    QObject::connect(ui->buttonDuplicate, SIGNAL(clicked()),
                     this,                SLOT(onDuplicate()));
    QObject::connect(ui->buttonDuplicateCancel, SIGNAL(clicked()),
                     this,                      SLOT(onDuplicateCancel()));
    QObject::connect(ui->buttonDuplicateSave,   SIGNAL(clicked()),
                     this,                      SLOT(onDuplicateSave()));
}

/**
 * @brief pluginModelerSummary::~pluginModelerSummary
 *        Default destructor
 *
 */
pluginModelerSummary::~pluginModelerSummary()
{
    delete ui;
}

/**
 * @brief pluginModelerSummary::addClassList
 *        Add an entry to the class list
 *
 */
void pluginModelerSummary::addClassList(QString name, QString filename)
{
    // Allocate a new item
    QListWidgetItem *item = new QListWidgetItem();
    // This item show the class name, and save the associated file
    item->setText(name);
    item->setData(Qt::UserRole, QVariant(filename));
    // Then, add tis new item to the class list
    ui->classList->addItem(item);
}

/**
 * @brief pluginModelerSummary::onSelectClass (slot)
 *        Called when user select a class into list widget
 *
 */
void pluginModelerSummary::onSelectClass(QListWidgetItem *item)
{
    if (item == 0)
        return;

    // Get the file associated with the selected class
    QVariant vFilename = item->data(Qt::UserRole);
    if ( ! vFilename.isValid())
        return;

    emit openClass(vFilename.toString());
}

/**
 * @brief pluginModelerSummary::onNewClass (slot)
 *        Called when user click the "New" button
 *
 */
void pluginModelerSummary::onNewClass()
{
    emit newClass();
}

/**
 * @brief pluginModelerSummary::onDuplicate (slot)
 *        Called when user click the "Duplicate" button
 *
 */
void pluginModelerSummary::onDuplicate()
{
    QListWidgetItem *item = ui->classList->currentItem();
    if (item == 0)
        return;

    emit duplicateReq(item->text());
}

/**
 * @brief pluginModelerSummary::onDuplicateCancel (slot)
 *        Called when user click the "Cancel" button of Duplicate group box
 *
 */
void pluginModelerSummary::onDuplicateCancel()
{
    endDuplicate();
}

/**
 * @brief pluginModelerSummary::onDuplicateSave (slot)
 *        Called when user click the "Save" button of Duplicate group box
 *
 */
void pluginModelerSummary::onDuplicateSave()
{
    QListWidgetItem *item = ui->classList->currentItem();
    if (item == 0)
        return;

    // Get the file associated with the selected class
    QVariant vFilename = item->data(Qt::UserRole);
    if ( ! vFilename.isValid())
        return;

    emit duplicateClass(vFilename.toString(), ui->duplicateName->text());
}

/**
 * @brief pluginModelerSummary::startDuplicate
 *        Disable the class list and show the duplication ui
 *
 */
void pluginModelerSummary::startDuplicate()
{
    QListWidgetItem *item = ui->classList->currentItem();
    if (item == 0)
        return;

    // Disable the list during duplication process
    ui->groupBoxList->setEnabled(false);

    // Set a default new name for class
    QString newName = QString("%1%2").arg(tr("new")).arg(item->text());
    ui->duplicateName->setText(newName);
    // Show the option box to define the name of the new class
    ui->optionsStack->setCurrentIndex(1);
}

void pluginModelerSummary::endDuplicate()
{
    // Hide the option box
    ui->optionsStack->setCurrentIndex(0);
    // Clear the name edit widget
    ui->duplicateName->setText("");

    // Re-enable the class list
    ui->groupBoxList->setEnabled(true);
}
