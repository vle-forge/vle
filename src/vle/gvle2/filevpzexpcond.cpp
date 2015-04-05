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
#include <QDebug>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Matrix.hpp>
#include "filevpzexpcond.h"
#include "ui_filevpzexpcond.h"

FileVpzExpCond::FileVpzExpCond(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzExpCond), mVpz(0), mCurrCondName(""),
    mCurrPortName(""), mCurrValIndex(-1), mPlugin(0),
    mValueStack()
{
    ui->setupUi(this);

    ui->valueEdit->setCurrentIndex((int) PageBlank);

//    QTreeWidget *condTree  = ui->expTree;
//    QTreeWidget *paramTree = ui->paramTree;

    ui->expTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->setListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tupleListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
//    paramTree->setContextMenuPolicy(Qt::CustomContextMenu);
//    ui->paramValueList->setContextMenuPolicy(Qt::CustomContextMenu);
//    ui->paramValueTable->setContextMenuPolicy(Qt::CustomContextMenu);

//    BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
//               XMLTYPE, NIL, MATRIX, USER

    QObject::connect(ui->expTree,
            SIGNAL(itemSelectionChanged ()),
            this, SLOT(onConditionTreeSelected()));
    QObject::connect(ui->expTree,
            SIGNAL(itemChanged (QTreeWidgetItem *, int)),
            this, SLOT(onConditionChanged(QTreeWidgetItem *, int)));
    QObject::connect(ui->expTree,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onConditionTreeMenu(const QPoint&)));
    QObject::connect(ui->boolComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(boolEdited(const QString&)));
    QObject::connect(ui->intSpinBox,
            SIGNAL(valueChanged(int)),
            this, SLOT(intEdited(int)));
    QObject::connect(ui->doubleSpinBox,
            SIGNAL(valueChanged(double)),
            this, SLOT(doubleEdited(double)));
    QObject::connect(ui->stringTextEdit,
            SIGNAL(textChanged()),
            this, SLOT(stringEdited()));
    QObject::connect(ui->setListWidget,
            SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(setDoubleClicked(QListWidgetItem*)));
    QObject::connect(ui->setListWidget,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(setMenu(const QPoint&)));
    QObject::connect(ui->mapTreeWidget,
            SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(mapDoubleClicked(QTreeWidgetItem*, int)));
    QObject::connect(ui->mapTreeWidget,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onMapTreeMenu(const QPoint&)));
    QObject::connect(ui->mapTreeWidget,
            SIGNAL(itemChanged (QTreeWidgetItem *, int)),
            this, SLOT(onMapTreeChanged(QTreeWidgetItem *, int)));
    QObject::connect(ui->tupleListWidget,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(tupleMenu(const QPoint&)));
    QObject::connect(ui->tupleListWidget,
            SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(tupleEdited(QListWidgetItem*)));
    QObject::connect(ui->tableWidget,
            SIGNAL(cellChanged(int, int)),
            this, SLOT(tableEdited(int, int)));
    QObject::connect(ui->tableDimButton,
            SIGNAL(clicked(bool)),
            this, SLOT(tableDimButtonClicked(bool)));
    QObject::connect(ui->matrixWidget,
            SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this, SLOT(matrixDoubleClicked(QTableWidgetItem*)));
    QObject::connect(ui->matrixDimButton,
                SIGNAL(clicked(bool)),
                this, SLOT(matrixDimButtonClicked(bool)));

    QObject::connect(ui->upStackButton,
            SIGNAL(clicked(bool)),
            this, SLOT(upStackButonClicked(bool)));
}

FileVpzExpCond::~FileVpzExpCond()
{
    delete ui;
}


void
FileVpzExpCond::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
}

void
FileVpzExpCond::reload()
{
    // Reset current content
    ui->expTree->clear();
    ui->expTree->setColumnCount(1);

    QList<QTreeWidgetItem *> condItems;
    QDomNodeList condList = mVpz->condsListFromConds(mVpz->condsFromDoc());
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpz->attributeValue(cond, "name");

        QTreeWidgetItem *newCondItem = new QTreeWidgetItem();
        newCondItem->setText(0, name);
        newCondItem->setData(0, Qt::UserRole, FileVpzExpCond::ECondCondition);
        newCondItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));

        // Refresh the port (and values) for this experimental condition
        refresh(name, newCondItem);
        condItems.append(newCondItem);
    }
    ui->expTree->insertTopLevelItems(0, condItems);
}


void
FileVpzExpCond::refresh(const QString& condName, QTreeWidgetItem *expItem)
{
    if (expItem == 0)
    {
        QList<QTreeWidgetItem *>listWidgetItem =
            ui->expTree->findItems(condName, Qt::MatchExactly, 0);
        if (listWidgetItem.count() <= 0)
            return;
        expItem = listWidgetItem.at(0);
    }

    // Clear all the child items (ports)
    while (expItem->childCount())
    {
        QTreeWidgetItem *child = expItem->takeChild(0);
        delete child;
    }

    // Get the list of existing ports

    QDomNodeList portList = mVpz->portsListFromDoc(condName);
    // Then for each port, add an item into the tree
    for (unsigned int j = 0; j < portList.length(); j++) {
        QDomNode port = portList.at(j);

        QTreeWidgetItem *newPortItem = new QTreeWidgetItem();
        newPortItem->setText(0, mVpz->attributeValue(port, "name"));
        newPortItem->setData(0, Qt::UserRole+0,
                             FileVpzExpCond::ECondPort);
//        newPortItem->setData(0, Qt::UserRole+1,
//                             QVariant::fromValue((void*)port));
        newPortItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));

        expItem->addChild(newPortItem);

        QDomNodeList valueList = port.childNodes();
        for (unsigned int k = 0; k < valueList.length(); k++) {
            vle::value::Value* val = mVpz->buildValue(valueList.at(k));

            QString sDisp = getValueDisplay(*val, TypeOnly);

            QTreeWidgetItem *newValueItem = new QTreeWidgetItem();
            newValueItem->setText(0, sDisp);
            newValueItem->setData(0, Qt::UserRole, FileVpzExpCond::ECondValue);
            newPortItem->addChild(newValueItem);
        }
    }
}

