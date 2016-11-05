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
#include <QDebug>
#include <QUndoCommand>
#include <QDoubleValidator>
#include <QtGlobal>
#include <vle/utils/DateTime.hpp>
#include <float.h>
#include <iostream>
#include <limits>

#include "filevpzproject.h"
#include "ui_filevpzproject.h"

namespace vle {
namespace gvle {

FileVpzProject::FileVpzProject(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzProject), mTab(0), mId(0), mAuthor(0),
    mDate(0), mVersion(0), mName(0), mDuration(0),  mBegin(0), mVpz(0),
    maxPrecision(std::numeric_limits<double>::digits10)
{
    ui->setupUi(this);

    mAuthor = ui->authorLineEdit;
    mDate  = ui->dateTimeEdit;
    mVersion = ui->versionEdit;
    mName = ui->nameEdit;
    mDuration = ui->durationEdit;

    QDoubleValidator *durationValidator =
        new QDoubleValidator(0., DBL_MAX, maxPrecision, this);
    durationValidator->setNotation(QDoubleValidator::ScientificNotation);
    mDuration->setValidator(durationValidator);

    mBegin = ui->beginEdit;
    mBeginDateTime = ui->beginDateTimeEdit;

    QDoubleValidator *beginValidator =
        new QDoubleValidator(-DBL_MAX, DBL_MAX, maxPrecision, this);
    beginValidator->setNotation(QDoubleValidator::ScientificNotation);
    mBegin->setValidator(beginValidator);

    QObject::connect(mAuthor, SIGNAL(editingFinished()),
                     this, SLOT(setAuthorToVpz()) );
    QObject::connect(mDate, SIGNAL(editingFinished()),
                     this, SLOT(setDateToVpz()) );
    QObject::connect(mVersion, SIGNAL(editingFinished()),
                     this, SLOT(setVersionToVpz()) );
    QObject::connect(mName, SIGNAL(editingFinished()),
                     this, SLOT(setExpNameToVpz()) );
    QObject::connect(mDuration, SIGNAL(editingFinished()),
                     this, SLOT(setExpDurationToVpz()) );
    QObject::connect(mBegin, SIGNAL(editingFinished()),
                     this, SLOT(setExpBeginToVpz()));
    QObject::connect(mBeginDateTime, SIGNAL(editingFinished()),
                     this, SLOT(setExpBeginDateTimeToVpz()));

    mDuration->installEventFilter(this);
    mBegin->installEventFilter(this);
}

FileVpzProject::~FileVpzProject()
{
    delete ui;
}

void
FileVpzProject::setVpz(vleVpz* vpz)
{
    mVpz = vpz;

    QObject::connect(mVpz,
                     SIGNAL(conditionsUpdated()),
                     this,
                     SLOT(reload()));

    reload();
}

void FileVpzProject::reload()
{
    mAuthor->setText(mVpz->getAuthor());
    mDate->setDateTime(QDateTime::fromString(mVpz->getDate(),
                                             "dddd d MMMM yyyy hh:mm"));
    mVersion->setText(mVpz->getVersion());
    mName->setText(mVpz->getExpName());
    mDuration->setText(mVpz->getExpDuration());
    QString beginNumString = mVpz->getExpBegin();
    mBegin->setText(beginNumString);
    QDateTime beginDateTime;
    if (beginNumtoDate(beginNumString, beginDateTime)) {
        mBeginDateTime->setDateTime(beginDateTime);
    }
}

void FileVpzProject::setAuthorToVpz()
{
    if (mAuthor->text() != mVpz->getAuthor()) {
        mVpz->setAuthor(mAuthor->text());
    }
}

void FileVpzProject::setDateToVpz()
{
    if (mDate->text() != mVpz->getDate()) {
        mVpz->setDate(mDate->text());
    }
}
void FileVpzProject::setVersionToVpz()
{
    if (mVersion->text() != mVpz->getVersion()) {
        mVpz->setVersion(mVersion->text());
    }
}

void FileVpzProject::setExpNameToVpz()
{
    if (mName->text() != mVpz->getExpName()) {
        mVpz->setExpName(mName->text());
    }
}

void FileVpzProject::setExpDurationToVpz()
{
    if (mDuration->text() != mVpz->getExpDuration()) {
        mVpz->setExpDuration(mDuration->text());
    }
}

void FileVpzProject::setExpBeginToVpz()
{
    if (mBegin->text() != mVpz->getExpBegin()) {
        QString beginNumString = mBegin->text();
        mVpz->setExpBegin(beginNumString);
        QDateTime beginDateTime;
        if (beginNumtoDate(beginNumString, beginDateTime)) {
            mBeginDateTime->setDateTime(beginDateTime);
        }
    }
}

void FileVpzProject::setExpBeginDateTimeToVpz()
{
    std::string beginDateTimeString =
        mBeginDateTime->text().toStdString();
    try {
        double beginNum = vle::utils::DateTime::toJulianDay(beginDateTimeString);
        mVpz->setExpBegin(QString::number(beginNum, 'g', maxPrecision));
        mBegin->setText(QString::number(beginNum, 'g', maxPrecision));
    } catch (...) {
    }
}

void

FileVpzProject::onUndoRedoVpz(QDomNode /*oldVpz*/, QDomNode /*newVpz*/,
        QDomNode /*oldVpm*/, QDomNode /*newVpm*/)
{
    reload();
}

bool
FileVpzProject::eventFilter(QObject *target, QEvent *event)
{
    if (target == mDuration) {
        switch(event->type()) {
        case QEvent::FocusOut:
            if (mDuration->text().isEmpty()) {
                mDuration->setText(mVpz->getExpDuration());
            }
            break;
        default:
            break;
        }
    } else if (target == mBegin) {
        switch(event->type()) {
        case QEvent::FocusOut:
            if (mBegin->text().isEmpty()) {
                mBegin->setText(mVpz->getExpBegin());
            }
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(target, event);
}

bool
FileVpzProject::beginNumtoDate(const QString& num, QDateTime& date) const
{
    bool ok;
    double beginNum = num.toDouble(&ok);
    if (ok) {
        try {
            QString beginNumQString = QString::fromStdString(
                vle::utils::DateTime::toJulianDay(beginNum));
            QString restoreLocale = QLocale::system().name();
            QLocale::setDefault(QLocale::C);
            date = QLocale().toDateTime(beginNumQString,
                                        "yyyy-MMM-dd hh:mm:ss");
            QLocale::setDefault(restoreLocale);
        } catch (...) {
            return false;
        }
        return true;
    }
    return false;
}

}} //namespaces
