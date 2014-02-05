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
#include <QCheckBox>

/**
 * @brief WidgetVpzProperty::WidgetVpzProperty
 *        Default constructor
 *
 */
WidgetVpzProperty::WidgetVpzProperty(QWidget *parent) :
    QWidget(parent)
{
    isMaximized = false;
}

/**
 * @brief WidgetVpzProperty::~WidgetVpzProperty
 *        Default destructor
 *
 */
WidgetVpzProperty::~WidgetVpzProperty()
{
    // Nothing to free/destroy
}

/**
 * @brief WidgetVpzProperty::setMaximized
 *        Mutator for the Maximized flag
 *
 */
void WidgetVpzProperty::setMaximized(bool isMax)
{
    isMaximized = isMax;
}

/**
 * @brief WidgetVpzProperty::setModel
 *        Mutator for the current model reference
 *
 */
void WidgetVpzProperty::setModel(vleVpzModel *model)
{
    mModel = model;
}


/* ---------------------------------------------------------------------- */
/* --                                                                  -- */
/* --                             Dynamics                             -- */
/* --                                                                  -- */
/* ---------------------------------------------------------------------- */

/**
 * @brief WidgetVpzPropertyDynamics::WidgetVpzPropertyDynamics
 *        Default constructor
 *
 */
WidgetVpzPropertyDynamics::WidgetVpzPropertyDynamics(QWidget *parent) :
    WidgetVpzProperty(parent),
    ui(new Ui::WidgetVpzPropertyDynamics)
{
    ui->setupUi(this);
    mMode = ModeSimple;

    setFocusPolicy(Qt::StrongFocus);

    // Hide the data widgets by default
    ui->labelPackage->setVisible(false);
    ui->labelLibrary->setVisible(false);
    ui->comboBox->setVisible(false);

    // Signal used in edit mode when combo-box value change
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                     this,         SLOT  (onChange(int)));
}

/**
 * @brief WidgetVpzPropertyDynamics::~WidgetVpzPropertyDynamics
 *        Default destructor
 *
 */
WidgetVpzPropertyDynamics::~WidgetVpzPropertyDynamics()
{
    delete ui;
}

/**
 * @brief WidgetVpzPropertyDynamics::setModel
 *        Called when a new model is selected
 */
void WidgetVpzPropertyDynamics::setModel(vleVpzModel *model)
{
    WidgetVpzProperty::setModel(model);

    // Get the Dynamic of the model
    vleVpzDynamic *dyn = model->getDynamic();

    if (dyn)
    {
        ui->labelName->setText   ( dyn->getName()    );
        ui->labelLibrary->setText( dyn->getLibrary() );
        ui->labelPackage->setText( dyn->getPackage() );
    }
    // If no Dynamic set ... display "none"
    else
    {
        ui->labelName->setText   ( tr("<none>")      );
        ui->labelLibrary->setText( tr("<noPackage>") );
        ui->labelPackage->setText( tr("<noLibrary>") );
    }
}

/**
 * @brief WidgetVpzPropertyDynamics::setModel
 *        Override the Maximize method to show/hide extended informations
 *
 */
void WidgetVpzPropertyDynamics::setMaximized(bool isMax)
{
    WidgetVpzProperty::setMaximized(isMax);

    if (isMax)
    {
        ui->labelLibrary->setVisible(true);
        ui->labelPackage->setVisible(true);
        mMode = ModeExtend;
    }
    else
    {
        ui->labelLibrary->setVisible(false);
        ui->labelPackage->setVisible(false);
        ui->comboBox->setVisible(false);
        mMode = ModeSimple;
    }
}

/**
 * @brief WidgetVpzPropertyDynamics::mouseDoubleClickEvent (extend QWidget)
 *        Called when property is double-clicked - Switch to edit mode
 *
 */