void
FileVpzExpCond::onConditionTreeMenu(const QPoint pos)
{
    QPoint globalPos = ui->expTree->mapToGlobal(pos);
    QModelIndex index = ui->expTree->indexAt(pos);
    ExpCondTreeType curItemType;
    QTreeWidgetItem* item = ui->expTree->currentItem();
    if ((index.row() == -1) or (index.column() == -1)) {
        curItemType = ECondNone;
    } else if (item) {
        QVariant vItemType = item->data(0, Qt::UserRole);
        curItemType = (ExpCondTreeType)vItemType.toInt();
    } else {
        curItemType = ECondNone;
    }

    QAction* lastAction;
    QMenu myMenu;
    switch (curItemType) {
    case ECondNone: {
        lastAction = myMenu.addAction("Add condition");
        lastAction->setData(EMenuCondAdd);
        break;
    } case ECondCondition: {
        lastAction = myMenu.addAction("Add condition");
        lastAction->setData(EMenuCondAdd);
        lastAction = myMenu.addAction("Rename condition");
        lastAction->setData(EMenuCondRename);
        myMenu.addSeparator();
        lastAction = myMenu.addAction("Add port");
        lastAction->setData(EMenuPortAdd);
        myMenu.addSeparator();
        lastAction = myMenu.addAction(tr("Remove condition"));
        lastAction->setData(EMenuCondRemove);
        break;
    } case ECondPort: {
        lastAction = myMenu.addAction("Add port");
        lastAction->setData(EMenuPortAdd);
        lastAction = myMenu.addAction("Rename port");
        lastAction->setData(EMenuPortRename);
        myMenu.addSeparator();
        buildAddValueMenu(myMenu);
        //missing xml, nil, user
        myMenu.addSeparator();
        lastAction = myMenu.addAction("Remove port");
        lastAction->setData(EMenuPortRemove);
        break;
    } case ECondValue: {
        buildAddValueMenu(myMenu);
        myMenu.addSeparator();
        lastAction = myMenu.addAction("Remove value");
        lastAction->setData(EMenuValueRemove);
        break;
    }}

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == EMenuCondAdd) {
            // Create a new condition into the current VPZ
            QString condName =mVpz->newCondNameToDoc();
            mVpz->addConditionToDoc(condName);
            mCurrCondName = condName;

            // Create a Widget to show this condition into tree
            QTreeWidgetItem *newCondItem = new QTreeWidgetItem();
            newCondItem->setText(0,condName);
            newCondItem->setData(0, Qt::UserRole,
                    FileVpzExpCond::ECondCondition);
            newCondItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));

            // Update widget flags to allow name update
            newCondItem->setFlags(newCondItem->flags() | Qt::ItemIsEditable);
            //ui->expTree->editItem(item, 0);
            ui->expTree->addTopLevelItem(newCondItem);
            ui->expTree->setCurrentItem(newCondItem);
        } else if ((actCode == EMenuCondRename) && (item != 0)) {
            mCurrCondName = item->text(0);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->expTree->editItem(item, 0);
            ui->expTree->setCurrentItem(item);
        } if (actCode == EMenuCondRemove) {
            if (mPlugin)
                condHidePlugin();
            mVpz->rmConditionToDoc(item->text(0));
            // ToDo : test if used by a model ...
            int index = ui->expTree->indexOfTopLevelItem(item);
            ui->expTree->takeTopLevelItem(index);
            delete item;
        } else if (actCode == EMenuPortAdd) {
            mCurrCondName =  item->text(0);
            QTreeWidgetItem* itemToMod = item;
            if (item->parent()) {
                mCurrCondName = item->parent()->text(0);
                itemToMod = item->parent();
            } else {
                item->setExpanded(true);
            }
            mCurrPortName = mVpz->newCondPortNameToDoc(mCurrCondName);
            mVpz->addCondPortToDoc(mCurrCondName, mCurrPortName);

            // Create a Widget to show this port into tree
            QTreeWidgetItem *newPortItem = new QTreeWidgetItem();
            newPortItem->setText(0, mCurrPortName);
            newPortItem->setData(0, Qt::UserRole, FileVpzExpCond::ECondPort);
            newPortItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));
            // Set widget flags to allow name update
            newPortItem->setFlags(newPortItem->flags() | Qt::ItemIsEditable);
            itemToMod->addChild(newPortItem);
            ui->expTree->editItem(newPortItem, 0);
            ui->expTree->setCurrentItem(newPortItem);
        } else if ((actCode == EMenuPortRename) && (item != 0)) {
            mCurrCondName = item->parent()->text(0);
            mCurrPortName = item->text(0);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->expTree->editItem(item, 0);
            ui->expTree->setCurrentItem(item);
        } else if (actCode == EMenuPortRemove) {
            // Get the port associated to the selected widget
            mCurrCondName = item->parent()->text(0);
            mCurrPortName = item->text(0);
            mVpz->rmCondPortToDoc(mCurrCondName, mCurrPortName);
            QTreeWidgetItem *p = item->parent();
            p->removeChild(item);
        } else if ((actCode == EMenuValueAddBoolean) or
                (actCode == EMenuValueAddInteger) or
                (actCode == EMenuValueAddDouble) or
                (actCode == EMenuValueAddString) or
                (actCode == EMenuValueAddSet) or
                (actCode == EMenuValueAddMap) or
                (actCode == EMenuValueAddTuple) or
                (actCode == EMenuValueAddTable) or
                (actCode == EMenuValueAddMatrix)) {
            ExpCondTreeType curItemType =
                    (ExpCondTreeType)item->data(0, Qt::UserRole).toInt();
            QTreeWidgetItem* itemToMod;
            bool ok = false;
            switch (curItemType) {
            case ECondPort: {
                mCurrPortName = item->text(0);
                mCurrCondName = item->parent()->text(0);
                item->setExpanded(true);
                itemToMod = item;
                ok = true;
                break;
            } case ECondValue: {
                mCurrPortName = item->parent()->text(0);
                mCurrCondName = item->parent()->parent()->text(0);
                itemToMod = item->parent();
                ok = true;
                break;
            } default: {
                break;
            }}
            if (ok) {
                vle::value::Value* val = 0;
                if (actCode == EMenuValueAddBoolean) {
                    val = new vle::value::Boolean();
                } else if (actCode == EMenuValueAddInteger) {
                    val = new vle::value::Integer(0);
                }  else if (actCode == EMenuValueAddDouble) {
                    val = new vle::value::Double(0.0);
                } else if (actCode == EMenuValueAddString) {
                    val = new vle::value::String("");
                } else if (actCode == EMenuValueAddSet) {
                    val = new vle::value::Set();
                } else if (actCode == EMenuValueAddMap) {
                    val = new vle::value::Map();
                } else if (actCode == EMenuValueAddTuple) {
                    val = new vle::value::Tuple();
                } else if (actCode == EMenuValueAddTable) {
                    val = new vle::value::Table();
                } else if (actCode == EMenuValueAddMatrix) {
                    val = new vle::value::Matrix();
                }
                mVpz->addValuePortCondToDoc(mCurrCondName, mCurrPortName, *val);
                QTreeWidgetItem* newValueItem = new QTreeWidgetItem();
                newValueItem->setText(0, getValueDisplay(*val,TypeOnly));
                newValueItem->setData(0, Qt::UserRole, FileVpzExpCond::ECondValue);
                itemToMod->addChild(newValueItem);
                delete val;
            }
        }  else if (actCode == EMenuValueRemove) {
            mCurrPortName = item->parent()->text(0);
            mCurrCondName = item->parent()->parent()->text(0);
            mVpz->rmValuePortCondToDoc(mCurrCondName, mCurrPortName,
                    index.row());
            QTreeWidgetItem* p = item->parent();
            p->removeChild(item);
        }
        // Case of Plugins submenu
        else if (actCode >= EMenuPlugins)
        {
//            // Get the condition associated to the selected widget
//            QVariant vCond = item->data(0, Qt::UserRole+1);
//            vpzExpCond *cond = (vpzExpCond *)vCond.value<void *>();
//            // First value is the special "None/remove"
//            if (actCode == EMenuPlugins)
//                // Set an empty name to disable plugin support
//                cond->setPlugin("");
//            else
//            {
//                // Get the plugin
//                int index = actCode - EMenuPlugins - 1;
//                QString pluginName = mVpz->getPackage()->getExpPluginName(index);
//                // Set the selected plugin to the experimental condition
//                cond->setPlugin(pluginName);
//            }
//            onConditionTreeSelected();
        }
    }
}
void
FileVpzExpCond::setMenu(const QPoint& pos)
{
    QPoint globalPos = ui->setListWidget->mapToGlobal(pos);
    QModelIndex index = ui->setListWidget->indexAt(pos);
    QAction* lastAction;
    QMenu myMenu;
    buildAddValueMenu(myMenu);
    if (index.row() != -1) {
        myMenu.addSeparator();
        lastAction = myMenu.addAction(tr("Remove value"));
        lastAction->setData(EMenuValueRemove);
    }
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        //try add value
        vle::value::Value* val = buildDefaultValue((eCondMenuActions) actCode);
        if (val) {
            vle::value::Value* vleval = mValueStack.editingValue();
            vleval->toSet().add(val);
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                    *mValueStack.startValue);
            QListWidgetItem* newItem = new QListWidgetItem();
            newItem->setText(getValueDisplay(*val, Insight));
            newItem->setFlags(newItem->flags() &~ Qt::ItemIsEditable);
            ui->setListWidget->addItem(newItem);
        } else if (actCode == EMenuValueRemove) {
            vle::value::Value* vleval = mValueStack.editingValue();
            delete vleval->toSet().get(index.row());
            vleval->toSet().value().erase(
                    vleval->toSet().begin() + index.row());
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                                *mValueStack.startValue);
            ui->setListWidget->removeItemWidget(
                    ui->setListWidget->item(index.row()));
            condValueShowDetail();//STRANGE no required in other situations
        }
    }
}

