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

#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>
#include "filevpzview.h"
#include "ui_filevpzview.h"

/**
 * @brief fileVpzView::fileVpzView
 *        Default constructor for VPZ tab
 *
 */
fileVpzView::fileVpzView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileVpzView),
    uiTool(new Ui::fileVpzRtool)
{
    ui->setupUi(this);

    mUndoStack = new QUndoStack();

    // createundoview
    undoView = new QUndoView(mUndoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);

    mVpz = 0;
    mUseSim = false;
    mDynamicsTab = 0;
    mExpCondTab = 0;
    mObservablesTab = 0;
    mProjectTab = 0;

    // Init the right column toolbox UI
    mWidgetTool = new QWidget();
    uiTool->setupUi(mWidgetTool);
    //uiTool->widSimStyle->setVisible(false);
    mScene.setBackgroundBrush(Qt::white);

    QTreeWidget *viewTree = uiTool->modelTree;
    QObject::connect(viewTree, SIGNAL(itemSelectionChanged ()),
                     this,     SLOT  (onTreeModelSelected()));
    // Set a custom context menu on model tree
    viewTree->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(viewTree, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,     SLOT  (onViewTreeMenu(const QPoint&)));

    // Configure table of models properties
    QTableWidget *tProp;
    tProp = uiTool->modelProperty;
    tProp->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    tProp->horizontalHeader()->setStretchLastSection(true);
    QObject::connect(tProp, SIGNAL(itemChanged(QTableWidgetItem *)),
                     this,  SLOT  (onPropertyChanged(QTableWidgetItem *)));
    QObject::connect(tProp, SIGNAL(currentCellChanged(int, int, int, int)),
                     this,  SLOT  (onPropertySelected(int, int, int, int)));

    // Configure Experimental Conditions tab
    mExpCondTab = new FileVpzExpCond();
    int expTabId = ui->tabWidget->addTab(mExpCondTab, tr("Conditions"));

    // Configure Dynamics tab
    mDynamicsTab = new FileVpzDynamics();
    int dynTabId = ui->tabWidget->addTab(mDynamicsTab, tr("Dynamics"));

    // Configure Observables tab
    mObservablesTab = new FileVpzObservables();
    int observablesTabId = ui->tabWidget->addTab(mObservablesTab, tr("Observables"));


    // Configure Project tab
    mProjectTab = new FileVpzProject();
    int projectTabId = ui->tabWidget->addTab(mProjectTab, tr("Project"));
    mProjectTab->setTabId(projectTabId);
    mProjectTab->setTab(ui->tabWidget);

    // Configure View tab
    mExpViewTab = new FileVpzExpView();
    int viewTabId = ui->tabWidget->addTab(mExpViewTab, tr("Views"));

    ui->tabWidget->setTabsClosable(true);
    QTabBar *tabBar = ui->tabWidget->findChild<QTabBar *>();
    tabBar->setTabButton(0, QTabBar::RightSide, 0);
    tabBar->setTabButton(1, QTabBar::RightSide, 0);
    tabBar->setTabButton(2, QTabBar::RightSide, 0);
    tabBar->setTabButton(3, QTabBar::RightSide, 0);
    tabBar->setTabButton(dynTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(expTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(viewTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(projectTabId, QTabBar::RightSide, 0);
    QObject::connect(ui->tabWidget,   SIGNAL(tabCloseRequested(int)),
                     this,            SLOT  (onTabClose(int)));
}

/**
 * @brief fileVpzView::~fileVpzView
 *        Default destructor
 *
 */
fileVpzView::~fileVpzView()
{
    if (mDynamicsTab)
        delete mDynamicsTab;

    if (mExpCondTab)
        delete mExpCondTab;

    if (mObservablesTab)
        delete mObservablesTab;

    if (mProjectTab)
        delete mProjectTab;

    delete ui;
}

/**
 * @brief fileVpzView::setVpz
 *        Associate the tab with an allocated VPZ object
 *
 */
void fileVpzView::setVpz(vleVpz *vpz)
{
    mVpz = vpz;

    QTreeWidget *viewTree = uiTool->modelTree;
    viewTree->clear();
    viewTree->setColumnCount(1);

    // Get the root model
    vleVpzModel *model = mVpz->mRootModel;
    mCurrentModel = model;

    // Insert root model into tree widget
    treeInsertModel(model, 0);
    viewTree->insertTopLevelItems(0, mViewsItems);

#ifdef USE_GRAPHICVIEW
    QGraphicsProxyWidget *pWidget;
    QListIterator<vleVpzModel*> subItems(*model->getSubmodels());
    while( subItems.hasNext() )
    {
        vleVpzModel *modelItem = subItems.next();
        pWidget = mScene.addWidget(modelItem, Qt::Widget);
    }

    QListIterator<vleVpzConn*> connItems(*model->getConnections());
    while( connItems.hasNext() )
    {
        vleVpzConn *conn = connItems.next();
        conn->route();
        mScene.addPolygon(conn->mLines);
    }
    ui->graphicView->setScene(&mScene);
#else
    ui->stackedWidget->setCurrentIndex(1);
    // Show the root model into central diag display
    model->setParent(ui->diagContent);
    model->dispMaximize();
    QVBoxLayout *diagLayout = (QVBoxLayout *)ui->diagContent->layout();
    diagLayout->addWidget(model);
    diagLayout->addStretch();

    QListIterator<vleVpzModel*> items(*model->getSubmodels());
    while( items.hasNext() )
    {
        vleVpzModel *submodel = items.next();
        submodel->setParent(model);
        submodel->show();
    }
#endif

    QObject::connect(mVpz, SIGNAL(sigFocus(vleVpzModel*)),
                     this, SLOT(onFocusChanged(vleVpzModel*)));
    QObject::connect(mVpz, SIGNAL(sigEnterModel(vleVpzModel*)),
                     this, SLOT(onModelDblClick(vleVpzModel*)));

//    QObject::connect(mVpz, SIGNAL(sigDynamicsChanged()),
//                     this, SLOT( /* ToDo */ ));
    QObject::connect(mVpz, SIGNAL(sigOpenModeler(vleVpzModel*)),
                     this, SLOT(onAddModelerTab(vleVpzModel*)));

    // ---- Dynamics Tab ----
    if (mDynamicsTab) {
        mDynamicsTab->setVpz(mVpz);
        mDynamicsTab->setUndo(mUndoStack);
        mDynamicsTab->reload();
    }

    // ---- Experimental Conditions Tab ----
    if (mExpCondTab) {
        mExpCondTab->setVpz(mVpz);
        mExpCondTab->reload();
    }

    // ---- Experimental Conditions Tab ----
    if (mObservablesTab) {
        mObservablesTab->setVpz(mVpz);
    }

    // ---- Experimental Conditions Tab ----
    if (mProjectTab) {
        mProjectTab->setVpz(mVpz);
        mProjectTab->setUndo(mUndoStack);
        mProjectTab->reload();
    }

    // ---- View Tab ----
    if (mExpViewTab) {
        mExpViewTab->setVpz(mVpz);
        mExpViewTab->reload();
    }
}

void fileVpzView::save()
{
    if ( ! mVpz)
        return;

    mVpz->save();
}

/**
 * @brief fileVpzView::vpz
 *        Readback the VPZ currently used
 *
 */
vleVpz* fileVpzView::vpz()
{
    //QString fileName = mVpz->getFilename();
    //mVleLibVpz = new vle::vpz::Vpz(fileName.toStdString());

    return mVpz;
}

void fileVpzView::usedBySim(bool isUsed)
{
    mUseSim = isUsed;
}

bool fileVpzView::isUsed(int *reason = 0)
{
    int flag = 0;
    if (mUseSim)
        flag = 1;
    else if (mVpz)
    {
        flag = 2;
    }

    if (reason)
        *reason = flag;

    return (flag != 0);
}

void fileVpzView::diagSelectModel(vleVpzModel *base, bool force)
{
    // If the current item has no parent ... it is the root item
    if (base == 0)
        base = mVpz->mRootModel;

    if ((base != mCurrentModel) || force)
    {
        // The the model to maximize is currently selected, deselect it
        if (base->isSelected())
            base->deselect();

        // Get all models inside current base model
        QList<QWidget *> subModels = mCurrentModel->findChildren<QWidget *>();
        QListIterator<QWidget*> items(subModels);
        while( items.hasNext() )
            // Hide them
            items.next()->hide();
        // Hide the root model
        mCurrentModel->hide();
        // Restore default display params
        mCurrentModel->dispNormal();
        // Unlink
        mCurrentModel->setParent(0);

        // Clean the scrollable area before inserting new root model
        QVBoxLayout *diagLayout = (QVBoxLayout *)ui->diagContent->layout();
        QLayoutItem *child;
        while(diagLayout->count())
        {
            child = diagLayout->takeAt(0);
            delete child;
        }
        // Configure the new base model to be child of scrollable area
        base->setParent(ui->diagContent);
        base->dispMaximize();
        base->show();
        // Insert the new base model into layout
        diagLayout->addWidget(base);
        diagLayout->addStretch();

        QListIterator<vleVpzModel*> subItems(*base->getSubmodels());
        while( subItems.hasNext() )
        {
            vleVpzModel *submodel = subItems.next();
            submodel->setParent(base);
            submodel->show();
        }

        // Save the new root model displayed
        mCurrentModel = base;
    }
}

void fileVpzView::treeInsertModel(vleVpzModel *model, QTreeWidgetItem *base)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    newItem->setText(0, model->getName());
    newItem->setData(0, Qt::UserRole+0, "Model");
    newItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)model));
    newItem->setData(0, Qt::UserRole+2, QVariant::fromValue((void*)base));

    // If a root widget has been defined
    if (base)
        // Insert the new item as a child of it
        base->addChild(newItem);
    else
        // Else, insert into the root items list
        mViewsItems.append(newItem);

    QListIterator<vleVpzModel*> items(*model->getSubmodels());
    while( items.hasNext() )
    {
        vleVpzModel *submodel = items.next();
        treeInsertModel(submodel, newItem);
    }
}

