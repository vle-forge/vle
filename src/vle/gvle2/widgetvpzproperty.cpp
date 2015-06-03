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
#include "widgetvpzproperty.h"
#include "ui_widgetvpzpropertydynamics.h"
#include "ui_widgetvpzpropertyexpcond.h"
#include "ui_widgetvpzpropertyobservables.h"
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QDebug>

WidgetVpzPropertyDynamics::WidgetVpzPropertyDynamics(QWidget *parent) :
    QWidget(parent), ui(new Ui::WidgetVpzPropertyDynamics),
    mModel(0)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

    ui->comboBox->setVisible(true);

    // Signal used in edit mode when combo-box value change
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                     this,         SLOT  (onChange(int)));
}

WidgetVpzPropertyDynamics::~WidgetVpzPropertyDynamics()
{
    delete ui;
}

void
WidgetVpzPropertyDynamics::setModel(vleVpzModel* model)
{
    int index = 0;
    int count = 1;
    mModel = model;
    if (not mModel)
        return;
    // Clear the current list
    while(ui->comboBox->count())
        ui->comboBox->removeItem(0);
    // Add the first (default) item
    ui->comboBox->addItem( tr("<none>") );
    ui->comboBox->setCurrentIndex(0);
    // Get the Dynamic of the model
    vleVpzDynamic* dyn = mModel->getDynamic();
    if (dyn) {
        //atomic model TODO in vleVpzModel
        QListIterator<vleVpzDynamic*> dynItems(
                *mModel->getVpz()->getDynamicsList());
        while( dynItems.hasNext() )
        {
            vleVpzDynamic *dynItem = dynItems.next();
            ui->comboBox->addItem(dynItem->getName());

            if (dynItem == mModel->getDynamic())
                index = count;
            count ++;
        }
        ui->comboBox->setCurrentIndex(index);
    }
}

void
WidgetVpzPropertyDynamics::onChange(int index)
{
    // Get the selected dynamic name from widget
    QString selectedName = ui->comboBox->itemText(index);

    // If the selected Dynamic exists
    if (mModel->getVpz()->getDynamic(selectedName)) {
        // Update the model with the selected Dynamic
        mModel->setDynamic(selectedName);
    }
}

WidgetVpzPropertyExpCond::WidgetVpzPropertyExpCond(QWidget *parent) :
        QWidget(parent), ui(new Ui::WidgetVpzPropertyExpCond),
        mModel(0)
{
    ui->setupUi(this);
    ui->listCond->setVisible(true);
}

WidgetVpzPropertyExpCond::~WidgetVpzPropertyExpCond()
{
    delete ui;
}

void
WidgetVpzPropertyExpCond::setModel(vleVpzModel *model)
{
    mModel = model;
    vleVpz* vpz = mModel->getVpz();
    // First, clear the current list content
    ui->listCond->clear();
    vleVpzDynamic* dyn = mModel->getDynamic();
    if (dyn) {
        //TODO only way to see if it is an atomic model ??
        QDomNodeList conds = vpz->condsListFromConds(vpz->condsFromDoc());
        for (unsigned int i=0; i< conds.length(); i++) {
            QDomNode cond = conds.at(i);
            QString condName = vpz->attributeValue(cond, "name");
            QListWidgetItem *wi = new QListWidgetItem(ui->listCond);
            ui->listCond->addItem(wi);
            QCheckBox *cb = new QCheckBox(this);
            cb->setText( condName);
            if (mModel->hasCondition(condName)) {
                cb->setCheckState( Qt::Checked );
            } else {
                cb->setCheckState( Qt::Unchecked );
            }
            QObject::connect(cb,   SIGNAL(toggled(bool)),
                    this, SLOT(onCheckboxToggle(bool)));
            ui->listCond->setItemWidget(wi, cb);
        }
    }
}

void
WidgetVpzPropertyExpCond::onCheckboxToggle(bool checked)
{
    // Get the QCheckbox that emit signal
    QObject   *sender = QObject::sender();
    QCheckBox *cb     = qobject_cast<QCheckBox *>(sender);

    // Sanity check ... if no model set or checkbox not found
    if ( (mModel == 0) || (cb == 0) )
        return;

    // Search the vleExpCond associated with the checkbox
    vleVpz* vpz   = mModel->getVpz();
    if (checked) {
        vpz->attachCondToAtomicModel(mModel->getFullName(), cb->text());
    } else {
        vpz->detachCondToAtomicModel(mModel->getFullName(), cb->text());
    }
}

WidgetVpzPropertyObservables::WidgetVpzPropertyObservables(QWidget *parent) :
        QWidget(parent), ui(new Ui::WidgetVpzPropertyObservables),
        mModel(0)
{
    ui->setupUi(this);
    ui->listObs->setVisible(true);
}

WidgetVpzPropertyObservables::~WidgetVpzPropertyObservables()
{
    delete ui;
}

void
WidgetVpzPropertyObservables::refresh()
{
    setModel(mModel);
}

void
WidgetVpzPropertyObservables::setModel(vleVpzModel *model)
{
    mModel = model;
    vleVpz* vpz = mModel->getVpz();
    ui->listObs->clear();

    vleVpzDynamic* dyn = mModel->getDynamic();
    if (dyn) {

        QDomNodeList obss = vpz->obssListFromObss(
            vpz->obsFromViews(vpz->viewsFromDoc()));

        for (unsigned int i=0; i< obss.length(); i++) {
            QDomNode obs = obss.at(i);
            QString obsName = vpz->attributeValue(obs, "name");
            QListWidgetItem *wi = new QListWidgetItem(ui->listObs);
            ui->listObs->addItem(wi);
            QCheckBox *cb = new QCheckBox(this);
            cb->setText(obsName);
            if (vpz->modelObsFromDoc(mModel->getFullName()) == obsName) {
                cb->setCheckState( Qt::Checked );
            } else {
                cb->setCheckState( Qt::Unchecked );
            }
            QObject::connect(cb, SIGNAL(clicked(bool)),
                             this, SLOT(onCheckboxClick(bool)));
            ui->listObs->setItemWidget(wi, cb);
        }
    }
}

void
WidgetVpzPropertyObservables::onCheckboxClick(bool checked)
{
    QObject *sender = QObject::sender();
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender);

    if ( (mModel == 0) || (cb == 0) )
        return;

    vleVpz* vpz = mModel->getVpz();

    if (checked) {
        vpz->setObsToAtomicModel(mModel->getFullName(), cb->text());

        for(int row = 0; row <  ui->listObs->count(); row++)
        {
            QListWidgetItem *item = ui->listObs->item(row);
            QCheckBox *cbi = qobject_cast<QCheckBox *>(ui->listObs->itemWidget(item));
            if (cbi->text() != cb->text()) {
                cbi->setCheckState(Qt::Unchecked );
            } else {
                cbi->setCheckState(Qt::Checked );
            }
        }
    } else {
        vpz->unsetObsFromAtomicModel(mModel->getFullName());
    }
}
