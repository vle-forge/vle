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

#include "vpzActions.h"

namespace Ui {

ChangeAuthor::ChangeAuthor(QString newAuthor, vleVpz  *vpz,
                           QLineEdit *line, QTabWidget *tab, int id,
                           QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mAuthor = newAuthor;
    mPreviousAuthor = mVpz->getAuthor();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Author %1 to %2")
            .arg(mPreviousAuthor).arg(mAuthor));

}

void ChangeAuthor::undo()
{
    mVpz->setAuthor(mPreviousAuthor);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousAuthor);
}

void ChangeAuthor::redo()
{
    mVpz->setAuthor(mAuthor);
    mTab->setCurrentIndex(mId);
    mLine->setText(mAuthor);
}

ChangeDate::ChangeDate(QString newDate, vleVpz  *vpz,
                       QDateTimeEdit *line, QTabWidget *tab, int id,
                       QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mDate = newDate;
    mPreviousDate = mVpz->getDate();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Date %1 to %2")
            .arg(mPreviousDate).arg(mDate));
}

void ChangeDate::undo()
{
    mVpz->setDate(mPreviousDate);
    mTab->setCurrentIndex(mId);
    mLine->setDateTime(QDateTime::fromString(mPreviousDate, "dddd d MMMM yyyy hh:mm"));
}

void ChangeDate::redo()
{
    mVpz->setDate(mDate);
    mTab->setCurrentIndex(mId);
    mLine->setDateTime(QDateTime::fromString(mDate, "dddd MMMM d yyyy hh:mm"));
}

ChangeVersion::ChangeVersion(QString newVersion, vleVpz  *vpz,
                             QLineEdit *line, QTabWidget *tab, int id,
                             QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mVersion = newVersion;
    mPreviousVersion = mVpz->getVersion();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Version %1 to %2")
            .arg(mPreviousVersion).arg(mVersion));

}

void ChangeVersion::undo()
{
    mVpz->setVersion(mPreviousVersion);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousVersion);
}

void ChangeVersion::redo()
{
    mVpz->setVersion(mVersion);
    mTab->setCurrentIndex(mId);
    mLine->setText(mVersion);
}

ChangeExpName::ChangeExpName(QString newExpName, vleVpz  *vpz,
                             QLineEdit *line, QTabWidget *tab, int id,
                             QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mName = newExpName;
    mPreviousName = mVpz->getExpName();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Name %1 to %2")
            .arg(mPreviousName).arg(mName));

}

void ChangeExpName::undo()
{
    mVpz->setExpName(mPreviousName);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousName);
}

void ChangeExpName::redo()
{
    mVpz->setExpName(mName);
    mTab->setCurrentIndex(mId);
    mLine->setText(mName);
}

ChangeExpDuration::ChangeExpDuration(QString newExpDuration, vleVpz  *vpz,
                                     QLineEdit *line, QTabWidget *tab, int id,
                                     QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mDuration = newExpDuration;
    mPreviousDuration = mVpz->getExpDuration();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Duration %1 to %2")
            .arg(mPreviousDuration).arg(mDuration));

}

void ChangeExpDuration::undo()
{
    mVpz->setExpDuration(mPreviousDuration);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousDuration);
}

void ChangeExpDuration::redo()
{
    mVpz->setExpDuration(mDuration);
    mTab->setCurrentIndex(mId);
    mLine->setText(mDuration);
}

ChangeExpBegin::ChangeExpBegin(QString newExpBegin, vleVpz  *vpz,
                               QLineEdit *line, QTabWidget *tab, int id,
                               QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpz = vpz;
    mBegin = newExpBegin;
    mPreviousBegin = mVpz->getExpBegin();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Begin %1 to %2")
            .arg(mPreviousBegin).arg(mBegin));

}

void ChangeExpBegin::undo()
{
    mVpz->setExpBegin(mPreviousBegin);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousBegin);
}

void ChangeExpBegin::redo()
{
    mVpz->setExpBegin(mBegin);
    mTab->setCurrentIndex(mId);
    mLine->setText(mBegin);
}
}
