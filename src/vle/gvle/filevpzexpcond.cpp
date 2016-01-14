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


#include "vlepackage.h"
#include "gvle_plugins.h"

#include "filevpzexpcond.h"
#include "ui_filevpzexpcond.h"
#include "gvle_widgets.h"

#include <QDebug>

namespace vle {
namespace gvle {

FileVpzExpCond::FileVpzExpCond(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzExpCond), mVpm(0), mCurrCondName(""),
    mCurrPortName(""), mCurrValIndex(-1),  mPlugin(0)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 700 << 300;
    ui->splitter->setSizes(sizes);

    ui->table->setAutoScroll(false);
    ui->table->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(ui->table,
            SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(onCellDoubleClicked(int, int)));
    QObject::connect(ui->table,
                SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(onConditionMenu(const QPoint&)));

}

FileVpzExpCond::~FileVpzExpCond()
{
    delete ui;
}


void
FileVpzExpCond::setVpm(vleVpm* vpm)
{
    mVpm = vpm;
    QObject::connect(mVpm,
                     SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
                     this,
                     SLOT(onUndoRedoVpm(QDomNode, QDomNode, QDomNode, QDomNode)));
    QObject::connect(mVpm,
                     SIGNAL(experimentUpdated()),
                     this,
                     SLOT(onExpUpdated()));

}

void
FileVpzExpCond::resizeTable()
{
    int nbRows = 0;
    int nbCols = 0;
    QDomNodeList condList = mVpm->condsListFromConds(mVpm->condsFromDoc());
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpm->vdo()->attributeValue(cond, "name");
        if (mVpm->getCondGUIplugin(name) != "") {
            nbRows++;
        } else {
            QDomNodeList portList = mVpm->portsListFromDoc(name);
            if (portList.length() == 0) {
                nbRows++;
            }
            for (unsigned int j = 0; j < portList.length(); j++) {
                nbRows++;
                QDomNode port = portList.at(j);
                nbCols = std::max(nbCols,
                        (int) mVpm->childNodesWithoutText(port).size());
            }
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
    QDomNodeList condList = mVpm->condsListFromConds(mVpm->condsFromDoc());

    int rows = 0;
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpm->vdo()->attributeValue(cond, "name");
        QDomNodeList portList = mVpm->portsListFromDoc(name);
        if (mVpm->getCondGUIplugin(name) != ""){
            insertTextEdit(rows, 0, name);
            insertTextEdit(rows, 1, QString("[plugin:%1]").arg(
                    mVpm->getCondGUIplugin(name)));
            for (int j=2; j < ui->table->columnCount(); j++) {
                insertNullWidget(rows, j);
            }
            rows++;
        } else if (portList.length() == 0) {
            insertTextEdit(rows, 0, name);
            insertTextEdit(rows, 1, "<Click here to add a port>");
            for (int j=2; j < ui->table->columnCount(); j++) {
                insertNullWidget(rows, j);
            }
            rows++;
        } else  {
            // Then for each port, add an item into the tree
            for (unsigned int j = 0; j < portList.length(); j++) {
                QDomNode port = portList.at(j);
                insertTextEdit(rows, 0, name);
                insertTextEdit(rows, 1, mVpm->vdo()->attributeValue(port,
                        "name"));
                std::vector<vle::value::Value*> valuesToFill;
                mVpm->fillWithMultipleValue(port, valuesToFill);
                for (int k = 0; k < ui->table->columnCount()-2; k++) {
                    if (k < (int) valuesToFill.size()) {
                        vle::value::Value* val = valuesToFill[k];
                        switch(val->getType()) {
                        case vle::value::Value::BOOLEAN: {
                            insertBooleanCombo(rows, k+2,
                                    val->toBoolean().value());
                            break;
                        } case vle::value::Value::INTEGER: {
                            insertSpinBox(rows, k+2, val->toInteger().value());
                            break;
                        } case vle::value::Value::DOUBLE: {
                            insertDoubleSpinBox(rows, k+2,
                                    val->toDouble().value());
                            break;
                        } case vle::value::Value::STRING: {
                            insertTextEdit(rows, k+2,
                                    val->toString().value().c_str());
                            break;
                        } case vle::value::Value::SET:
                        case vle::value::Value::MAP:
                        case vle::value::Value::TUPLE:
                        case vle::value::Value::TABLE:
                        case vle::value::Value::XMLTYPE:
                        case vle::value::Value::NIL:
                        case vle::value::Value::MATRIX: {
                            insertTextEdit(rows, k+2,
                                    VleValueWidget::getValueDisplay(
                                            *val, VleValueWidget::Insight));
                            break;
                        } case vle::value::Value::USER: {
                            //TODO
                            break;
                        }}
                    } else {
                        insertNullWidget(rows, k+2);
                    }
                }
                rows++;
            }
        }

    }
    if (resize) {
        ui->table->resizeColumnsToContents();
    }
    showEditPlace();
}


void
FileVpzExpCond::showEditPlace()
{
    if (mPlugin) {
        mPlugin->delWidget();
        mPlugin->delWidgetToolbar();
        mPlugin = 0;
    } else {
        while (QWidget* w = ui->value->findChild<QWidget*>() ) {
            delete w;
        }
    }


    if (mCurrCondName == "") {

        QLabel* lab = new QLabel("Nothing to edit", ui->value);
        lab->show();
        return;
    }
    if (mVpm->getCondGUIplugin(mCurrCondName) != "") {
        mPlugin = mVpm->provideCondGUIplugin(mCurrCondName);
        mPlugin->getWidget()->setParent(ui->value);
        mPlugin->getWidget()->show();
        return;
    }
    if (mCurrPortName == "") {
        QLabel* lab = new QLabel("Nothing to edit", ui->value);
        lab->show();
        return;
    }
    if (mCurrValIndex < 0) {
        QLabel* lab = new QLabel("Nothing to edit", ui->value);
        lab->show();
        return;
    }
    if (mCurrValIndex >= 0) {
        QDomNode portNode = mVpm->portFromDoc(mCurrCondName, mCurrPortName);
        std::vector<vle::value::Value*> values;
        mVpm->fillWithMultipleValue(portNode, values);
        if (values.size() > (unsigned int) mCurrValIndex) {
            vle::value::Value* val = values[mCurrValIndex];
            switch (val->getType()) {
            case vle::value::Value::BOOLEAN:
            case vle::value::Value::INTEGER:
            case vle::value::Value::DOUBLE:
            case vle::value::Value::STRING:
            case vle::value::Value::USER: {
                QLabel* lab = new QLabel("Nothing to edit", ui->value);
                lab->show();
                break;
            } case vle::value::Value::SET:
            case vle::value::Value::MAP:
            case vle::value::Value::TUPLE:
            case vle::value::Value::TABLE:
            case vle::value::Value::XMLTYPE:
            case vle::value::Value::NIL:
            case vle::value::Value::MATRIX: {
                VleValueWidget* valWidget = new VleValueWidget(ui->value);
                QObject::connect(valWidget,
                        SIGNAL(valUpdated(const vle::value::Value&)),
                        this, SLOT(onValUpdated(const vle::value::Value&)));
                valWidget->setValue(val);
                valWidget->show();
                break;
            }}
        }
    }
}



void
FileVpzExpCond::onCellDoubleClicked(int row, int column)
{
    VleTextEdit* w = qobject_cast<VleTextEdit*>(
            ui->table->cellWidget(row,column));
    if (w) {
        w->setTextEdition(true);
    }
}

void
FileVpzExpCond::onConditionMenu(const QPoint& pos)
{
    QPoint globalPos = ui->table->mapToGlobal(pos);
    QModelIndex index = ui->table->indexAt(pos);
    QAction* action;
    QMenu menu;

    QMenu* subMenu = menu.addMenu("Add condition");
    action = subMenu->addAction("default");
    action->setData("EMenuCondAdd");
    action->setEnabled(true);

    //check plugins
    gvle_plugins plugs;
    plugs.loadPlugins();
    QStringList plugList = plugs.getCondPluginsList();
    for (int i =0; i<plugList.size(); i++) {
        action = subMenu->addAction(plugList.at(i)+" (plugin)");
        action->setEnabled(true);
        action->setData(plugList.at(i));
    }



    QString condName = "";
    if (index.row() > -1) {
        condName = getTextEdit(index.row(), 0)->getCurrentText();
    }

    action = menu.addAction("Remove condition");
    action->setData("EMenuCondRemove");
    action->setEnabled(index.column() == 0);
    menu.addSeparator();
    action = menu.addAction("Add port");
    action->setData("EMenuPortAdd");
    action->setEnabled((index.column() == 1) and
            (mVpm->getCondGUIplugin(condName) == ""));
    action = menu.addAction("Remove port");
    action->setData("EMenuPortRemove");
    action->setEnabled((index.column() == 1) and
            (mVpm->getCondGUIplugin(condName) == ""));
    menu.addSeparator();
    subMenu = buildAddValueMenu(menu, "Add");
    subMenu->setEnabled((index.column() == 1) and
            (getTextEdit(index.row(), index.column())->getCurrentText() !=
                    "<Click here to add a port>") and
            (mVpm->getCondGUIplugin(condName) == ""));
    subMenu = buildAddValueMenu(menu, "Set");
    subMenu->setEnabled(ui->table->item(index.row(), index.column()) and
            (index.column() > 1) and
            (ui->table->cellWidget(index.row(), index.column()) != 0));
    action = menu.addAction("Remove value");
    action->setData("EMenuValueRemove");
    action->setEnabled(ui->table->item(index.row(), index.column()) and
            (index.column() > 1) and
            (ui->table->cellWidget(index.row(), index.column()) != 0));

    QAction* act = menu.exec(globalPos);
    if (act) {
        QString actCode = act->data().toString();
        if (actCode == "EMenuCondAdd") {
            mVpm->addConditionToDoc(mVpm->newCondNameToDoc());
            reload(false);
        } else if (actCode == "EMenuCondRemove") {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            mVpm->rmConditionToDoc(cond);
            reload(false);
        } else if (actCode == "EMenuPortAdd") {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            mVpm->addCondPortToDoc(cond, mVpm->newCondPortNameToDoc(cond));
            reload(false);
        } else if (actCode == "EMenuPortRemove") {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            QString port = getTextEdit(index.row(),1)->getCurrentText();
            mVpm->rmCondPortToDoc(cond, port);
            reload(false);
        } else if ((actCode == "EMenuValueAddBoolean") or
                (actCode == "EMenuValueAddInteger") or
                (actCode == "EMenuValueAddDouble") or
                (actCode == "EMenuValueAddString") or
                (actCode == "EMenuValueAddSet") or
                (actCode == "EMenuValueAddMap") or
                (actCode == "EMenuValueAddTuple") or
                (actCode == "EMenuValueAddTable") or
                (actCode == "EMenuValueAddMatrix")) {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            QString port = getTextEdit(index.row(),1)->getCurrentText();
            vle::value::Value* v = buildDefaultValue(actCode);
            if (index.column() == 1) {
                mVpm->addValuePortCondToDoc(cond, port,*v);
            } else {
                mVpm->fillWithValue(cond, port, index.column()-2, *v);
            }
            delete v;
            reload(false);
        } else if (actCode == "EMenuValueRemove") {
            QString cond = getTextEdit(index.row(),0)->getCurrentText();
            QString port = getTextEdit(index.row(),1)->getCurrentText();
            mVpm->rmValuePortCondToDoc(cond, port, index.column()-2);
            reload(false);
        } else {//add from plugin
            mCurrCondName = mVpm->newCondNameToDoc();
            mVpm->addConditionFromPluginToDoc(mCurrCondName, actCode);
            reload(false);
        }
    }
}

void
FileVpzExpCond::onUndoRedoVpm(QDomNode /*oldValVpz*/, QDomNode /*newValVpz*/,
        QDomNode /*oldValVpm*/, QDomNode /*newValVpm*/)
{
    showEditPlace();
    reload(false);
}

void
FileVpzExpCond::onExpUpdated()
{
    showEditPlace();
    reload(false);
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
        mVpm->renameConditionToDoc(oldVal, cond);
        mCurrCondName = cond;
    } else if (col == 1) {//edit port name
        mCurrCondName = cond;
        if (oldVal == "<Click here to add a port>"){
            mVpm->addCondPortToDoc(cond, newVal);
        } else {
            mVpm->renameCondPortToDoc(cond, oldVal, newVal);
            mCurrPortName = newVal;
        }

    } else {//edit value String
        QString port = getTextEdit(row, 1)->getCurrentText();
        vle::value::Value* curr = mVpm->buildValueFromDoc(cond, port, col-2);
        curr->toString().set(newVal.toStdString());
        mVpm->fillWithValue(cond, port, col-2, *curr);
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
        vle::value::Value* curr = mVpm->buildValueFromDoc(cond, port, col-2);
        curr->toInteger().set(newVal);
        mVpm->fillWithValue(cond, port, col-2, *curr);
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
        vle::value::Value* curr = mVpm->buildValueFromDoc(cond, port, col-2);
        curr->toDouble().set(newVal);
        mVpm->fillWithValue(cond, port, col-2, *curr);
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
        vle::value::Value* curr = mVpm->buildValueFromDoc(cond, port, col-2);
        curr->toBoolean().set(QVariant(newVal).toBool());
        mVpm->fillWithValue(cond, port, col-2, *curr);
        delete curr;
    }
}

