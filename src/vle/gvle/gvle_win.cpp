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

#include <algorithm>
#include <iostream>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/RemoteManager.hpp>

#include <QActionGroup>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <vle/utils/Tools.hpp>

#include "aboutbox.h"
#include "gvle_file.h"
#include "gvle_win.h"
#include "help.h"
#include "plugin_cond.h"
#include "plugin_mainpanel.h"
#include "plugin_output.h"
#include "ui_gvle_win.h"

namespace vu = vle::utils;

namespace vle {
namespace gvle {

struct gvle_ctx_log : vle::utils::Context::LogFunctor
{
    bool first;
    gvle_ctx_log()
      : first(true)
    {
    }

    virtual ~gvle_ctx_log()
    {
    }

    virtual void write(const vle::utils::Context& ctx,
                       int priority,
                       const char* file,
                       int line,
                       const char* fn,
                       const char* format,
                       va_list args) noexcept override
    {
        FILE* fp;
        if (first) {
            first = false;
            fp = fopen(ctx.getLogFile("gvle").string().c_str(), "w");
        } else {
            fp = fopen(ctx.getLogFile("gvle").string().c_str(), "a");
        }
        if (fp) {
            if (priority == 7)
                fprintf(fp, "[dbg] %s:%d %s: ", file, line, fn);
            else if (priority == 6)
                fprintf(fp, "%s: ", fn);
            else
                fprintf(fp, "[Error] %s: ", fn);

            vfprintf(fp, format, args);
            fclose(fp);
        }
    }
};

std::map<std::string, std::string>
gvle_win::defaultDistrib()
{
    std::map<std::string, std::string> ret;
    ret["http://www.vle-project.org/pub/2.0"] = "vle";
    ret["http://recordb.toulouse.inra.fr/distributions/2.0"] = "record";
    return ret;
}

gvle_win::gvle_win(const utils::ContextPtr& ctx, QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::gvleWin)
  , mLogger(0)
  , mTimerRemote()
  , mTimerConfigure()
  , mTimerBuild()
  , mTimerInstall()
  , mSettings(0)
  , mSimOpened(false)
  , mMenuSimGroup(0)
  , mSimulatorPlugins()
  , mCurrentSimName("")
  , mPanels()
  , mProjectFileSytem(0)
  , mGvlePlugins(ctx)
  , mCtx(ctx)
  , mCurrPackage(ctx)
  , mSpawn(ctx)
  , mDistributions()
{
    // GUI init
    ui->setupUi(this);

    // log init (gvle::Logger and Ctx logger)
    mLogger = new Logger();
    mLogger->setWidget(ui->statusLog);
    mCtx->set_log_function(
      std::unique_ptr<utils::Context::LogFunctor>(new gvle_ctx_log()));
    // VLE init
    mCurrPackage.refreshPath();

    // Open the configuration file
    std::string configFile = mCtx->getHomeFile("gvle.conf").string();
    mSettings =
      new QSettings(QString(configFile.c_str()), QSettings::IniFormat);
    menuRecentProjectRefresh();
    menuDistributionsRefresh();
    menuLocalPackagesRefresh();

    QObject::connect(
      ui->actionNewProject, SIGNAL(triggered()), this, SLOT(onNewProject()));
    QObject::connect(
      ui->actionOpenProject, SIGNAL(triggered()), this, SLOT(onOpenProject()));
    QObject::connect(
      ui->actionRecent1, SIGNAL(triggered()), this, SLOT(onProjectRecent1()));
    QObject::connect(
      ui->actionRecent2, SIGNAL(triggered()), this, SLOT(onProjectRecent2()));
    QObject::connect(
      ui->actionRecent3, SIGNAL(triggered()), this, SLOT(onProjectRecent3()));
    QObject::connect(
      ui->actionRecent4, SIGNAL(triggered()), this, SLOT(onProjectRecent4()));
    QObject::connect(
      ui->actionRecent5, SIGNAL(triggered()), this, SLOT(onProjectRecent5()));
    QObject::connect(ui->actionCloseProject,
                     SIGNAL(triggered()),
                     this,
                     SLOT(onCloseProject()));
    QObject::connect(
      ui->actionSaveFile, SIGNAL(triggered()), this, SLOT(onSaveFile()));
    QObject::connect(
      ui->actionQuit, SIGNAL(triggered()), this, SLOT(onQuit()));
    QObject::connect(
      ui->actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
    QObject::connect(
      ui->actionRedo, SIGNAL(triggered()), this, SLOT(onRedo()));
    QObject::connect(ui->actionConfigureProject,
                     SIGNAL(triggered()),
                     this,
                     SLOT(onProjectConfigure()));
    QObject::connect(ui->actionBuildProject,
                     SIGNAL(triggered()),
                     this,
                     SLOT(onProjectBuild()));
    QObject::connect(ui->actionCleanProject,
                     SIGNAL(triggered()),
                     this,
                     SLOT(onProjectClean()));
    QObject::connect(ui->actionUninstall,
                     SIGNAL(triggered()),
                     this,
                     SLOT(onProjectUninstall()));
    QObject::connect(ui->actionSimNone,
                     SIGNAL(toggled(bool)),
                     this,
                     SLOT(onSelectSimulator(bool)));
    QObject::connect(
      ui->actionHelp, SIGNAL(triggered()), this, SLOT(onHelp()));
    QObject::connect(
      ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    QObject::connect(ui->treeProject,
                     SIGNAL(doubleClicked(QModelIndex)),
                     this,
                     SLOT(onTreeDblClick(QModelIndex)));
    QObject::connect(ui->treeProject,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,
                     SLOT(onCustomContextMenu(const QPoint&)));

    QObject::connect(ui->tabWidget,
                     SIGNAL(currentChanged(int)),
                     this,
                     SLOT(onTabChange(int)));
    QObject::connect(ui->tabWidget,
                     SIGNAL(tabCloseRequested(int)),
                     this,
                     SLOT(onTabClose(int)));

    QObject::connect(
      ui->statusTitleToggle, SIGNAL(clicked()), this, SLOT(onStatusToggle()));

    // Initiate an mutual exclusive selection on simulators menu
    mMenuSimGroup = new QActionGroup(this);
    ui->actionSimNone->setActionGroup(mMenuSimGroup);

    // Update window title
    setWindowTitle("GVLE");
    //
    QList<int> sizes;
    sizes.append(200);
    sizes.append(500);
    sizes.append(200);
    ui->splitter->setSizes(sizes);

    mGvlePlugins.registerPlugins();
    treeProjectUpdate();
    mLogger->log(QString("Using VLE_HOME: %1")
                   .arg(QString(mCtx->getHomeDir().string().c_str())));
    ui->menuProject->setEnabled(false);
    ui->menuDependencies->setEnabled(false);

    QObject::connect(
        &mTimerRemote, SIGNAL(timeout()), this, SLOT(remoteInstallTimer()));
    QObject::connect(
        &mTimerConfigure, SIGNAL(timeout()), this, SLOT(projectConfigureTimer()));
    QObject::connect(
        &mTimerBuild, SIGNAL(timeout()), this, SLOT(projectBuildTimer()));
    QObject::connect(
        &mTimerInstall, SIGNAL(timeout()), this, SLOT(projectInstallTimer()));
}

gvle_win::~gvle_win()
{
    ui->tabWidget->blockSignals(true);
    mSettings->sync();
    delete mSettings;
    delete mLogger;
    ui->tabWidget->clear();
    delete ui;
}

void
gvle_win::closeEvent(QCloseEvent* event)
{
    if (closeProject()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void
gvle_win::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    statusWidgetClose();
}

void
gvle_win::onNewProject()
{
    if (not closeProject()) {
        return;
    }

    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setWindowTitle("New Project");
    FileChooserDialog.setFileMode(QFileDialog::AnyFile);
    FileChooserDialog.setOptions(QFileDialog::ShowDirsOnly);
    FileChooserDialog.setAcceptMode(QFileDialog::AcceptSave);
    FileChooserDialog.setLabelText(QFileDialog::FileName,
                                   "Name of the VLE project");
    if (FileChooserDialog.exec()) {
        QString dirPkgName = FileChooserDialog.selectedFiles().first();
        QFileInfo fInfo(dirPkgName);
        if (fInfo.exists()) {
            if (fInfo.isDir()) {
                removeDir(dirPkgName);
            } else {
                QFile::remove(dirPkgName);
            }
        }
        newProject(dirPkgName);
    }
}
/**
 * @brief gvle_win::onOpenProject
 *        Handler for menu function : File > Open Project
 */
void
gvle_win::onOpenProject()
{
    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setFileMode(QFileDialog::Directory);
    if (FileChooserDialog.exec()) {
        openProject(FileChooserDialog.selectedFiles().first());
    }
}
void
gvle_win::onProjectRecent1()
{
    QVariant path = mSettings->value("Projects/recent1");
    if (path.isValid()) {
        openProject(path.toString());
    }
}
void
gvle_win::onProjectRecent2()
{
    QVariant path = mSettings->value("Projects/recent2");
    if (path.isValid()) {
        openProject(path.toString());
    }
}
void
gvle_win::onProjectRecent3()
{
    QVariant path = mSettings->value("Projects/recent3");
    if (path.isValid()) {
        openProject(path.toString());
    }
}
void
gvle_win::onProjectRecent4()
{
    QVariant path = mSettings->value("Projects/recent4");
    if (path.isValid()) {
        openProject(path.toString());
    }
}
void
gvle_win::onProjectRecent5()
{
    QVariant path = mSettings->value("Projects/recent5");
    if (path.isValid()) {
        openProject(path.toString());
    }
}

void
gvle_win::newProject(QString pathName)
{
    if (not closeProject()) {
        return;
    }
    QDir dir(pathName);
    std::string basename = dir.dirName().toStdString();
    dir.cdUp();
    QDir::setCurrent(dir.path());

    mCurrPackage.select(basename);

    mLogger->log(QString("New Project %1").arg(dir.dirName()));

    // Update window title
    setWindowTitle(QString("GVLE - ") + basename.c_str());

    try {
        mCurrPackage.create();
    } catch (...) {
        QMessageBox::warning(this,
                             "Warning",
                             QString("Project '") + basename.c_str() +
                               "' cannot be created in directory '" +
                               QDir::currentPath() + "'",
                             QMessageBox::Ok);
        mCurrPackage.select("");
        return;
    }
    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);
    ui->menuDependencies->setEnabled(true);
}

/**
 * @brief gvle_win::openProject
 *        Handler for menu function : File > Open Project
 */
void
gvle_win::openProject(QString pathName)
{
    if (not closeProject()) {
        return;
    }
    QDir dir(pathName);
    std::string basename = dir.dirName().toStdString();
    dir.cdUp();
    QDir::setCurrent(dir.path());

    mCurrPackage.select(basename);

    mLogger->log(QString("Open Project %1").arg(mCurrPackage.name().c_str()));

    // Update window title
    setWindowTitle(QString("GVLE - ") + mCurrPackage.name().c_str());

    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->menuProject->setEnabled(true);
    ui->menuDependencies->setEnabled(true);
    ui->actionCloseProject->setEnabled(true);
    menuLocalPackagesRefresh();
}

bool
gvle_win::closeProject()
{

    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--) {
        gvle_file gf = getGvleFileFromTabIndex(i);
        if (gf.relPath != "Welcome") {
            ui->tabWidget->setCurrentIndex(i);
            if (not tabClose(i)) {
                return false;
            }
        }
    }

    // Update menus
    mCurrPackage.select("");
    ui->treeProject->setModel(mProjectFileSytem);
    ui->menuProject->setEnabled(false);
    ui->menuDependencies->setEnabled(false);
    ui->actionCloseProject->setEnabled(false);
    // Update window title
    setWindowTitle("GVLE");

    mLogger->log(QString("Project closed"));

    treeProjectUpdate();
    return true;
}

gvle_file
gvle_win::getGvleFileFromTabIndex(int i)
{
    QString relPath = ui->tabWidget->tabText(i);
    if (relPath.startsWith("* ")) {
        relPath = relPath.remove("* ");
    }
    return gvle_file(mCurrPackage, relPath);
}

gvle_file
gvle_win::getGvleFileFromFileInfo(QFileInfo& f)
{
    QString relPath = f.filePath();
    QString currentDir =
      QDir::currentPath() + "/" + mCurrPackage.name().c_str() + "/";

    relPath.replace(currentDir, "");

    return gvle_file(mCurrPackage, relPath);
}

gvle_file
gvle_win::getGvleFileFromFSIndex(QModelIndex index)
{
    QFileInfo f = QFileInfo(mProjectFileSytem->filePath(index));
    return getGvleFileFromFileInfo(f);
}

gvle_file
gvle_win::getGvleFileFromMainPanel(PluginMainPanel* p)
{
    QMap<QString, PluginMainPanel*>::iterator i;
    for (i = mPanels.begin(); i != mPanels.end(); ++i) {
        if (i.value() == p) {
            return gvle_file(mCurrPackage, i.key());
        }
    }
    return gvle_file();
}

QModelIndex
gvle_win::getFSIndex(gvle_file gf)
{
    QModelIndex index = mProjectFileSytem->index(gf.relPath);
    return index;
}

PluginMainPanel*
gvle_win::getMainPanelFromTabIndex(int i)
{
    gvle_file gf = getGvleFileFromTabIndex(i);
    if (mPanels.contains(gf.relPath)) {
        return mPanels[gf.relPath];
    }
    return 0;
}

PluginMainPanel*
gvle_win::getMainPanel(gvle_file gf)
{
    if (mPanels.contains(gf.relPath)) {
        return mPanels[gf.relPath];
    }
    return 0;
}

int
gvle_win::getTabIndex(gvle_file gf)
{
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        if (getGvleFileFromTabIndex(i).relPath == gf.relPath) {
            return i;
        }
    }
    return -1;
}

gvle_win::StatusFile
gvle_win::getStatus(gvle_file gf)
{
    if (not mPanels.contains(gf.relPath)) {
        return NOT_OPENED;
    }
    int i = findTabIndex(gf);
    QString tabText = ui->tabWidget->tabText(i);
    if (tabText.startsWith("* ")) {
        return OPENED_AND_MODIFIED;
    }
    return OPENED;
}

void
gvle_win::onCloseProject()
{
    closeProject();
}

void
gvle_win::onSaveFile()
{
    gvle_file gf = getGvleFileFromTabIndex(ui->tabWidget->currentIndex());
    if (mPanels.contains(gf.relPath) and mProjectFileSytem and
        mProjectFileSytem->isReadOnly()) {
        mPanels[gf.relPath]->save();
    }
}

/**
 * @brief gvle_win::onQuit
 *        Handler for menu function : File > Quit
 */
void
gvle_win::onQuit()
{
    if (closeProject()) {
        qApp->exit();
    }
}

void
gvle_win::onProjectConfigure()
{
    mLogger->log(tr("Project configuration started"));
    statusWidgetOpen();

    try {
        mCurrPackage.configure();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project configuration failed"));
        return;
    }
    ui->menuProject->setEnabled(false);
    ui->actionConfigureProject->setEnabled(false);

    ui->menuDependencies->setEnabled(false);

    mTimerConfigure.start(2);
}

void
gvle_win::onProjectBuild()
{
    mLogger->log(tr("Project compilation started"));
    statusWidgetOpen();

    try {
        mCurrPackage.build();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project compilation failed"));
        return;
    }
    ui->menuProject->setEnabled(false);
    ui->actionBuildProject->setEnabled(false);

    ui->menuDependencies->setEnabled(false);

    mTimerBuild.start(2);
}

void
gvle_win::onProjectClean()
{
    mLogger->log(tr("Project clean started"));
    statusWidgetOpen();
    bool success = true;
    try {
        mCurrPackage.clean();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);

        success = false;
    }
    if (success) {
        mLogger->log(tr("Project clean complete"));
    } else {
        mLogger->log(tr("Project clean failed"));
    }
    ui->menuProject->setEnabled(true);
    ui->menuDependencies->setEnabled(true);
}

void
gvle_win::onProjectUninstall()
{
    mLogger->log(tr("Project uninstall started"));
    statusWidgetOpen();
    bool success = true;
    try {
        mCurrPackage.rclean();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);

        success = false;
    }
    if (success) {
        mLogger->log(tr("Project uninstall complete"));
    } else {
        mLogger->log(tr("Project uninstall failed"));
    }
    ui->menuProject->setEnabled(true);
    ui->menuDependencies->setEnabled(true);
}

