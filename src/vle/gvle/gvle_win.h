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
    enum StatusFile {
        NOT_OPENED, //no tab opened
        OPENED, //tab opened but content not modified
        OPENED_AND_MODIFIED //tab opened and modified
    };

    /***
     * @brief the couple of a source file (either cpp or vpz) with the metadata
     * file
     */
    struct gvle_file {
        gvle_file():
            source_file(""), metadata_file(""){}
        gvle_file(const gvle_file& f):
            source_file(f.source_file), metadata_file(f.metadata_file){}
        QString source_file;
        QString metadata_file;

    };

    /**
     * @brief type of vle file names required by gvle Application
     *
     */
    enum GvleFileRequirement {
        COPY,          // from a file (cpp or vpz) get the gvle_file which
                       // is a copy name that does not exist
        METADATA_FILE, // from a file (cpp or vpz) get the gvle_file which is
                       // associated to (ie. computes metada file)
        NEW_VPZ,       // get a new vpz gvle_file
        NEW_CPP        // get a new cpp gvle_file
    };

    explicit gvle_win(QWidget* parent = 0);
    ~gvle_win();

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);
    void setRightWidget(QWidget* rightWidget);
    int findTabIndex(QString relPath);
    bool insideSrc(QModelIndex index);
    bool insideOut(QModelIndex index);
    /**
     * @brief get a gvle file name (source and metadata)
     * @param relPath, eg: src/NewCpp.cpp
     * @param req, type of file names path building
     */
    gvle_file getGvleFile(QString relPath, GvleFileRequirement req);
    /**
     * @brief get the plugin attached to a cpp by reading the metadata
     * associated to the cpp file
     * @param the relPath eg 'src/NewCpp.cpp' to the cpp file
     * @return the cpp plugin eg gvle.discrete-time
     */
    QString getCppPlugin(QString relpath);
    QString getOutPlugin(QString relpath);


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
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from a QFileInfo
     * which text can be eg. '* exp/empty.vpz' or 'exp/empty.vpz'.
     * The relative path (relPath) is the id of the main panel at index i
     * of the tab widget
     * @param i, tab widget index
     * @return the relative path of the tab
     */
    QString getRelPathFromTabIndex(int i);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from an index of
     * file info
     * @param f, a file info
     * @return the relative path
     */
    QString getRelPathFromFileInfo(QFileInfo& f);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from an index of the
     * filesystem tree.
     * @param index, filesystem index
     * @return the relative path of the tab
     */
    QString getRelPathFromFSIndex(QModelIndex index);
    /**
     * @brief get the relative path (eg. exp/empty.vpz) from a pointer to
     * a main panel
     * @param p, the main panel
     * @return the relative path corresponding to p
     */
    QString getRelPathFromMainPanel(PluginMainPanel* p);
    /**
     * @brief get the file system index from a relative path (eg exp/empty.vpz)
     * @param relPath, the relative path
     * @return the file system index
     */
    QModelIndex getFSIndexFromRelPath(QString relPath);
    /**
     * @brief get the main panel from a tab widget index
     * @param i, tab widget index
     * @return the main panel into the tab widget at index i
     */
    PluginMainPanel* getMainPanelFromTabIndex(int i);
    /**
     * @brief get the main panel from its id (relative path)
     * @param relPath, id of the file
     * @return the main panel associated to relPath or 0
     */
    PluginMainPanel* getMainPanelFromRelPath(QString relPath);
    /**
     * @brief get the tab index form a relPath
     * @param relPath, id of the file
     * @return the tab index
     */
    int getTabIndexFromRelPath(QString relPath);


    /**
     * @brief get status ogf a file idetified by relative path
     * (eg. src/Simple.cpp).
     * @param relPath, id of the file
     * @return the status of this file.
     */
    StatusFile getStatus(QString relPath);


    bool removeDir(const QString& dirName);
    /**
     * @brief copy file from the index of files tree, get a new name for
     * the copy and copy metadata for vpz and cpp files.
     * @param the index from the files tree.
     */
    void copyFile(QModelIndex index);
    void removeFile(QModelIndex index);


private:
    vle::utils::Package mCurrPackage;
};

}}//namespaces

#endif // gvle_win_H
