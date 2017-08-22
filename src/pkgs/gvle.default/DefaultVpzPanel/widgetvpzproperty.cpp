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
#include <QCheckBox>
#include <QDebug>
#include <QRadioButton>
#include <QVBoxLayout>

namespace vle {
namespace gvle {

WidgetVpzPropertyDynamics::WidgetVpzPropertyDynamics(QWidget* parent)
  : QComboBox(parent)
  , mVpz(0)
  , mModQuery("")
{
    QComboBox::setFocusPolicy(Qt::ClickFocus);

    QObject::connect(
      this, SIGNAL(currentIndexChanged(int)), this, SLOT(onChange(int)));
}

WidgetVpzPropertyDynamics::~WidgetVpzPropertyDynamics()
{
}

void
WidgetVpzPropertyDynamics::selectModel(vleVpz* model, const QString& mod_query)
{
    int index = 0;
    int count = 1;
    mVpz = model;
    mModQuery = mod_query;

    if (not mVpz)
        return;

    // Get the Dynamic of the model
    QString dyn = mVpz->modelDynFromDoc(mModQuery);
    bool oldBlockComboBox = QComboBox::blockSignals(true);
    while (QComboBox::count()) {
        QComboBox::removeItem(0);
    }
    QComboBox::addItem(tr("<none>"));

    QList<QString> dynList;
    mVpz->fillWithDynamicsList(dynList);
    for (int i = 0; i < dynList.length(); i++) {
        QComboBox::addItem(dynList.at(i));
        if (dyn == dynList.at(i)) {
            index = count;
        }
        count++;
    }
    QComboBox::setCurrentIndex(index);
    QComboBox::blockSignals(oldBlockComboBox);
}

void
WidgetVpzPropertyDynamics::onChange(int index)
{
    // Get the selected dynamic name from widget
    QString selectedName = QComboBox::itemText(index);

    // If the selected Dynamic exists
    if (mVpz->existDynamicIntoDoc(selectedName)) {
        // Update the model with the selected Dynamic
        mVpz->setDynToAtomicModel(mModQuery, selectedName);
    } else if (selectedName == "<none>") {
        mVpz->setDynToAtomicModel(mModQuery, "");
    }
}

/****************************************************************************/

WidgetVpzPropertyExpCond::WidgetVpzPropertyExpCond(QWidget* parent)
  : QListWidget(parent)
  , mVpz(0)
  , mModQuery("")
{
}

WidgetVpzPropertyExpCond::~WidgetVpzPropertyExpCond()
{
}

void
WidgetVpzPropertyExpCond::selectModel(vleVpz* model, const QString& model_query)
{
    mVpz = model;
    QObject::connect(mVpz, SIGNAL(conditionsUpdated()), this, SLOT(refresh()));

    mModQuery = model_query;
    refresh();
}

void
WidgetVpzPropertyExpCond::refresh()
{
    // First, clear the current list content
    bool oldBlock = QListWidget::blockSignals(true);
    QListWidget::clear();
    QString dyn = mVpz->modelDynFromDoc(mModQuery);
    // TODO only way to see if it is an atomic model ??
    QDomNodeList conds = mVpz->condsListFromConds(mVpz->condsFromDoc());
    for (int i = 0; i < conds.length(); i++) {
        QDomNode cond = conds.at(i);
        QString condName = DomFunctions::attributeValue(cond, "name");
        QListWidgetItem* wi = new QListWidgetItem(this);
        QListWidget::addItem(wi);
        QCheckBox* cb = new QCheckBox(this);
        cb->setText(condName);
        if (mVpz->isAttachedCond(mModQuery, condName)) {
            cb->setCheckState(Qt::Checked);
        } else {
            cb->setCheckState(Qt::Unchecked);
        }
        QObject::connect(
          cb, SIGNAL(toggled(bool)), this, SLOT(onCheckboxToggle(bool)));
        QListWidget::setItemWidget(wi, cb);
    }
    QListWidget::blockSignals(oldBlock);
}

void
WidgetVpzPropertyExpCond::onCheckboxToggle(bool checked)
{
    // Get the QCheckbox that emit signal
    QObject* sender = QObject::sender();
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender);

    // Sanity check ... if no model set or checkbox not found
    if ((mVpz == 0) || (cb == 0))
        return;

    // Search the vleExpCond associated with the checkbox
    // bool oldBlock = mVpz->blockSignals(true);
    if (checked) {
        mVpz->attachCondToAtomicModel(mModQuery, cb->text());
    } else {
        mVpz->detachCondToAtomicModel(mModQuery, cb->text());
    }
    // mVpz->blockSignals(oldBlock);
}

/****************************************************************************/

WidgetVpzPropertyObservables::WidgetVpzPropertyObservables(QWidget* parent)
  : QComboBox(parent)
  , mVpz(0)
  , mModQuery("")
{
    QComboBox::setFocusPolicy(Qt::ClickFocus);

    QObject::connect(
      this, SIGNAL(currentIndexChanged(int)), this, SLOT(onChange(int)));
}

WidgetVpzPropertyObservables::~WidgetVpzPropertyObservables()
{
}

void
WidgetVpzPropertyObservables::refresh()
{

    bool oldBlock = QComboBox::blockSignals(true);
    QComboBox::clear();
    QString dyn = mVpz->modelDynFromDoc(mModQuery);
    QDomNodeList obss = mVpz->obssListFromObss(mVpz->obsFromDoc());
    QComboBox::addItem("<none>");
    int selectIndex = 0;
    for (int i = 0; i < obss.length(); i++) {
        QDomNode obs = obss.at(i);
        QString obsName = DomFunctions::attributeValue(obs, "name");
        QComboBox::addItem(obsName);
        if (mVpz->modelObsFromDoc(mModQuery) == obsName) {
            selectIndex = i + 1;
        }
    }
    QComboBox::setCurrentIndex(selectIndex);
    QComboBox::blockSignals(oldBlock);
}

void
WidgetVpzPropertyObservables::selectModel(vleVpz* model,
                                       const QString& model_query)
{
    mVpz = model;

    QObject::connect(
      mVpz, SIGNAL(observablesUpdated()), this, SLOT(refresh()));

    mModQuery = model_query;
    refresh();
}

void
WidgetVpzPropertyObservables::onChange(int index)
{

    // Get the selected dynamic name from widget
    QString selectedObs = QComboBox::itemText(index);
    if (mVpz->existObsFromDoc(selectedObs)) {
        mVpz->setObsToAtomicModel(mModQuery, selectedObs);
    } else if (selectedObs == "<none>") {
        mVpz->unsetObsFromAtomicModel(mModQuery);
    }
}
}
} // namepsaces
