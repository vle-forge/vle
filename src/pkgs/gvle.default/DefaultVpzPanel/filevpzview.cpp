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

#include "filevpzview.h"
#include "ui_filevpzview.h"
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>

namespace vle {
namespace gvle {

fileVpzView::fileVpzView(vle::utils::Package* pkg,
                         gvle_plugins* plugs,
                         Logger* log,
                         const utils::ContextPtr& ctx,
                         QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::fileVpzView)
  , mUseSim(false)
  , mVpz(0)
  , mRtool(0)
  , mGvlePlugins(plugs)
  , mPackage(pkg)
  , mLog(log)
  , mScene(log, ctx)
{
    ui->setupUi(this);

    mUseSim = false;
    mDynamicsTab = 0;
    mExpCondTab = 0;
    mObservablesTab = 0;
    mProjectTab = 0;
    mClassesTab = 0;

    // Setup uiTool
    //    mWidgetTool = new QWidget();
    //    uiTool->setupUi(mWidgetTool);

    // Configure Experimental Conditions tab
    mExpCondTab = new vle::gvle::FileVpzExpCond(mGvlePlugins);
    int expTabId = ui->tabWidget->addTab(mExpCondTab, "Conditions");

    // Configure Dynamics tab
    mDynamicsTab = new vle::gvle::FileVpzDynamics(ctx);
    int dynTabId = ui->tabWidget->addTab(mDynamicsTab, "Dynamics");

    // Configure Observables tab
    mObservablesTab = new FileVpzObservables();
    int observablesTabId =
      ui->tabWidget->addTab(mObservablesTab, "Observables");

    // Configure View tab
    mExpViewTab = new vle::gvle::FileVpzExpView(ctx, log);
    int viewTabId = ui->tabWidget->addTab(mExpViewTab, "Views");

    // Configure Classes tab
    mClassesTab = new vle::gvle::FileVpzClasses(log, ctx);
    int classesTabId = ui->tabWidget->addTab(mClassesTab, "Classes");

    // Configure Sim Tab
    mSimTab = new vle::gvle::FileVpzSim(mPackage, mGvlePlugins, log);
    int simTabId = ui->tabWidget->addTab(mSimTab, "Simulation");

    // Configure Project tab
    mProjectTab = new FileVpzProject();
    int projectTabId = ui->tabWidget->addTab(mProjectTab, "Project");
    mProjectTab->setTabId(projectTabId);
    mProjectTab->setTab(ui->tabWidget);

    ui->tabWidget->setTabsClosable(true);
    QTabBar* tabBar = ui->tabWidget->findChild<QTabBar*>();
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
    tabBar->setTabButton(classesTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(simTabId, QTabBar::RightSide, 0);
    tabBar->setTabButton(projectTabId, QTabBar::RightSide, 0);

    // set signals/clots
    QObject::connect(ui->tabWidget,
                     SIGNAL(tabCloseRequested(int)),
                     this,
                     SLOT(onTabClose(int)));
}

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

    if (mSimTab)
        delete mSimTab;

    delete ui;
}

/**
 * @brief fileVpzView::setVpz
 *        Associate the tab with an allocated VPZ object
 *
 */
void
fileVpzView::setVpz(vleVpz* vpz)
{
    mVpz = vpz;

    mVpz->setCurrentSource(
      ui->tabWidget->tabText(ui->tabWidget->currentIndex()));

    // ---- Dynamics Tab ----
    if (mDynamicsTab) {
        mDynamicsTab->setVpz(mVpz);
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
    }

    // ---- View Tab ----
    if (mExpViewTab) {
        mExpViewTab->setVpz(mVpz);
        mExpViewTab->reload();
    }

    // ---- View Tab ----
    if (mClassesTab) {
        mClassesTab->setVpz(mVpz);
    }

    // ---- View Tab ----
    if (mSimTab) {
        mSimTab->setVpz(mVpz);
    }

    // Build Scene
    mScene.init(mVpz, "");
    ui->graphicView->setSceneRect(QRect(0, 0, 0, 0));
    ui->graphicView->setScene(&mScene);
    mScene.update();
}

void
fileVpzView::setRtool(FileVpzRtool* tool)
{
    mRtool = tool;
}

void
fileVpzView::save()
{
    if (!mVpz)
        return;

    mVpz->save();
}

/**
 * @brief fileVpzView::vpz
 *        Readback the VPZ currently used
 *
 */
vleVpz*
fileVpzView::vpz()
{
    // QString fileName = mVpz->getFilename();
    // mVleLibVpz = new vle::vpz::Vpz(fileName.toStdString());

    return mVpz;
}

void
fileVpzView::usedBySim(bool isUsed)
{
    mUseSim = isUsed;
}

bool
fileVpzView::isUsed(int* reason = 0)
{
    int flag = 0;
    if (mUseSim)
        flag = 1;
    else if (mVpz) {
        flag = 2;
    }

    if (reason)
        *reason = flag;

    return (flag != 0);
}

QWidget*
fileVpzView::getTool()
{
    return 0;
    //    return mRtool->mWidgetTool;
}

QString
fileVpzView::getname()
{
    return "VpzMainpanel";
}
QWidget*
fileVpzView::leftPanel()
{
    return ui->tabWidget;
}
QWidget*
fileVpzView::rigthPanel()
{
    return 0;
}

QString
fileVpzView::getCurrentTab()
{
    return ui->tabWidget->tabText(ui->tabWidget->currentIndex());
}

void
fileVpzView::onTabClose(int index)
{
    QWidget* w = ui->tabWidget->widget(index);
    delete w;
}

void
fileVpzView::onUndoRedoVpz(QDomNode oldValVpz,
                           QDomNode newValVpz,
                           QDomNode oldValVpm,
                           QDomNode newValVpm)
{
    if (newValVpz.nodeName() == "vle_project") {
        setVpz(mVpz);
        return;
    }

    QString tab = getCurrentTab();
    if (tab == "Diagram") {
        mScene.onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Conditions") {
        mExpCondTab->onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Dynamics") {
        mDynamicsTab->onUndoRedoVpz(
          oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Observables") {
        mObservablesTab->onUndoRedoVpz(
          oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Views") {
        mExpViewTab->onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Classes") {
        mClassesTab->onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    } else if (tab == "Simulation") {
        //        mSimTab->onUndoRedoVpz(oldValVpz, newValVpz,
        //                oldValVpm, newValVpm);
    } else if (tab == "Project") {
        mProjectTab->onUndoRedoVpz(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}
}
} // namespaces