void fileVpzView::treeUpdateModel(vleVpzModel *model, QString oldName, QString newName)
{
    QTreeWidget *viewTree = uiTool->modelTree;

    QList<QTreeWidgetItem *>schItems = viewTree->findItems(oldName, Qt::MatchRecursive);

    qDebug() << "treeUpdateModel() list (" << oldName << ") count " << schItems.length();
    QListIterator<QTreeWidgetItem*> items(schItems);
    while (items.hasNext())
    {
        QTreeWidgetItem *item = items.next();
        QVariant vModel = item->data(0, Qt::UserRole+1);
        if (vModel.isValid())
        {
            vleVpzModel *itemModel;
            itemModel = (vleVpzModel *)vModel.value<void *>();
            if (itemModel != model)
                continue;
            qDebug() << "Found !";
            item->setText(0, newName);
        }
    }
}

QWidget *fileVpzView::getTool()
{
    return mWidgetTool;
}

/**
 * @brief fileVpzView::onTreeModelSelected (slot)
 *        Called when the selection has changed into modelTree widget
 *
 */
void fileVpzView::onTreeModelSelected()
{
    vleVpzModel *base = 0;
    // Get a pointer to the currently selected item into modelTree
    QTreeWidgetItem *item = uiTool->modelTree->currentItem();

    if (item == 0)
        return;

    // 1) Check if the selected model is into the currently displayed hierarchy level

    // Get the parent item from saved datas
    QVariant vBaseItem = item->data(0, Qt::UserRole+2);
    if (vBaseItem.isValid())
    {
        QTreeWidgetItem *baseItem;
        baseItem = (QTreeWidgetItem *)vBaseItem.value<void *>();
        if (baseItem)
        {
            QVariant vBase = baseItem->data(0, Qt::UserRole+1);
            if (vBase.isValid())
                base = (vleVpzModel *)vBase.value<void *>();
        }
    }
    // If no parent item found, current selection can be the root model
    if (base == 0)
    {
        vleVpzModel *selModel = 0;
        // Load the vleVpzModel associated whith the selected item
        QVariant vModel = item->data(0, Qt::UserRole+1);
        if (vModel.isValid())
            selModel = (vleVpzModel *)vModel.value<void *>();
        // If the model is valid, set the focus on it
        if (selModel)
        {
            if (selModel != mCurrentModel)
                diagSelectModel(base);
            selModel->deselectSub();
            onFocusChanged(selModel);
        }
        return;
    }

    // If the base model is already maximized, nothing to do
    if ( ! base->isMaximized())
        diagSelectModel(base);

    // 2) Set focus on the selected model

    vleVpzModel *model;
    QVariant vType = item->data(0, Qt::UserRole+1);
    if (vType.isValid())
    {
        model = (vleVpzModel *)vType.value<void *>();
        if (mCurrentModel)
            mCurrentModel->deselectSub();
        model->select(true);
    }
    else
    {
        if (mCurrentModel)
            mCurrentModel->deselectSub();
    }
}