void
FileVpzExpCond::onMapTreeMenu(const QPoint pos)
{
    QPoint globalPos = ui->mapTreeWidget->mapToGlobal(pos);
    QModelIndex index = ui->mapTreeWidget->indexAt(pos);
    QAction* lastAction;
    QMenu myMenu;
    buildAddValueMenu(myMenu);
    if ((index.row() != -1) and (index.column() != -1)) {
        myMenu.addSeparator();
        lastAction = myMenu.addAction(tr("Remove value"));
        lastAction->setData(EMenuValueRemove);
    }
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        //try add value
        vle::value::Value* val = buildDefaultValue((eCondMenuActions) actCode);
        if (val) {
            vle::value::Value* vleval = mValueStack.editingValue();
            QString keyName = "NewKey";
            unsigned int keyI = 0;
            bool found =false;
            while (not found) {
                if (keyI > 0) {
                    if (not vleval->toMap().exist(QString("%1_%2")
                            .arg(keyName).arg(keyI).toStdString())) {
                        keyName = QString("%1_%2").arg(keyName).arg(keyI);
                        found = true;
                    }
                } else {
                    if (not vleval->toMap().exist(keyName.toStdString())) {
                        found = true;
                    }
                }
                keyI++;
            }
            vleval->toMap().value().insert(
                    std::make_pair(keyName.toStdString(),val));
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                    *mValueStack.startValue);
            QTreeWidgetItem *newCondItem = new QTreeWidgetItem();
            newCondItem->setText(0, keyName);
            newCondItem->setData(0, Qt::UserRole, keyName);
            newCondItem->setText(1, getValueDisplay(*val, Insight));
            newCondItem->setFlags(newCondItem->flags() | Qt::ItemIsEditable);
            ui->mapTreeWidget->insertTopLevelItem(0, newCondItem);
            ui->mapTreeWidget->editItem(newCondItem, 0);
        } else if (actCode == EMenuValueRemove) {
            QTreeWidgetItem* item = ui->mapTreeWidget->itemAt(pos);
            QString sel = item->data(0, Qt::UserRole).toString();
            vle::value::Value* vleval = mValueStack.editingValue();
            delete vleval->toMap().find(sel.toStdString())->second;
            vleval->toMap().value().erase(sel.toStdString());
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                                *mValueStack.startValue);
            condValueShowDetail();
        }
    }
}

