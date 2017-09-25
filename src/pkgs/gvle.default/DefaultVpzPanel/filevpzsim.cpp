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

#include "filevpzsim.h"
#include "ui_filevpzsim.h"
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>

namespace vle {
namespace gvle {

FileVpzSim::FileVpzSim(vle::utils::Package* pkg,
                       gvle_plugins* plugs,
                       Logger* log,
                       QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::FileVpzSim)
  , mVpz(0)
  , mGvlePlugins(plugs)
  , mPluginSimPanel(0)
  , mPackage(pkg)
  , mLog(log)
{
    ui->setupUi(this);
    QObject::connect(ui->pluginList,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(onPluginChanged(const QString&)));
}

FileVpzSim::~FileVpzSim()
{
    delete ui;
    mVpz = 0;
    mGvlePlugins = 0;
    delete mPluginSimPanel;
    mPackage = 0;
    mLog = 0;
}

void
FileVpzSim::setVpz(vleVpz* vpz)
{
    mVpz = vpz;

    // update plugin list
    bool oldBlock = ui->pluginList->blockSignals(true);
    ui->pluginList->clear();
    ui->pluginList->addItem("Default");
    QStringList plugList = mGvlePlugins->getSimPanelPluginsList();
    ui->pluginList->addItems(plugList);

    for (int i = 0; i < plugList.size(); i++) {
        QString pluginName = plugList.at(i);
    }
    ui->pluginList->blockSignals(oldBlock);

    // TODO check if a plugin
    QString plug = "Default";
    mPluginSimPanel = mGvlePlugins->newInstanceSimPanelPlugin(plug);
    mPluginSimPanel->init(mVpz, mPackage, mLog);
    setSimLeftWidget(mPluginSimPanel->leftWidget());

    emit rightWidgetChanged();
}

void
FileVpzSim::setSimLeftWidget(QWidget* leftWidget)
{
    QStackedWidget* pluginStack = ui->pluginHere;
    int stackSize = pluginStack->count();
    if (stackSize < 1 or stackSize > 2) {
        qDebug() << " Internal error DefaultVpzPanel::setSimLeftWidget ";
        return;
    }

    if (stackSize == 2) {
        pluginStack->removeWidget(pluginStack->widget(1));
    }
    if (leftWidget) {
        pluginStack->addWidget(leftWidget);
        pluginStack->setCurrentIndex(1);
    } else {
        pluginStack->setCurrentIndex(0);
    }
}

QWidget*
FileVpzSim::rightWidget()
{
    return mPluginSimPanel->rightWidget();
}

void
FileVpzSim::onPluginChanged(const QString& plugName)
{
    delete mPluginSimPanel;
    mPluginSimPanel = mGvlePlugins->newInstanceSimPanelPlugin(plugName);
    mPluginSimPanel->init(mVpz, mPackage, mLog);
    setSimLeftWidget(mPluginSimPanel->leftWidget());

    emit rightWidgetChanged();
}
}
} // namespaces
