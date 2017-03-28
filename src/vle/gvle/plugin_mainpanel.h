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

#ifndef GVLE_PLUGIN_MAINPANEL_H
#define GVLE_PLUGIN_MAINPANEL_H

#include <QWidget>
#include <QDebug>
#include "gvle_plugins.h"

#include "logger.h"

#include <vle/utils/Package.hpp>

namespace vle {
namespace gvle {

struct gvle_file;

class PluginMainPanel : public QObject
{
    Q_OBJECT
public:
    PluginMainPanel(){}
    virtual ~PluginMainPanel(){}

    virtual QString  getname()                                              =0;
    virtual QWidget* leftWidget()                                           =0;
    virtual QWidget* rightWidget()                                          =0;
    virtual void undo()                                                     =0;
    virtual void redo()                                                     =0;
    /**
     * Note: this function is in charge of creating files (including metadata)
     * at the creation time. Also it has to handle renaming of files and
     * copies from the QFileSystem.
     */
    virtual void init(const gvle_file& file, utils::Package* pkg,
            Logger* log, gvle_plugins* plugs, const utils::ContextPtr& ctx) =0;
    virtual QString canBeClosed()                                           =0;
    virtual void save()                                                     =0;
    virtual void discard()                                                  =0;
    virtual PluginMainPanel* newInstance()                                  =0;

signals:
    void rightWidgetChanged();
    void undoAvailable(bool);
};

}} //namespaces

Q_DECLARE_INTERFACE(vle::gvle::PluginMainPanel, "fr.inra.vle.gvle.PluginMainPanel")

#endif