void
gvle_win::projectConfigureTimer()
{
    if (mCurrPackage.isFinish()) {
        mTimerConfigure.stop();
        mLogger->log(tr("Project configuration complete"));
        ui->menuProject->setEnabled(true);
        ui->actionConfigureProject->setEnabled(true);

        ui->menuDependencies->setEnabled(true);
    }
    std::string oo;
    std::string oe;
    bool ret = mCurrPackage.get(&oo, &oe);
    if (ret) {
        if (oe.length())
            mLogger->logExt(oe.c_str(), true);
        if (oo.length())
            mLogger->logExt(oo.c_str());
    }
}

void
gvle_win::projectBuildTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if (oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimerBuild.stop();
        if (mCurrPackage.get(&oo, &oe)) {
            if (oe.length()) {
                mLogger->logExt(oe.c_str(), true);
            }
            if (oo.length()) {
                mLogger->logExt(oo.c_str());
            }
        }
        if (mCurrPackage.isSuccess()) {
            projectInstall();
            mLogger->log(tr("Project compilation complete"));
        } else {
            mLogger->log(tr("Project compilation failed"));
        }
        ui->menuProject->setEnabled(true);
        ui->actionBuildProject->setEnabled(true);

        ui->menuDependencies->setEnabled(true);
    }
}

