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

#include <iostream>

#include <QFileDialog>
#include <QStyleFactory>
#include <QActionGroup>
#include <QMessageBox>
#include <QDirIterator>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

#include "gvle_win.h"
#include "ui_gvle_win.h"
#include "plugin_mainpanel.h"
#include "help.h"
#include "aboutbox.h"
#include "filevpzview.h"
#include "vlevpm.h"
#include "DefaultVpzPanel.h"
#include "DefaultCppPanel.h"
#include "plugin_cond.h"
#include "plugin_output.h"


namespace vu = vle::utils;

namespace vle {
namespace gvle {

gvle_win::gvle_win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gvleWin)
{

    mTimer = 0;
    mLogger = 0;
    mSimOpened = false;
    // VLE init
    mCurrPackage.refreshPath();

    // GUI init
    ui->setupUi(this);

    mProjectFileSytem = new QFileSystemModel(this);
    QTreeView *tree = ui->treeProject;
    tree->setModel(mProjectFileSytem);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    QItemSelectionModel *selmod = tree->selectionModel();

    // Open the configuration file
    std::string configFile = vu::Path::path().getHomeFile("gvle.conf");
    mSettings = new QSettings(QString(configFile.c_str()),QSettings::IniFormat);
    menuRecentProjectRefresh();

    QObject::connect(ui->actionNewProject,
                     SIGNAL(triggered()), this,
                     SLOT(onNewProject()));
    QObject::connect(ui->actionOpenProject,
                     SIGNAL(triggered()), this,
                     SLOT(onOpenProject()));
    QObject::connect(ui->actionRecent1,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectRecent1()));
    QObject::connect(ui->actionRecent2,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectRecent2()));
    QObject::connect(ui->actionRecent3,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectRecent3()));
    QObject::connect(ui->actionRecent4,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectRecent4()));
    QObject::connect(ui->actionRecent5,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectRecent5()));
    QObject::connect(ui->actionCloseProject,
                     SIGNAL(triggered()), this,
                     SLOT(onCloseProject()));
    QObject::connect(ui->actionSaveFile,
                     SIGNAL(triggered()), this,
                     SLOT(onSaveFile()));
    QObject::connect(ui->actionQuit,
                     SIGNAL(triggered()), this,
                     SLOT(onQuit()));
    QObject::connect(ui->actionUndo,
                     SIGNAL(triggered()), this,
                     SLOT(onUndo()));
    QObject::connect(ui->actionRedo,
                     SIGNAL(triggered()), this,
                     SLOT(onRedo()));
    QObject::connect(ui->actionConfigureProject,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectConfigure()));
    QObject::connect(ui->actionBuildProject,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectBuild()));
    QObject::connect(ui->actionCleanProject,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectClean()));
    QObject::connect(ui->actionUninstall,
                     SIGNAL(triggered()), this,
                     SLOT(onProjectUninstall()));
    QObject::connect(ui->actionSimNone,
                     SIGNAL(toggled(bool)), this,
                     SLOT(onSelectSimulator(bool)));
    QObject::connect(ui->actionHelp,
                     SIGNAL(triggered()), this,
                     SLOT(onHelp()));
    QObject::connect(ui->actionAbout,
                     SIGNAL(triggered()), this,
                     SLOT(onAbout()));

    QObject::connect(ui->treeProject,
                     SIGNAL(doubleClicked(QModelIndex)), this,
                     SLOT(onTreeDblClick(QModelIndex)));
    QObject::connect(ui->treeProject,
                     SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(onCustomContextMenu(const QPoint &)));
    // QObject::connect(ui->treeProject,
    //                  SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
    //                  SLOT(onItemChanged(QTreeWidgetItem *, int)));
    QObject::connect(mProjectFileSytem,
                     SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
                     SLOT(onDataChanged(QModelIndex, QModelIndex)));
    QObject::connect(mProjectFileSytem,
                     SIGNAL(fileRenamed(const QString &,
                                        const QString &,
                                        const QString &)),this,
                     SLOT(onFileRenamed(const QString &,
                                        const QString &,
                                        const QString &)));

    QObject::connect(selmod,
                     SIGNAL(currentChanged(const QModelIndex &,
                                           const QModelIndex &)), this,
                     SLOT(onCurrentChanged(const QModelIndex &)));

    QObject::connect(ui->tabWidget,
                     SIGNAL(currentChanged(int)), this,
                     SLOT(onTabChange(int)));
    QObject::connect(ui->tabWidget,
                     SIGNAL(tabCloseRequested(int)), this,
                     SLOT(onTabClose(int)));

    QObject::connect(ui->statusTitleToggle,
                     SIGNAL(clicked()), this, SLOT(onStatusToggle()));

    // Initiate an mutual exclusive selection on simulators menu
    mMenuSimGroup = new QActionGroup(this);
    ui->actionSimNone->setActionGroup(mMenuSimGroup);

    mLogger = new Logger();
    mLogger->setWidget(ui->statusLog);

    // Update window title
    setWindowTitle("GVLE");
    //
    QList<int> sizes;
    sizes.append(200);
    sizes.append(500);
    sizes.append(200);
    ui->splitter->setSizes(sizes);


    mGvlePlugins.registerPlugins();

}

