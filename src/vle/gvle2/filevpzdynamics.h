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
#include "vlevpz.h"

namespace Ui {
class FileVpzDynamics;
}

class FileVpzDynamics : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzDynamics(QWidget *parent = 0);
    ~FileVpzDynamics();
    void setVpz(vleVpz *vpz);
    void reload();

public slots:
    void onSelected(int cr, int cc, int pr, int pc);
    void onSelectPackage(int index);
    void onSelectLibrary(int index);
    void onAddButton();
    void onCloneButton();
    void onRemoveButton();
    void onSaveButton();

protected:
    void updatePackageList(QString name = "");
    void updateLibraryList(QString package = "", QString name = "");

private:
    vleVpzDynamic *getSelected();

private:
    Ui::FileVpzDynamics *ui;
    vleVpz  *mVpz;
    bool     mPackageLoaded;
};

#endif // FILEVPZDYNAMICS_H