void
gvle_win::projectInstallTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if (oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimerInstall.stop();
        if (mCurrPackage.isSuccess()) {
            mLogger->log(tr("Project installation complete"));
        } else {
            mLogger->log(tr("Project installation failed"));
        }
        ui->menuProject->setEnabled(true);
        ui->actionBuildProject->setEnabled(true);

        ui->menuDependencies->setEnabled(true);
    }
}

void
gvle_win::remoteInstallTimer()
{
    std::string oo, oe;
    bool status;

    if (mSpawn.get(&oo, &oe)) {
        if (oe.length()) {
            mLogger->logExt(oe.c_str());
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mSpawn.isfinish()) {
        mTimerRemote.stop();
        mSpawn.status(&oe, &status);
        if (status) {
            mLogger->log("Remote package installation complete");
        } else {
            mLogger->logExt("Remote package installation failed", true);
        }
        menuLocalPackagesRefresh();
        ui->menuPackages->setEnabled(true);
    }
}

void
gvle_win::onUndo()
{
    if (not ui->tabWidget->currentWidget()) {
        return;
    }
    QString relPath =
      ui->tabWidget->currentWidget()->property("relPath").toString();
    if (relPath != "Welcome" and mPanels.find(relPath) != mPanels.end()) {
        mPanels[relPath]->undo();
    }
}

void
gvle_win::onRedo()
{
    if (not ui->tabWidget->currentWidget()) {
        return;
    }
    QString relPath =
      ui->tabWidget->currentWidget()->property("relPath").toString();
    if (relPath != "Welcome" and mPanels.find(relPath) != mPanels.end()) {
        mPanels[relPath]->redo();
    }
}

void
gvle_win::onSelectSimulator(bool isChecked)
{
    QAction* act = (QAction*)sender();

    if (isChecked) {
        if (act->objectName() != "actionSimNone")
            mCurrentSimName = act->objectName();
    }
}

/**
 * @brief gvle_win::onHelp Handler of menu Help > Help
 *        Open a new tab and display embedded help page
 */
void
gvle_win::onHelp()
{
    bool helpOpened = false;
    help* tabHelp = 0;
    int idx = 0;

    // Get current widget to detect contextual help entry
    // QWidget *wid = QApplication::focusWidget();

    // Search if an Help tab is already open
    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--) {
        QWidget* w = ui->tabWidget->widget(i);
        QVariant tabType = w->property("type");
        if (tabType.isValid() && (tabType.toString() == "help")) {
            tabHelp = (help*)w;
            idx = i;
            helpOpened = true;
            break;
        }
    }
    // If no help tab found, create a new one
    if (!helpOpened) {
        tabHelp = new help(ui->tabWidget);
        tabHelp->setProperty("type", QString("help"));
        idx = ui->tabWidget->addTab(tabHelp, "Help");
    }
    // Set the focus on help tab
    ui->tabWidget->setCurrentIndex(idx);
}

