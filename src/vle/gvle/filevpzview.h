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

#ifndef gvle_FILEVPZVIEW_H
#define gvle_FILEVPZVIEW_H

#include <QGraphicsScene>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>
#include <QUndoView>

#include "ui_filevpzrtool.h"
#include "vlevpm.h"
#include "plugin_mainpanel.h"
#include "widgetvpzproperty.h"
#include "filevpzdynamics.h"
#include "filevpzexpcond.h"
#include "filevpzexpview.h"
#include "filevpzobservables.h"
#include "filevpzproject.h"
#include "filevpzclasses.h"
#include "filevpzsim.h"
#include "filevpzrtool.h"
#include "vpzDiagScene.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

namespace Ui {
class fileVpzView;
class fileVpzRTool;
}

namespace vle {
namespace gvle {

class fileVpzView : public QWidget
{
    Q_OBJECT
public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondParameter };

public:
    /**
     * @brief fileVpzView::fileVpzView
     *        Default constructor for VPZ tab
     */
    explicit fileVpzView(vle::utils::Package* pkg,
            gvle_plugins* plugs, Logger* log, QWidget *parent = 0);
    ~fileVpzView();
    void setVpm(vleVpm *v);
    void setRtool(FileVpzRtool* tool);
    vleVpm* vpm();
    bool isUsed(int *reason);
    void usedBySim(bool isUsed);
    void save();
    QWidget *getTool();
    QString  getname();
    QWidget* leftPanel();
    QWidget* rigthPanel();
    QString getCurrentTab();
    void undo();
    void redo();
//    void diagSelectModel(vleVpzModel *base, bool force = FALSE);
//    void treeUpdateModel(vleVpzModel *model, QString oldName, QString newName);

public slots:
    void onTabClose(int index);
//    void onViewTreeMenu(const QPoint pos);
//    void onFocusChanged(vleVpzModel *model);
//    void onModelDblClick(vleVpzModel *model);
//    void onPropertyChanged(QTableWidgetItem *item);
//    void onPropertySelected(int cr, int cc, int pr, int pc);
//    void onPropertyMode(bool isSelected);

//    void onAddModelerTab(vleVpzModel *model);
//    void onExpCondChanged(const QString& condName);

public:
    bool             mUseSim;
    Ui::fileVpzView*ui;
    FileVpzDynamics *mDynamicsTab;
    FileVpzExpCond  *mExpCondTab;
    FileVpzExpView*mExpViewTab;
    FileVpzObservables*      mObservablesTab;
    FileVpzProject*          mProjectTab;
    FileVpzClasses*          mClassesTab;
    FileVpzSim*              mSimTab;
    vleVpm*                  mVpm;
    FileVpzRtool*            mRtool;
    QList<QTreeWidgetItem *> mViewsItems;
    gvle_plugins*            mGvlePlugins;
    vle::utils::Package*     mPackage;
    Logger*                  mLog;
//    vleVpzModel     *mCurrentModel;

public:

    VpzDiagScene       mScene;
};

}}  //namespaces

#endif // FILEVPZVIEW_H
