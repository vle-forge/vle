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
#ifndef SOURCECPP_H
#define SOURCECPP_H

#include <QObject>
#include <QString>
#include "sourcecpptemplate.h"
#include "vlepackage.h"
#include "plugin_modeler.h"

class PluginModeler;
class vlePackage;

class sourceCpp : public QObject
{
    Q_OBJECT
public:
    sourceCpp();
    ~sourceCpp();
    sourceCpp(QString filename, vlePackage *package = 0);
    void               load();
    void               write(QString data);
    sourceCppTemplate *getTemplate();
    PluginModeler     *getModeler();
    QString            getModelerName();
    QString            getFilename();
    bool               isNew();
    void               setPackage(vlePackage *package);

private:
    bool    mIsNew;
    QString mFileName;
    vlePackage        *mPackage;
    sourceCppTemplate *mTemplate;
    PluginModeler     *mModeler;
    QString            mContent;
};

#endif // SOURCECPP_H
