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

namespace vle {
namespace gvle2 {


WidgetVpzPropertyDynamics::WidgetVpzPropertyDynamics(QWidget *parent) :
    QWidget(parent), ui(new Ui::WidgetVpzPropertyDynamics),
    mVpm(0), mModQuery("")
{
    ui->setupUi(this);
    setFocusPolicy(Qt::ClickFocus);

    ui->comboBox->setVisible(true);
    ui->comboBox->setFocusPolicy(Qt::ClickFocus);
//    ui->comboBox->setVisible(true);

    // Signal used in edit mode when combo-box value change
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                     this,         SLOT  (onChange(int)));
}

WidgetVpzPropertyDynamics::~WidgetVpzPropertyDynamics()
{
    delete ui;
}

void
WidgetVpzPropertyDynamics::setModel(vleVpz* model, const QString& mod_query)
{
    int index = 0;
    int count = 1;
    mVpm = model;
    mModQuery = mod_query;

    if (not mVpm)
        return;

    // Get the Dynamic of the model
    QString dyn = mVpm->modelDynFromDoc(mModQuery);
    bool oldBlockComboBox = ui->comboBox->blockSignals(true);
    while(ui->comboBox->count()) {
        ui->comboBox->removeItem(0);
    }
    ui->comboBox->addItem( tr("<none>") );

    QList<QString> dynList;
    mVpm->fillWithDynamicsList(dynList);
    for (int i=0; i< dynList.length(); i++) {
        ui->comboBox->addItem(dynList.at(i));
        if (dyn == dynList.at(i)) {
            index = count;
        }
        count ++;
    }
    ui->comboBox->setCurrentIndex(index);
    ui->comboBox->blockSignals(oldBlockComboBox);

}

void
WidgetVpzPropertyDynamics::onChange(int index)
{
    // Get the selected dynamic name from widget
    QString selectedName = ui->comboBox->itemText(index);

    // If the selected Dynamic exists
    if (mVpm->existDynamicIntoDoc(selectedName)) {
        // Update the model with the selected Dynamic
        mVpm->setDynToAtomicModel(mModQuery, selectedName);
    } else if (selectedName == "<none>") {
        mVpm->setDynToAtomicModel(mModQuery, "");
    }
}

/****************************************************************************/

WidgetVpzPropertyExpCond::WidgetVpzPropertyExpCond(QWidget *parent) :
        QWidget(parent), ui(new Ui::WidgetVpzPropertyExpCond),
        mVpm(0), mModQuery("")
{
    ui->setupUi(this);
    ui->listCond->setVisible(true);
}

WidgetVpzPropertyExpCond::~WidgetVpzPropertyExpCond()
{
    delete ui;
}

void
WidgetVpzPropertyExpCond::setModel(vleVpz *model, const QString& model_query)
{
    mVpm = model;
    QObject::connect(mVpm,   SIGNAL(conditionsUpdated()),
                     this, SLOT(refresh()));

    mModQuery = model_query;
    refresh();
}

void
WidgetVpzPropertyExpCond::refresh()
{
    // First, clear the current list content
    ui->listCond->clear();
    QString dyn = mVpm->modelDynFromDoc(mModQuery);
//    if (dyn != "") {
        //TODO only way to see if it is an atomic model ??
        QDomNodeList conds = mVpm->condsListFromConds(mVpm->condsFromDoc());
        for (unsigned int i=0; i< conds.length(); i++) {
            QDomNode cond = conds.at(i);
            QString condName = mVpm->vdo()->attributeValue(cond, "name");
            QListWidgetItem *wi = new QListWidgetItem(ui->listCond);
            ui->listCond->addItem(wi);
            QCheckBox *cb = new QCheckBox(this);
            cb->setText( condName);
            if (mVpm->isAttachedCond(mModQuery, condName)) {
                cb->setCheckState( Qt::Checked );
            } else {
                cb->setCheckState( Qt::Unchecked );
            }
            QObject::connect(cb,   SIGNAL(toggled(bool)),
                    this, SLOT(onCheckboxToggle(bool)));
            ui->listCond->setItemWidget(wi, cb);
        }
//    }
}

void
WidgetVpzPropertyExpCond::onCheckboxToggle(bool checked)
{
    // Get the QCheckbox that emit signal
    QObject   *sender = QObject::sender();
    QCheckBox *cb     = qobject_cast<QCheckBox *>(sender);

    // Sanity check ... if no model set or checkbox not found
    if ( (mVpm == 0) || (cb == 0) )
        return;

    // Search the vleExpCond associated with the checkbox
    bool oldBlock = mVpm->blockSignals(true);
    if (checked) {
        mVpm->attachCondToAtomicModel(mModQuery, cb->text());
    } else {
        mVpm->detachCondToAtomicModel(mModQuery, cb->text());
    }
    mVpm->blockSignals(oldBlock);

}

/****************************************************************************/

WidgetVpzPropertyObservables::WidgetVpzPropertyObservables(QWidget *parent) :
        QWidget(parent), ui(new Ui::WidgetVpzPropertyObservables),
        mVpm(0), mModQuery("")
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
    ui->listObs->clear();
    QString dyn = mVpm->modelDynFromDoc(mModQuery);
    QDomNodeList obss = mVpm->obssListFromObss(mVpm->obsFromDoc());
    for (unsigned int i=0; i< obss.length(); i++) {
        QDomNode obs = obss.at(i);
        QString obsName = mVpm->vdo()->attributeValue(obs, "name");
        QListWidgetItem *wi = new QListWidgetItem(ui->listObs);
        ui->listObs->addItem(wi);
        QCheckBox *cb = new QCheckBox(this);
        cb->setText(obsName);
        if (mVpm->modelObsFromDoc(mModQuery) == obsName) {
            cb->setCheckState( Qt::Checked );
        } else {
            cb->setCheckState( Qt::Unchecked );
        }
        QObject::connect(cb, SIGNAL(clicked(bool)),
                this, SLOT(onCheckboxClick(bool)));
        ui->listObs->setItemWidget(wi, cb);
    }
}

void
WidgetVpzPropertyObservables::setModel(vleVpz *model,
        const QString& model_query)
{
    mVpm = model;

    QObject::connect(mVpm, SIGNAL(observablesUpdated()),
            this, SLOT(refresh()));

    mModQuery = model_query;
    refresh();
}

void
WidgetVpzPropertyObservables::onCheckboxClick(bool checked)
{
    QObject *sender = QObject::sender();
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender);

    if ( (mVpm == 0) || (cb == 0) )
        return;

    bool oldBlock = mVpm->blockSignals(true);
    if (checked) {
        mVpm->setObsToAtomicModel(mModQuery, cb->text());

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
        mVpm->unsetObsFromAtomicModel(mModQuery);
    }
    mVpm->blockSignals(oldBlock);
}

}}//namepsaces
