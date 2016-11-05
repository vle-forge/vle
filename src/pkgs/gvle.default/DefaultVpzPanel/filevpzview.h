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
#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/plugin_mainpanel.h>
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
#include "ui_filevpzrtool.h"

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
            gvle_plugins* plugs, Logger* log, const utils::ContextPtr& ctx,
            QWidget *parent = 0);
    ~fileVpzView();
    void setVpz(vleVpz *v);
    void setRtool(FileVpzRtool* tool);
    vleVpz* vpz();
    bool isUsed(int *reason);
    void usedBySim(bool isUsed);
    void save();
    QWidget *getTool();
    QString  getname();
    QWidget* leftPanel();
    QWidget* rigthPanel();
    QString getCurrentTab();

public slots:
    void onTabClose(int index);
    void onUndoRedoVpz(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);

public:
    Ui::fileVpzView*         ui;
    bool                     mUseSim;
    FileVpzDynamics*         mDynamicsTab;
    FileVpzExpCond*          mExpCondTab;
    FileVpzExpView*          mExpViewTab;
    FileVpzObservables*      mObservablesTab;
    FileVpzProject*          mProjectTab;
    FileVpzClasses*          mClassesTab;
    FileVpzSim*              mSimTab;
    vleVpz*                  mVpz;
    FileVpzRtool*            mRtool;
    QList<QTreeWidgetItem *> mViewsItems;
    gvle_plugins*            mGvlePlugins;
    vle::utils::Package*     mPackage;
    Logger*                  mLog;
    VpzDiagScene             mScene;
};

}}  //namespaces

#endif // FILEVPZVIEW_H
