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

#ifndef VPZACTIONS_H
#define VPZACTIONS_H

#include <QUndoCommand>
#include "vlevpz.h"
#include "filevpzproject.h"

namespace Ui {

class ChangeAuthor : public QUndoCommand
{
public:
    ChangeAuthor(QString newAuthor, vleVpz  *vpz,
		 QLineEdit *line, QTabWidget *tab, int id,
		 QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mAuthor;
    QString mPreviousAuthor;
    QLineEdit *mLine;
    QTabWidget *mTab;
    int mId;
};

class ChangeDate : public QUndoCommand
{
public:
    ChangeDate(QString newDate, vleVpz  *vpz,
	       QDateTimeEdit *line, QTabWidget *tab, int id,
	       QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mDate;
    QString mPreviousDate;
    QDateTimeEdit *mLine;
    QTabWidget *mTab;
    int mId;
};

class ChangeVersion : public QUndoCommand
{
public:
    ChangeVersion(QString newVersion, vleVpz  *vpz,
		  QLineEdit *line, QTabWidget *tab, int id,
		  QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mVersion;
    QString mPreviousVersion;
    QLineEdit *mLine;
    QTabWidget *mTab;
    int mId;
};

class ChangeExpName : public QUndoCommand
{
public:
    ChangeExpName(QString newName, vleVpz  *vpz,
		  QLineEdit *line, QTabWidget *tab, int id,
		  QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mName;
    QString mPreviousName;
    QLineEdit *mLine;
    QTabWidget *mTab;
    int mId;
};

class ChangeExpDuration : public QUndoCommand
{
public:
    ChangeExpDuration(QString newDuration, vleVpz  *vpz,
		      QLineEdit *line, QTabWidget *tab, int id,
		      QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mDuration;
    QString mPreviousDuration;
    QLineEdit *mLine;
    QTabWidget *mTab;
    int mId;
};

class ChangeExpBegin : public QUndoCommand
{
public:
    ChangeExpBegin(QString newBegin, vleVpz  *vpz,
		   QLineEdit *line, QTabWidget *tab, int id,
		   QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    vleVpz  *mVpz;
    QString mBegin;
    QString mPreviousBegin;
    QLineEdit *mLine;
    QTabWidget *mTab;
    int mId;
};
}
#endif // VPZACTIONS_H
