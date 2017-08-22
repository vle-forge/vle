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
#ifndef gvle_WIDGETVPZPROPERTY_H
#define gvle_WIDGETVPZPROPERTY_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <vle/gvle/vlevpz.hpp>

namespace Ui {
class WidgetVpzPropertyObservables;
}

namespace vle {
namespace gvle {


class WidgetVpzPropertyDynamics : public QComboBox
{
    Q_OBJECT

public:
    explicit WidgetVpzPropertyDynamics(QWidget *parent = 0);
    ~WidgetVpzPropertyDynamics();
    void selectModel(vleVpz* vpz, const QString& mod_query);

signals:
    void sigActivated(bool isActive);

private slots:
    void onChange(int index);

private:
    vleVpz* mVpz;
    QString mModQuery;
};

class WidgetVpzPropertyExpCond : public QListWidget
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyExpCond(QWidget *parent = 0);
    ~WidgetVpzPropertyExpCond();
    void selectModel(vleVpz* vpz, const QString& mod_query);

//protected:
//    void mouseDoubleClickEvent (QMouseEvent *event);

private slots:
    void onCheckboxToggle(bool checked);
    void refresh();

private:
    vleVpz* mVpz;
    QString mModQuery;
};

class WidgetVpzPropertyObservables : public QComboBox
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyObservables(QWidget *parent = 0);
    ~WidgetVpzPropertyObservables();
    void selectModel(vleVpz* model, const QString& mod_query);

public slots:
    void refresh();

private slots:
    void onChange(int index);

private:
    vleVpz* mVpz;
    QString mModQuery;
};

}}//namepsaces

#endif // WIDGETVPZPROPERTY_H
