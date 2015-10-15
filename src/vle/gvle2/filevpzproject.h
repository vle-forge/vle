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

#ifndef GVLE2_FILEVPZPROJECT_H
#define GVLE2_FILEVPZPROJECT_H

#include <QWidget>
#include <QUndoStack>
#include <QTabWidget>
#include <QDateTime>
#include "vlevpz.h"
#include "vpzActions.h"

namespace Ui {
class FileVpzProject;
}

namespace vle {
namespace gvle2 {

class FileVpzProject : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzProject(QWidget *parent = 0);
    ~FileVpzProject();
    void setVpz(vleVpz *vpz)
    {mVpz = vpz;};
    void setUndo(QUndoStack *undo);
    void setTabId(int i)
    {mId = i;};
    void setTab(QTabWidget *tab)
    {mTab = tab;};
    void reload();

public slots:
    void setAuthorToVpz();
    void setDateToVpz();
    void setVersionToVpz();
    void setExpNameToVpz();
    void setExpDurationToVpz();
    void setExpBeginToVpz();

protected:

private:

private:
    Ui::FileVpzProject *ui;

    QUndoStack *mUndoStack;
    QTabWidget *mTab;
    int mId;

    QLineEdit *mAuthor;
    QDateTimeEdit *mDate;
    QLineEdit *mVersion;
    QLineEdit *mName;
    QLineEdit *mDuration;
    QLineEdit *mBegin;

    vleVpz  *mVpz;
};

}} //namepsaces

#endif // FILEVPZPROJECT_H