void
FileVpzExpCond::tupleMenu(const QPoint& pos)
{
    QPoint globalPos = ui->tupleListWidget->mapToGlobal(pos);
    QModelIndex index = ui->tupleListWidget->indexAt(pos);
    QAction* lastAction;
    QMenu myMenu;
    lastAction = myMenu.addAction(tr("Add double"));
    lastAction->setData(EMenuValueAddDouble);
    if (index.row() != -1) {
        myMenu.addSeparator();
        lastAction = myMenu.addAction(tr("Remove value"));
        lastAction->setData(EMenuValueRemove);
    } else {

    }
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        vle::value::Value* vleval = mValueStack.editingValue();
        if (actCode == EMenuValueAddDouble) {
            vleval->toTuple().add(0.0);
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                    *mValueStack.startValue);
            QListWidgetItem* newItem = new QListWidgetItem();
            newItem->setText("0.0");
            newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
            ui->tupleListWidget->addItem(newItem);
        } else if (actCode == EMenuValueRemove) {
            vleval->toTuple().value().erase(
                    vleval->toTuple().value().begin() + index.row());
            mVpz->fillWithValue(mCurrCondName, mCurrPortName,mCurrValIndex,
                    *mValueStack.startValue);
            ui->tupleListWidget->removeItemWidget(
                    ui->tupleListWidget->item(index.row()));
            condValueShowDetail();//STRANGE no required in other situations
        }
    }

}

void
FileVpzExpCond::onConditionChanged(QTreeWidgetItem *item, int /*column*/)
{
    QVariant vItemType = item->data(0, Qt::UserRole);
    ExpCondTreeType curItemType = (ExpCondTreeType)vItemType.toInt();
    switch (curItemType) {
    case ECondNone: {
        break;
    } case ECondCondition: {
        if (mCurrCondName != item->text(0)) {
            mVpz->renameConditionToDoc(mCurrCondName, item->text(0));
        }
        break;
    } case ECondPort: {
        if (mCurrPortName != item->text(0)) {
            mVpz->renameCondPortToDoc(mCurrCondName, mCurrPortName,
                    item->text(0));
        }
        break;

    } case ECondValue: {
        break;
    }}
}

void
FileVpzExpCond::onMapTreeChanged(QTreeWidgetItem *item, int /*column*/)
{
    QString oldname = item->data(0, Qt::UserRole).toString();
    QString newname = item->text(0);
    if (oldname != newname) {
        vle::value::Value* vleval = mValueStack.editingValue();
        vle::value::MapValue& val = vleval->toMap().value();
        vle::value::MapValue::iterator it =  val.find(oldname.toStdString());
        vle::value::Value* v = it->second;
        val.erase(it);
        val.insert(std::make_pair(newname.toStdString(), v));
        mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
                *vleval);
        item->setData(0, Qt::UserRole, newname);
    }
    condValueShowDetail();
}

