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

#ifndef GVLE2_FILEVPZVIEW_H
#define GVLE2_FILEVPZVIEW_H

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
#include "widgetvpzproperty.h"
#include "filevpzdynamics.h"
#include "filevpzexpcond.h"
#include "filevpzexpview.h"
#include "filevpzobservables.h"
#include "filevpzproject.h"
#include "filevpzclasses.h"
#include "filevpzrtool.h"
#include "vpzDiagScene.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#define ROW_NAME 0
#define ROW_DYN  1
#define ROW_EXP  2
#define ROW_OBS  3

namespace Ui {
class fileVpzView;
class fileVpzRTool;
}

namespace vle {
namespace gvle2 {

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
    explicit fileVpzView(QWidget *parent = 0);
    ~fileVpzView();
    void setVpm(vleVpm *v);
    vleVpm* vpm();
    bool isUsed(int *reason);
    void usedBySim(bool isUsed);
    void save();
    QWidget *getTool();
//    void diagSelectModel(vleVpzModel *base, bool force = FALSE);
//    void treeUpdateModel(vleVpzModel *model, QString oldName, QString newName);

public slots:
    void onTabClose(int index);
    void onCurrentChanged(int index);
//    void onViewTreeMenu(const QPoint pos);
//    void onFocusChanged(vleVpzModel *model);
//    void onModelDblClick(vleVpzModel *model);
//    void onPropertyChanged(QTableWidgetItem *item);
//    void onPropertySelected(int cr, int cc, int pr, int pc);
//    void onPropertyMode(bool isSelected);

//    void onAddModelerTab(vleVpzModel *model);
//    void onExpCondChanged(const QString& condName);

private:
    bool             mUseSim;
    Ui::fileVpzView *ui;
    FileVpzRtool*    mRtool;
    FileVpzDynamics *mDynamicsTab;
    FileVpzExpCond  *mExpCondTab;
    FileVpzExpView  *mExpViewTab;
    FileVpzObservables  *mObservablesTab;
    FileVpzProject  *mProjectTab;
    FileVpzClasses  *mClassesTab;
    vleVpm          *mVpm;
    //QWidget         *mWidgetTool;
    QList<QTreeWidgetItem *> mViewsItems;
//    vleVpzModel     *mCurrentModel;

private:

    VpzDiagScene       mScene;
};

}}  //namespaces

#endif // FILEVPZVIEW_H