void WidgetVpzPropertyDynamics::mouseDoubleClickEvent (QMouseEvent *event)
{
    int index = 0;
    int count = 1;

    (void)event;

    // Set extended display (not already called if select line by double-click)
    setMaximized(true);

    // Clear the current list
    while(ui->comboBox->count())
        ui->comboBox->removeItem(0);

    // Add the first (default) item
    ui->comboBox->addItem( tr("<none>") );
    ui->comboBox->setCurrentIndex(0);

    if (mModel != 0)
    {
        vleVpz *vpz = mModel->getVpz();

        QListIterator<vleVpzDynamic*> dynItems(*vpz->getDynamicsList());
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

    // Now, show the combo-box
    ui->comboBox->setVisible(true);

    mMode = ModeEdit;

    emit sigActivated(true);
}

/**
 * @brief WidgetVpzPropertyDynamics::onChange (slot)
 *        Called when a new value is selected from combobox
 *
 */
void WidgetVpzPropertyDynamics::onChange(int index)
{
    // This signal is used only in Edit mode
    if (mMode != ModeEdit)
        return;

    // Get the selected dynamic name from widget
    QString selectedName = ui->comboBox->itemText(index);

    // If the selected Dynamic exists
    if (mModel->getVpz()->getDynamic(selectedName))
        // Update the model with the selected Dynamic
        mModel->setDynamic(selectedName);
    else
        // Remove current dynamic
        mModel->removeDynamic();

    // Call setModel to refresh display
    setModel(mModel);

    // Hide the combo-box to finish selection
    ui->comboBox->setVisible(false);
    mMode = ModeExtend;
}

/* ----------------------------------------------------------------------- */
/* --                                                                   -- */
/* --                      Experimental Conditions                      -- */
/* --                                                                   -- */
/* ----------------------------------------------------------------------- */

/**
 * @brief WidgetVpzPropertyExpCond::WidgetVpzPropertyExpCond
 *        Default constructor
 *
 */
WidgetVpzPropertyExpCond::WidgetVpzPropertyExpCond(QWidget *parent) :
    WidgetVpzProperty(parent),
    ui(new Ui::WidgetVpzPropertyExpCond)
{
    ui->setupUi(this);
    ui->listCond->setVisible(false);
}
/**
 * @brief WidgetVpzPropertyExpCond::~WidgetVpzPropertyExpCond
 *        Default destructor
 *
 */
WidgetVpzPropertyExpCond::~WidgetVpzPropertyExpCond()
{
    delete ui;
}

/**
 * @brief WidgetVpzPropertyExpCond::setModel
 *        Called when a new model is selected
 */
void WidgetVpzPropertyExpCond::setModel(vleVpzModel *model)
{
    int condCount = 0;

    WidgetVpzProperty::setModel(model);

    vleVpz *vpz = model->getVpz();

    vpzExpCond *econd;
    for (econd = vpz->getFirstCondition(); econd != 0; econd = vpz->getNextCondition() )
    {
        if (model->hasCondition(econd))
            condCount++;
    }
    ui->labelCount->setText(QString("%1").arg(condCount));
}

/**
 * @brief WidgetVpzPropertyExpCond::setMaximized
 *        Called when the property line is selected
 */
void WidgetVpzPropertyExpCond::setMaximized(bool isMax)
{
    if (mModel == 0)
        return;

    ui->listCond->setVisible(false);

    if (isMax == false)
        return;

    // First, clear the current list content
    ui->listCond->clear();

    vleVpz *vpz = mModel->getVpz();
    vpzExpCond *econd;
    for (econd = vpz->getFirstCondition(); econd != 0; econd = vpz->getNextCondition() )
    {
        if ( ! mModel->hasCondition(econd))
            continue;

        QListWidgetItem *wi = new QListWidgetItem(ui->listCond);
        ui->listCond->addItem(wi);

        QLabel *label = new QLabel(this);
        label->setText( econd->getName() );
        ui->listCond->setItemWidget(wi, label);
    }

    ui->listCond->setVisible(true);
}

/**
 * @brief WidgetVpzPropertyExpCond::mouseDoubleClickEvent (extend QWidget)
 *        Called when the widget is double-clicked (enter edit mode)
 */
void WidgetVpzPropertyExpCond::mouseDoubleClickEvent (QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);

    // First, clear the current list content
    ui->listCond->clear();

    vleVpz *vpz = mModel->getVpz();
    vpzExpCond *econd;
    for (econd = vpz->getFirstCondition(); econd != 0; econd = vpz->getNextCondition() )
    {
        QListWidgetItem *wi = new QListWidgetItem(ui->listCond);
        ui->listCond->addItem(wi);

        QCheckBox *cb = new QCheckBox(this);
        cb->setText( econd->getName() );
        if ( mModel->hasCondition(econd) )
            cb->setCheckState( Qt::Checked );
        else
            cb->setCheckState( Qt::Unchecked );
        QObject::connect(cb,   SIGNAL(toggled(bool)),
                         this, SLOT(onCheckboxToggle(bool)));
        ui->listCond->setItemWidget(wi, cb);
    }

}

/**
 * @brief WidgetVpzPropertyExpCond::onCheckboxToggle (slot)
 *        Called when one the the checkbox is checked/unchecked
 */
void WidgetVpzPropertyExpCond::onCheckboxToggle(bool checked)
{
    // Get the QCheckbox that emit signal
    QObject   *sender = QObject::sender();
    QCheckBox *cb     = qobject_cast<QCheckBox *>(sender);

    // Sanity check ... if no model set or checkbox not found
    if ( (mModel == 0) || (cb == 0) )
        return;

    // Search the vleExpCond associated with the checkbox
    vleVpz     *vpz   = mModel->getVpz();
    vpzExpCond *econd = vpz->getCondition( cb->text() );
    if (econd == 0)
        return;

    // Add or remove the Experimental Condition according to checkbox state
    if (checked)
        mModel->addCondition(econd);
    else
        mModel->removeCondition(econd);
}