void
FileVpzExpCond::onConditionTreeSelected()
{


    // Get a pointer to the currently selected item into modelTree
    QTreeWidgetItem *item = ui->expTree->currentItem();
    if (item == 0) {
        return ;
    }

    ExpCondTreeType itemType =
            (ExpCondTreeType)item->data(0, Qt::UserRole).toInt();

    switch (itemType) {
    case ECondNone: {
        break;
    } case ECondCondition: {
        mCurrCondName = item->text(0);
        mCurrPortName = "";
        mCurrValIndex = -1;
        mValueStack.delStartValue();
        break;
    } case ECondPort: {
        mCurrCondName = item->parent()->text(0);
        mCurrPortName = item->text(0);
        mCurrValIndex = -1;
        mValueStack.delStartValue();
        break;
    } case ECondValue: {
        mCurrCondName = item->parent()->parent()->text(0);
        mCurrPortName = item->parent()->text(0);
        mCurrValIndex = item->parent()->indexOfChild(item);
        mValueStack.delStartValue();
        mValueStack.setStartValue(mVpz->buildValue(mVpz->portFromDoc(
                mCurrCondName,mCurrPortName).childNodes().at(mCurrValIndex)));
        condValueShowDetail();
        break;
    }}

}


void
FileVpzExpCond::onCondParamTreeSelected()
{
    vle::value::Value* value = 0;

    // Get a pointer to the currently selected item into paramTree
    QTreeWidgetItem* item = 0;

    if (item) {
        //check
        QTreeWidgetItem* itemCond = ui->expTree->currentItem();
        QString condName = itemCond->parent()->text(0);
        QString portName = itemCond->text(0);
        QDomNode portNode = mVpz->portFromDoc(condName, portName);
        QDomNode valueDom = portNode.childNodes().at(0);//TODO index on value index
        value = mVpz->buildValue(valueDom);
    }
    if (value == 0) {
//        ui->editValuesStack->setCurrentIndex(0);
        return;
    }

    condValueShowDetail(/*value*/);
    delete value;
}







void FileVpzExpCond::onCondParamCancel()
{
    onCondParamTreeSelected();
}



void
FileVpzExpCond::condShowPlugin(const QString& /*condName*/)
{
//    QString pluginName = cond->getPlugin();
//    PluginExpCond *plugin = mVpz->getPackage()->getExpPlugin(pluginName);
//
//    if (plugin == 0)
//    {
//        QString msg = QString("Experimental Condition plugin cannot be loaded %1").arg(pluginName);
//        QMessageBox msgBox;
//        msgBox.setText(msg);
//        msgBox.exec();
//        return;
//    }
//
//    if (mPlugin)
//        condHidePlugin();
//    mPlugin = plugin;
//
//    mPlugin->setExpCond(cond);
//    QWidget *w = mPlugin->getWidget();
//
//    // Stay informed of changes made by plugin
//    QObject::connect(w,    SIGNAL(valueChanged(vpzExpCond *)),
//                     this, SLOT(onPluginChanges(vpzExpCond *)));
//
//    int index = ui->stackedWidget->addWidget(w);
//
//    ui->stackedWidget->setCurrentIndex(index);
//
//    // Refresh tree in case of early changes
//    refresh(cond->getName());
}

/**
 * @brief FileVpzExpCond::condHidePlugin
 *        Hide the custom (plugin) widget, and restore default display
 */
void FileVpzExpCond::condHidePlugin()
{
//    QWidget *ow = mPlugin->getWidget();
//    int index = ui->stackedWidget->indexOf(ow);
//    if (index >= 0)
//        ui->stackedWidget->removeWidget(ow);
//    mPlugin->delWidget();
//    // Plugin aloc/desalloc is managed elseware, so we can just forget reference
//    mPlugin = 0;
}


void
FileVpzExpCond::onPluginChanges(const QString& condName)
{
    refresh(condName);
}

void
FileVpzExpCond::boolEdited(const QString& text)
{
    bool b;
    if (text == "true") {
        b = true;
    } else {
        b = false;
    }
    vle::value::Value* val = mValueStack.editingValue();
    val->toBoolean().set(b);
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
}

void
FileVpzExpCond::intEdited(int v)
{
    vle::value::Value* val = mValueStack.editingValue();
    val->toInteger().set(v);
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
}

void
FileVpzExpCond::doubleEdited(double v)
{
    vle::value::Value* val = mValueStack.editingValue();
    val->toDouble().set(v);
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
}
void
FileVpzExpCond::stringEdited()
{
    vle::value::Value* val = mValueStack.editingValue();
    val->toString().set(ui->stringTextEdit->toPlainText().toStdString());
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
}

void
FileVpzExpCond::tupleEdited(QListWidgetItem* item)
{

    vle::value::Tuple& valEdit = mValueStack.editingValue()->toTuple();
    int row = ui->tupleListWidget->row(item);
    QVariant qv = QVariant(item->text());
    bool ok = false;
    double val = qv.toDouble(&ok);
    double& inVal = valEdit.at(row);
    if (ok) {
        inVal = val;
        mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
                *mValueStack.startValue);
    } else {
        item->setText(QVariant(inVal).toString());
    }
}

void
FileVpzExpCond::tableEdited(int row, int col)
{
    vle::value::Value* val = mValueStack.editingValue();
    double& inValue = val->toTable().get(col, row);
    QTableWidgetItem* cell = ui->tableWidget->item(row, col);
    bool ok = false;
    double cellValue = QVariant(cell->text()).toDouble(&ok);
    if (not ok) {
        cell->setText(QVariant(inValue).toString());
    } else {
        if (inValue != cellValue) {
            inValue = cellValue;
            mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
                    *mValueStack.startValue);
        }
    }
}

