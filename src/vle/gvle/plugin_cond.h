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

#ifndef gvle_PLUGIN_COND_H
#define gvle_PLUGIN_COND_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QSettings>
#include <vle/gvle/logger.h>
#include <vle/gvle/vlevpz.hpp>

namespace vle {
namespace gvle {

class PluginExpCond: public QObject
{
    Q_OBJECT
public:
    virtual QString  getname()   = 0;
    virtual QWidget* getWidget() = 0;
    virtual void  setSettings(QSettings *s) = 0;
    virtual void  setLogger(Logger *logger) = 0;
    virtual void  init(vleVpz* vpz, const QString& condName) = 0;
    virtual PluginExpCond* newInstance() = 0;
};

}} //namepsaces

Q_DECLARE_INTERFACE(vle::gvle::PluginExpCond, "fr.inra.vle.gvle.PluginExpCond/1.0")

#endif
