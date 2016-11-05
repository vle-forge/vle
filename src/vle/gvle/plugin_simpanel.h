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

#ifndef GVLE_PLUGIN_SIMPANEL_H
#define GVLE_PLUGIN_SIMPANEL_H

#include <QWidget>
#include <vle/utils/Package.hpp>
#include <vle/gvle/vlevpz.hpp>
#include "logger.h"

namespace vle {
namespace gvle {

class PluginSimPanel :public QObject
{
    Q_OBJECT
public:
    PluginSimPanel(){};
    virtual ~PluginSimPanel(){};
    virtual void init(vleVpz* vpz, vle::utils::Package* pkg, Logger* log) = 0;
    virtual QString  getname()                                            = 0;
    virtual QWidget* leftWidget()                                         = 0;
    virtual QWidget* rightWidget()                                        = 0;
    virtual void undo()                                                   = 0;
    virtual void redo()                                                   = 0;
    virtual PluginSimPanel* newInstance()                                 = 0;

};

}} //namespaces

Q_DECLARE_INTERFACE(vle::gvle::PluginSimPanel, "fr.inra.vle.gvle.PluginSimPanel")

#endif