void
FileVpzExpCond::tableDimButtonClicked(bool /*b*/)
{
    vle::value::Value* val = mValueStack.editingValue();
    val->toTable().resize(ui->tableColsSpin->value(),
            ui->tableRowsSpin->value());
    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
    condValueShowDetail();
}

void
FileVpzExpCond::matrixDoubleClicked(QTableWidgetItem* item)
{
    mValueStack.push(item->row(), item->column());
    condValueShowDetail();
}

void
FileVpzExpCond::matrixDimButtonClicked(bool /*b*/)
{
    vle::value::Matrix& val = mValueStack.editingValue()->toMatrix();
    int rowsInGui = ui->matrixRowsSpin->value();
    int colsInGui = ui->matrixColsSpin->value();
    val.setResizeColumn(1);
    val.setResizeRow(1);
    for (int i=0; i < ((int) rowsInGui - (int) val.rows()); i++) {
        val.addRow();
        for (unsigned int j =0; j< val.columns(); j++) {
            val.set(j, val.rows()-1, new vle::value::Double(0.0));
        }
    }
    for (int i=0; i < ((int) colsInGui - (int) val.columns()); i++) {
        val.addColumn();
        for (unsigned int j =0; j< val.rows(); j++) {
            val.set(val.columns()-1, j, new vle::value::Double(0.0));
        }
    }

    val.resize(colsInGui, rowsInGui);

    mVpz->fillWithValue(mCurrCondName, mCurrPortName, mCurrValIndex,
            *mValueStack.startValue);
    condValueShowDetail();
}

void
FileVpzExpCond::mapDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (column == 0) {
        ui->mapTreeWidget->editItem(item, 0);
    } else {
        QString keyName = item->text(0);
        mValueStack.push(keyName.toStdString());
        condValueShowDetail();
    }
}

void
FileVpzExpCond::setDoubleClicked(QListWidgetItem* item)
{
    mValueStack.push(ui->setListWidget->row(item));
    condValueShowDetail();
}

void
FileVpzExpCond::upStackButonClicked(bool /*b*/)
{
    mValueStack.stack.pop_back();
    condValueShowDetail();
}

void
FileVpzExpCond::buildAddValueMenu(QMenu& menu)
{
    QAction* lastAction;
    QMenu* subMenu  = menu.addMenu("Add value");
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
}

vle::value::Value*
FileVpzExpCond::buildDefaultValue(eCondMenuActions actCode)
{
    vle::value::Value* val = 0;
    if (actCode == EMenuValueAddBoolean) {
        val = new vle::value::Boolean();
    } else if (actCode == EMenuValueAddInteger) {
        val = new vle::value::Integer(0);
    }  else if (actCode == EMenuValueAddDouble) {
        val = new vle::value::Double(0.0);
    } else if (actCode == EMenuValueAddString) {
        val = new vle::value::String("");
    } else if (actCode == EMenuValueAddSet) {
        val = new vle::value::Set();
    } else if (actCode == EMenuValueAddMap) {
        val = new vle::value::Map();
    } else if (actCode == EMenuValueAddTuple) {
        val = new vle::value::Tuple();
    } else if (actCode == EMenuValueAddTable) {
        val = new vle::value::Table();
    } else if (actCode == EMenuValueAddMatrix) {
        val = new vle::value::Matrix();
    } else {
        val = 0;
    }
    return val;
}

/**
 * @brief FileVpzExpCond::condUpdateTree
 *        Parse a vpzExpCondValue list and make a QTreeWidgetItem list :)
 *
 */
void
FileVpzExpCond::condUpdateTree(const std::vector<vle::value::Value*>& values,
        QList<QTreeWidgetItem *> *widList)
{
    // Read each value from the source list
    for (unsigned int i = 0; i < values.size(); i++) {
        const vle::value::Value* val = values[i];

        QTreeWidgetItem *newItem = new QTreeWidgetItem();
        newItem->setText(0, getValueDisplay(*val, TypeOnly));
        newItem->setData(0, Qt::UserRole, FileVpzExpCond::ECondValue);

        switch (val->getType()) {
        case vle::value::Value::BOOLEAN: {
            if (val->toBoolean().value()) {
                newItem->setText(1, "true");
            } else {
                newItem->setText(1, "false");
            }
            break;
        } case vle::value::Value::INTEGER: {
            newItem->setText(1, QString("%1").arg(val->toInteger().value()));
            break;
        } case vle::value::Value::DOUBLE: {
            newItem->setText(1, QString("%1").arg(val->toDouble().value()));
            break;
        }  case vle::value::Value::STRING: {
            newItem->setText(1, QString("%1").arg(
                    val->toString().value().c_str()));
            break;
        }  case vle::value::Value::SET: {
            QList<QTreeWidgetItem *> setTreeItems;
            condUpdateTree(val->toSet().value(), &setTreeItems);
            newItem->addChildren(setTreeItems);
            break;
        }  case vle::value::Value::MAP: {
            QList<QTreeWidgetItem *> mapTreeItems;
            vle::value::Map::const_iterator itb = val->toMap().begin();
            vle::value::Map::const_iterator ite = val->toMap().end();
            std::vector<vle::value::Value*> values_in;
            for (; itb != ite; itb++) {
                values_in.push_back(itb->second);
            }
            condUpdateTree(values_in, &mapTreeItems);
            newItem->addChildren(mapTreeItems);
            break;
        } default: {
            qDebug(" not yet implemented condUpdateTree ");
            break;
        }}
        widList->append(newItem);
    }
}

