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
#include "DateCondition.h"
#include <iostream>
#include <vle/utils/DateTime.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Integer.hpp>

namespace vle{
namespace gvle{

/**
 * @brief DateCondition::DateCondition
 *        Default constructor
 */
DateCondition::DateCondition() :
  mSettings(0), mLogger(0),
  mExpCond(), mVpz(0), mcalendar(0)

{
    mcalendar = new QCalendarWidget();
    QObject::connect(mcalendar, SIGNAL(destroyed(QObject*)),
                         this,         SLOT(calendarDestroyed(QObject*)) );
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
DateCondition::setSettings(QSettings *s)
{
    mSettings = s;
}

void
DateCondition::setLogger(Logger *logger)
{
    mLogger = logger;
}

QWidget*
DateCondition::getWidget()
{

    return mcalendar;
}

void
DateCondition::init(vleVpz* vpz, const QString& cond)
{
    // Save it
    mVpz = vpz;
    mExpCond = cond;

    QDomNode datePort;

    // Search the "date" port of the experimental condition

    QDomNode condXml = mVpz->condFromConds(mVpz->condsFromDoc(), cond);

    if (not mVpz->existPortFromCond(condXml, "date")) {
        value::Map portValues;
        portValues.addInt("date",utils::DateTime::toJulianDayNumber(
                "2016-09-01"));
        vleVpz::fillConditionWithMap(mVpz->getDomDoc(),condXml,portValues);
    }

    std::unique_ptr<value::Value> dateV = std::move(
            mVpz->buildValueFromDoc(cond, "date",0));
    double dateI = dateV->toInteger().value();
    int dayI = utils::DateTime::dayOfMonth(dateI);
    int monthI = utils::DateTime::month(dateI);
    int yearI = utils::DateTime::year(dateI);

    QDate selDate;
    selDate.setDate(yearI, monthI, dayI);

    QObject::disconnect(mcalendar, SIGNAL(clicked(QDate)),
                     this,         SLOT(dateSelected(QDate)) );
    mcalendar->setSelectedDate(selDate);
    mcalendar->setCurrentPage(monthI, yearI);
    mcalendar->showSelectedDate();
    QObject::connect(mcalendar, SIGNAL(clicked(QDate)),
                     this,         SLOT(dateSelected(QDate)) );
}

void
DateCondition::calendarDestroyed(QObject *obj)
{
    (void) obj;
}

void
DateCondition::dateSelected(QDate date)
{
    value::Map portValues;
    portValues.addInt("date",utils::DateTime::toJulianDayNumber(
            date.toString("yyyy-MM-dd").toStdString()));
    mVpz->fillConditionWithMapToDoc(mExpCond,portValues);
    mcalendar->setCurrentPage(date.month(), date.year());
    mcalendar->showSelectedDate();
}
}}//namespaces
