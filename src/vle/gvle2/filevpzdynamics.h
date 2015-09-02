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

#ifndef FILEVPZDYNAMICS_H
#define FILEVPZDYNAMICS_H

#include <QWidget>
#include <QComboBox>
#include <QUndoStack>
#include "vlevpz.h"

namespace Ui {
class FileVpzDynamics;
}


/**
 * Required to identify which combo is pressed (id is the name of the dynamic)
 * reimplement mousePressEvent(QMouseEvent * e)
 * send
 */
class AdhocCombo : public QComboBox
{
    Q_OBJECT
public:
    AdhocCombo(QWidget *parent);
    ~AdhocCombo();
    void setId(const QString& text);
    void  mousePressEvent(QMouseEvent * e);
    QString combo_id;
signals:
    void comboSelected(const QString& name);
};


/**
 * Required for setting a new value only on lost focus (not on all changes)
 * reimplement focusOutEvent
 * send textUpdated on lost focus
 */
class AdhocQTextEdit: public QPlainTextEdit
{
    Q_OBJECT
public:
    AdhocQTextEdit(QWidget *parent = 0);
    ~AdhocQTextEdit();
    void setText(const QString& text);
    void setTextEdition(bool val);
    void focusOutEvent(QFocusEvent *e);
    QString saved_value;
signals:
    void textUpdated(const QString& oldval, const QString& newVal);

};

class FileVpzDynamics : public QWidget
{
    Q_OBJECT

public:
    enum FVD_MENU { FVD_add_dynamic, FVD_rename_dynamic, FVD_remove_dynamic };
public:
    explicit FileVpzDynamics(QWidget *parent = 0);
    ~FileVpzDynamics();
    void setVpz(vleVpz *vpz);
    void setUndo(QUndoStack *undo);
    void reload();

public slots:
    void onComboSelected(const QString& dyn);
    void onSelectPackage(const QString & text);
    void onSelectLibrary(const QString& text);
    void onDynamicsTableMenu(const QPoint&);
    void onUndoRedoVpz(QDomNode, QDomNode);
    void onTextUpdated(const QString&, const QString&);

protected:
    void updatePackageList(QString name = "");
    void updateLibraryList(QString package = "", QString name = "");

private:
    QString getSelected();
    QString curr_dyn;

private:
    Ui::FileVpzDynamics *ui;

    QUndoStack *mUndoStack;

    vleVpz  *mVpz;
    bool     mPackageLoaded;
};

#endif // FILEVPZDYNAMICS_H