void
FileVpzExpCond::condValueShowDetail()
{
    if (mCurrValIndex == -1) {
        ui->valueEdit->setCurrentIndex((int) PageBlank);
        ui->labelValue->setText("No value edition");
        ui->labelValueStack->setText("");
        ui->upStackButton->setEnabled(false);
        return;
    }
    if (mValueStack.stack.empty()) {
        ui->upStackButton->setEnabled(false);
    } else {
        ui->upStackButton->setEnabled(true);
    }
    ui->labelValueStack->setText(mValueStack.toString().c_str());
    const vle::value::Value* editingValue = mValueStack.editingValue();

    ui->labelValue->setText(QString("Editing %1.%2 (index %3)")
            .arg(mCurrCondName).arg(mCurrPortName).arg(mCurrValIndex));
    switch(editingValue->getType()) {
    case vle::value::Value::BOOLEAN: {
        ui->valueEdit->setCurrentIndex((int) PageBoolean);
        ui->boolComboBox->setCurrentIndex(
                (int) editingValue->toBoolean().value());
        break;
    } case vle::value::Value::INTEGER: {
        ui->valueEdit->setCurrentIndex((int) PageInteger);
        ui->intSpinBox->setValue(editingValue->toInteger().value());
        break;
    } case vle::value::Value::DOUBLE: {
        ui->valueEdit->setCurrentIndex((int) PageDouble);
        ui->doubleSpinBox->setValue(editingValue->toDouble().value());
        break;
    } case vle::value::Value::STRING: {
        ui->valueEdit->setCurrentIndex((int) PageString);
        ui->stringTextEdit->setPlainText(
                editingValue->toString().value().c_str());
        break;
    } case vle::value::Value::SET: {
        ui->valueEdit->setCurrentIndex((int) PageSet);
        ui->setListWidget->clear();
        vle::value::Set::const_iterator itb = editingValue->toSet().begin();
        vle::value::Set::const_iterator ite = editingValue->toSet().end();
        for (; itb !=ite; itb++) {
            QListWidgetItem* newItem = new QListWidgetItem();
            newItem->setText(getValueDisplay(**itb, Insight));
            ui->setListWidget->addItem(newItem);
        }
        break;
    } case vle::value::Value::MAP: {
        ui->valueEdit->setCurrentIndex((int) PageMap);
        ui->mapTreeWidget->clear();
        vle::value::Map::const_iterator itb = editingValue->toMap().begin();
        vle::value::Map::const_iterator ite = editingValue->toMap().end();
        for (; itb !=ite; itb++) {
            QTreeWidgetItem *newItem = new QTreeWidgetItem();
            newItem->setText(0, itb->first.c_str());
            newItem->setText(1, getValueDisplay(*itb->second, Insight));
            newItem->setData(0, Qt::UserRole, itb->first.c_str());
            newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
            ui->mapTreeWidget->insertTopLevelItem(0, newItem);
        }
        break;
    } case vle::value::Value::TUPLE: {
        ui->valueEdit->setCurrentIndex((int) PageTuple);
        ui->tupleListWidget->clear();
        vle::value::Tuple::const_iterator itb =
                editingValue->toTuple().value().begin();
        vle::value::Tuple::const_iterator ite =
                editingValue->toTuple().value().end();
        for (; itb !=ite; itb++) {
            QListWidgetItem* newItem = new QListWidgetItem();
            newItem->setText(QVariant(*itb).toString());
            newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
            ui->tupleListWidget->addItem(newItem);
        }
        break;
    } case vle::value::Value::TABLE: {
        ui->valueEdit->setCurrentIndex((int) PageTable);
        const vle::value::Table& table = editingValue->toTable();
        ui->tableWidget->setColumnCount(table.width());
        ui->tableWidget->setRowCount(table.height());
        for (unsigned int row=0; row< table.height(); row++) {
            for (unsigned int col=0; col< table.width(); col++) {
                QTableWidgetItem* newItem =
                        new QTableWidgetItem(tr("%1").arg(table.get(col,row)));
                //WARNING inversed above
                ui->tableWidget->setItem(row, col, newItem);
            }
        }
        ui->tableRowsSpin->setValue(table.height());
        ui->tableColsSpin->setValue(table.width());
        break;
    } case vle::value::Value::MATRIX: {
        ui->valueEdit->setCurrentIndex((int) PageMatrix);
        const vle::value::Matrix& matrix = editingValue->toMatrix();
        ui->matrixWidget->setColumnCount(matrix.columns());
        ui->matrixWidget->setRowCount(matrix.rows());
        for (unsigned int row=0; row< matrix.rows(); row++) {
            for (unsigned int col=0; col< matrix.columns(); col++) {
                const vle::value::Value* val = matrix.get(col, row);
                //WARNING inversed above
                QTableWidgetItem* newItem =
                        new QTableWidgetItem(getValueDisplay(*val, Insight));
                newItem->setFlags(newItem->flags() &~ Qt::ItemIsEditable);
                ui->matrixWidget->setItem(row, col, newItem);
            }
        }
        ui->matrixRowsSpin->setValue(matrix.rows());
        ui->matrixColsSpin->setValue(matrix.columns());
        break;
    } default: {
        break;
    }}
}

