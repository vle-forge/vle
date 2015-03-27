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
#include <float.h>

#include "filevpzproject.h"
#include "ui_filevpzproject.h"

FileVpzProject::FileVpzProject(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileVpzProject)
{
    mVpz = 0;

    ui->setupUi(this);

    mAuthor = ui->authorLineEdit;
    mDate  = ui->dateTimeEdit;
    mVersion = ui->versionEdit;
    mName = ui->nameEdit;
    mDuration = ui->durationEdit;

    QDoubleValidator *durationValidator = new QDoubleValidator(0., DBL_MAX, 3, this);
    durationValidator->setNotation(QDoubleValidator::ScientificNotation);
    mDuration->setValidator(durationValidator);

    mBegin = ui->beginEdit;

    QDoubleValidator *beginValidator = new QDoubleValidator(DBL_MIN, DBL_MAX, 3, this);
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
                     this, SLOT(setExpBeginToVpz()) );

}

FileVpzProject::~FileVpzProject()
{
    delete ui;
}

void FileVpzProject::setUndo(QUndoStack *undo)
{
    mUndoStack = undo;
    QAction *undoAction = mUndoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    QAction *redoAction = mUndoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    addAction(undoAction);
    addAction(redoAction);
}

void FileVpzProject::reload()
{
    mAuthor->setText(mVpz->getAuthor());
    mDate->setDateTime(QDateTime::fromString(mVpz->getDate(), "dddd d MMMM yyyy hh:mm"));
    mVersion->setText(mVpz->getVersion());
    mName->setText(mVpz->getExpName());
    mDuration->setText(mVpz->getExpDuration());
    mBegin->setText(mVpz->getExpBegin());
}

void FileVpzProject::setAuthorToVpz()
{
    if (mAuthor->text() != mVpz->getAuthor()) {
        QUndoCommand *changeAuthor = new Ui::ChangeAuthor(mAuthor->text(), mVpz,
                                                          mAuthor, mTab, mId);
        mUndoStack->push(changeAuthor);
    }
}

void FileVpzProject::setDateToVpz()
{
    if (mDate->text() != mVpz->getDate()) {
        QUndoCommand *changeDate = new Ui::ChangeDate(
            mDate->dateTime().toString("dddd d MMMM yyyy hh:mm"), mVpz,
            mDate, mTab, mId);
        mUndoStack->push(changeDate);
    }
}
void FileVpzProject::setVersionToVpz()
{
    if (mVersion->text() != mVpz->getVersion()) {
        QUndoCommand *changeVersion = new Ui::ChangeVersion(mVersion->text(), mVpz,
                                                            mVersion, mTab, mId);
        mUndoStack->push(changeVersion);
    }
}

void FileVpzProject::setExpNameToVpz()
{
    if (mName->text() != mVpz->getExpName()) {
        QUndoCommand *changeName = new Ui::ChangeExpName(mName->text(), mVpz,
                                                         mName, mTab, mId);
        mUndoStack->push(changeName);
    }
}

void FileVpzProject::setExpDurationToVpz()
{
    if (mDuration->text() != mVpz->getExpDuration()) {
        QUndoCommand *changeDuration = new Ui::ChangeExpDuration(mDuration->text(), mVpz,
                                                                 mDuration, mTab, mId);
        mUndoStack->push(changeDuration);
    }
}

void FileVpzProject::setExpBeginToVpz()
{
    if (mBegin->text() != mVpz->getExpBegin()) {
        QUndoCommand *changeBegin = new Ui::ChangeExpBegin(mBegin->text(), mVpz,
                                                           mBegin, mTab, mId);
        mUndoStack->push(changeBegin);
    }
}
