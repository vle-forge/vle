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
#ifndef WIDGETVPZPROPERTY_H
#define WIDGETVPZPROPERTY_H

#include <QWidget>
#include "vlevpz.h"

namespace Ui {
class WidgetVpzPropertyDynamics;
class WidgetVpzPropertyExpCond;
}

class WidgetVpzPropertyDynamics : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetVpzPropertyDynamics(QWidget *parent = 0);
    ~WidgetVpzPropertyDynamics();
    void setModel(vleVpzModel *model);

signals:
    void sigActivated(bool isActive);

private slots:
    void onChange(int index);

private:
    Ui::WidgetVpzPropertyDynamics *ui;
    vleVpzModel *mModel;
};

class WidgetVpzPropertyExpCond : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyExpCond(QWidget *parent = 0);
    ~WidgetVpzPropertyExpCond();
    void setModel(vleVpzModel *model);

//protected:
//    void mouseDoubleClickEvent (QMouseEvent *event);

private slots:
    void onCheckboxToggle(bool checked);

private:
    Ui::WidgetVpzPropertyExpCond *ui;
    vleVpzModel *mModel;
};

#endif // WIDGETVPZPROPERTY_H
