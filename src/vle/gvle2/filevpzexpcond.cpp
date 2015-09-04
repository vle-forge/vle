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
#include <QMessageBox>
#include <QScrollArea>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <vle/value/Set.hpp>
#include <vle/value/Matrix.hpp>



#include "filevpzexpcond.h"
#include "ui_filevpzexpcond.h"
#include "gvle2_widgets.h"

#include <QDebug>

namespace vle {
namespace gvle2 {

FileVpzExpCond::FileVpzExpCond(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzExpCond), mVpz(0), mCurrCondName(""),
    mCurrPortName(""), mCurrValIndex(-1), mValWidget(), mPlugin(0)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 1000 << 0;
    ui->splitter->setSizes(sizes);
    ui->table->setAutoScroll(false);
    ui->table->setContextMenuPolicy(Qt::CustomContextMenu);
    mValWidget = new VleValueWidget(ui->value);

    QObject::connect(ui->table,
            SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(onCellDoubleClicked(int, int)));
    QObject::connect(ui->table,
                SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(onConditionMenu(const QPoint&)));
    QObject::connect(mValWidget,
            SIGNAL(valUpdated(const vle::value::Value&)),
            this, SLOT(onValUpdated(const vle::value::Value&)));
}

FileVpzExpCond::~FileVpzExpCond()
{
    delete ui;
}


void
FileVpzExpCond::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
    QObject::connect(mVpz->undoStack, SIGNAL(undoRedoVpz(QDomNode, QDomNode)),
            this, SLOT(onUndoRedoVpz(QDomNode, QDomNode)));

}

void
FileVpzExpCond::resizeTable()
{
    int nbRows = 0;
    int nbCols = 0;
    QDomNodeList condList = mVpz->condsListFromConds(mVpz->condsFromDoc());
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpz->attributeValue(cond, "name");
        QDomNodeList portList = mVpz->portsListFromDoc(name);
        if (portList.length() == 0) {
            nbRows++;
        }
        for (unsigned int j = 0; j < portList.length(); j++) {
            nbRows++;
            QDomNode port = portList.at(j);
            nbCols = std::max(nbCols,
                    (int) mVpz->childNodesWithoutText(port).size());
        }
    }
    nbCols = nbCols+2;
    ui->table->setRowCount(nbRows);
    ui->table->setColumnCount(nbCols);
}

void
FileVpzExpCond::reload(bool resize)
{
    ui->table->clearContents();
    resizeTable();
    QDomNodeList condList = mVpz->condsListFromConds(mVpz->condsFromDoc());

    int rows = 0;
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpz->attributeValue(cond, "name");
        QDomNodeList portList = mVpz->portsListFromDoc(name);

        if (portList.length() == 0) {
            //add a line with empty port
            insertTextEdit(rows, 0, name);
            insertTextEdit(rows, 1, "<Click here to add a port>");
            rows++;
        }

        // Then for each port, add an item into the tree
        for (unsigned int j = 0; j < portList.length(); j++) {
            QDomNode port = portList.at(j);
            insertTextEdit(rows, 0, name);
            insertTextEdit(rows, 1, mVpz->attributeValue(port, "name"));
            std::vector<vle::value::Value*> valuesToFill;
            mVpz->fillWithMultipleValue(port, valuesToFill);
            for (unsigned int k = 0; k < valuesToFill.size(); k++) {
                vle::value::Value* val = valuesToFill[k];
                switch(val->getType()) {
                case vle::value::Value::BOOLEAN: {
                    insertBooleanCombo(rows, k+2, val->toBoolean().value());
                    break;
                } case vle::value::Value::INTEGER: {
                    insertSpinBox(rows, k+2, val->toInteger().value());
                    break;
                } case vle::value::Value::DOUBLE: {
                    insertDoubleSpinBox(rows, k+2, val->toDouble().value());
                    break;
                } case vle::value::Value::STRING: {
                    insertTextEdit(rows, k+2, val->toString().value().c_str());
                    break;
                } case vle::value::Value::SET:
                case vle::value::Value::MAP:
                case vle::value::Value::TUPLE:
                case vle::value::Value::TABLE:
                case vle::value::Value::XMLTYPE:
                case vle::value::Value::NIL:
                case vle::value::Value::MATRIX: {
                    insertTextEdit(rows, k+2, VleValueWidget::getValueDisplay(
                            *val, VleValueWidget::Insight));
                    break;
                } case vle::value::Value::USER: {
                    //TODO
                    break;
                }}
            }
            rows++;
        }
    }
    if (resize) {
        ui->table->resizeColumnsToContents();
    }
}