/**
 * @brief gvle_win::onAbout Handler of menu Help > About
 *        Open the about box as dialog and show it
 */
void
gvle_win::onAbout()
{
    AboutBox box;

    box.exec();
}

/**
 * @brief gvle_win::menuRecentProjectRefresh
 *        Read the recent opened projects list and update menu
 */
void
gvle_win::menuRecentProjectRefresh()
{
    QVariant v1 = mSettings->value("Projects/recent1");
    QVariant v2 = mSettings->value("Projects/recent2");
    QVariant v3 = mSettings->value("Projects/recent3");
    QVariant v4 = mSettings->value("Projects/recent4");
    QVariant v5 = mSettings->value("Projects/recent5");
    if (v1.isValid())
        menuRecentProjectSet(v1.toString(), ui->actionRecent1);
    else
        ui->actionRecent1->setText(tr("<none>"));

    if (v2.isValid())
        menuRecentProjectSet(v2.toString(), ui->actionRecent2);
    if (v3.isValid())
        menuRecentProjectSet(v3.toString(), ui->actionRecent3);
    if (v4.isValid())
        menuRecentProjectSet(v4.toString(), ui->actionRecent4);
    if (v5.isValid())
        menuRecentProjectSet(v5.toString(), ui->actionRecent5);
}

void
gvle_win::menuRecentProjectSet(QString path, QAction* menu)
{
    QDir dir(path);
    menu->setText(dir.dirName());
    menu->setToolTip(path);
    menu->setEnabled(true);
    menu->setVisible(true);
}

void
gvle_win::menuRecentProjectUpdate(QString path)
{
    QVariant v1 = mSettings->value("Projects/recent1");
    QVariant v2 = mSettings->value("Projects/recent2");
    QVariant v3 = mSettings->value("Projects/recent3");
    QVariant v4 = mSettings->value("Projects/recent4");
    QVariant v5 = mSettings->value("Projects/recent5");

    QSet<QString> recentList;
    recentList.insert(path);

    if (v1.isValid())
        recentList.insert(v1.toString());
    if (v2.isValid())
        recentList.insert(v2.toString());
    if (v3.isValid())
        recentList.insert(v3.toString());
    if (v4.isValid())
        recentList.insert(v4.toString());
    if (v5.isValid())
        recentList.insert(v5.toString());

    QSetIterator<QString> i(recentList);
    if (i.hasNext())
        mSettings->setValue("Projects/recent1", i.next());
    if (i.hasNext())
        mSettings->setValue("Projects/recent2", i.next());
    if (i.hasNext())
        mSettings->setValue("Projects/recent5", i.next());
    if (i.hasNext())
        mSettings->setValue("Projects/recent4", i.next());
    if (i.hasNext())
        mSettings->setValue("Projects/recent3", i.next());

    mSettings->sync();
}

void
gvle_win::menuDistributionsRefresh()
{
    std::map<std::string, std::string> def = defaultDistrib();

    mDistributions.clear();

    std::string remotes = "";
    mCtx->get_setting("vle.remote.url", &remotes);
    QStringList remotesQStr = QString(remotes.c_str()).split(",");
    remotesQStr.removeAll("");
    int unknown = 0;
    for (int r = 0; r < remotesQStr.size(); r++) {
        QString rem = remotesQStr[r];
        std::map<std::string, std::string>::iterator itf =
          def.find(rem.toStdString());
        if (itf != def.end()) {
            mDistributions[itf->second] = std::make_pair(itf->first, true);
        } else {
            std::string key;
            if (unknown > 0) {
                key = utils::format("unknown_%d", unknown);
            } else {
                key = "unknown";
            }
            mDistributions[key] =
              std::make_pair(remotesQStr[r].toStdString(), true);
            unknown++;
        }
    }

    for (std::map<std::string, std::string>::iterator itb = def.begin();
         itb != def.end();
         itb++) {
        mDistributions[itb->second] = std::make_pair(itb->first, false);
    }

    ui->menuDistributions->clear();
    for (auto& d : mDistributions) {
        QString el = d.first.c_str();
        el += " (";
        el += d.second.first.c_str();
        el += ")";
        QAction* action = ui->menuDistributions->addAction(el);
        action->setCheckable(true);
        action->setData(QString(d.first.c_str()));
        d.second.second =
          remotesQStr.contains(QString(d.second.first.c_str()));
        action->setChecked(d.second.second);
        QObject::connect(
          action, SIGNAL(changed()), this, SLOT(onCheckDistrib()));
    }

    menuPackagesInstallRefresh();
}

