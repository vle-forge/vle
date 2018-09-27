/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2014-2018 INRA http://www.inra.fr
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

#ifndef GVLE_DEFAULT_VPZ_PANEL_H
#define GVLE_DEFAULT_VPZ_PANEL_H

#include <QDebug>
#include <QObject>
#include <QWidget>

#include <vle/gvle/gvle_file.h>
#include <vle/gvle/plugin_mainpanel.h>
#include <vle/gvle/plugin_simpanel.h>
#include <vle/gvle/vlevpz.hpp>

#include "filevpzrtool.h"
#include "filevpzview.h"
#include "ui_filevpzview.h"

namespace vle {
namespace gvle {

class DefaultVpzPanel : public PluginMainPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")
    Q_INTERFACES(vle::gvle::PluginMainPanel)
public:
    DefaultVpzPanel();
    ~DefaultVpzPanel() override;

    QString getname() override;
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo() override;
    void redo() override;
    void init(const gvle_file& file,
              utils::Package* pkg,
              Logger*,
              gvle_plugins* plugs,
              const utils::ContextPtr& ctx) override;
    QString canBeClosed() override;
    void discard() override
    {}
    void save() override;
    void setSimLeftWidget(QWidget*);
    PluginMainPanel* newInstance() override
    {
        return new DefaultVpzPanel();
    }

public slots:
    void onCurrentChanged(int index);
    void onUndoAvailable(bool);
    void onRightSimWidgetChanged();

public:
    vleVpz* m_vpz;
    fileVpzView* m_vpzview;
    FileVpzRtool* m_rtool;
    gvle_plugins* mGvlePlugins;
};
}
} // namespaces

#endif
