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

#ifndef VLEVPZDYNAMIC_H
#define VLEVPZDYNAMIC_H

#include <QList>
#include <QString>
#ifndef Q_MOC_RUN
#include <vle/utils/Path.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

class vleVpzDynamic
{
public:
    vleVpzDynamic(QString name = "", QString lib = "", QString package = "");
    vleVpzDynamic(vleVpzDynamic *dynamic);

    void setName(QString name);
    void setLibrary(QString name);
    void setPackage(QString name);
    QString getName();
    QString getLibrary();
    QString getPackage();
    QList <QString> *getPackageList();
    QList <QString> *getLibraryList(QString package);
    bool    isAltered();
    bool    isValid(bool again = false);

private:
    bool    mIsAltered;
    bool    mValidPackage;
    bool    mValidLibrary;
    QString mName;
    QString mLibrary;
    QString mPackage;
    QList <QString> mPackageList;
    QList <QString> mLibraryList;
};

#endif // VLEVPZDYNAMIC_H
