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


#include "DefaultVpzPanel.h"
#include "DefaultSimSubpanel.h"
#include <QStackedWidget>
#include "ui_filevpzsim.h"

namespace vle {
namespace gvle {



DefaultVpzPanel::DefaultVpzPanel():
            PluginMainPanel(), m_vpzview(0), m_rtool(0),
            mGvlePlugins(0)
{

}

DefaultVpzPanel::~DefaultVpzPanel()
{
    delete m_vpzview;
    delete m_rtool;

}


void
DefaultVpzPanel::init(QString& relPath, utils::Package* pkg, Logger* log,
        gvle_plugins* plugs)
{
    mGvlePlugins = plugs;

    m_vpzview = new fileVpzView(pkg, mGvlePlugins, log);
    m_rtool = new FileVpzRtool();

    QObject::connect(&m_vpzview->mScene, SIGNAL(enterCoupledModel(QDomNode)),
            m_rtool,    SLOT  (onEnterCoupledModel(QDomNode)));
    QObject::connect(&(m_vpzview->mClassesTab->mScene), SIGNAL(enterCoupledModel(QDomNode)),
            m_rtool,    SLOT  (onEnterCoupledModel(QDomNode)));
    QObject::connect(&m_vpzview->mScene, SIGNAL(selectionChanged()),
            m_rtool,    SLOT  (onSelectionChanged()));
    QObject::connect(&(m_vpzview->mClassesTab->mScene), SIGNAL(selectionChanged()),
            m_rtool,    SLOT  (onSelectionChanged()));
    QObject::connect(&m_vpzview->mScene, SIGNAL(initializationDone(VpzDiagScene*)),
            m_rtool, SLOT (onInitializationDone(VpzDiagScene*)));
    QObject::connect(&(m_vpzview->mClassesTab->mScene), SIGNAL(initializationDone(VpzDiagScene*)),
            m_rtool, SLOT (onInitializationDone(VpzDiagScene*)));

    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    QString relPathVpm = relPath;

    relPathVpm.replace(".vpz", ".vpm");
    vleVpm* vpm = new vleVpm(basepath+"/"+relPath,
            basepath+"/metadata/"+relPathVpm, mGvlePlugins);

    vpm->setLogger(log);
    vpm->setBasePath("coucouTODO");

    m_rtool->setVpm(vpm);
    m_vpzview->setVpm(vpm);
    m_vpzview->setRtool(m_rtool);


    QObject::connect(vpm, SIGNAL(modelsUpdated()),
                     m_rtool, SLOT (onModelsUpdated()));
    QObject::connect(vpm, SIGNAL(undoAvailable(bool)),
                     this, SLOT (onUndoAvailable(bool)));
    QObject::connect(m_vpzview->ui->tabWidget,   SIGNAL(currentChanged(int)),
                     this,            SLOT  (onCurrentChanged(int)));
    QObject::connect(m_vpzview->mSimTab,   SIGNAL(rightWidgetChanged()),
                     this,            SLOT  (onRightSimWidgetChanged()));
}

QString
DefaultVpzPanel::canBeClosed()
{
    return "";
}

void
DefaultVpzPanel::save()
{
    m_vpzview->save();
}



QString
DefaultVpzPanel::getname()
{
    return "Default";
}

QWidget*
DefaultVpzPanel::leftWidget()
{
    return m_vpzview;
}

QWidget*
DefaultVpzPanel::rightWidget()
{
    QString tab = m_vpzview->getCurrentTab();
    if (tab == "Diagram") {
        return m_rtool;
    } else if (tab == "Conditions") {
        return m_rtool;
    } else if (tab == "Dynamics") {
        return m_rtool;
    } else if (tab == "Observables") {
        return m_rtool;
    } else if (tab == "Views") {
        return m_rtool;
    } else if (tab == "Classes") {
        return m_rtool;
    } else if (tab == "Simulation") {
        return  m_vpzview->mSimTab->rightWidget();
    } else if (tab == "Project") {
        return 0;
    }
    return 0;
}

void
DefaultVpzPanel::undo()
{
    m_vpzview->vpm()->undo();
}

void
DefaultVpzPanel::redo()
{
    m_vpzview->vpm()->redo();
}

void
DefaultVpzPanel::onCurrentChanged(int /*index*/)
{
    QString tab = m_vpzview->getCurrentTab();
    m_vpzview->vpm()->setCurrentTab(tab);
    //return  m_plugin_sim->leftWidget();
    if (tab == "Diagram") {
        m_rtool->onInitializationDone(&m_vpzview->mScene);
    } else if (tab == "Conditions") {
        //keep last right widget
    } else if (tab == "Dynamics") {
        //keep last right widget
    } else if (tab == "Observables") {
        //keep last right widget
    } else if (tab == "Views") {
        //keep last right widget
    } else if (tab == "Classes") {
        m_rtool->onInitializationDone(&(m_vpzview->mClassesTab->mScene));
        emit rightWidgetChanged();
    } else if (tab == "Simulation") {
        emit rightWidgetChanged();
    } else if (tab == "Project") {
        emit rightWidgetChanged();
    }
}

void
DefaultVpzPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

void
DefaultVpzPanel::onRightSimWidgetChanged()
{
    emit rightWidgetChanged();
}

}} //namespaces