QString
FileVpzExpCond::getValueDisplay(const vle::value::Value& v,
        ValueDisplayType displayType) const
{

    switch (v.getType()) {
    case vle::value::Value::BOOLEAN: {
        if (displayType == TypeOnly) {
            return "boolean";
        }
        return QString("%1 (%2)")
                .arg(v.toBoolean().value())
                .arg("bool");
        break;
    } case vle::value::Value::INTEGER: {
        if (displayType == TypeOnly) {
            return "integer";
        }
        return QString("%1 (%2)")
                .arg(v.toInteger().value())
                .arg("int");
        break;
    } case vle::value::Value::DOUBLE: {
        if (displayType == TypeOnly) {
            return "double";
        }
        return QString("%1 (%2)")
                .arg(v.toDouble().value())
                .arg("double");
        break;
    } case vle::value::Value::STRING: {
        if (displayType == TypeOnly) {
            return "string";
        }
        return QString("%1 (%2)")
                .arg(v.toString().value().c_str())
                .arg("string");
        break;
    } case vle::value::Value::SET: {
        if (displayType == TypeOnly) {
            return "set";
        }
        return QString("set (%1)")
                .arg(v.toSet().size());
        break;
    } case vle::value::Value::MAP: {
        if (displayType == TypeOnly) {
            return "map";
        }
        return QString("map (%1)")
                .arg(v.toMap().size());
        break;
    } case vle::value::Value::TUPLE: {
        if (displayType == TypeOnly) {
            return "tuple";
        }
        return QString("tuple (%1)")
                .arg(v.toTuple().size());
        break;
    } case vle::value::Value::TABLE: {
        if (displayType == TypeOnly) {
            return "table";
        }
        return QString("table (%1, %2)")
                .arg(v.toTable().height())
                .arg(v.toTable().width());
        break;
    } case vle::value::Value::XMLTYPE: {
        if (displayType == TypeOnly) {
            return "xml";
        }
        return QString("xml");
        break;
    } case vle::value::Value::NIL: {
        if (displayType == TypeOnly) {
            return "null";
        }
        return QString("nil");
        break;
    } case vle::value::Value::MATRIX: {
        if (displayType == TypeOnly) {
            return "matrix";
        }
        return QString("matrix (%1, %2)")
                .arg(v.toMatrix().rows())
                .arg(v.toMatrix().columns());
        break;
    } case vle::value::Value::USER: {
        if (displayType == TypeOnly) {
            return "user";
        }
        return QString("user");
        break;
    }}
    return "error";
}

FileVpzExpCond::value_stack::value_stack() :
        startValue(0), stack()
{

}
FileVpzExpCond::value_stack::~value_stack()
{
    delete startValue;
    cont::iterator itb = stack.begin();
    cont::iterator ite = stack.end();
    for (; itb != ite; itb++) {
        delete (*itb);
    }
}

void
FileVpzExpCond::value_stack::delStartValue()
{
    delete startValue;
    startValue = 0;
    stack.clear();
}

void
FileVpzExpCond::value_stack::setStartValue(vle::value::Value* val)
{
    delStartValue();
    startValue = val;

}

void
FileVpzExpCond::value_stack::push(const std::string& key)
{
    stack.push_back(new vle::value::String(key));
}
void
FileVpzExpCond::value_stack::push(int index)
{
    stack.push_back(new vle::value::Integer(index));
}

void
FileVpzExpCond::value_stack::push(int row, int col)
{
    vle::value::Tuple* val = new vle::value::Tuple(0);
    val->add((double) row);
    val->add((double) col);
    stack.push_back(val);
}

vle::value::Value*
FileVpzExpCond::value_stack::editingValue()
{
    vle::value::Value* res = startValue;
    cont::const_iterator itb = stack.begin();
    cont::const_iterator ite = stack.end();
    for (; itb != ite; itb++) {
        switch ((*itb)->getType()) {
        case vle::value::Value::INTEGER: { //for id a Set elem
            res = res->toSet().value().at((*itb)->toInteger().value());
            break;
        } case vle::value::Value::STRING: {//for id a Map elem
            res = res->toMap().value().at((*itb)->toString().value());
            break;
        } case vle::value::Value::TUPLE: {//for id a Matrix elem
            const vle::value::Tuple& tuple = (*itb)->toTuple();
            res = res->toMatrix().get((int) tuple.at(1), (int) tuple.at(0));
            //WARNING inversed above
            break;
        } default: {
            qDebug() << " value_stack::editingValue not yet implemented";
            break;
        }}
    }
    return res;
}

std::string
FileVpzExpCond::value_stack::toString()
{
    std::string res;
    cont::const_iterator itb = stack.begin();
    cont::const_iterator ite = stack.end();
    for (; itb != ite; itb++) {
        if (itb != stack.begin()) {
            res += "/";
        }
        switch ((*itb)->getType()) {
        case vle::value::Value::INTEGER: {
            res += QVariant((*itb)->toInteger().value()).toString().toStdString();
            break;
        } case vle::value::Value::STRING: {
            res += (*itb)->toString().value();
            break;
        } case vle::value::Value::TUPLE: {
            const vle::value::Tuple& tuple = (*itb)->toTuple();
            res += "(";
            res += QVariant((int) tuple.at(1)).toString().toStdString();
            res += ",";
            res += QVariant((int) tuple.at(0)).toString().toStdString();
            res += ")";
            //WARNING inversed above
            break;
        } default: {
            qDebug() << " value_stack::toString not yet implemented";
            break;
        }}
    }
    return res;
}