/**
 * @brief fileVpzView::onViewTreeMenu (slot)
 *        Called on right-click into the model tree to show contextual menu
 *
 */
void fileVpzView::onViewTreeMenu(const QPoint pos)
{
    vleVpzModel *model = 0;

    // Get the global mouse position
    QPoint globalPos = uiTool->modelTree->mapToGlobal(pos);

    // Get a pointer to the currently selected item into modelTree
    QTreeWidgetItem *item = uiTool->modelTree->currentItem();
    if (item)
    {
        // Get the associated model
        QVariant vModel = item->data(0, Qt::UserRole+1);
        if (vModel.isValid())
            model = (vleVpzModel *)vModel.value<void *>();
    }

    QAction *lastAction;

    QMenu ctxMenu;
    lastAction = ctxMenu.addAction(tr("Add model"));    lastAction->setData(1);
    lastAction = ctxMenu.addAction(tr("Remove model")); lastAction->setData(2);
    // Menu entry "remove" can't remove root model
    if ( (model == 0) || (model == mVpz->mRootModel) )
        lastAction->setEnabled(false);
    lastAction = ctxMenu.addAction(tr("Copy model"));   lastAction->setData(3);
    if (model == 0)
        lastAction->setEnabled(false);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem)
    {
        int actCode = selectedItem->data().toInt();
        if (actCode == 1)
        {
            vleVpzModel *newModel = new vleVpzModel(mVpz);
            newModel->setName(tr("New Model"));
            newModel->dispNormal();
            model->addSubmodel(newModel);
            treeInsertModel(newModel, item);
            diagSelectModel(mCurrentModel, true);
        }
        if (actCode == 2)
        {
            QTreeWidgetItem *baseItem;
            vleVpzModel     *baseModel = 0;
            // Get the parent item
            QVariant vBaseItem = item->data(0, Qt::UserRole+2);
            baseItem = (QTreeWidgetItem *)vBaseItem.value<void *>();
            if (baseItem)
            {
                QVariant vBaseModel = baseItem->data(0, Qt::UserRole+1);
                if (vBaseModel.isValid())
                    baseModel = (vleVpzModel *)vBaseModel.value<void *>();
            }
            if (baseModel)
            {
                baseModel->delSubmodel(model);
                // De-select current item before removing it (to avoid automatic selection of next item)
                uiTool->modelTree->setCurrentItem(baseItem);
                // Remove the selected item from his parent
                baseItem->removeChild(item);
                // Clean memory
                delete model;
                // Refresh diagram view
                mCurrentModel->repaint();
            }
        }
        if (actCode == 3)
        {
            QString newName = model->getName();
            newName += tr("_Copy");

            QWidget *w = model->parentWidget();
            vleVpzModel *parentModel = qobject_cast<vleVpzModel *>(w);
            if (parentModel)
            {
                vleVpzModel *newModel = new vleVpzModel(mVpz);
                newModel->setName(newName);
                newModel->copyFromModel(model);
                newModel->dispNormal();
                parentModel->addSubmodel(newModel);
                treeInsertModel(newModel, item->parent());
                diagSelectModel(mCurrentModel, true);
                //Todo: a modeler does not edit directly a condition: to see when a plugin available
//                if (model->hasModeler())
//                {
//                    // Get informations about the Experimental Condition of the source model
//                    vpzExpCond * srcExp = model->getModelerExpCond();
//                    QString srcExpName = srcExp->getName();
//                    QString srcClass = srcExpName.left(srcExpName.length() - model->getName().length());
//                    // Create a new Experimental Condition
//                    QString newExpName = srcClass;
//                    newExpName += newModel->getName();
//                    vpzExpCond *newCond = mVpz->addCondition( newExpName );
//                    newCond->copyFromExpCond(srcExp);
//                    // Add this new condition to the new model
//                    newModel->addCondition(newCond);
//                    // Then, reload the Experimental Condition tab
//                    mExpCondTab->reload();
//
//                    // Get the Dynamic used by the source model
//                    vleVpzDynamic *dyn = model->getDynamic();
//                    // Then, configure the new model to use the same Dynamic
//                    newModel->setDynamic(dyn->getName());
//                }
            }
        }
    }
}