void
FileVpzExpCond::showValueEdit(vle::value::Value* val)
{
    if (not val) {
        mValWidget->setHidden(true);
        QList<int> sizes = ui->splitter->sizes();
        sizes[0] = (sizes[0]+sizes[1]);
        sizes[1] = 0;
        ui->splitter->setSizes(sizes);
    } else {
        QList<int> sizes = ui->splitter->sizes();
        sizes[0] = (sizes[0]+sizes[1])/2;
        sizes[1] = sizes[0];
        ui->splitter->setSizes(sizes);
        mValWidget->setValue(val);
        mValWidget->show();
    }
}



void
FileVpzExpCond::onCellDoubleClicked(int row, int column)
{
    VleTextEdit* w;
    if (column <= 1) {//edit cond name or port name
        w = (qobject_cast<VleTextEdit*>(ui->table->cellWidget(row,column)));
        w->setTextEdition(true);
    } else {//edit a value
        w = (qobject_cast<VleTextEdit*>(ui->table->cellWidget(row,0)));
        QString cond = w->document()->toPlainText();
        w = (qobject_cast<VleTextEdit*>(ui->table->cellWidget(row,1)));
        QString port = w->document()->toPlainText();
        QDomNode portNode = mVpz->portFromDoc(cond, port);
        std::vector<vle::value::Value*> values;

        mVpz->fillWithMultipleValue(portNode, values);
        if (values.size() >= (unsigned int) (column -1)) {
            vle::value::Value* val = values[column - 2];
            w = (qobject_cast<VleTextEdit*>(ui->table->cellWidget(row,column)));
            switch (val->getType()) {
            case vle::value::Value::BOOLEAN:
            case vle::value::Value::INTEGER:
            case vle::value::Value::DOUBLE: {
                break;
            }
            case vle::value::Value::STRING: {
                VleTextEdit* w = getTextEdit(row, column);
                w->setTextEdition(true);
                break;
            } case vle::value::Value::SET:
            case vle::value::Value::MAP:
            case vle::value::Value::TUPLE:
            case vle::value::Value::TABLE:
            case vle::value::Value::XMLTYPE:
            case vle::value::Value::NIL:
            case vle::value::Value::MATRIX: {
                showValueEdit(val);
                ui->table->scrollToItem(ui->table->item(row,column),
                        QAbstractItemView::PositionAtCenter);
                break;
            } case vle::value::Value::USER: {
                break;
            }}
        }
    }
}

