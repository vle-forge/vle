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

#ifndef gvle_gvle_win_H
#define gvle_gvle_win_H

#include "vle/gvle/logger.h"
#include "plugin_mainpanel.h"
#include <QMainWindow>
#include <QTranslator>
#include <QTreeWidgetItem>
#include <QActionGroup>
#include <QPluginLoader>
#include <QSettings>
#include <QTimer>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>




#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Template.hpp>
#endif

namespace Ui {
class gvleWin;
}

namespace vle {
namespace gvle {

class gvle_win : public QMainWindow
{
    Q_OBJECT

public:
    explicit gvle_win(QWidget* parent = 0);
    ~gvle_win();

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);
    void setRightWidget(QWidget* rightWidget);
    int findTabIndex(QString relPath);
    bool insideSrc(QModelIndex index);
    QString getNewCpp();
    QString getCppPlugin(QString relpath);


private slots:
    void onNewProject();
    void onOpenProject();
    void onProjectRecent1();
    void onProjectRecent2();
    void onProjectRecent3();
    void onProjectRecent4();
    void onProjectRecent5();
    void onCloseProject();
    void onSaveFile();
    void onQuit();
    void onProjectConfigure();
    void onProjectBuild();
    void onProjectClean();
    void onProjectUninstall();
    void onUndo();
    void onRedo();
    void onSelectSimulator(bool isChecked);
    void onHelp();
    void onAbout();
    void onTabChange(int index);
    void onTabClose(int index);
    void onStatusToggle();
    void onTreeDblClick(QModelIndex index);
    void onCustomContextMenu(const QPoint& point);
    void onItemChanged(QTreeWidgetItem* item, int col);
    void onDataChanged(QModelIndex indexTL, QModelIndex indexBR);
    void onFileRenamed(const QString& path, const QString& oldName,
		       const QString& newName);
    void onCurrentChanged(const QModelIndex& index);
    void projectConfigureTimer();
    void projectBuildTimer();
    void projectInstallTimer();
    void projectCleanTimer();
    void projectUninstallTimer();
    void onRefreshFiles();
    void onRightWidgetChanged();
    void onUndoAvailable(bool);

private:
    void projectInstall();

    Ui::gvleWin*          ui;
    Logger*               mLogger;
    QTimer*               mTimer;
    QSettings*            mSettings;
    bool                  mSimOpened;
    QActionGroup*         mMenuSimGroup;
    QMap<QString,QString> mSimulatorPlugins;
    QString               mCurrentSimName;
    QMap<QString, PluginMainPanel*>
                          mPanels;
    QFileSystemModel*     mProjectFileSytem;
    gvle_plugins          mGvlePlugins;


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
    bool tabClose(int index);
    void removeTab(int index);
    bool closeProject();
    PluginMainPanel* getMainPanelFromTabIndex(int index);
    QString getRelPathFromTabIndex(int index);

    bool removeDir(const QString& dirName);
    QString treeProjectRelativePath(const QModelIndex index) const;
    void copyFile(QModelIndex index);
    void removeFile(QModelIndex index);


private:
    vle::utils::Package mCurrPackage;
};

}}//namespaces

#endif // gvle_win_H