void
gvle_win::menuPackagesInstallRefresh()
{
    // update packages to install
    utils::RemoteManager rm(mCtx);
    rm.start(utils::REMOTE_MANAGER_UPDATE, "", &std::cout);
    rm.join();
    if (rm.hasError()) {
        mLogger->logExt(QString(rm.messageError().c_str()), true);
        return;
    }
    rm.start(utils::REMOTE_MANAGER_SEARCH, ".*", &std::cout);
    rm.join();
    if (rm.hasError()) {
        mLogger->logExt(QString(rm.messageError().c_str()), true);
        return;
    }

    vle::utils::Packages res;
    rm.getResult(&res);
    std::sort(res.begin(),
              res.end(),
              [](const utils::PackageId& a, const utils::PackageId& b) {
                  return a.name < b.name;
              });

    vle::utils::Packages::const_iterator itb = res.begin();
    vle::utils::Packages::const_iterator ite = res.end();
    ui->menuInstall->clear();
    for (; itb != ite; itb++) {
        QString distrib = "";
        for (auto& d : mDistributions) {
            if (d.second.first == itb->url) {
                distrib = QString(d.first.c_str());
            }
        }
        QString el = itb->name.c_str();
        el += " from ";
        el += distrib;
        QAction* action = ui->menuInstall->addAction(el);
        action->setData(QString(itb->name.c_str()));
        QObject::connect(
          action, SIGNAL(triggered()), this, SLOT(onPackageInstall()));
    }
}

void
gvle_win::menuLocalPackagesRefresh()
{
    //
    std::vector<utils::Path> pkgs = mCtx->getBinaryPackages();
    std::vector<std::string> pkgsName;
    for (auto& p : pkgs) {
        pkgsName.push_back(p.filename());
    }
    std::sort(pkgsName.begin(), pkgsName.end());

    QStringList dep = getPackageToBuildDepends();

    ui->menuUninstall->clear();
    ui->menuDependencies->clear();
    for (auto& p : pkgsName) {
        QString el = p.c_str();
        QAction* action = ui->menuUninstall->addAction(el);
        action->setData(el);
        QObject::connect(
          action, SIGNAL(triggered()), this, SLOT(onPackageUninstall()));

        action = ui->menuDependencies->addAction(el);
        action->setData(el);
        action->setCheckable(true);
        action->setChecked(dep.contains(el));
        QObject::connect(
          action, SIGNAL(changed()), this, SLOT(onCheckDependency()));
    }
}

/* ---------- Manage the central tabs ---------- */

void
gvle_win::onTabChange(int index)
{

    if (ui->tabWidget->count() == 0)
        return;
    PluginMainPanel* w = getMainPanelFromTabIndex(index);
    if (w) {
        setRightWidget(w->rightWidget());
    } else {
        setRightWidget(0);
    }
}

bool
gvle_win::tabClose(int index)
{
    if (index < 0) {
        return true;
    }
    gvle_file gf = getGvleFileFromTabIndex(index);
    PluginMainPanel* w = getMainPanelFromTabIndex(index);
    if (not w) {
        removeTab(index);
        return true;
    }
    QString canBeClosed = w->canBeClosed();
    if (canBeClosed != "") {
        QMessageBox msgBox;
        msgBox.setText(
          QString("This file cannot be closed: %1").arg(canBeClosed));
        msgBox.exec();
        return false;
    }
    QString tabText = ui->tabWidget->tabText(index);
    if (tabText.startsWith("* ")) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
          this,
          tr("Question"),
          tr("Save file ") + gf.relPath + " before closing it ?",
          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch (reply) {
        case QMessageBox::Save:
            w->save();
            mPanels.remove(gf.relPath);
            removeTab(index);
            delete w;
            break;
        case QMessageBox::Discard:
            w->discard();
            mPanels.remove(gf.relPath);
            removeTab(index);
            delete w;
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        default:
            // should never be reached
            break;
        }
    } else {
        mPanels.remove(gf.relPath);
        removeTab(index);
        delete w;
    }
    return true;
}

void
gvle_win::removeTab(int index)
{
    QString tabText = ui->tabWidget->tabText(index);
    ui->tabWidget->removeTab(index);
    PluginMainPanel* w =
      getMainPanelFromTabIndex(ui->tabWidget->currentIndex());
    if (w) {
        setRightWidget(w->rightWidget());
    }
}

void
gvle_win::onTabClose(int index)
{
    tabClose(index);
}

/* ---------- Manage the status bar ---------- */

/**
 * @brief gvle_win::onProjectConfigure
 *        Handler for menu function : Project > Configure Project
 */
void
gvle_win::onStatusToggle()
{
    if (ui->statusLog->isVisible())
        statusWidgetClose();
    else
        statusWidgetOpen();
}