void
FileVpzExpCond::onConditionMenu(const QPoint& pos)
{
    QPoint globalPos = ui->table->mapToGlobal(pos);
    QModelIndex index = ui->table->indexAt(pos);
    QAction* action;
    QMenu menu;
    action = menu.addAction("Add condition");
    action->setData(EMenuCondAdd);
    action->setEnabled(index.column() <= 0);
    action = menu.addAction("Remove condition");
    action->setData(EMenuCondRemove);
    action->setEnabled(index.column() == 0);
    menu.addSeparator();
    action = menu.addAction("Add port");
    action->setData(EMenuPortAdd);
    action->setEnabled(index.column() == 1);
    action = menu.addAction("Remove port");
    action->setData(EMenuPortRemove);
    action->setEnabled(index.column() == 1);
    menu.addSeparator();
    QMenu* subMenu = buildAddValueMenu(menu, "Add");
    subMenu->setEnabled((index.column() == 1) and
            (getTextEdit(index.row(), index.column())->getCurrentText() !=
                    "<Click here to add a port>"));
    subMenu = buildAddValueMenu(menu, "Set");
    subMenu->setEnabled(ui->table->item(index.row(), index.column()) and
            (index.column() > 1));

    QAction* act = menu.exec(globalPos);
    if (act) {
        eCondMenuActions actCode = (eCondMenuActions) act->data().toInt();
        switch(actCode) {
        case EMenuCondAdd: {
            mVpz->addConditionToDoc(mVpz->newCondNameToDoc());
            reload(false);
            break;
        } case EMenuCondRemove: {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            mVpz->rmConditionToDoc(cond);
            reload(false);
            break;
        } case EMenuPortAdd: {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            mVpz->addCondPortToDoc(cond, mVpz->newCondPortNameToDoc(cond));
            reload(false);
            break;
        } case EMenuPortRemove: {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            QString port = getTextEdit(index.row(),1)->getCurrentText();
            mVpz->rmCondPortToDoc(cond, port);
            reload(false);
            break;
        } case EMenuValueAddBoolean:
          case EMenuValueAddInteger:
          case EMenuValueAddDouble:
          case EMenuValueAddString:
          case EMenuValueAddSet:
          case EMenuValueAddMap:
          case EMenuValueAddTuple:
          case EMenuValueAddTable:
          case EMenuValueAddMatrix: {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            QString port = getTextEdit(index.row(),1)->getCurrentText();
            vle::value::Value* v = buildDefaultValue(actCode);
            if (index.column() == 1) {
                mVpz->addValuePortCondToDoc(cond, port,*v);
            } else {
                mVpz->fillWithValue(cond, port, index.column()-2, *v);
            }
            delete v;
            reload(false);
            break;
        } default: {
            break;
        }}
    }
}

void
FileVpzExpCond::onUndoRedoVpz(QDomNode /*oldVal*/, QDomNode /*newVal*/)
{
    reload(false);
    showValueEdit(0);
}

void
FileVpzExpCond::onTextUpdated(const QString& id, const QString& oldVal,
        const QString& newVal)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    QString cond = getTextEdit(row, 0)->getCurrentText();
    if (col == 0) {//edit cond name
        mVpz->renameConditionToDoc(oldVal, newVal);
    } else if (col == 1) {//edit port name
        if (oldVal == "<Click here to add a port>"){
            mVpz->addCondPortToDoc(cond, newVal);
        } else {
            mVpz->renameCondPortToDoc(cond, oldVal, newVal);
        }
    } else {//edit value String
        QString port = getTextEdit(row, 1)->getCurrentText();
        vle::value::Value* curr = mVpz->buildValueFromDoc(cond, port, col-2);
        curr->toString().set(newVal.toStdString());
        mVpz->fillWithValue(cond, port, col-2, *curr);
        delete curr;
    }
    reload(false);
}

void
FileVpzExpCond::onIntUpdated(const QString& id, int newVal)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) {//necesserlay a value
        QString cond = getTextEdit(row, 0)->getCurrentText();
        QString port = getTextEdit(row, 1)->getCurrentText();
        vle::value::Value* curr = mVpz->buildValueFromDoc(cond, port, col-2);
        curr->toInteger().set(newVal);
        mVpz->fillWithValue(cond, port, col-2, *curr);
        delete curr;
    }
}

void
FileVpzExpCond::onDoubleUpdated(const QString& id, double newVal)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) {//necesserlay a value
        QString cond = getTextEdit(row, 0)->getCurrentText();
        QString port = getTextEdit(row, 1)->getCurrentText();
        vle::value::Value* curr = mVpz->buildValueFromDoc(cond, port, col-2);
        curr->toDouble().set(newVal);
        mVpz->fillWithValue(cond, port, col-2, *curr);
        delete curr;
    }
}
void
FileVpzExpCond::onBoolUpdated(const QString& id, const QString& newVal)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) {//necesserlay a value
        QString cond = getTextEdit(row, 0)->getCurrentText();
        QString port = getTextEdit(row, 1)->getCurrentText();
        vle::value::Value* curr = mVpz->buildValueFromDoc(cond, port, col-2);
        curr->toBoolean().set(QVariant(newVal).toBool());
        mVpz->fillWithValue(cond, port, col-2, *curr);
        delete curr;
    }
}

void
FileVpzExpCond::onValUpdated(const vle::value::Value& newVal)
{
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex, newVal);
    reload(false);
}

