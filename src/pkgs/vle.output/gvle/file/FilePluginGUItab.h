/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FilePluginGUItab_H
#define FilePluginGUItab_H

#include <QObject>
#include <QtXml>
#include <QtCore/qnamespace.h>
#include <vle/gvle/vlevpz.hpp>
#include <vle/value/Map.hpp>

namespace Ui {
class FilePluginGvle;
}

class FilePluginGUItab : public QWidget
{
    Q_OBJECT

public:
    explicit FilePluginGUItab(QWidget *parent = 0);
    ~FilePluginGUItab();
    void init(vle::gvle::vleVpz* vpz, const QString& viewName);

private slots:
    void flushByBagChanged(int val);
    void julianDayChanged(int val);
    void localeChanged(const QString& val);
    void destinationChanged(const QString& val);
    void fileTypeChanged(const QString& val);


private:
    int getCheckState(const vle::value::Boolean& v);
    bool wellFormed();
    void buildDefaultConfig();

    Ui::FilePluginGvle* ui;
    vle::gvle::vleVpz*  mvleVpz;
    QString             mViewName;
    std::unique_ptr<vle::value::Map>    outputNodeConfig;
};

#endif
