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
#include <QLineEdit>

namespace Ui {

using namespace vle::gvle2;

ChangeAuthor::ChangeAuthor(QString newAuthor, vleVpz  *vpz,
                           QLineEdit *line, QTabWidget *tab, int id,
                           QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mAuthor = newAuthor;
    mPreviousAuthor = mVpm->getAuthor();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Author %1 to %2")
            .arg(mPreviousAuthor).arg(mAuthor));

}

void ChangeAuthor::undo()
{
    mVpm->setAuthor(mPreviousAuthor);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousAuthor);
}

void ChangeAuthor::redo()
{
    mVpm->setAuthor(mAuthor);
    mTab->setCurrentIndex(mId);
    mLine->setText(mAuthor);
}

ChangeDate::ChangeDate(QString newDate, vleVpz  *vpz,
                       QDateTimeEdit *line, QTabWidget *tab, int id,
                       QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mDate = newDate;
    mPreviousDate = mVpm->getDate();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Date %1 to %2")
            .arg(mPreviousDate).arg(mDate));
}

void ChangeDate::undo()
{
    mVpm->setDate(mPreviousDate);
    mTab->setCurrentIndex(mId);
    mLine->setDateTime(QDateTime::fromString(mPreviousDate, "dddd d MMMM yyyy hh:mm"));
}

void ChangeDate::redo()
{
    mVpm->setDate(mDate);
    mTab->setCurrentIndex(mId);
    mLine->setDateTime(QDateTime::fromString(mDate, "dddd MMMM d yyyy hh:mm"));
}

ChangeVersion::ChangeVersion(QString newVersion, vleVpz  *vpz,
                             QLineEdit *line, QTabWidget *tab, int id,
                             QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mVersion = newVersion;
    mPreviousVersion = mVpm->getVersion();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Version %1 to %2")
            .arg(mPreviousVersion).arg(mVersion));

}

void ChangeVersion::undo()
{
    mVpm->setVersion(mPreviousVersion);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousVersion);
}

void ChangeVersion::redo()
{
    mVpm->setVersion(mVersion);
    mTab->setCurrentIndex(mId);
    mLine->setText(mVersion);
}

ChangeExpName::ChangeExpName(QString newExpName, vleVpz  *vpz,
                             QLineEdit *line, QTabWidget *tab, int id,
                             QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mName = newExpName;
    mPreviousName = mVpm->getExpName();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Name %1 to %2")
            .arg(mPreviousName).arg(mName));

}

void ChangeExpName::undo()
{
    mVpm->setExpName(mPreviousName);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousName);
}

void ChangeExpName::redo()
{
    mVpm->setExpName(mName);
    mTab->setCurrentIndex(mId);
    mLine->setText(mName);
}

ChangeExpDuration::ChangeExpDuration(QString newExpDuration, vleVpz  *vpz,
                                     QLineEdit *line, QTabWidget *tab, int id,
                                     QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mDuration = newExpDuration;
    mPreviousDuration = mVpm->getExpDuration();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Duration %1 to %2")
            .arg(mPreviousDuration).arg(mDuration));

}

void ChangeExpDuration::undo()
{
    mVpm->setExpDuration(mPreviousDuration);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousDuration);
}

void ChangeExpDuration::redo()
{
    mVpm->setExpDuration(mDuration);
    mTab->setCurrentIndex(mId);
    mLine->setText(mDuration);
}

ChangeExpBegin::ChangeExpBegin(QString newExpBegin, vleVpz  *vpz,
                               QLineEdit *line, QTabWidget *tab, int id,
                               QUndoCommand *parent) :
    QUndoCommand(parent)
{
    mVpm = vpz;
    mBegin = newExpBegin;
    mPreviousBegin = mVpm->getExpBegin();
    mLine = line;
    mTab = tab;
    mId = id;

    setText(QObject::tr("Change Experiment Begin %1 to %2")
            .arg(mPreviousBegin).arg(mBegin));

}

void ChangeExpBegin::undo()
{
    mVpm->setExpBegin(mPreviousBegin);
    mTab->setCurrentIndex(mId);
    mLine->setText(mPreviousBegin);
}

void ChangeExpBegin::redo()
{
    mVpm->setExpBegin(mBegin);
    mTab->setCurrentIndex(mId);
    mLine->setText(mBegin);
}
}