/**
 * @brief fileVpzView::onFocusChanged (slot)
 *        Called when a specific model get focus
 *
 */
void fileVpzView::onFocusChanged(vleVpzModel *model)
{
    if (model == 0)
    {
        uiTool->modelTree->clearSelection();
        uiTool->stackProperty->setCurrentIndex(0);
        return;
    }

    bool treeUpdate = true;
    QTreeWidgetItem *item = uiTool->modelTree->currentItem();
    if (item)
    {
        vleVpzModel *treeModel = 0;
        QVariant vModel = item->data(0, Qt::UserRole+1);
        if (vModel.isValid())
        {
            treeModel = (vleVpzModel *)vModel.value<void *>();
            if (model == treeModel)
                treeUpdate = false;
        }
    }

    if (treeUpdate)
    {
        uiTool->modelTree->clearSelection();

        QList<QTreeWidgetItem*> items;
        items = uiTool->modelTree->findItems(model->getName(), Qt::MatchExactly | Qt::MatchRecursive);

        for (int i = 0; i < items.length(); i++)
        {
            QTreeWidgetItem *selWItem = items.at(i);
            QVariant vItemModel = selWItem->data(0, Qt::UserRole+1);
            if (vItemModel.isValid())
            {
                vleVpzModel *selModel = (vleVpzModel *)vItemModel.value<void *>();
                if (selModel == model)
                    uiTool->modelTree->setCurrentItem(selWItem);
            }
        }
    }

    QTableWidgetItem *newItem = new QTableWidgetItem(model->getName());
    newItem->setData(Qt::UserRole, ROW_NAME);
    newItem->setData(Qt::UserRole+1, QVariant::fromValue((void*)model));
    uiTool->modelProperty->setItem(ROW_NAME, 1, newItem);
    // This row Name is selected by default
    uiTool->modelProperty->setCurrentCell(ROW_NAME, 0);

    WidgetVpzPropertyDynamics *wpd = new WidgetVpzPropertyDynamics();
    wpd->setModel(mVpz, model->getFullName());
    uiTool->modelProperty->setCellWidget(ROW_DYN, 1, wpd);
    uiTool->modelProperty->resizeRowToContents(ROW_DYN);
    QObject::connect(wpd,  SIGNAL(sigActivated(bool)),
                     this, SLOT  (onPropertyMode(bool)));

    WidgetVpzPropertyExpCond *wpec = new WidgetVpzPropertyExpCond();
    wpec->setModel(mVpz, model->getFullName());
    uiTool->modelProperty->setCellWidget(ROW_EXP, 1, wpec);
    uiTool->modelProperty->resizeRowToContents(ROW_EXP);

    WidgetVpzPropertyObservables *wpo = new WidgetVpzPropertyObservables();
    wpo->setModel(mVpz, model->getFullName());
    uiTool->modelProperty->setCellWidget(ROW_OBS, 1, wpo);
    uiTool->modelProperty->resizeRowToContents(ROW_OBS);

    QObject::connect(mVpz, SIGNAL(observablesUpdated()),
                     wpo, SLOT(refresh()));

    uiTool->stackProperty->setCurrentIndex(1);
}

