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

#ifndef FILEVPZCLASSES_H
#define FILEVPZCLASSES_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWidget>
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>
#include <QUndoView>

#include "ui_filevpzrtool.h"
#include <vle/gvle/vlevpz.hpp>
#include "vpzDiagScene.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#include <QtDebug>


namespace Ui {
    class FileVpzClasses;
}

namespace vle {
namespace gvle {

class FileVpzClasses : public QWidget
{
    Q_OBJECT
public:
    explicit FileVpzClasses(Logger* log,
            const vle::utils::ContextPtr& ctx,
            QWidget *parent = 0);
    ~FileVpzClasses();
    void setVpz(vleVpz* vpz);
    void reload();
public slots:
    void onUndoRedoVpz(QDomNode, QDomNode, QDomNode, QDomNode);
    void onNewAtomicTriggered(bool checked);
    void onNewCoupledTriggered(bool checked);
    void onTextChanged(const QString & text);
    void onPushCopy(bool checked);
    void onPushDelete(bool checked);
    void onCurrentIndexChanged(const QString & text);
    void onChanged(const QList<QRectF> & region);
    void onSceneRectChanged(const QRectF& rect);
    void onSelectionChanged();
    void onMenu(const QPoint& pt);

public:
    VpzDiagScene        mScene;
private:
    Ui::FileVpzClasses* ui;
    vleVpz*             mVpz;
    QString             mSelClass;
    QMenu               mMenuButton;//either coupled or atomic

};

}}

#endif // FILEVPZCLASSES_H
