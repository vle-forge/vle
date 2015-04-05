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
#include <QTableWidgetItem>
#include "vlevpz.h"
#include "plugin_cond.h"

namespace Ui {
class FileVpzExpCond;
}

class FileVpzExpCond : public QWidget
{
    Q_OBJECT

public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondPort,
                           ECondValue };
    enum ValueDisplayType {TypeOnly, Insight};

//    BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
//                XMLTYPE, NIL, MATRIX, USER

    enum PageEditType {PageBoolean, PageInteger, PageDouble,
         PageString, PageSet, PageMap, PageTuple, PageTable,
         PageMatrix, PageBlank};
    enum eCondMenuActions { EMenuCondAdd, EMenuCondRename, EMenuCondRemove,
      EMenuPortAdd, EMenuPortRename, EMenuPortRemove, EMenuPlugins,
      EMenuValueAddBoolean, EMenuValueAddInteger, EMenuValueAddDouble,
      EMenuValueAddString, EMenuValueAddSet, EMenuValueAddMap,
      EMenuValueAddTuple, EMenuValueAddTable, EMenuValueAddMatrix,
      EMenuValueRemove };

    enum eMapMenuActions { EMapBooleanAdd, EMapValueRename, EMapValueRemove};

public:
    explicit FileVpzExpCond(QWidget *parent = 0);
    ~FileVpzExpCond();
    void setVpz(vleVpz *vpz);
    void reload();
    void refresh(const QString& condName, QTreeWidgetItem *expItem = 0);

public slots:
    void onConditionTreeMenu(const QPoint pos);
    void onMapTreeMenu(const QPoint pos);
    void tupleMenu(const QPoint&);
    void setMenu(const QPoint&);
    void onConditionChanged(QTreeWidgetItem *item, int column);
    void onMapTreeChanged(QTreeWidgetItem *item, int column);
    void onConditionTreeSelected();
    void onCondParamTreeSelected();

    void onCondParamCancel();
    void onPluginChanges(const QString& condName);
    void boolEdited(const QString&);
    void intEdited(int v);
    void doubleEdited(double);
    void stringEdited();
    void tupleEdited(QListWidgetItem*);
    void tableEdited(int, int);
    void tableDimButtonClicked(bool);
    void mapDoubleClicked(QTreeWidgetItem* item, int column);
    void setDoubleClicked(QListWidgetItem* item);
    void matrixDoubleClicked(QTableWidgetItem* item);
    void matrixDimButtonClicked(bool b);
    void upStackButonClicked(bool b);

private:
    void buildAddValueMenu(QMenu& menu);
    vle::value::Value* buildDefaultValue(eCondMenuActions act);
    void condUpdateTree(const std::vector<vle::value::Value*>& values,
            QList<QTreeWidgetItem *> *widList);
    void condValueShowDetail();
    void condShowPlugin(const QString& condName);
    void condHidePlugin();
    QString getValueDisplay(const vle::value::Value& v,
            ValueDisplayType displayType) const;

private:
    Ui::FileVpzExpCond* ui;
    vleVpz*             mVpz;
    QString             mCurrCondName;
    QString             mCurrPortName;
    int                 mCurrValIndex;
    //QTreeWidgetItem *mCondEdition;
    PluginExpCond*      mPlugin;
    struct value_stack {
        value_stack();
        ~value_stack();
        vle::value::Value* startValue;
        typedef  std::vector<vle::value::Value*> cont;
        cont stack;
        void delStartValue();
        void setStartValue(vle::value::Value* val);
        /*
         * @brief push a vle::value::Map key
         */
        void push(const std::string& key);
        /*
         * @brief push a vle::value::Set index
         */
        void push(int index);
        /*
         * @brief push a vle::value::Matrix index (row column)
         */
        void push(int row, int col);

        vle::value::Value* editingValue();
        std::string toString();
    };
    value_stack         mValueStack;
};

#endif // FILEVPZEXPCOND_H
