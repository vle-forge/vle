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

#ifndef gvle_FILEVPZDYNAMICS_H
#define gvle_FILEVPZDYNAMICS_H

#include <vle/gvle/vlevpz.hpp>

#ifndef Q_MOC_RUN
#include <vle/utils/Context.hpp>
#endif

namespace Ui {
class FileVpzDynamics;
}

namespace vle {
namespace gvle {

class FileVpzDynamics : public QWidget
{
    Q_OBJECT

public:
    enum FVD_MENU { FVD_add_dynamic,  FVD_remove_dynamic };
public:
    explicit FileVpzDynamics(const utils::ContextPtr& ctx, QWidget* parent=0);
    ~FileVpzDynamics();
    void setVpz(vleVpz* vpz);

public slots:

    void reload();
    void onSelectPackage(const QString & id, const QString & text);
    void onSelectLibrary(const QString & id, const QString& text);
    void onDynamicsTableMenu(const QPoint&);
    void onUndoRedoVpz(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);
    void onTextUpdated(const QString& id, const QString&, const QString&);

protected:
    void updatePackageList(QString name = "");
    void updateLibraryList(QString package = "", QString name = "");
    QString getSelected();


private:
    Ui::FileVpzDynamics* ui;
    vleVpz*              mVpz;
    utils::ContextPtr    mCtx;

};

}}//namespaces

#endif
