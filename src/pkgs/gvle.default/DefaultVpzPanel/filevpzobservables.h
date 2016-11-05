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
#ifndef gvle_FILEVPZOBSERVABLES_H
#define gvle_FILEVPZOBSERVABLES_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <vle/gvle/vlevpz.hpp>

namespace Ui {
    class FileVpzObservables;
}

namespace vle {
namespace gvle {


class FileVpzObservables : public QWidget
{
    Q_OBJECT

public:
    enum ObsTreeType { EObsNone, EObsObs, EObsPort,
		       EObsOut};

    explicit FileVpzObservables(QWidget *parent = 0);
    ~FileVpzObservables();
    void setVpz(vleVpz* vpz);
    void reload();

public slots:
    void onViewTreeMenu(const QPoint pos);
    void onItemChanged(QTreeWidgetItem *item,
                             int /*column*/);
    void onViewsUpdated();
    void onObservablesUpdated();
    void onUndoRedoVpz(QDomNode oldVpz, QDomNode newVpz,
            QDomNode oldVpm, QDomNode newVpm);

private:
    int itemType(const QTreeWidgetItem * item) const;
    QString itemName(QTreeWidgetItem * item) const;
    QTreeWidgetItem* newItem(const ObsTreeType type, const QString name);
    void reloadObservables();
    void reloadPorts(const QString& obsName,
                    QTreeWidgetItem *obsItem = 0);
    void reloadViews();

    Ui::FileVpzObservables* ui;
    vleVpz* mVpz;
};

}} //namespaces

#endif // FILEVPZOBSERVABLES_H