void
FileVpzExpCond::onSelected(const QString& id)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();

    mCurrCondName = "";
    mCurrPortName = "";
    mCurrValIndex = -1;

    if (col >= 0 ) {
        mCurrCondName =  getTextEdit(row, 0)->getCurrentText();
        if (col >= 1) {
            mCurrPortName =  getTextEdit(row, 1)->getCurrentText();
            if (col >= 2) {
                mCurrValIndex = col - 2;
            }
        }
    }
    showValueEdit(0);
}


QMenu*
FileVpzExpCond::buildAddValueMenu(QMenu& menu, const QString& setOrAdd)
{
    QAction* lastAction;
    QMenu* subMenu  = menu.addMenu(setOrAdd +" value");
    lastAction = subMenu->addAction("boolean");
    lastAction->setData(EMenuValueAddBoolean);
    lastAction = subMenu->addAction("integer");
    lastAction->setData(EMenuValueAddInteger);
    lastAction = subMenu->addAction("double");
    lastAction->setData(EMenuValueAddDouble);
    lastAction = subMenu->addAction("string");
    lastAction->setData(EMenuValueAddString);
    lastAction = subMenu->addAction("set");
    lastAction->setData(EMenuValueAddSet);
    lastAction = subMenu->addAction("map");
    lastAction->setData(EMenuValueAddMap);
    lastAction = subMenu->addAction("tuple");
    lastAction->setData(EMenuValueAddTuple);
    lastAction = subMenu->addAction("table");
    lastAction->setData(EMenuValueAddTable);
    lastAction = subMenu->addAction("matrix");
    lastAction->setData(EMenuValueAddMatrix);
    return subMenu;
}

vle::value::Value*
FileVpzExpCond::buildDefaultValue(eCondMenuActions type)
{
    switch(type) {
    case EMenuValueAddBoolean: {
        return new vle::value::Boolean();
        break;
    } case EMenuValueAddInteger: {
        return new vle::value::Integer();
        break;
    } case EMenuValueAddDouble: {
        return new vle::value::Double();
        break;
    } case EMenuValueAddString: {
        return new vle::value::String();
        break;
    } case EMenuValueAddSet: {
        return new vle::value::Set();
        break;
    } case EMenuValueAddMap: {
        return new vle::value::Map();
        break;
    } case EMenuValueAddTuple: {
        return new vle::value::Tuple();
        break;
    } case EMenuValueAddTable: {
        return new vle::value::Table();
        break;
    } case EMenuValueAddMatrix: {
        return new vle::value::Matrix();
        break;
    } default: {
        return 0;
    }}
}

void
FileVpzExpCond::insertTextEdit(int row, int col, const QString& val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleTextEdit* w = new VleTextEdit(ui->table, val, id, false);
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem);//used to find it
    QObject::connect(w, SIGNAL(
            textUpdated(const QString&, const QString&, const QString&)),
            this, SLOT(
            onTextUpdated(const QString&, const QString&, const QString&)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertSpinBox(int row, int col, int val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleSpinBox* w = new VleSpinBox(ui->table, val, id);

    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem);//used to find it
    QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
            this, SLOT(onIntUpdated(const QString&, int)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertDoubleSpinBox(int row, int col, double val)
{

    QString id = QString("%1,%2").arg(row).arg(col);
    VleDoubleSpinBox* w = new VleDoubleSpinBox(ui->table, val, id);
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem);//used to find it
    QObject::connect(w, SIGNAL(valUpdated(const QString&, double)),
            this, SLOT(onDoubleUpdated(const QString&, double)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertBooleanCombo(int row, int col, bool val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleCombo* w = new VleCombo(ui->table, id);
    QList <QString> l;
    l.append("true");
    l.append("false");
    QString currVal = "false";
    if (val) {
        currVal="true";
    }
    w->addItems(l);
    w->setCurrentIndex(w->findText(currVal));
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem);//used to find it
    QObject::connect(w, SIGNAL(valUpdated(const QString&, const QString&)),
            this, SLOT(onBoolUpdated(const QString&, const QString&)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

VleTextEdit*
FileVpzExpCond::getTextEdit(int row, int col)
{
    return qobject_cast<VleTextEdit*>(ui->table->cellWidget(row,col));
}

}}