gvle_win::~gvle_win()
{
    mSettings->sync();
    delete mSettings;

    while(ui->tabWidget->count())
    {
        QWidget *w = ui->tabWidget->widget(0);

        ui->tabWidget->removeTab(0);
        delete w;
    }
    delete ui;
}

void
gvle_win::closeEvent(QCloseEvent *event)
{
    if (closeProject()) {
        event->accept();
    } else {
        event->ignore();
    }
}


void
gvle_win::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    statusWidgetClose();
}



void
gvle_win::onNewProject()
{
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
    if (FileChooserDialog.exec())
        openProject(FileChooserDialog.selectedFiles().first());
}
void
gvle_win::onProjectRecent1()
{
    QVariant path = mSettings->value("Projects/recent1");
    if (path.isValid())
        openProject(path.toString());
}
void
gvle_win::onProjectRecent2()
{
    QVariant path = mSettings->value("Projects/recent2");
    if (path.isValid())
        openProject(path.toString());
}
void
gvle_win::onProjectRecent3()
{
    QVariant path = mSettings->value("Projects/recent3");
    if (path.isValid())
        openProject(path.toString());
}
void
gvle_win::onProjectRecent4()
{
    QVariant path = mSettings->value("Projects/recent4");
    if (path.isValid())
        openProject(path.toString());
}
void
gvle_win::onProjectRecent5()
{
    QVariant path = mSettings->value("Projects/recent5");
    if (path.isValid())
        openProject(path.toString());
}

void
gvle_win::newProject(QString pathName)
{
    QDir dir(pathName);
    std::string basename = dir.dirName().toStdString ();
    dir.cdUp();
    QDir::setCurrent( dir.path() );

    mCurrPackage.select(basename);

    mLogger->log(QString("New Project %1").arg(dir.dirName()));

    // Update window title
    setWindowTitle(QString("GVLE - ") + basename.c_str());

    mCurrPackage.select(basename);
    mCurrPackage.create();
    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);
}

/**
 * @brief gvle_win::openProject
 *        Handler for menu function : File > Open Project
 */
void
gvle_win::openProject(QString pathName)
{
    QDir dir(pathName);
    std::string basename = dir.dirName().toStdString ();
    dir.cdUp();
    QDir::setCurrent( dir.path() );

    mCurrPackage.select(basename);

    mLogger->log(QString("Open Project %1").arg(mCurrPackage.name().c_str()));

    // Update window title
    setWindowTitle(QString("GVLE - ") + mCurrPackage.name().c_str());

    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->menuProject->setEnabled(true);
}