/**
 * @brief fileVpzView::onModelDblClick (slot)
 *        Called when a model widget receive a double-clic
 *
 */
void fileVpzView::onModelDblClick(vleVpzModel *model)
{
    diagSelectModel(model);
}

/**
 * @brief fileVpzView::onPropertyChanged (slot)
 *        Called when a value of right column TableWidget has changed
 *
 */
void fileVpzView::onPropertyChanged(QTableWidgetItem *item)
{
    if (item->data(Qt::UserRole) == ROW_NAME)
    {
        vleVpzModel *model = 0;
        // Retrieve the model object from WidgetItem
        QVariant vModel = item->data(Qt::UserRole + 1);
        if (vModel.isValid())
        {
            void *ptr = vModel.value<void *>();
            model = (vleVpzModel *)ptr;
        }
        if (model == 0)
            return;
        // Get the new name from item
        QString newName = item->text();
        QString oldName = model->getName();
        if (newName == oldName)
            return;

        //Todo a modeler does not have condition attached to

// If the model use a modeler plugin, associated experimental condition must be renamed
//        if (model->hasModeler())
//        {
//            // Get the experimental condition used by plugin
//            vpzExpCond *exp = model->getModelerExpCond();
//            // Search the class name from exp cond name
//            QString expName = exp->getName();
//            QString className = expName.left( expName.length() - oldName.length() );
//            // Build the new exp cond name
//            QString newExpName = className;
//            newExpName += newName;
//
//            // Update the experimental condition, and refresh UI
//            exp->setName( newExpName );
//            mExpCondTab->reload();
//        }

        // Set the new name to the model ...
        model->setName(newName);

        // Update the model tree into right toolbar
        treeUpdateModel(model, oldName, newName);
    }
}

