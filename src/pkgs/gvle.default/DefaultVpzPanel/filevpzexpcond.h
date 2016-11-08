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
#ifndef gvle_FILEVPZEXPCOND_H
#define gvle_FILEVPZEXPCOND_H

#include <vle/gvle/plugin_cond.h>
#include <vle/gvle/gvle_widgets.h>

#include <iostream>

namespace Ui {
class FileVpzExpCond;
}

namespace vle {
namespace gvle {


class FileVpzExpCond : public QWidget
{
    Q_OBJECT

public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondPort,
                           ECondValue };
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
    explicit FileVpzExpCond(gvle_plugins* plugs, QWidget *parent = 0);

    ~FileVpzExpCond();
    void setVpz(vleVpz* vpz);
    void resizeTable();
    void reload(bool resize=true);
    void showEditPlace();

public slots:
    void onConditionMenu(const QPoint&);
    void onUndoRedoVpz(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);
    void onExpUpdated();
    void onCondUpdated();
    void onTextUpdated(const QString& id, const QString& oldVal,
            const QString& newVal);
    void onIntUpdated(const QString& id, int newVal);
    void onDoubleUpdated(const QString& id, double newVal);
    void onBoolUpdated(const QString& id, const QString& newVal);
    void onValUpdated(const vle::value::Value& newVal);
    void onSelected(const QString& id);

private:
    QMenu* buildAddValueMenu(QMenu& menu, const QString& setOrAdd);
    vle::value::Value* buildDefaultValue(QString type);
    void insertNullWidget(int row, int col);
    bool hasWidget(int row, int col);
    void insertTextEdit(int row, int col, const QString& val);
    void insertSpinBox(int row, int col, int val);
    void insertDoubleEdit(int row, int col, double val);
    void insertBooleanCombo(int row, int col, bool val);
    VleLineEdit* getLineEdit(int row, int col);

private:
    void disconnectConds();
    void connectConds();

    Ui::FileVpzExpCond* ui;
    vleVpz*             mVpz;
    QString             mCurrCondName;
    QString             mCurrPortName;
    int                 mCurrValIndex;
    PluginExpCond*      mPlugin;
    gvle_plugins*       mGvlePlugins;
};

}}//namespaces
#endif
