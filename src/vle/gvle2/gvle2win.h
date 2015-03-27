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

#ifndef GVLE2WIN_H
#define GVLE2WIN_H

#include "vle/gvle2/logger.h"
#include "widgetprojecttree.h"
#include <QMainWindow>
#include <QTranslator>
#include <QTreeWidgetItem>
#include <QActionGroup>
#include <QPluginLoader>
#include <QSettings>
#include <QTimer>

#include "vlepackage.h"
#include "pluginmodelerview.h"

#include "plugin_cond.h"
#include "plugin_modeler.h"
#include "plugin_sim.h"

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Template.hpp>
#endif

#define QTVPZ 1

namespace Ui {
class GVLE2Win;
}

class GVLE2Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit GVLE2Win(QWidget *parent = 0);
    ~GVLE2Win();

protected:
    void loadPlugins();
    void loadExpCondPlugins   (PluginExpCond   *plugin, QString fileName);
    void loadModelerPlugins   (PluginModeler   *plugin, QString fileName);
    void loadSimulationPlugins(PluginSimulator *plugin, QString fileName);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    pluginModelerView *openModeler(QString filename);
    void loadModelerClasses(pluginModelerView *modeler);

private slots:
    void onNewProject();
    void onOpenProject();
    void onProjectRecent1();
    void onProjectRecent2();
    void onProjectRecent3();
    void onProjectRecent4();
    void onProjectRecent5();
    void onCloseProject();
    void onQuit();
    void onProjectConfigure();
    void onProjectBuild();
    void projectInstall();
    void onLaunchSimulation();
    void onSelectSimulator(bool isChecked);
    void onHelp();
    void onAbout();
    void onTabChange(int index);
    void onTabClose(int index);
    void onStatusToggle();
    void onTreeDblClick(QTreeWidgetItem *item, int column);
    void projectConfigureTimer();
    void projectBuildTimer();
    void projectInstallTimer();
    void onOpenModeler();
    void onNewModelerClass();
    void onRefreshFiles();
    void setChangedVpz(QString filename);

private:
    Ui::GVLE2Win *ui;
    Logger           * mLogger;
    QTimer           * mTimer;
    WidgetProjectTree* mProjectTree;
    QSettings        * mSettings;
    vle::vpz::Vpz    * mVpz;
    bool               mSimOpened;
    QActionGroup     * mMenuSimGroup;
private:
    QMap <QString,QString> mSimulatorPlugins;
    QString            mCurrentSimName;
    PluginSimulator  * mCurrentSim;
    QPluginLoader    * mCurrentSimPlugin;
private:
    QList <QString>    mModelers;
    QList<QPluginLoader *> mModelerPlugins;
private:
    QMap <QString,QString> mExpPlugins;

protected:
    void newProject(QString pathName);
    void openProject(QString pathName);
private:
    void menuRecentProjectRefresh();
    void menuRecentProjectSet(QString path, QAction *menu);
    void menuRecentProjectUpdate(QString path);
    void statusWidgetOpen();
    void statusWidgetClose();
    void treeProjectUpdate();
    void treeProjectUpdate(QTreeWidgetItem *base, QString folderName);
    bool tabClose(int index);
    bool closeProject();
private:
    vlePackage         *mPackage;
    vle::utils::Package mCurrPackage;
};

#endif // GVLE2WIN_H
