/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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
#ifndef GVLE2_WIDGETVPZPROPERTY_H
#define GVLE2_WIDGETVPZPROPERTY_H

#include <QWidget>
#ifndef Q_MOC_RUN
#include "vlevpz.h"
#endif

namespace Ui {
class WidgetVpzPropertyDynamics;
class WidgetVpzPropertyExpCond;
class WidgetVpzPropertyObservables;
}

namespace vle {
namespace gvle2 {


class WidgetVpzPropertyDynamics : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetVpzPropertyDynamics(QWidget *parent = 0);
    ~WidgetVpzPropertyDynamics();
    void setModel(vleVpz *model, const QString& mod_query);

signals:
    void sigActivated(bool isActive);

private slots:
    void onChange(int index);

private:
    Ui::WidgetVpzPropertyDynamics *ui;
    vleVpz *mVpm;
    QString mModQuery;
};

class WidgetVpzPropertyExpCond : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyExpCond(QWidget *parent = 0);
    ~WidgetVpzPropertyExpCond();
    void setModel(vleVpz *model, const QString& mod_query);

//protected:
//    void mouseDoubleClickEvent (QMouseEvent *event);

private slots:
    void onCheckboxToggle(bool checked);
    void refresh();

private:
    Ui::WidgetVpzPropertyExpCond *ui;
    vleVpz *mVpm;
    QString mModQuery;
};

class WidgetVpzPropertyObservables : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyObservables(QWidget *parent = 0);
    ~WidgetVpzPropertyObservables();
    void setModel(vleVpz *model, const QString& mod_query);

public slots:
    void refresh();

private slots:
    void onCheckboxClick(bool checked);

private:
    Ui::WidgetVpzPropertyObservables *ui;
    vleVpz *mVpm;
    QString mModQuery;
};

}}//namepsaces

#endif // WIDGETVPZPROPERTY_H