bool
gvle_win::closeProject()
{
    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--) {

        QWidget *w = ui->tabWidget->widget(i);

        QVariant tabType = w->property("type");
        QVariant rtool = w->property("wTool");

        if (tabType.isValid()) {
            if (not tabClose(i)) {
                return false;
            }
        }

        if (rtool.isValid()) {
            setRightWidget(0);
        }
    }

    // Update menus
    ui->menuProject->setEnabled(false);
    // Update window title
    setWindowTitle("GVLE");

    mLogger->log(QString("Project closed"));

    return true;
}

PluginMainPanel*
gvle_win::getMainPanelFromTabIndex(int index)
{
    QString relPath = getRelPathFromTabIndex(index);
    if (mPanels.contains(relPath)) {
        return mPanels[relPath];
    }
    return 0;
}

QString
gvle_win::getRelPathFromTabIndex(int index)
{
    QString relPath = ui->tabWidget->tabText(index);
    if (relPath.startsWith("* ")) {
        relPath = relPath.remove("* ");
    }
    return relPath;
}

void gvle_win::onCloseProject()
{
    closeProject();
}



void
gvle_win::onSaveFile()
{
    QString relPath = getRelPathFromTabIndex(ui->tabWidget->currentIndex());
    mPanels[relPath]->save();
}

/**
 * @brief gvle_win::onQuit
 *        Handler for menu function : File > Quit
 */
void gvle_win::onQuit()
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
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project configuration failed"));
        return;
    }
    ui->menuProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectConfigureTimer()));
    mTimer->start(2);
}

void
gvle_win::projectConfigureTimer()
{
    if (mCurrPackage.isFinish())
    {
        mTimer->stop();
        delete mTimer;
        mLogger->log(tr("Project configuration complete"));
        ui->menuProject->setEnabled(true);
    }
    std::string oo;
    std::string oe;
    bool ret = mCurrPackage.get(&oo, &oe);
    if (ret)
    {
        if(oe.length())
            mLogger->logExt(oe.c_str(), true);
        if (oo.length())
            mLogger->logExt(oo.c_str());
    }
}

void
gvle_win::onProjectBuild()
{
    mLogger->log(tr("Project compilation started"));
    statusWidgetOpen();

    try {
        mCurrPackage.build();
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project compilation failed"));
        return;
    }
    ui->menuProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectBuildTimer()));
    mTimer->start(2);
}

void
gvle_win::onProjectClean()
{
    mLogger->log(tr("Project clean started"));
    statusWidgetOpen();

    try {
        mCurrPackage.clean();
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project clean failed"));
        return;
    }
    ui->menuProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectCleanTimer()));
    mTimer->start(2);
}

void
gvle_win::onProjectUninstall()
{
    mLogger->log(tr("Project uninstall started"));
    statusWidgetOpen();

    try {
        mCurrPackage.rclean();
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project uninstall failed"));
        return;
    }
    ui->menuProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectUninstallTimer()));
    mTimer->start(2);
}


void
gvle_win::projectBuildTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if(oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimer->stop();
        delete mTimer;
        if (mCurrPackage.get(&oo, &oe)) {
            if(oe.length()) {
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
    }
}

void
gvle_win::projectInstallTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if(oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimer->stop();
        delete mTimer;
        if (mCurrPackage.isSuccess()) {
            mLogger->log(tr("Project installation complete"));
        } else {
            mLogger->log(tr("Project installation failed"));
        }
        ui->menuProject->setEnabled(true);
    }
}

void
gvle_win::projectCleanTimer()
{
    if (mCurrPackage.isFinish())
    {
        mTimer->stop();
        delete mTimer;
        mLogger->log(tr("Project clean complete"));
        ui->menuProject->setEnabled(true);
    }
    std::string oo;
    std::string oe;
    bool ret = mCurrPackage.get(&oo, &oe);
    if (ret)
    {
        if(oe.length())
            mLogger->logExt(oe.c_str(), true);
        if (oo.length())
            mLogger->logExt(oo.c_str());
    }
}

