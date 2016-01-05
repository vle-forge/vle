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

#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>
#include "filevpzview.h"
#include "ui_filevpzview.h"

namespace vle {
namespace gvle2 {


fileVpzView::fileVpzView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileVpzView)
{
    ui->setupUi(this);

    mVpm = 0;
    mUseSim = false;
    mDynamicsTab = 0;
    mExpCondTab = 0;
    mObservablesTab = 0;
    mProjectTab = 0;
    mClassesTab = 0;

    //Setup uiTool
//    mWidgetTool = new QWidget();
//    uiTool->setupUi(mWidgetTool);

    //Setup vpzRtool
    mRtool = new vle::gvle2::FileVpzRtool();

    // Configure Experimental Conditions tab
    mExpCondTab = new vle::gvle2::FileVpzExpCond();
    int expTabId = ui->tabWidget->addTab(mExpCondTab, tr("Conditions"));

    // Configure Dynamics tab
    mDynamicsTab = new vle::gvle2::FileVpzDynamics();
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
    mExpViewTab = new vle::gvle2::FileVpzExpView();
    int viewTabId = ui->tabWidget->addTab(mExpViewTab, tr("Views"));

    // Configure View tab
    mClassesTab = new vle::gvle2::FileVpzClasses();
    int classesTabId = ui->tabWidget->addTab(mClassesTab, tr("Classes"));

    ui->tabWidget->setTabsClosable(true);
    QTabBar *tabBar = ui->tabWidget->findChild<QTabBar *>();
    tabBar->setTabButton(0, QTabBar::RightSide, 0);
    tabBar->setTabButton(1, QTabBar::RightSide, 0);
    tabBar->setTabButton(2, QTabBar::RightSide, 0);
    tabBar->setTabButton(3, QTabBar::RightSide, 0);
    tabBar->setTabButton(4, QTabBar::RightSide, 0);
    tabBar->setTabButton(5, QTabBar::RightSide, 0);
    tabBar->setTabButton(dynTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(expTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(viewTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(observablesTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(projectTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(classesTabId, QTabBar::RightSide, 0);

    //set signals/clots
    QObject::connect(ui->tabWidget,   SIGNAL(tabCloseRequested(int)),
                     this,            SLOT  (onTabClose(int)));
    QObject::connect(ui->tabWidget,   SIGNAL(currentChanged(int)),
                     this,            SLOT  (onCurrentChanged(int)));
    QObject::connect(&mScene, SIGNAL(enterCoupledModel(QDomNode)),
                     mRtool,    SLOT  (onEnterCoupledModel(QDomNode)));
    QObject::connect(&(mClassesTab->mScene), SIGNAL(enterCoupledModel(QDomNode)),
                     mRtool,    SLOT  (onEnterCoupledModel(QDomNode)));
    QObject::connect(&mScene, SIGNAL(selectionChanged()),
                     mRtool,    SLOT  (onSelectionChanged()));
    QObject::connect(&(mClassesTab->mScene), SIGNAL(selectionChanged()),
                     mRtool,    SLOT  (onSelectionChanged()));
    QObject::connect(&mScene, SIGNAL(initializationDone(VpzDiagScene*)),
                     mRtool, SLOT (onInitializationDone(VpzDiagScene*)));
    QObject::connect(&(mClassesTab->mScene), SIGNAL(initializationDone(VpzDiagScene*)),
                     mRtool, SLOT (onInitializationDone(VpzDiagScene*)));

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

    if (mClassesTab)
        delete mClassesTab;

    delete ui;
}

/**
 * @brief fileVpzView::setVpz
 *        Associate the tab with an allocated VPZ object
 *
 */
void fileVpzView::setVpm(vleVpm* vpm)
{
    mVpm = vpm;

    QObject::connect(mVpm, SIGNAL(modelsUpdated()),
                     mRtool, SLOT (onModelsUpdated()));

    mVpm->setCurrentTab(ui->tabWidget->tabText(
            ui->tabWidget->currentIndex()));

    if (mRtool) {
        mRtool->setVpm(mVpm);
    }

    // ---- Dynamics Tab ----
    if (mDynamicsTab) {
        mDynamicsTab->setVpm(mVpm);
        mDynamicsTab->reload();
    }

    // ---- Experimental Conditions Tab ----
    if (mExpCondTab) {
        mExpCondTab->setVpm(mVpm);
        mExpCondTab->reload();
    }

    // ---- Experimental Conditions Tab ----
    if (mObservablesTab) {
        mObservablesTab->setVpm(mVpm);
    }

    // ---- Experimental Conditions Tab ----
    if (mProjectTab) {
        mProjectTab->setVpm(mVpm);
    }

    // ---- View Tab ----
    if (mExpViewTab) {
        mExpViewTab->setVpm(mVpm);
        mExpViewTab->reload();
    }

    // ---- View Tab ----
    if (mClassesTab) {
        mClassesTab->setVpm(mVpm);
    }

    //Build Scene
    mScene.init(mVpm, "");
    ui->graphicView->setSceneRect(QRect(0,0,0,0));
    ui->graphicView->setScene(&mScene);
    mScene.update();


}

void fileVpzView::save()
{
    if ( ! mVpm)
        return;

    mVpm->save();
}

/**
 * @brief fileVpzView::vpz
 *        Readback the VPZ currently used
 *
 */
vleVpm* fileVpzView::vpm()
{
    //QString fileName = mVpm->getFilename();
    //mVleLibVpz = new vle::vpz::Vpz(fileName.toStdString());

    return mVpm;
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
    else if (mVpm)
    {
        flag = 2;
    }

    if (reason)
        *reason = flag;

    return (flag != 0);
}


QWidget *fileVpzView::getTool()
{
    return mRtool->mWidgetTool;
}


void fileVpzView::onTabClose(int index)
{
    QWidget *w = ui->tabWidget->widget(index);
    delete w;
}

void
fileVpzView::onCurrentChanged(int index)
{
    QString tab = ui->tabWidget->tabText(index);
    if (tab == "Diagram") {
        mRtool->onInitializationDone(&mScene);
    } else if (tab == "Classes") {
        mRtool->onInitializationDone(&(mClassesTab->mScene));
    }
    vpm()->setCurrentTab(tab);
}

}}//namespaces
