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
#ifndef FILEVPZEXPVIEW_H
#define FILEVPZEXPVIEW_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#ifndef Q_MOC_RUN
#include <vle/utils/Context.hpp>
#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/plugin_output.h>
#endif

namespace Ui {
class FileVpzExpView;
}

namespace vle {
namespace gvle {

enum FILE_VPZ_EXP_VIEW_MENU {
    FVEVM_add_view,
    FVEVM_remove_view,
    FVEVM_rename_view,
    FVEVM_disable_view
};

class FileVpzExpView : public QWidget
{
    Q_OBJECT

public:
    explicit FileVpzExpView(const utils::ContextPtr& ctx,  Logger* log, 
	                        QWidget *parent = 0);
    ~FileVpzExpView();
    void setVpz(vleVpz* vpz);
    void reload();

signals:
    void outputChange(QString viewName, QString outputName);

public slots:
    void onViewSelected(QListWidgetItem * item);
    void onOutputSelected(const QString& item);
    void onTimeStepChanged(double v);
    void onTimedCheck(int);
    void onOutputCheck(int);
    void onInternalCheck(int);
    void onExternalCheck(int);
    void onConfluentCheck(int);
    void onFinishCheck(int);
    void onViewListMenu(const QPoint&);
    void onItemChanged(QListWidgetItem* );
    void onUndoRedoVpz(QDomNode oldVpz, QDomNode newVpz,
            QDomNode oldVpm, QDomNode newVpm);

private:
    QString getSelectedOutputPlugin();
    void updateViewType();
    void updatePlugin();


    Ui::FileVpzExpView* ui;
    vleVpz*             mVpz;
    PluginOutput*       mPlugin;
    QString             currView;
    utils::ContextPtr   mCtx;
	Logger*             mLog;

};
}}//namespaces
#endif // FILEVPZEXPVIEW_H