void
gvle_win::projectUninstallTimer()
{
    if (mCurrPackage.isFinish())
    {
        mTimer->stop();
        delete mTimer;
        mLogger->log(tr("Project uninstall complete"));
        ui->menuProject->setEnabled(true);
    }
    std::string oo;
    std::string oe;
    bool ret = mCurrPackage.get(&oo, &oe);
    if (ret)
    {
        if(oe.length())
            mLogger->logExt(oe.c_str(), true);
        if (oo.length())
            mLogger->logExt(oo.c_str());
    }
}

void
gvle_win::onUndo()
{
    QString relPath = ui->tabWidget->currentWidget()->property("relPath").toString();
    mPanels[relPath]->undo();
}

void
gvle_win::onRedo()
{
    QString relPath = ui->tabWidget->currentWidget()->property("relPath").toString();
    mPanels[relPath]->redo();

}

void
gvle_win::onSelectSimulator(bool isChecked)
{
    QAction *act = (QAction *)sender();

    if (isChecked)
    {
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
    bool  helpOpened = false;
    help *tabHelp = 0;
    int   idx = 0;

    // Get current widget to detect contextual help entry
    //QWidget *wid = QApplication::focusWidget();

    // Search if an Help tab is already open
    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--)
    {
        QWidget *w = ui->tabWidget->widget(i);
        QVariant tabType = w->property("type");
        if (tabType.isValid() && (tabType.toString() == "help"))
        {
            tabHelp = (help *)w;
            idx = i;
            helpOpened = true;
            break;
        }
    }
    // If no help tab found, create a new one
    if ( ! helpOpened)
    {
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
gvle_win::menuRecentProjectSet(QString path, QAction *menu)
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

    QSet <QString> recentList;
    recentList.insert(path);

    if (v1.isValid()) recentList.insert(v1.toString());
    if (v2.isValid()) recentList.insert(v2.toString());
    if (v3.isValid()) recentList.insert(v3.toString());
    if (v4.isValid()) recentList.insert(v4.toString());
    if (v5.isValid()) recentList.insert(v5.toString());

    QSetIterator<QString> i(recentList);
    if (i.hasNext()) mSettings->setValue("Projects/recent1", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent2", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent5", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent4", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent3", i.next());

    mSettings->sync();
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
    QString relPath = getRelPathFromTabIndex(index);
    PluginMainPanel* w = getMainPanelFromTabIndex(index);
    if (not w) {
        removeTab(index);
        return true;
    }
    QString canBeClosed =  w->canBeClosed();
    if (canBeClosed != "") {
        QMessageBox msgBox;
        msgBox.setText(
                QString("This file cannot be closed: %1").arg(canBeClosed));
        msgBox.exec();
        return false;
    }
    QString tabText = ui->tabWidget->tabText(index);
    if (tabText.startsWith("* ")) {
        QMessageBox msgBox;
        msgBox.setText("Save file before closing it ?");
        msgBox.addButton(QMessageBox::Save);
        msgBox.addButton(QMessageBox::Discard);
        msgBox.addButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            w->save();
            mPanels.remove(relPath);
            removeTab(index);
            delete w;
            break;
        case QMessageBox::Discard:
            mPanels.remove(relPath);
            removeTab(index);
            delete w;
            break;
        case QMessageBox::Cancel:
            break;
        default:
            // should never be reached
            break;
        }
    } else {
        mPanels.remove(relPath);
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
    PluginMainPanel* w = getMainPanelFromTabIndex(ui->tabWidget->currentIndex());
    if (w)  {
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
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void
gvle_win::statusWidgetClose()
{
    ui->statusLog->hide();
    QSize titleSize = ui->statusTitleFrame->size();
    ui->statusWidget->setFixedHeight(titleSize.height());
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

/* ---------- Manage the Project navigator ---------- */

void
gvle_win::treeProjectUpdate()
{
    QTreeView* projectTreeView = ui->treeProject;
    QString projectName(vu::Path::filename(mCurrPackage.name()).c_str());

    QModelIndex projectIndex = mProjectFileSytem->setRootPath(projectName);

    projectTreeView->setRootIndex(projectIndex);

    projectTreeView->resizeColumnToContents(0);
}

QString
gvle_win::treeProjectRelativePath(const QModelIndex index) const
{
    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";

    fileName.replace(currentDir, "");

    QString relPath;
    QStringList fileNameSplit = fileName.split("/");

    for (int i=1;i<fileNameSplit.length();i++) {
        if (i>1) {
            relPath += "/";
        }
        relPath += fileNameSplit.at(i);
    }
    return relPath;
}

void
gvle_win::onTreeDblClick(QModelIndex index)
{
    if (not index.isValid()) return;

    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";
    fileName.replace(currentDir, "");
    QFileInfo selectedFileInfo = QFileInfo(fileName);
    QString relPath = treeProjectRelativePath(index);

    int alreadyOpened = findTabIndex(relPath);
    if (alreadyOpened != -1) {
        ui->tabWidget->setCurrentIndex(alreadyOpened);
        return ;
    }

    PluginMainPanel* newPanel = 0;
    if (selectedFileInfo.suffix() == "vpz") {
        //TODO check if a plugin
        newPanel = new DefaultVpzPanel();
        QObject::connect(newPanel, SIGNAL(rightWidgetChanged()),
                         this, SLOT(onRightWidgetChanged()));

    } else if ((selectedFileInfo.suffix() == "cpp") or
               (selectedFileInfo.suffix() == "hpp")){
        QString plug = getCppPlugin(relPath);
        if (plug == "") {
            newPanel = new DefaultCppPanel();
        } else {
            newPanel = mGvlePlugins.newInstanceMainPanelPlugin(plug);
        }


    }
    if (newPanel) {
        QObject::connect(newPanel, SIGNAL(undoAvailable(bool)),
                         this, SLOT(onUndoAvailable(bool)));

        newPanel->init(relPath, &mCurrPackage, mLogger, &mGvlePlugins);

        int n = ui->tabWidget->addTab(newPanel->leftWidget(), relPath);
        bool oldBlock = ui->tabWidget->blockSignals(true);
        ui->tabWidget->setCurrentIndex(n);
        ui->tabWidget->widget(n)->setProperty("relPath",relPath);
        ui->tabWidget->blockSignals(oldBlock);
        mPanels.insert(relPath,newPanel);
        newPanel->leftWidget()->show();

        // Create a new toolbox for the right column
        setRightWidget(newPanel->rightWidget());
    }
    //set undo redo enabled
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(true);
}

void
gvle_win::onCustomContextMenu(const QPoint &point)
{
    QTreeView* projectTreeView = ui->treeProject;

    QPoint globalPos = projectTreeView->mapToGlobal(point);
    QModelIndex index = projectTreeView->indexAt(point);
    if (not index.isValid()) return;

    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";
    fileName.replace(currentDir, "");
    QFileInfo selectedFileInfo = QFileInfo(fileName);

    QAction *lastAction;

    QMenu ctxMenu;
    lastAction = ctxMenu.addAction(tr("Remove File"));
    lastAction->setData(1);
    lastAction = ctxMenu.addAction(tr("Rename File"));
    lastAction->setData(2);
    if (selectedFileInfo.suffix() != "vpz") {
        lastAction->setDisabled(true);
    }
    lastAction = ctxMenu.addAction(tr("Copy File"));
    lastAction->setData(3);
    if (selectedFileInfo.suffix() != "vpz") {
        lastAction->setDisabled(true);
    }
    if (insideSrc(index)) {

        QStringList plugList = mGvlePlugins.getMainPanelPluginsList();
        for (int i =0; i<plugList.size(); i++) {
            QString pluginName = plugList.at(i);
            lastAction = ctxMenu.addAction(tr("New") + " " + pluginName);
            lastAction->setProperty("srcPlugin",pluginName);
        }
    }

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == 1) {
            removeFile(index);
        } else if (actCode == 2) {
            mProjectFileSytem->setReadOnly(false);
            projectTreeView->edit(index);
        } else if (actCode == 3) {
            mProjectFileSytem->setReadOnly(false);
            copyFile(index);
        } else {
            QVariant plugName = selectedItem->property("srcPlugin");
            if ( ! plugName.isValid()) {
                return;
            }

            //TODO check if a plugin
            PluginMainPanel* newPanel = mGvlePlugins.newInstanceMainPanelPlugin(
                    plugName.toString());

            QObject::connect(newPanel, SIGNAL(undoAvailable(bool)),
                             this, SLOT(onUndoAvailable(bool)));

            QString relPath = getNewCpp();

            newPanel->init(relPath, &mCurrPackage, mLogger, &mGvlePlugins);

            int n = ui->tabWidget->addTab(newPanel->leftWidget(), relPath);
            bool oldBlock = ui->tabWidget->blockSignals(true);
            ui->tabWidget->setCurrentIndex(n);
            ui->tabWidget->widget(n)->setProperty("relPath",relPath);
            ui->tabWidget->blockSignals(oldBlock);

            mPanels.insert(relPath,newPanel);
            newPanel->leftWidget()->show();

            // Create a new toolbox for the right column
            setRightWidget(newPanel->rightWidget());

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

QString
gvle_win::getNewCpp()
{
    QString pkgPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    QString cppPrefix = "NewCpp";
    QString relPath = QString("src/")+cppPrefix+".cpp";

    if (not QFile(pkgPath + "/" + relPath).exists()) {
        return relPath;
    }
    unsigned int i = 1 ;
    bool found = false;
    while (not found) {
        relPath = QString("src/")+cppPrefix+"_"+QVariant(i).toString()+".cpp";
        found = not QFile(pkgPath + "/" + relPath).exists();
        i++;
    }
    return relPath;
}

QString
gvle_win::getCppPlugin(QString relPath)
{
    QString metaPath = QString(mCurrPackage.getDir(utils::PKG_SOURCE).c_str());
    metaPath += "/metadata/"+relPath;
    metaPath.replace(".cpp",".sm");
    QFile file(metaPath);
    if (not QFile(metaPath).exists()) {
        return "";
    }
    QDomDocument dom("vle_project_metadata");
    QXmlInputSource source(&file);
    QXmlSimpleReader reader;
    dom.setContent(&source, &reader);
    QDomElement docElem = dom.documentElement();
    QDomNode srcPluginNode = dom.elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("name").nodeValue();

}

void
gvle_win::onCurrentChanged(const QModelIndex& /*index*/)
{
    mProjectFileSytem->setReadOnly(true);
}


void
gvle_win::onFileRenamed(const QString & path,
                              const QString & oldName, const QString & newName)
{
    QFileInfo oldFileInfo = QFileInfo(path + "/" + oldName);
    QFileInfo newFileInfo = QFileInfo(path + "/" + newName);

    if (newFileInfo.suffix() != "vpz") {
        QFile::rename(path + "/" + newName,
                      path + "/" + oldName);
    } else {
        QString newVpm = newName;
        newVpm.replace(".vpz", ".vpm");
        QString oldVpm = oldName;
        oldVpm.replace(".vpz", ".vpm");

        QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();

        QString pathOldVpm = basepath + "/metadata/exp/" + oldVpm;
        QString pathNewVpm = basepath + "/metadata/exp/" + newVpm;

        QFile::rename(pathOldVpm,
                      pathNewVpm);
    }

    mProjectFileSytem->setReadOnly(true);
}

void
gvle_win::onDataChanged(QModelIndex /*indexTL*/, QModelIndex /*indexBR*/)
{
    mProjectFileSytem->setReadOnly(true);
}

void
gvle_win::onItemChanged(QTreeWidgetItem* /*item*/, int /*col*/)
{
}


bool
gvle_win::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
                                                    QDir::System |
                                                    QDir::Hidden |
                                                    QDir::AllDirs |
                                                    QDir::Files,
                                                    QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
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

void gvle_win::copyFile(QModelIndex index)
{
    QString fileName = mProjectFileSytem->filePath(index);
    QFileInfo fileInfo = QFileInfo(fileName);
    QString suffix = fileInfo.suffix();
    QString baseName = fileInfo.baseName();
    QString dirPath = fileInfo.absoluteDir().absolutePath();

    int counter = 1;
    QString addCounter = "";
    while (QFile(dirPath + "/" + baseName + "_copy" +
                 addCounter+ "." + suffix).exists()){
        counter++;
        addCounter = QString::number(counter);
    }

    QFile::copy(fileName,
                dirPath + "/" + baseName + "_copy" +
                addCounter + "." + suffix);

    QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();
    QString pathOldVpm = basepath + "/metadata/exp/" + baseName + ".vpm";
    QString pathNewVpm = basepath + "/metadata/exp/" + baseName + "_copy" +
        addCounter + ".vpm";
    QFile::copy(pathOldVpm, pathNewVpm);
}

void gvle_win::removeFile(QModelIndex index)
{
    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";

    fileName.replace(currentDir, "");

    int i;
    for (i = 0; i < ui->tabWidget->count(); i++)
    {
        if (ui->tabWidget->tabText(i) == fileName)
            break;
    }

    if (i != ui->tabWidget->count()) {
        QMessageBox::question(this, tr("Warning"),
                              fileName +
                              " is currently being edited,\n"   \
                              "you don't want to remove it!");
    } else {

        QFileInfo selectedFileInfo = QFileInfo(fileName);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Question"),
                                      tr("Remove ") +
                                      fileName + " ?",
                                      QMessageBox::Yes|
                                      QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (selectedFileInfo.isDir()) {
                mProjectFileSytem->rmdir(index);
            } else {
                mProjectFileSytem->remove(index);
                if (selectedFileInfo.suffix() == "vpz") {
                    QString relPath;
                    relPath = treeProjectRelativePath(index);
                    QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();
                    QString relPathVpm = relPath;
                    relPathVpm.replace(".vpz", ".vpm");
                    QString fullPathVpm = basepath + "/metadata/" + relPathVpm;
                    QFile::remove(fullPathVpm);
                }
                if (selectedFileInfo.suffix() == "src") {
                    QString relPath;
                    relPath = treeProjectRelativePath(index);
                    QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();
                    QString relPathVpm = relPath;
                    relPathVpm.replace(".src", ".sm");
                    QString fullPathVpm = basepath + "/metadata/" + relPathVpm;
                    QFile::remove(fullPathVpm);
                }
            }
        }
    }
}

void gvle_win::onRefreshFiles()
{
    qDebug() << "gvle_win::onRefreshFiles()";
    treeProjectUpdate();
}

void
gvle_win::onRightWidgetChanged()
{
    QString relPath = getRelPathFromTabIndex(ui->tabWidget->currentIndex());
    PluginMainPanel* w = getMainPanelFromTabIndex(ui->tabWidget->currentIndex());
    if (w) {
        setRightWidget(mPanels[relPath]->rightWidget());
    }

    //ui->rightStack->removeWidget()(right);
}

void
gvle_win::onUndoAvailable(bool b)
{
    int curr = ui->tabWidget->currentIndex();
    QString relPath = ui->tabWidget->widget(curr)->property("relPath").toString();
    QString tabName;
    if (b) {
        tabName = QString("* %1").arg(relPath);
    } else {
        tabName = relPath;
    }
    ui->tabWidget->setTabText(curr, tabName);
}

void gvle_win::projectInstall()
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

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()),
                     this, SLOT(projectInstallTimer()));
    mTimer->start(2);
}

void
gvle_win::setRightWidget(QWidget* rightWidget)
{
    int stackSize = ui->rightStack->count();
    if (stackSize < 1 or stackSize > 2) {
        qDebug() << " Internal error gvle_win::setRightWidget ";
        return ;
    }

    if (stackSize == 2){
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
gvle_win::findTabIndex(QString relPath)
{
    QString relPathModified = QString("* %1").arg(relPath);
    QString currentPath;
    for (int i = 0; i< ui->tabWidget->count(); i++) {
        currentPath = ui->tabWidget->tabText(i);
        if (currentPath == relPath) {
            return i;
        }
        if (currentPath == relPathModified) {
            return i;
        }
    }
    return -1;
}


}} //namespaces
