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
#ifndef gvle_SOURCECPPTEMPLATE_H
#define gvle_SOURCECPPTEMPLATE_H

#include <QMap>
#include <QString>

#ifndef Q_MOC_RUN
#include <vle/utils/Template.hpp>
#endif


namespace vle {
namespace gvle {

class sourceCpp;

class sourceCppTemplate
{
public:
    sourceCppTemplate(sourceCpp *parent = 0);
    bool    isValid();
    QString getPluginName();
    bool    tagArrayLoad(QString name);
    QString getTagValue(QString name);
    int     getTagArrayCount();
    QString getTagArrayName(int pos);
    QString getTagArrayValue(int pos);
private:
    void tagLoad();
private:
    sourceCpp  *mSource;
    bool        mTagLoaded;
    QString     mTagPlugin;
    QString     mTagPackage;
    QMap<QString,QString> mTagConf;
    QMap<QString,QString> mTagConfValue;
};

}}//namepsaces

#endif // SOURCECPPTEMPLATE_H
