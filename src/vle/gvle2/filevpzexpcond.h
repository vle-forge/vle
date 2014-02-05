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
#ifndef FILEVPZEXPCOND_H
#define FILEVPZEXPCOND_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include "vlevpz.h"
#include "plugin_cond.h"

namespace Ui {
class FileVpzExpCond;
}

class FileVpzExpCond : public QWidget
{
    Q_OBJECT

public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondParameter };

public:
    explicit FileVpzExpCond(QWidget *parent = 0);
    ~FileVpzExpCond();
    void setVpz(vleVpz *vpz);
    void reload();
    void refresh(vpzExpCond *exp, QTreeWidgetItem *expItem = 0);

public slots:
    void onConditionTreeRefresh();
    void onConditionTreeMenu(const QPoint pos);
    void onConditionChanged(QTreeWidgetItem *item, int column);
    void onConditionTreeSelected();
    void onCondParamTreeSelected();
    void onCondParamListDblClick(QListWidgetItem *item);
    void onParamTableChange();
    void onCondParamTableDblClick(int row, int col);
    void onCondParamTableMenu(const QPoint pos);
    void onCondParamTreeMenu(const QPoint pos);
    void onCondParamListMenu(const QPoint pos);
    void onParamTextChange();
    void onCondParamCancel();
    void onCondParamSave();
    void onPluginChanges(vpzExpCond *cond);

private:
    void condParamTreeSelectByValue(vpzExpCondValue *reqValue, QTreeWidgetItem *base = 0);
    void condUpdateTree(QList <vpzExpCondValue *> *valueList, QList<QTreeWidgetItem *> *widList);
    void condValueShowDetail(vpzExpCondValue *value);
    void condShowPlugin(vpzExpCond *cond);
    void condHidePlugin();
    void paramTreeUpdateList(vpzExpCondValue *value);
    void paramTreeUpdateTable(vpzExpCondValue *value);
    void paramTreeUpdateText(vpzExpCondValue *value);

private:
    Ui::FileVpzExpCond *ui;
    vleVpz  *mVpz;
    QTreeWidgetItem *mCondEdition;
    PluginExpCond   *mPlugin;
};

#endif // FILEVPZEXPCOND_H