/**
 * @brief fileVpzView::onPropertySelected (slot)
 *        Called when a property is selected into right column list
 *
 */
void
fileVpzView::onPropertySelected(int cr, int cc, int pr, int pc)
{
    (void)cc;
    (void)pc;

    uiTool->modelProperty->resizeRowToContents(pr);
    uiTool->modelProperty->resizeRowToContents(cr);
}

void
fileVpzView::onPropertyMode(bool isSelected)
{
    (void)isSelected;

    QObject *sender = QObject::sender();
    QWidget *widget = qobject_cast<QWidget *>(sender);

    for (int i = 0; i < uiTool->modelProperty->rowCount(); i++)
    {
        QWidget *cellWidget = uiTool->modelProperty->cellWidget(i, 1);
        if (cellWidget == widget)
        {
            uiTool->modelProperty->resizeRowToContents(i);
            break;
        }
    }
}

/**
 * @brief fileVpzView::onAddModelerTab (slot)
 *        Called by vpzModel to open a model modeler tab
 *
 */
void fileVpzView::onAddModelerTab(vleVpzModel *model)
{
    sourceCpp *modelClass = model->getModelerClass();
    if (modelClass == 0)
        return;

    PluginModeler *modeler = modelClass->getModeler();

    if (modeler == 0)
    {
        QMessageBox msgBox;

        msgBox.setText(tr("The modeler plugin could not be loaded (%1)").arg(modelClass->getModelerName()));
        msgBox.exec();
        return;
    }

    // Stay informed of changes into experimental conditions
    QObject::connect(modeler, SIGNAL(expCondChanged(vpzExpCond *)),
                     this,    SLOT  (onExpCondChanged(vpzExpCond *)));

    QWidget *newTab = modeler->addEditModel(model);

    int n = ui->tabWidget->addTab(newTab, model->getName());
    ui->tabWidget->setCurrentIndex(n);
}

/**
 * @brief fileVpzView::onTabClose (slot)
 *        Called when a tab must be closed
 *
 */
void fileVpzView::onTabClose(int index)
{
    QWidget *w = ui->tabWidget->widget(index);
    delete w;
}

/**
 * @brief fileVpzView::onExpCondChanged (slot)
 *        Called when an experimental condition has been changed by plugin
 *
 */
void fileVpzView::onExpCondChanged(const QString& condName)
{
    mExpCondTab->refresh(condName);
}
