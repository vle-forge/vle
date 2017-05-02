/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QObject>
#include <QtPlugin>
#include <QtWidgets/QVBoxLayout>

#include <iostream>
#include <vle/gvle/gvle_widgets.h>
#include <vle/utils/DateTime.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/vpz/AtomicModel.hpp>

#include "DateCondition.h"

namespace vle {
namespace gvle {

/**
 * @brief DateCondition::DateCondition
 *        Default constructor
 */
DateCondition::DateCondition()
  : mSettings(0)
  , mLogger(0)
  , mExpCond()
  , mVpz(0)
  , mWidget(0)
  , mNamePortField(0)
  , mComboBox(0)
  , mcalendar(0)

{
    mWidget = new QWidget();

    mNamePortField = new VleLineEdit(0, "begin_date");

    mComboBox = new QComboBox();
    mComboBox->addItem("real");
    mComboBox->addItem("character");
    mcalendar = new QCalendarWidget();
    QObject::connect(
      mNamePortField,
      SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
      this,
      SLOT(onTextUpdated(const QString&, const QString&, const QString&)));
    QObject::connect(mComboBox,
                     SIGNAL(currentTextChanged(const QString&)),
                     this,
                     SLOT(onTypeChanged(const QString&)));
    QObject::connect(mcalendar,
                     SIGNAL(destroyed(QObject*)),
                     this,
                     SLOT(calendarDestroyed(QObject*)));
    QObject::connect(
      mcalendar, SIGNAL(clicked(QDate)), this, SLOT(dateSelected(QDate)));

    QVBoxLayout* layout = new QVBoxLayout(mWidget);
    layout->addWidget(mNamePortField);
    layout->addWidget(mComboBox);
    layout->addWidget(mcalendar);
}

DateCondition::~DateCondition()
{
    mcalendar->deleteLater();
}

QString
DateCondition::getname()
{
    QString name = "date";
    return name;
}

void
DateCondition::setSettings(QSettings* s)
{
    mSettings = s;
}

void
DateCondition::setLogger(Logger* logger)
{
    mLogger = logger;
}

QWidget*
DateCondition::getWidget()
{
    return mWidget;
}

void
DateCondition::init(vleVpz* vpz, const QString& cond)
{
    bool oldBlock = mNamePortField->blockSignals(true);
    bool oldBlock1 = mcalendar->blockSignals(true);
    bool oldBlock2 = mComboBox->blockSignals(true);

    // Save it
    mVpz = vpz;
    mExpCond = cond;

    QString datePort = "";

    // Search the "date" port of the experimental condition
    QDomNode condXml = mVpz->condFromConds(mVpz->condsFromDoc(), cond);
    QDomNodeList portList = mVpz->portsListFromDoc(mExpCond);
    if (portList.size() == 1) {
        datePort = DomFunctions::attributeValue(portList.at(0), "name");
        std::vector<std::unique_ptr<value::Value>> values;
        mVpz->fillWithMultipleValue(portList.at(0), values);
        if (values.size() == 1) {
            if (values[0]->isString()) {
                mComboBox->setCurrentText("character");
            } else {
                mComboBox->setCurrentText("real");
            }
        }
    } else {
        datePort = "begin_date";
        value::Map portValues;
        portValues.addString(datePort.toStdString(), "2016-09-01");
        vleDomStatic::fillConditionWithMap(
          mVpz->getDomDoc(), condXml, portValues);
        mComboBox->setCurrentText("character");
    }
    std::unique_ptr<value::Value> dateV =
      mVpz->buildValueFromDoc(cond, datePort, 0);
    double dateI = 0;
    if (dateV->isString()) {
        dateI = utils::DateTime::toJulianDay(dateV->toString().value());
    } else if (dateV->isDouble()) {
        dateI = dateV->toDouble().value();
    }
    int dayI = utils::DateTime::dayOfMonth(dateI);
    int monthI = utils::DateTime::month(dateI);
    int yearI = utils::DateTime::year(dateI);

    QDate selDate;
    selDate.setDate(yearI, monthI, dayI);

    mNamePortField->setValue(datePort);
    mNamePortField->backup = datePort;

    if (dateV->isString()) {
        mComboBox->setCurrentText("character");
    } else if (dateV->isDouble()) {
        mComboBox->setCurrentText("real");
    }
    mcalendar->setSelectedDate(selDate);
    mcalendar->setCurrentPage(monthI, yearI);
    mcalendar->showSelectedDate();

    mNamePortField->blockSignals(oldBlock);
    mcalendar->blockSignals(oldBlock1);
    mComboBox->blockSignals(oldBlock2);
}

void
DateCondition::dateSelected(QDate date)
{
    value::Map portValues;
    if (mComboBox->currentText() == "real") {
        portValues.addDouble(mNamePortField->text().toStdString(),
                             utils::DateTime::toJulianDay(
                               date.toString("yyyy-MM-dd").toStdString()));
    } else {
        portValues.addString(mNamePortField->text().toStdString(),
                             date.toString("yyyy-MM-dd").toStdString());
    }
    mVpz->fillConditionWithMapToDoc(mExpCond, portValues, true);
    mcalendar->setCurrentPage(date.month(), date.year());
    mcalendar->showSelectedDate();
}

void
DateCondition::calendarDestroyed(QObject* obj)
{
    (void)obj;
}

void
DateCondition::onTextUpdated(const QString& /*id*/,
                             const QString& old,
                             const QString& neW)
{
    value::Map portValues;

    QDomNode portNode = mVpz->portFromDoc(mExpCond, old);
    std::vector<std::unique_ptr<value::Value>> values;
    mVpz->fillWithMultipleValue(portNode, values);
    if (values.size() > 0) {
        portValues.set(neW.toStdString(), std::move(values[0]));
        mVpz->fillConditionWithMapToDoc(mExpCond, portValues, true);
    }
}

void
DateCondition::onTypeChanged(const QString& type)
{

    QDomNode portNode = mVpz->portFromDoc(mExpCond, mNamePortField->text());
    std::vector<std::unique_ptr<value::Value>> values;
    mVpz->fillWithMultipleValue(portNode, values);
    value::Map portValues;
    if (type == "real") {
        portValues.addDouble(
          mNamePortField->text().toStdString(),
          utils::DateTime::toJulianDay(values[0]->toString().value()));
    } else if (type == "character") {
        portValues.addString(
          mNamePortField->text().toStdString(),
          utils::DateTime::toJulianDay(values[0]->toDouble().value()));
    }
    mVpz->fillConditionWithMapToDoc(mExpCond, portValues, true);
}
}
} // namespaces
