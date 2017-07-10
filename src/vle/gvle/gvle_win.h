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
#include <QMainWindow>
#include <QTranslator>
#include <QFileInfo>
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
#include <vle/utils/Context.hpp>
#include <vle/utils/Template.hpp>
#include <vle/utils/Spawn.hpp>
#endif

#include "plugin_mainpanel.h"
#include "gvle_file.h"

namespace Ui {
class gvleWin;
}

namespace vle {
namespace gvle {

class gvle_win : public QMainWindow
{
    Q_OBJECT

public:
    typedef std::map<std::string,std::pair<std::string, bool>> Distributions;

    static std::map<std::string, std::string> defaultDistrib();

    enum StatusFile {
        NOT_OPENED, //no tab opened
        OPENED, //tab opened but content not modified
        OPENED_AND_MODIFIED //tab opened and modified
    };

    explicit gvle_win( const vle::utils::ContextPtr& ctx, QWidget* parent =0);
    virtual ~gvle_win();

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);
    void openMainPanel(gvle_file gf, PluginMainPanel* p);
    void setRightWidget(QWidget* rightWidget);
    int findTabIndex(gvle_file gf);
    bool insideSrc(QModelIndex index);
    bool insideOut(QModelIndex index);
    bool insideExp(QModelIndex index);
    bool insideData(QModelIndex index);

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
    void remoteInstallTimer();
    void onRefreshFiles();
    void onRightWidgetChanged();
    void onUndoAvailable(bool);
    void onCheckDistrib();
    void onCheckDependency();
    void onPackageInstall();
    void onPackageUninstall();

private:
    void projectInstall();



    Ui::gvleWin*                    ui;
    Logger*                         mLogger;
    QTimer                          mTimerRemote;
    QTimer                          mTimerConfigure;
    QTimer                          mTimerBuild;
    QTimer                          mTimerInstall;
    QSettings*                      mSettings;
    bool                            mSimOpened;
    QActionGroup*                   mMenuSimGroup;
    QMap<QString,QString>           mSimulatorPlugins;
    QString                         mCurrentSimName;
    QMap<QString, PluginMainPanel*> mPanels;
    QFileSystemModel*               mProjectFileSytem;
    gvle_plugins                    mGvlePlugins;
    utils::ContextPtr               mCtx;
    utils::Package                  mCurrPackage;
    utils::Spawn                    mSpawn;
    Distributions                   mDistributions;


protected:
    void newProject(QString pathName);
    void openProject(QString pathName);
private:

    void menuRecentProjectRefresh();
    void menuRecentProjectSet(QString path, QAction *menu);
    void menuRecentProjectUpdate(QString path);
    void menuDistributionsRefresh();
    void menuPackagesInstallRefresh();
    void menuLocalPackagesRefresh();
    void statusWidgetOpen();
    void statusWidgetClose();
    void treeProjectUpdate();
    bool tabClose(int index);
    void removeTab(int index);
    bool closeProject();
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from a QFileInfo
     * which text can be eg. '* exp/empty.vpz' or 'exp/empty.vpz'.
     * The relative path (relPath) is the id of the main panel at index i
     * of the tab widget
     * @param i, tab widget index
     * @return the relative path of the tab
     */
    gvle_file getGvleFileFromTabIndex(int i);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from an index of
     * file info
     * @param f, a file info
     * @return the relative path
     */
    gvle_file getGvleFileFromFileInfo(QFileInfo& f);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from an index of the
     * filesystem tree.
     * @param index, filesystem index
     * @return the relative path of the tab
     */
    gvle_file getGvleFileFromFSIndex(QModelIndex index);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from a pointer to
     * a main panel
     * @param p, the main panel
     * @return the relative path corresponding to p
     */
    gvle_file getGvleFileFromMainPanel(PluginMainPanel* p);
    /**
     * @brief get the file system index from a relative path (eg exp/empty.vpz)
     * @param gf, the gvle_file
     * @return the file system index
     */
    QModelIndex getFSIndex(gvle_file gf);
    /**
     * @brief get the main panel from a tab widget index
     * @param i, tab widget index
     * @return the main panel into the tab widget at index i
     */
    PluginMainPanel* getMainPanelFromTabIndex(int i);
    /**
     * @brief get the main panel from its id (relative path)
     * @param gf, the gvle_file
     * @return the main panel associated to relPath or 0
     */
    PluginMainPanel* getMainPanel(gvle_file gf);
    /**
     * @brief get the tab index form a relPath
     * @param gf, the gvle_file
     * @return the tab index
     */
    int getTabIndex(gvle_file gf);


    /**
     * @brief get status ogf a file idetified by relative path
     * (eg. src/Simple.cpp).
     * @param gf, the gvle_file
     * @return the status of this file.
     */
    StatusFile getStatus(gvle_file gf);


    bool removeDir(const QString& dirName);
    /**
     * @brief copy file from the index of files tree, get a new name for
     * the copy and copy metadata for vpz and cpp files.
     * @param the index from the files tree.
     */
    void copyFile(QModelIndex index);
    void removeFile(QModelIndex index);


    QStringList getDescriptionFileContent();
    int setDescriptionFileContent(const QStringList& content);


    /**
     * @brief Open the Description.txt file and add or rm a package as a build
     * dependency and write the file.
     * @param pkg, name of the package
     * @param add, if true pkg is added otherwise it is removed
     * @return error code
     */
    int setPackageToBuildDepends(const QString& pkg, bool add);
    QStringList getPackageToBuildDepends();

};

}}//namespaces

#endif // gvle_win_H
