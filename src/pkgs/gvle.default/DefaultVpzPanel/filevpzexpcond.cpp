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

#include <QClipboard>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QWidget>
#include <vle/gvle/gvle_plugins.h>
#include <vle/gvle/gvle_widgets.h>
#include <vle/gvle/vlevpz.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

#include "filevpzexpcond.h"

#include "ui_filevpzexpcond.h"

namespace vle {
namespace gvle {

FileVpzExpCond::FileVpzExpCond(gvle_plugins* plugs, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::FileVpzExpCond)
  , mVpz(0)
  , mCurrCondName("")
  , mCurrPortName("")
  , mCurrValIndex(-1)
  , mPlugin(0)
  , mGvlePlugins(plugs)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 700 << 300;
    ui->splitter->setSizes(sizes);

    ui->table->setAutoScroll(false);
    ui->table->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(ui->table,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,
                     SLOT(onConditionMenu(const QPoint&)));
}

FileVpzExpCond::~FileVpzExpCond()
{
    delete ui;
}

void
FileVpzExpCond::setVpz(vleVpz* vpz)
{
    mVpz = vpz;
    QObject::connect(
      mVpz, SIGNAL(experimentUpdated()), this, SLOT(onExpUpdated()));
    connectConds();
}

void
FileVpzExpCond::resizeTable()
{
    int nbRows = 0;
    int nbCols = 0;
    QDomNodeList condList = mVpz->condsListFromConds(mVpz->condsFromDoc());
    for (auto i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = DomFunctions::attributeValue(cond, "name");
        if (mVpz->getCondGUIplugin(name) != "") {
            nbRows++;
        } else {
            QDomNodeList portList = mVpz->portsListFromDoc(name);
            if (portList.length() == 0) {
                nbRows++;
            }
            for (auto j = 0; j < portList.length(); j++) {
                nbRows++;
                QDomNode port = portList.at(j);
                nbCols = std::max(
                  nbCols,
                  (int)DomFunctions::childNodesWithoutText(port).size());
            }
        }
    }
    nbCols = nbCols + 2;
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
    for (auto i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = DomFunctions::attributeValue(cond, "name");
        QDomNodeList portList = mVpz->portsListFromDoc(name);
        if (mVpz->getCondGUIplugin(name) != "") {
            insertTextEdit(rows, 0, name);
            insertTextEdit(
              rows,
              1,
              QString("[plugin:%1]").arg(mVpz->getCondGUIplugin(name)));

            for (int j = 2; j < ui->table->columnCount(); j++) {
                insertNullWidget(rows, j);
            }
            rows++;
        } else if (portList.length() == 0) {
            insertTextEdit(rows, 0, name);
            insertTextEdit(rows, 1, "<Click here to add a port>");
            for (int j = 2; j < ui->table->columnCount(); j++) {
                insertNullWidget(rows, j);
            }
            rows++;
        } else {
            // Then for each port, add an item into the tree
            for (auto j = 0; j < portList.length(); j++) {
                QDomNode port = portList.at(j);
                insertTextEdit(rows, 0, name);
                insertTextEdit(
                  rows, 1, DomFunctions::attributeValue(port, "name"));
                std::vector<std::unique_ptr<value::Value>> valuesToFill;
                mVpz->fillWithMultipleValue(port, valuesToFill);
                for (int k = 0; k < ui->table->columnCount() - 2; k++) {
                    if (k < (int)valuesToFill.size()) {
                        std::unique_ptr<value::Value> val =
                          std::move(valuesToFill[k]);
                        switch (val->getType()) {
                        case vle::value::Value::BOOLEAN: {
                            insertBooleanCombo(
                              rows, k + 2, val->toBoolean().value());
                            break;
                        }
                        case vle::value::Value::INTEGER: {
                            insertSpinBox(
                              rows, k + 2, val->toInteger().value());
                            break;
                        }
                        case vle::value::Value::DOUBLE: {
                            insertDoubleEdit(
                              rows, k + 2, val->toDouble().value());
                            break;
                        }
                        case vle::value::Value::STRING: {
                            insertTextEdit(
                              rows, k + 2, val->toString().value().c_str());
                            break;
                        }
                        case vle::value::Value::SET:
                        case vle::value::Value::MAP:
                        case vle::value::Value::TUPLE:
                        case vle::value::Value::TABLE:
                        case vle::value::Value::XMLTYPE:
                        case vle::value::Value::NIL:
                        case vle::value::Value::MATRIX: {
                            insertTextEdit(rows,
                                           k + 2,
                                           VleValueWidget::getValueDisplay(
                                             *val, VleValueWidget::Insight));
                            break;
                        }
                        case vle::value::Value::USER: {
                            // TODO
                            break;
                        }
                        }
                    } else {
                        insertNullWidget(rows, k + 2);
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
        delete mPlugin;
        mPlugin = 0;
    }
    for (int i = 0; i < ui->vlValue->count(); i++) {
        QWidget* currwid = ui->vlValue->itemAt(i)->widget();
        ui->vlValue->removeWidget(currwid);
        currwid->deleteLater();
    }

    if (mVpz->getCondGUIplugin(mCurrCondName) != "") {
        mPlugin = mVpz->provideCondGUIplugin(mCurrCondName);
        if (mPlugin) {
            mPlugin->getWidget()->setParent(this);
            ui->vlValue->addWidget(mPlugin->getWidget());
            mPlugin->getWidget()->show();
        }
        return;
    }
    if (mCurrCondName == "" or mCurrPortName == "" or mCurrValIndex < 0) {
        return;
    }

    QDomNode portNode = mVpz->portFromDoc(mCurrCondName, mCurrPortName);
    std::vector<std::unique_ptr<value::Value>> values;
    mVpz->fillWithMultipleValue(portNode, values);
    if (values.size() > (unsigned int)mCurrValIndex) {
        std::unique_ptr<vle::value::Value> val =
          std::move(values[mCurrValIndex]);
        switch (val->getType()) {
        case vle::value::Value::BOOLEAN:
        case vle::value::Value::INTEGER:
        case vle::value::Value::DOUBLE:
        case vle::value::Value::STRING:
        case vle::value::Value::USER: {
            QLabel* lab = new QLabel("Nothing to edit");
            ui->vlValue->addWidget(lab);
            lab->show();
            break;
        }
        case vle::value::Value::SET:
        case vle::value::Value::MAP:
        case vle::value::Value::TUPLE:
        case vle::value::Value::TABLE:
        case vle::value::Value::XMLTYPE:
        case vle::value::Value::NIL:
        case vle::value::Value::MATRIX: {
            VleValueWidget* valWidget = new VleValueWidget(this);
            QObject::connect(valWidget,
                             SIGNAL(valUpdated(const vle::value::Value&)),
                             this,
                             SLOT(onValUpdated(const vle::value::Value&)));
            ui->vlValue->addWidget(valWidget);
            valWidget->setValue(std::move(val));
            valWidget->show();
            break;
        }
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

    QMenu* subMenu = menu.addMenu("Add condition");
    action = subMenu->addAction("default");
    action->setData("EMenuCondAdd");
    action->setEnabled(true);

    // check plugins
    QStringList plugList = mGvlePlugins->getCondPluginsList();
    for (int i = 0; i < plugList.size(); i++) {
        action = subMenu->addAction(plugList.at(i) + " (plugin)");
        action->setEnabled(true);
        action->setData(plugList.at(i));
    }

    QString condName = "";
    if (index.row() > -1) {
        condName = getLineEdit(index.row(), 0)->text();
    }

    action = menu.addAction("Remove condition");
    action->setData("EMenuCondRemove");
    action->setEnabled(index.column() == 0);
    action = menu.addAction("Detach plugin");
    action->setData("EMenuDetachPlugin");
    action->setEnabled((index.column() == 0) and
                       (mVpz->getCondGUIplugin(condName) != ""));
    menu.addSeparator();
    action = menu.addAction("Add port");
    action->setData("EMenuPortAdd");
    action->setEnabled((index.column() == 1) and
                       (mVpz->getCondGUIplugin(condName) == ""));
    action = menu.addAction("Remove port");
    action->setData("EMenuPortRemove");
    action->setEnabled((index.column() == 1) and
                       (mVpz->getCondGUIplugin(condName) == ""));
    menu.addSeparator();
    subMenu = buildAddValueMenu(menu, "Add");
    subMenu->setEnabled((index.column() == 1) and
                        (getLineEdit(index.row(), index.column())->text() !=
                         "<Click here to add a port>") and
                        (mVpz->getCondGUIplugin(condName) == ""));
    subMenu = buildAddValueMenu(menu, "Set");
    subMenu->setEnabled(ui->table->item(index.row(), index.column()) and
                        (index.column() > 1) and
                        (hasWidget(index.row(), index.column())));
    action = menu.addAction("Remove value");
    action->setData("EMenuValueRemove");
    action->setEnabled(ui->table->item(index.row(), index.column()) and
                       (index.column() > 1) and
                       (hasWidget(index.row(), index.column())));
    menu.addSeparator();
    action = menu.addAction("Copy");
    action->setData("EMenuCopy");
    action->setEnabled(hasWidget(index.row(), index.column()));
    action = menu.addAction("Paste");
    action->setData("EMenuPaste");
    action->setEnabled(true);

    QAction* act = menu.exec(globalPos);
    if (act) {
        disconnectConds();
        QString actCode = act->data().toString();
        if (actCode == "EMenuCondAdd") {
            mVpz->addConditionToDoc(mVpz->newCondNameToDoc());
            reload(false);
        } else if (actCode == "EMenuCondRemove") {
            QString cond = getLineEdit(index.row(), 0)->text();
            mVpz->rmConditionToDoc(cond);
            reload(false);
        } else if (actCode == "EMenuDetachPlugin") {
            QString cond = getLineEdit(index.row(), 0)->text();
            mVpz->removeCondGUIplugin(cond);
            reload(false);
        } else if (actCode == "EMenuPortAdd") {
            QString cond = getLineEdit(index.row(), 0)->text();
            mVpz->addCondPortToDoc(cond, mVpz->newCondPortNameToDoc(cond));
            reload(false);
        } else if (actCode == "EMenuPortRemove") {
            QString cond = getLineEdit(index.row(), 0)->text();
            QString port = getLineEdit(index.row(), 1)->text();
            mVpz->rmCondPortToDoc(cond, port);
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
            QString cond = getLineEdit(index.row(), 0)->text();
            QString port = getLineEdit(index.row(), 1)->text();
            vle::value::Value* v = buildDefaultValue(actCode);
            if (index.column() == 1) {
                mVpz->addValuePortCondToDoc(cond, port, *v);
            } else {
                mVpz->fillWithValue(cond, port, index.column() - 2, *v);
            }
            delete v;
            reload(false);
        } else if (actCode == "EMenuValueRemove") {
            QString cond = getLineEdit(index.row(), 0)->text();
            QString port = getLineEdit(index.row(), 1)->text();
            mVpz->rmValuePortCondToDoc(cond, port, index.column() - 2);
            reload(false);
        } else if (actCode == "EMenuCopy") {
            QString cond = getLineEdit(index.row(), 0)->text();
            QDomNode condAtom =
              mVpz->condFromConds(mVpz->condsFromDoc(), cond);
            if (index.column() == 0 or
                (index.column() == 1 and
                 (mVpz->getCondGUIplugin(cond) != ""))) {
                mVpz->exportToClipboard(condAtom);
            } else if (index.column() == 1) {
                mVpz->exportToClipboard(mVpz->portFromCond(
                  condAtom, getLineEdit(index.row(), 1)->text()));
            }
        } else if (actCode == "EMenuPaste") {
            if (not hasWidget(index.row(), index.column())) {
                mVpz->importFromClipboard(mVpz->condsFromDoc());
            } else if (index.column() == 0) {
                QString cond = getLineEdit(index.row(), 0)->text();
                QDomNode condAtom =
                  mVpz->condFromConds(mVpz->condsFromDoc(), cond);
                if (mVpz->getCondGUIplugin(cond) != "") {
                    return;
                }
                mVpz->importFromClipboard(condAtom);
            }
            reload(false);
        } else { // add from plugin
            mCurrCondName = mVpz->newCondNameToDoc();
            mVpz->addConditionFromPluginToDoc(mCurrCondName, actCode);
            reload(false);
        }
        connectConds();
    }
}

void FileVpzExpCond::onUndoRedoVpz(QDomNode /*oldValVpz*/,
                                   QDomNode /*newValVpz*/,
                                   QDomNode /*oldValVpm*/,
                                   QDomNode /*newValVpm*/)
{
    bool shouldreset = true;
    if (mVpz->existCondFromDoc(mCurrCondName)) {
        if (mVpz->getCondGUIplugin(mCurrCondName) != "") {
            shouldreset = false;
        } else {
            std::unique_ptr<value::Value> v =
              vleDomStatic::getValueFromPortCond(
                mVpz->condFromConds(mVpz->condsFromDoc(), mCurrCondName),
                mCurrPortName,
                mCurrValIndex);
            if (not v) {
                shouldreset = true;
            } else {
                shouldreset = false;
            }
        }
    } else {
        shouldreset = true;
    }
    if (shouldreset) {
        mCurrCondName = "";
        mCurrPortName = "";
        mCurrValIndex = -1;
    }
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
FileVpzExpCond::onCondUpdated()
{
    showEditPlace();
    reload(false);
}

void
FileVpzExpCond::onTextUpdated(const QString& id,
                              const QString& oldVal,
                              const QString& newVal)
{
    disconnectConds();
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    QString cond = getLineEdit(row, 0)->text();
    if (col == 0) { // edit cond name
        mVpz->renameConditionToDoc(oldVal, cond);
        mCurrCondName = cond;
    } else if (col == 1) { // edit port name
        mCurrCondName = cond;
        if (oldVal == "<Click here to add a port>") {
            mVpz->addCondPortToDoc(cond, newVal);
        } else {
            mVpz->renameCondPortToDoc(cond, oldVal, newVal);
            mCurrPortName = newVal;
        }
    } else { // edit value String
        QString port = getLineEdit(row, 1)->text();
        std::unique_ptr<value::Value> curr =
          mVpz->buildValueFromDoc(cond, port, col - 2);
        curr->toString().set(newVal.toStdString());
        mVpz->fillWithValue(cond, port, col - 2, *curr);
    }
    reload(false);
    connectConds();
}

void
FileVpzExpCond::onIntUpdated(const QString& id, int newVal)
{
    disconnectConds();
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) { // necesserlay a value
        QString cond = getLineEdit(row, 0)->text();
        QString port = getLineEdit(row, 1)->text();
        std::unique_ptr<value::Value> curr =
          mVpz->buildValueFromDoc(cond, port, col - 2);
        curr->toInteger().set(newVal);
        mVpz->fillWithValue(cond, port, col - 2, *curr);
    }
    connectConds();
}

void
FileVpzExpCond::onDoubleUpdated(const QString& id, double newVal)
{
    disconnectConds();
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) { // necesserlay a value
        QString cond = getLineEdit(row, 0)->text();
        QString port = getLineEdit(row, 1)->text();
        std::unique_ptr<value::Value> curr =
          mVpz->buildValueFromDoc(cond, port, col - 2);
        curr->toDouble().set(newVal);
        mVpz->fillWithValue(cond, port, col - 2, *curr);
    }
    connectConds();
}
void
FileVpzExpCond::onBoolUpdated(const QString& id, const QString& newVal)
{
    disconnectConds();
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    int col = split.at(1).toInt();
    if (col >= 2) { // necesserlay a value
        QString cond = getLineEdit(row, 0)->text();
        QString port = getLineEdit(row, 1)->text();
        std::unique_ptr<value::Value> curr =
          mVpz->buildValueFromDoc(cond, port, col - 2);
        curr->toBoolean().set(QVariant(newVal).toBool());
        mVpz->fillWithValue(cond, port, col - 2, *curr);
    }
    connectConds();
}

void
FileVpzExpCond::onValUpdated(const vle::value::Value& newVal)
{
    disconnectConds();
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex, newVal);
    connectConds();
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

    if (col >= 0) {
        mCurrCondName = getLineEdit(row, 0)->text();
        if (col >= 1) {
            mCurrPortName = getLineEdit(row, 1)->text();
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
    QMenu* subMenu = menu.addMenu(setOrAdd + " value");
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
    } else if (type == "EMenuValueAddDouble") {
        return new vle::value::Double();
    } else if (type == "EMenuValueAddString") {
        return new vle::value::String();
    } else if (type == "EMenuValueAddSet") {
        return new vle::value::Set();
    } else if (type == "EMenuValueAddMap") {
        return new vle::value::Map();
    } else if (type == "EMenuValueAddTuple") {
        return new vle::value::Tuple();
    } else if (type == "EMenuValueAddTable") {
        return new vle::value::Table();
    } else if (type == "EMenuValueAddMatrix") {
        return new vle::value::Matrix();
    }
    return 0;
}

void
FileVpzExpCond::insertNullWidget(int row, int col)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleNullWidget* w = new VleNullWidget(ui->table, id);
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem); // used to find it
    QObject::connect(w,
                     SIGNAL(selected(const QString&)),
                     this,
                     SLOT(onSelected(const QString&)));
}

bool
FileVpzExpCond::hasWidget(int row, int col)
{
    QWidget* w = ui->table->cellWidget(row, col);
    if (w == 0) {
        return false;
    }
    VleNullWidget* wn = qobject_cast<VleNullWidget*>(w);
    return (wn == 0);
}

void
FileVpzExpCond::insertTextEdit(int row, int col, const QString& val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleLineEdit* w = new VleLineEdit(ui->table, val, id, false);
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem); // used to find it
    QObject::connect(
      w,
      SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
      this,
      SLOT(onTextUpdated(const QString&, const QString&, const QString&)));
    QObject::connect(w,
                     SIGNAL(selected(const QString&)),
                     this,
                     SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertSpinBox(int row, int col, int val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleSpinBox* w = new VleSpinBox(ui->table, val, id);

    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem); // used to find it
    QObject::connect(w,
                     SIGNAL(valUpdated(const QString&, int)),
                     this,
                     SLOT(onIntUpdated(const QString&, int)));
    QObject::connect(w,
                     SIGNAL(selected(const QString&)),
                     this,
                     SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertDoubleEdit(int row, int col, double val)
{

    QString id = QString("%1,%2").arg(row).arg(col);
    VleDoubleEdit* w = new VleDoubleEdit(ui->table, val, id, false);
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem); // used to find it
    QObject::connect(w,
                     SIGNAL(valUpdated(const QString&, double)),
                     this,
                     SLOT(onDoubleUpdated(const QString&, double)));
    QObject::connect(w,
                     SIGNAL(selected(const QString&)),
                     this,
                     SLOT(onSelected(const QString&)));
}

void
FileVpzExpCond::insertBooleanCombo(int row, int col, bool val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleCombo* w = new VleCombo(ui->table, id);
    QList<QString> l;
    l.append("true");
    l.append("false");
    QString currVal = "false";
    if (val) {
        currVal = "true";
    }
    w->addItems(l);
    w->setCurrentIndex(w->findText(currVal));
    ui->table->setCellWidget(row, col, w);
    ui->table->setItem(row, col, new QTableWidgetItem); // used to find it
    QObject::connect(w,
                     SIGNAL(valUpdated(const QString&, const QString&)),
                     this,
                     SLOT(onBoolUpdated(const QString&, const QString&)));
    QObject::connect(w,
                     SIGNAL(selected(const QString&)),
                     this,
                     SLOT(onSelected(const QString&)));
}

VleLineEdit*
FileVpzExpCond::getLineEdit(int row, int col)
{
    return qobject_cast<VleLineEdit*>(ui->table->cellWidget(row, col));
}

void
FileVpzExpCond::disconnectConds()
{
    if (mVpz) {
        QObject::disconnect(
          mVpz, SIGNAL(conditionsUpdated()), this, SLOT(onCondUpdated()));
    }
}
void
FileVpzExpCond::connectConds()
{
    if (mVpz) {
        disconnectConds();
        QObject::connect(
          mVpz, SIGNAL(conditionsUpdated()), this, SLOT(onCondUpdated()));
    }
}
}
}
