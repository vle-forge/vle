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
#include "ui_filevpzsim.h"
#include <QStackedWidget>

namespace vle {
namespace gvle {

DefaultVpzPanel::DefaultVpzPanel()
  : PluginMainPanel()
  , m_vpz(0)
  , m_vpzview(0)
  , m_rtool(0)
  , mGvlePlugins(0)
{
}

DefaultVpzPanel::~DefaultVpzPanel()
{
    if (m_vpzview) {
        m_vpzview->ui->tabWidget->blockSignals(true);
    }
    delete m_rtool;
    delete m_vpzview;
    delete m_vpz;
    mGvlePlugins = 0;
}

void
DefaultVpzPanel::init(const gvle_file& gf,
                      utils::Package* pkg,
                      Logger* log,
                      gvle_plugins* plugs,
                      const utils::ContextPtr& ctx)
{
    // Initialize if the vpe does not exist does not exist
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    if (not QFile(gf.source_file).exists()) {
        QString emptyVpzPath = ctx->getTemplate("package")
                                 .string(vle::utils::Path::native_path)
                                 .c_str();
        emptyVpzPath += "/exp/empty.vpz";
        QFile::copy(emptyVpzPath, gf.source_file);
    }

    mGvlePlugins = plugs;

    m_vpzview = new fileVpzView(pkg, mGvlePlugins, log, ctx);
    m_rtool = new FileVpzRtool();

    QObject::connect(&m_vpzview->mScene,
                     SIGNAL(enterCoupledModel(QDomNode)),
                     m_rtool,
                     SLOT(onEnterCoupledModel(QDomNode)));
    QObject::connect(&(m_vpzview->mClassesTab->mScene),
                     SIGNAL(enterCoupledModel(QDomNode)),
                     m_rtool,
                     SLOT(onEnterCoupledModel(QDomNode)));
    QObject::connect(&m_vpzview->mScene,
                     SIGNAL(selectionChanged()),
                     m_rtool,
                     SLOT(onSelectionChanged()));
    QObject::connect(&(m_vpzview->mClassesTab->mScene),
                     SIGNAL(selectionChanged()),
                     m_rtool,
                     SLOT(onSelectionChanged()));
    QObject::connect(&m_vpzview->mScene,
                     SIGNAL(initializationDone(VpzDiagScene*)),
                     m_rtool,
                     SLOT(onInitializationDone(VpzDiagScene*)));
    QObject::connect(&(m_vpzview->mClassesTab->mScene),
                     SIGNAL(initializationDone(VpzDiagScene*)),
                     m_rtool,
                     SLOT(onInitializationDone(VpzDiagScene*)));

    m_vpz = new vleVpz(gf.source_file, gf.metadata_file, mGvlePlugins);

    m_vpz->setLogger(log);

    m_rtool->setVpz(m_vpz);
    m_vpzview->setVpz(m_vpz);
    m_vpzview->setRtool(m_rtool);

    QObject::connect(
      m_vpz, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
    QObject::connect(m_vpzview->ui->tabWidget,
                     SIGNAL(currentChanged(int)),
                     this,
                     SLOT(onCurrentChanged(int)));
    QObject::connect(m_vpzview->mSimTab,
                     SIGNAL(rightWidgetChanged()),
                     this,
                     SLOT(onRightSimWidgetChanged()));
    QObject::connect(
      m_vpz,
      SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
      m_vpzview,
      SLOT(onUndoRedoVpz(QDomNode, QDomNode, QDomNode, QDomNode)));
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
        return m_vpzview->mSimTab->rightWidget();
    } else if (tab == "Project") {
        return 0;
    }
    return 0;
}

void
DefaultVpzPanel::undo()
{
    m_vpzview->vpz()->undo();
}

void
DefaultVpzPanel::redo()
{
    m_vpzview->vpz()->redo();
}

void
DefaultVpzPanel::onCurrentChanged(int /*index*/)
{

    QString tab = m_vpzview->getCurrentTab();
    QString previousTab = m_vpzview->vpz()->getCurrentSource();
    m_vpzview->vpz()->setCurrentSource(tab);
    // return  m_plugin_sim->leftWidget();
    if (tab == "Diagram") {
        m_rtool->onInitializationDone(&m_vpzview->mScene);
        emit rightWidgetChanged();
    } else if (tab == "Conditions") {
        if (previousTab == "Simulation") {
            m_rtool->onInitializationDone(&m_vpzview->mScene);
            emit rightWidgetChanged();
        }
        // else keep last right widget
    } else if (tab == "Dynamics") {
        if (previousTab == "Simulation") {
            m_rtool->onInitializationDone(&m_vpzview->mScene);
            emit rightWidgetChanged();
        }
        // else keep last right widget
    } else if (tab == "Observables") {
        if (previousTab == "Simulation") {
            m_rtool->onInitializationDone(&m_vpzview->mScene);
            emit rightWidgetChanged();
        }
        // else keep last right widget
    } else if (tab == "Views") {
        if (previousTab == "Simulation") {
            m_rtool->onInitializationDone(&m_vpzview->mScene);
            emit rightWidgetChanged();
        }
        // else keep last right widget
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
}
} // namespaces