void
FileVpzExpCond::onValUpdated(const vle::value::Value& newVal)
{
    mVpm->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex, newVal);
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
    showEditPlace();
}


QMenu*
FileVpzExpCond::buildAddValueMenu(QMenu& menu, const QString& setOrAdd)
{
    QAction* lastAction;
    QMenu* subMenu  = menu.addMenu(setOrAdd +" value");
    lastAction = subMenu->addAction("boolean");
    lastAction->setData("EMenuValueAddBoolean");
    lastAction = subMenu->addAction("integer");
    lastAction->setData("EMenuValueAddInteger");
    lastAction = subMenu->addAction("double");
    lastAction->setData("EMenuValueAddDouble");
    lastAction = subMenu->addAction("string");
    lastAction->setData("EMenuValueAddString");
    lastAction = subMenu->addAction("set");
    lastAction->setData("EMenuValueAddSet");
    lastAction = subMenu->addAction("map");
    lastAction->setData("EMenuValueAddMap");
    lastAction = subMenu->addAction("tuple");
    lastAction->setData("EMenuValueAddTuple");
    lastAction = subMenu->addAction("table");
    lastAction->setData("EMenuValueAddTable");
    lastAction = subMenu->addAction("matrix");
    lastAction->setData("EMenuValueAddMatrix");
    return subMenu;
}

vle::value::Value*
FileVpzExpCond::buildDefaultValue(QString type)
{
    if (type == "EMenuValueAddBoolean") {
        return new vle::value::Boolean();
    } else if (type == "EMenuValueAddInteger") {
        return new vle::value::Integer();
    } else if(type == "EMenuValueAddDouble") {
        return new vle::value::Double();
    } else if(type == "EMenuValueAddString") {
        return new vle::value::String();
    } else if(type == "EMenuValueAddSet") {
        return new vle::value::Set();
    } else if(type == "EMenuValueAddMap") {
        return new vle::value::Map();
    } else if(type == "EMenuValueAddTuple") {
        return new vle::value::Tuple();
    } else if(type == "EMenuValueAddTable") {
        return new vle::value::Table();
    } else if(type == "EMenuValueAddMatrix") {
        return new vle::value::Matrix();
    }
    return 0;
}

void
FileVpzExpCond::insertNullWidget(int row, int col)
{
    ui->table->setCellWidget(row, col, 0);
    QTableWidgetItem* item = new QTableWidgetItem;
    item->setFlags(item->flags() &  ~Qt::ItemIsEditable);
    ui->table->setItem(row, col, item);//used to find it
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