void
gvle_win::statusWidgetOpen()
{
    ui->statusLog->show();
    ui->statusWidget->setMinimumSize(0, 0);
    ui->statusWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->statusWidget->setSizePolicy(
      QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void
gvle_win::statusWidgetClose()
{
    ui->statusLog->hide();
    QSize titleSize = ui->statusTitleFrame->size();
    ui->statusWidget->setFixedHeight(titleSize.height());
    ui->statusWidget->setSizePolicy(
      QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

/* ---------- Manage the Project navigator ---------- */

void
gvle_win::treeProjectUpdate()
{
    delete mProjectFileSytem;
    mProjectFileSytem = new QFileSystemModel(this);
    QTreeView* tree = ui->treeProject;
    tree->setModel(mProjectFileSytem);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    QItemSelectionModel* selmod = tree->selectionModel();
    QTreeView* projectTreeView = ui->treeProject;

    vle::utils::Path p(mCurrPackage.name());
    QString projectName(p.filename().c_str());
    if (projectName != "") {
        QModelIndex projectIndex = mProjectFileSytem->setRootPath(projectName);
        projectTreeView->setRootIndex(projectIndex);
    }
    projectTreeView->resizeColumnToContents(0);
    QObject::connect(mProjectFileSytem,
                     SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                     this,
                     SLOT(onDataChanged(QModelIndex, QModelIndex)));
    QObject::connect(
      mProjectFileSytem,
      SIGNAL(fileRenamed(const QString&, const QString&, const QString&)),
      this,
      SLOT(onFileRenamed(const QString&, const QString&, const QString&)));
    QObject::connect(
      selmod,
      SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
      this,
      SLOT(onCurrentChanged(const QModelIndex&)));
}

void
gvle_win::onTreeDblClick(QModelIndex index)
{
    if (not index.isValid())
        return;

    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";
    fileName.replace(currentDir, "");
    QFileInfo selectedFileInfo = QFileInfo(fileName);
    gvle_file gf = getGvleFileFromFSIndex(index);

    int alreadyOpened = findTabIndex(gf);
    if (alreadyOpened != -1) {
        ui->tabWidget->setCurrentIndex(alreadyOpened);
        return;
    }
    PluginMainPanel* newPanel = gf.newInstanceMainPanel(mGvlePlugins);
    if (newPanel) {
        openMainPanel(gf, newPanel);
    }
    // set undo redo enabled
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(true);
}

void
gvle_win::onCustomContextMenu(const QPoint& point)
{
    QTreeView* projectTreeView = ui->treeProject;

    QPoint globalPos = projectTreeView->mapToGlobal(point);
    QModelIndex index = projectTreeView->indexAt(point);
    if (not index.isValid())
        return;

    bool inExp = insideExp(index);
    bool inSrc = insideSrc(index);
    bool inOut = insideOut(index);
    bool inData = insideData(index);
    gvle_file gf = getGvleFileFromFSIndex(index);
    QString suffix = gf.suffix();
    gvle_win::StatusFile status_file = getStatus(gf);

    QAction* action;

    QMenu ctxMenu;
    action = ctxMenu.addAction(tr("Remove File"));
    action->setData(1);
    action->setDisabled(
      (suffix != "vpz" and suffix != "cpp" and suffix != "txt") or
      (not(inExp or inSrc or inData)) or (status_file == OPENED) or
      (status_file == OPENED_AND_MODIFIED));
    action = ctxMenu.addAction(tr("Rename File"));
    action->setData(2);
    action->setDisabled(
      (suffix != "vpz" and suffix != "cpp" and suffix != "txt") or
      (not(inExp or inSrc or inData)) or (status_file == OPENED) or
      (status_file == OPENED_AND_MODIFIED));
    action = ctxMenu.addAction(tr("Copy File"));
    action->setData(3);
    action->setDisabled(
      (suffix != "vpz" and
       suffix != "cpp" and
       suffix != "txt" and
       suffix != "cpp") or
      (not(inExp or inSrc or inData)));
    ctxMenu.addSeparator();
    QMenu* subMenu = ctxMenu.addMenu("Add model");
    subMenu->setDisabled(gf.relPath != "exp");
    if (gf.relPath == "exp") {
        QStringList plugList = mGvlePlugins.getMainPanelVpzPluginsList();
        for (int i = 0; i < plugList.size(); i++) {
            QString pluginName = plugList.at(i);
            action = subMenu->addAction(pluginName);
            action->setProperty("vpzPlugin", pluginName);
        }
    }
    subMenu = ctxMenu.addMenu("Add dynamic");
    subMenu->setDisabled(gf.relPath != "src");
    if (gf.relPath == "src") {
        QStringList plugList = mGvlePlugins.getMainPanelPluginsList();
        for (int i = 0; i < plugList.size(); i++) {
            QString pluginName = plugList.at(i);
            action = subMenu->addAction(pluginName);
            action->setProperty("srcPlugin", pluginName);
        }
    }
    subMenu = ctxMenu.addMenu("Add data");
    subMenu->setDisabled(gf.relPath != "data");
    if (gf.relPath == "data") {
        QStringList plugList = mGvlePlugins.getMainPanelDataPluginsList();
        for (int i = 0; i < plugList.size(); i++) {
            QString pluginName = plugList.at(i);
            action = subMenu->addAction(pluginName);
            action->setProperty("dataPlugin", pluginName);
        }
    }
    subMenu = ctxMenu.addMenu("Open output with");
    bool disable = not inOut or (suffix != "dat");
    subMenu->setDisabled(disable);
    if (not disable) {
        QStringList plugList = mGvlePlugins.getMainPanelOutPluginsList();
        for (int i = 0; i < plugList.size(); i++) {
            QString pluginName = plugList.at(i);
            action = subMenu->addAction(pluginName);
            action->setProperty("outPlugin", pluginName);
        }
    }

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == 1) { // Remove File
            removeFile(index);
        } else if (actCode == 2) { // Rename File
            mProjectFileSytem->setReadOnly(false);
            projectTreeView->edit(index);
        } else if (actCode == 3) { // Copy File
            mProjectFileSytem->setReadOnly(false);
            copyFile(index);
        } else { // Add a new Cpp based on a plugin
            PluginMainPanel* newPanel = 0;
            gvle_file gfnew;
            QVariant srcPlugin = selectedItem->property("srcPlugin");
            QVariant outPlugin = selectedItem->property("outPlugin");
            QVariant vpzPlugin = selectedItem->property("vpzPlugin");
            QVariant dataPlugin = selectedItem->property("dataPlugin");
            if (srcPlugin.isValid()) {
                gfnew = gvle_file::getNewCpp(mCurrPackage);
                newPanel = mGvlePlugins.newInstanceMainPanelPlugin(
                  srcPlugin.toString());
            } else if (vpzPlugin.isValid()) {
                gfnew = gvle_file::getNewVpz(mCurrPackage);
                newPanel = mGvlePlugins.newInstanceMainPanelVpzPlugin(
                  vpzPlugin.toString());
            } else if (dataPlugin.isValid()) {
                gfnew = gvle_file::getNewData(mCurrPackage);
                newPanel = mGvlePlugins.newInstanceMainPanelDataPlugin(
                  dataPlugin.toString());
            } else if (outPlugin.isValid()) {
                gfnew = gf;
                newPanel = mGvlePlugins.newInstanceMainPanelOutPlugin(
                  outPlugin.toString());
            }
            if (newPanel) {
                openMainPanel(gfnew, newPanel);
            }
        }
    }
}

bool
gvle_win::insideSrc(QModelIndex index)
{
    QString filePath = mProjectFileSytem->filePath(index);
    QString pkgPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    QFile srcFile(pkgPath + "/src");
    return filePath.indexOf(srcFile.fileName()) == 0;
}

bool
gvle_win::insideOut(QModelIndex index)
{
    QString filePath = mProjectFileSytem->filePath(index);
    QString pkgPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    QFile srcFile(pkgPath + "/out");
    return filePath.indexOf(srcFile.fileName()) == 0;
}

bool
gvle_win::insideExp(QModelIndex index)
{
    QString filePath = mProjectFileSytem->filePath(index);
    QString pkgPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    QFile srcFile(pkgPath + "/exp");
    return filePath.indexOf(srcFile.fileName()) == 0;
}

bool
gvle_win::insideData(QModelIndex index)
{
    QString filePath = mProjectFileSytem->filePath(index);
    QString pkgPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    QFile srcFile(pkgPath + "/data");
    return filePath.indexOf(srcFile.fileName()) == 0;
}

void
gvle_win::onCurrentChanged(const QModelIndex& /*index*/)
{
    mProjectFileSytem->setReadOnly(true);
}

void
gvle_win::onFileRenamed(const QString& path,
                        const QString& oldName,
                        const QString& newName)
{
    QFileInfo oldFileInfo = QFileInfo(path + "/" + oldName);
    QFileInfo newFileInfo = QFileInfo(path + "/" + newName);
    gvle_file gfOld = getGvleFileFromFileInfo(oldFileInfo);
    gvle_file gfNew = getGvleFileFromFileInfo(newFileInfo);
    gfNew.plug_name = gfOld.plug_name;
    gfNew.plug_type = gfOld.plug_type;

    if (newFileInfo.suffix() != oldFileInfo.suffix()) {
        // reset old name: not handled by gvle
        QFile::rename(path + "/" + newName, path + "/" + oldName);
    } else {
        QFile::rename(gfOld.source_file, gfNew.source_file);
        QFile::rename(gfOld.metadata_file, gfNew.metadata_file);
        tabClose(findTabIndex(gfOld));
        PluginMainPanel* panel = gfNew.newInstanceMainPanel(mGvlePlugins);
        if (panel) {
            panel->init(gfNew, &mCurrPackage, mLogger, &mGvlePlugins, mCtx);
            delete panel;
        }
    }
    mProjectFileSytem->setReadOnly(true);
}

void gvle_win::onDataChanged(QModelIndex /*indexTL*/, QModelIndex /*indexBR*/)
{

    mProjectFileSytem->setReadOnly(true);
}

void
gvle_win::onItemChanged(QTreeWidgetItem* /*item*/, int /*col*/)
{
}

bool
gvle_win::removeDir(const QString& dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH (QFileInfo info,
                   dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                       QDir::Hidden | QDir::AllDirs |
                                       QDir::Files,
                                     QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void
gvle_win::copyFile(QModelIndex index)
{

    gvle_file gf = getGvleFileFromFSIndex(index);
    gvle_file gfcopy = gvle_file::getCopy(mCurrPackage, gf);
    QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();
    QFile::copy(gf.source_file, gfcopy.source_file);
    QFile::copy(gf.metadata_file, gfcopy.metadata_file);
    PluginMainPanel* panel = gfcopy.newInstanceMainPanel(mGvlePlugins);
    if (panel) {
        panel->init(gfcopy, &mCurrPackage, mLogger, &mGvlePlugins, mCtx);
        delete panel;
    }
}

void
gvle_win::removeFile(QModelIndex index)
{
    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";

    fileName.replace(currentDir, "");

    gvle_file gf = getGvleFileFromFSIndex(index);
    PluginMainPanel* panel = getMainPanel(gf);
    if (panel) {
        QMessageBox::warning(this,
                             "Warning",
                             QString("The file ") + gf.relPath +
                               " is opened. Please close it.",
                             QMessageBox::Ok);
        return;
    }
    QFileInfo selectedFileInfo = QFileInfo(fileName);

    if (not selectedFileInfo.isDir()) {
        // cannot remove dir for the moment (strange behavior)
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("Question"),
                                      tr("Remove ") + gf.relPath + " ?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString fileSuffix = selectedFileInfo.suffix();
            mProjectFileSytem->remove(index);
            gvle_file gf = getGvleFileFromFSIndex(index);
            QFile::remove(gf.metadata_file);
        }
    }
}

QStringList
gvle_win::getDescriptionFileContent()
{
    QStringList fileContent;
    std::string descrPath = mCurrPackage.getDir(vle::utils::PKG_SOURCE);
    if (descrPath == "") {
        return fileContent;
    }
    descrPath += "/Description.txt";
    // Read file
    QFile file(QString(descrPath.c_str()));
    if (!file.open(QFile::ReadOnly)) {
        mLogger->logExt(
          QString("Error opening for read: %1").arg(file.errorString()), true);
        return fileContent;
    }
    QTextStream stream(&file);
    QString line;

    while (not stream.atEnd()) {
        line = stream.readLine();
        fileContent.append(line);
    }
    file.close();
    return fileContent;
}

int
gvle_win::setDescriptionFileContent(const QStringList& content)
{
    std::string descrPath = mCurrPackage.getDir(vle::utils::PKG_SOURCE);
    descrPath += "/Description.txt";

    // Write file
    QFile file(QString(descrPath.c_str()));
    if (!file.open(QFile::WriteOnly)) {
        mLogger->logExt(
          QString("Error opening for write: %1").arg(file.errorString()),
          true);
        return -1;
    }
    QTextStream stream(&file);
    for (QStringList::ConstIterator it = content.begin(); it != content.end();
         ++it) {
        stream << *it;
        endl(stream);
    }
    return 0;
}

int
gvle_win::setPackageToBuildDepends(const QString& pkg, bool add)
{
    QStringList content = getDescriptionFileContent();
    for (QStringList::Iterator it = content.begin(); it != content.end();
         ++it) {
        QString& line = *it;
        if (line.startsWith("Build-Depends:")) {
            line = line.remove("Build-Depends:").remove(" ");

            QStringList dep = line.split(",");
            if (add) {
                if (not dep.contains(pkg)) {
                    dep.append(pkg);
                }
            } else {
                dep.removeAll(pkg);
            }
            dep.removeAll("");
            line = "Build-Depends: ";
            line += dep.join(",");
        }
    }
    return setDescriptionFileContent(content);
}

QStringList
gvle_win::getPackageToBuildDepends()
{
    QStringList content = getDescriptionFileContent();
    for (QStringList::Iterator it = content.begin(); it != content.end();
         ++it) {
        QString line = *it;
        if (line.startsWith("Build-Depends: ")) {
            line.replace("Build-Depends: ", "");
            QStringList dep = line.split(",");
            dep.removeAll("");
            return dep;
        }
    }
    return QStringList();
}

void
gvle_win::onRefreshFiles()
{
    treeProjectUpdate();
}

void
gvle_win::onRightWidgetChanged()
{
    gvle_file gf = getGvleFileFromTabIndex(ui->tabWidget->currentIndex());
    PluginMainPanel* w =
      getMainPanelFromTabIndex(ui->tabWidget->currentIndex());
    if (w) {
        setRightWidget(mPanels[gf.relPath]->rightWidget());
    }

    // ui->rightStack->removeWidget()(right);
}

void
gvle_win::onUndoAvailable(bool b)
{
    gvle_file gf =
      getGvleFileFromMainPanel((PluginMainPanel*)QObject::sender());

    int i = getTabIndex(gf);
    QString tabName;
    if (b) {
        tabName = QString("* %1").arg(gf.relPath);
    } else {
        tabName = gf.relPath;
    }
    ui->tabWidget->setTabText(i, tabName);
}

void
gvle_win::onCheckDistrib()
{
    QAction* senderAct = qobject_cast<QAction*>(QObject::sender());
    QString distrib = senderAct->data().toString();

    QStringList distribToVleConf;
    for (auto& d : mDistributions) {
        if (d.first == distrib.toStdString()) {
            d.second.second = senderAct->isChecked();
        }
        if (d.second.second) {
            distribToVleConf.append(QString(d.second.first.c_str()));
        }
    }
    std::string remoteLine = distribToVleConf.join(",").toStdString();
    mCtx->set_setting("vle.remote.url", remoteLine);
    mCtx->write_settings();
    menuDistributionsRefresh();
}

void
gvle_win::onCheckDependency()
{
    QAction* senderAct = qobject_cast<QAction*>(QObject::sender());
    QString pkg = senderAct->data().toString();
    setPackageToBuildDepends(pkg, senderAct->isChecked());
}

void
gvle_win::onPackageInstall()
{
    QAction* senderAct = qobject_cast<QAction*>(QObject::sender());
    QString pkg = senderAct->data().toString();

    std::string cmd = "vle -R install ";
    cmd += pkg.toStdString();

    std::vector<std::string> argv = mSpawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    try {
        mSpawn.start(
          exe, vle::utils::Path::temp_directory_path().string(), argv);
    } catch (const std::exception& e) {

        mLogger->logExt("Remote install error ", true);
    }

    ui->menuPackages->setEnabled(false);
    mTimerRemote.start(2);
}

void
gvle_win::onPackageUninstall()
{
    QAction* senderAct = qobject_cast<QAction*>(QObject::sender());
    QString pkg = senderAct->data().toString();

    mLogger->log(QString(
      utils::format("Uninstall package '%s' ", pkg.toStdString().c_str())
        .c_str()));
    statusWidgetOpen();
    utils::Package pack(mCtx, pkg.toStdString());
    bool success = true;

    try {
        pack.rclean();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        success = false;
    }
    if (success) {
        mLogger->log(tr("Uninstall complete"));
    } else {
        mLogger->log(tr("Uninstall failed"));
    }

    menuLocalPackagesRefresh();
}

void
gvle_win::projectInstall()
{
    mLogger->log(tr("Project installation started"));

    try {
        mCurrPackage.install();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project installation failed"));
        return;
    }

    mTimerInstall.start(2);
}

void
gvle_win::openMainPanel(gvle_file gf, PluginMainPanel* panel)
{

    panel->init(gf, &mCurrPackage, mLogger, &mGvlePlugins, mCtx);
    int n = ui->tabWidget->addTab(panel->leftWidget(), gf.relPath);
    bool oldBlock = ui->tabWidget->blockSignals(true);
    ui->tabWidget->setCurrentIndex(n);
    ui->tabWidget->widget(n)->setProperty("relPath", gf.relPath);
    ui->tabWidget->blockSignals(oldBlock);
    mPanels.insert(gf.relPath, panel);
    panel->leftWidget()->show();

    // Create a new toolbox for the right column
    setRightWidget(panel->rightWidget());

    QObject::connect(
      panel, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
    QObject::connect(
      panel, SIGNAL(rightWidgetChanged()), this, SLOT(onRightWidgetChanged()));
    // set undo redo enabled
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(true);
}

void
gvle_win::setRightWidget(QWidget* rightWidget)
{
    int stackSize = ui->rightStack->count();
    if (stackSize < 1 or stackSize > 2) {
        qDebug() << " Internal error gvle_win::setRightWidget ";
        return;
    }

    if (stackSize == 2) {
        ui->rightStack->removeWidget(ui->rightStack->widget(1));
    }
    if (rightWidget) {
        ui->rightStack->addWidget(rightWidget);
        ui->rightStack->setCurrentIndex(1);
        rightWidget->show();
    } else {
        ui->rightStack->setCurrentIndex(0);
    }
}

int
gvle_win::findTabIndex(gvle_file gf)
{
    QString relPathModified = QString("* %1").arg(gf.relPath);
    QString currentPath;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        currentPath = ui->tabWidget->tabText(i);
        if (currentPath == gf.relPath) {
            return i;
        }
        if (currentPath == relPathModified) {
            return i;
        }
    }
    return -1;
}
}
} // namespaces
