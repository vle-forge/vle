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

#include <QFileDialog>
#include <QStyleFactory>
#include <QActionGroup>
#include <QMessageBox>
#include <QDirIterator>
#include <QDir>

#include <iostream>

#include "gvle_win.h"
#include "ui_gvle_win.h"
#include "plugin_modeler.h"
#include "plugin_sim.h"
#include "help.h"
#include "aboutbox.h"
#include "filevpzview.h"
#include "simulation.h"
#include "vlevpm.h"
#include <QtDebug>
#include <QDebug>

namespace vu = vle::utils;

namespace vle {
namespace gvle {

gvle_win::gvle_win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gvleWin)
{

    mVpm = 0;
    mTimer = 0;
    mLogger = 0;
    mSimOpened = false;
    mCurrentSimPlugin = 0;
    mPackage = 0;
    // VLE init
    mCurrPackage.refreshPath();
    //
    mModelers.clear();

    // GUI init
    ui->setupUi(this);

    mProjectFileSytem = new QFileSystemModel(this);
    QTreeView *tree = ui->treeProject;
    tree->setModel(mProjectFileSytem);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->header()->setResizeMode(0, QHeaderView::ResizeToContents);

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
    QObject::connect(ui->actionLaunchSimulation,
                     SIGNAL(triggered()), this,
                     SLOT(onLaunchSimulation()));
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

    loadPlugins();
    ui->menuSelectSimulator->setEnabled(true);

}

gvle_win::~gvle_win()
{
    mSettings->sync();
    delete mSettings;
    if (mVpm)
        delete mVpm;

    while(ui->tabWidget->count())
    {
        QWidget *w = ui->tabWidget->widget(0);

        ui->tabWidget->removeTab(0);
        delete w;
    }
    delete ui;
}

void gvle_win::closeEvent(QCloseEvent *event)
{
    if (closeProject()) {
        event->accept();
    } else {
        event->ignore();
    }
}


void gvle_win::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    statusWidgetClose();
}

void gvle_win::loadPlugins()
{
    QString pathgvlec = "plugins/gvle/condition";
    QString pathgvles = "plugins/gvle/simulating";
    QString pathgvlem = "plugins/gvle/modeling";
    QString pathgvleo = "plugins/gvle/output";

    QString packagesDir = vu::Path::path().getBinaryPackagesDir().c_str();

    QDirIterator it(packagesDir, QDir::AllDirs);

    while (it.hasNext()) {
        it.next();
        if (QDir(it.filePath() + "/" + pathgvlec).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvlec, QDir::Files);
            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject *plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    continue;
                }
                PluginExpCond *expcond = qobject_cast<PluginExpCond *>(plugin);
                if (expcond) {
                    loadExpCondPlugins(expcond, libName);
                }
                loader.unload();
            }
        }

        if (QDir(it.filePath() + "/" + pathgvlem).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvlem, QDir::Files);
            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject *plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    continue;
                }
                PluginModeler *modeling = qobject_cast<PluginModeler *>(plugin);
                if (modeling) {
                    loadModelerPlugins(modeling, libName);
                }
                loader.unload();
            }
        }

        if (QDir(it.filePath() + "/" + pathgvles).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvles, QDir::Files);
            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject *plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    continue;
                }
                PluginSimulator *simulating = qobject_cast<PluginSimulator *>(plugin);
                if (simulating) {
                    loadSimulationPlugins(simulating, libName);
                }
                loader.unload();
            }
        }

        if (QDir(it.filePath() + "/" + pathgvleo).exists()) {
            QDirIterator itbis(it.filePath() + "/" + pathgvleo, QDir::Files);

            while (itbis.hasNext()) {
                QString libName =  itbis.next();
                QPluginLoader loader(libName);
                QObject *plugin = loader.instance();
                if ( ! loader.isLoaded()) {
                    continue;
                }
                PluginOutput *simulating = qobject_cast<PluginOutput *>(plugin);
                if (simulating) {
                    loadOutputPlugins(simulating, libName);
                }
                loader.unload();
            }
        }
    }
}

void gvle_win::loadExpCondPlugins(PluginExpCond *plugin, QString fileName)
{
    qDebug() << " - Found experimental condition plugin: " << plugin->getname();
    mExpPlugins.insert(plugin->getname(), fileName);
}

void gvle_win::loadModelerPlugins(PluginModeler *plugin, QString fileName)
{
    qDebug() << " - Found modeling plugin: " << plugin->getname();
    mModelers.append(fileName);

    QMenu *newPluginMenu = ui->menuModeling->addMenu(plugin->getname());

    QAction *newAct = newPluginMenu->addAction(tr("New class"));
    newAct->setProperty("fileName", fileName);
    QAction *updAct = newPluginMenu->addAction(tr("Update"));
    updAct->setEnabled(false);
    newPluginMenu->addSeparator();
    QAction *openAct = newPluginMenu->addAction(tr("Open"));
    openAct->setProperty("fileName", fileName);

    QObject::connect(newAct,  SIGNAL(triggered()), this, SLOT(onNewModelerClass()));
    QObject::connect(openAct, SIGNAL(triggered()), this, SLOT(onOpenModeler()));
}

void gvle_win::loadSimulationPlugins(PluginSimulator *sim, QString fileName)
{
    qDebug() << " - Found simulating plugin: " << sim->getname();
    mLogger->log(QString("Load simulator pluggin : %1").arg(sim->getname()));

    mSimulatorPlugins.insert(sim->getname(), fileName);

    // Update Menu
    QAction *newAct = ui->menuSelectSimulator->addAction(sim->getname());
    newAct->setCheckable(true);
    newAct->setActionGroup(mMenuSimGroup);
    newAct->setObjectName(sim->getname());
    newAct->setData(fileName);
    QObject::connect(newAct, SIGNAL(toggled(bool)), this, SLOT(onSelectSimulator(bool)));
}

void gvle_win::loadOutputPlugins(PluginOutput *out, QString fileName)
{
    qDebug() << " - Found output plugin: " << out->getname();
    mOutputPlugins.insert(out->getname(), fileName);
}

void gvle_win::onNewProject()
{
    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setFileMode(QFileDialog::AnyFile);
    FileChooserDialog.setOptions(QFileDialog::ShowDirsOnly);
    FileChooserDialog.setAcceptMode(QFileDialog::AcceptSave);
    FileChooserDialog.setLabelText(QFileDialog::LookIn,
                                   "Choose a directory");
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
void gvle_win::onOpenProject()
{
    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setFileMode(QFileDialog::Directory);
    if (FileChooserDialog.exec())
        openProject(FileChooserDialog.selectedFiles().first());
}
void gvle_win::onProjectRecent1()
{
    QVariant path = mSettings->value("Projects/recent1");
    if (path.isValid())
        openProject(path.toString());
}
void gvle_win::onProjectRecent2()
{
    QVariant path = mSettings->value("Projects/recent2");
    if (path.isValid())
        openProject(path.toString());
}
void gvle_win::onProjectRecent3()
{
    QVariant path = mSettings->value("Projects/recent3");
    if (path.isValid())
        openProject(path.toString());
}
void gvle_win::onProjectRecent4()
{
    QVariant path = mSettings->value("Projects/recent4");
    if (path.isValid())
        openProject(path.toString());
}
void gvle_win::onProjectRecent5()
{
    QVariant path = mSettings->value("Projects/recent5");
    if (path.isValid())
        openProject(path.toString());
}

void gvle_win::newProject(QString pathName)
{
    QDir dir(pathName);
    std::string basename = dir.dirName().toStdString ();

    if (mPackage) {
        if ( not closeProject()) {
            return;
        }
    }

    mPackage = new vlePackage(pathName);
    mPackage->setStdPackage( &mCurrPackage );

    mLogger->log(QString("New Project %1").arg(dir.dirName()));

    // Update window title
    setWindowTitle("GVLE - " + dir.dirName());

    dir.cdUp();
    QDir::setCurrent( dir.path() );
    mCurrPackage.select(basename);
    mCurrPackage.create();
    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);
    ui->menuModeling->setEnabled(true);
}

/**
 * @brief gvle_win::openProject
 *        Handler for menu function : File > Open Project
 */
void gvle_win::openProject(QString pathName)
{
    if (mPackage) {
        if ( not closeProject()) {
            return;
        }
    }
    mPackage = new vlePackage(pathName);
    mPackage->setStdPackage( &mCurrPackage );
    // Register modelers
    for (int i = 0; i < mModelers.count(); i++)
        mPackage->addModeler(mModelers.at(i));
    // Register Experimental Conditions plugins
    QList <QString> expNames = mExpPlugins.keys();
    for (int i = 0; i < mExpPlugins.count(); i++)
    {
        QString name = expNames.at(i);
        mPackage->addExpPlugin(name, mExpPlugins.value(name));
    }

    // Register Output plugins
    QList <QString> outNames = mOutputPlugins.keys();
    for (int i = 0; i < mOutputPlugins.count(); i++)
    {
        QString name = outNames.at(i);
        mPackage->addOutputPlugin(name, mOutputPlugins.value(name));
    }

    mLogger->log(QString("Open Project %1").arg(mPackage->getName()));

    // Update window title
    setWindowTitle("GVLE - " + mPackage->getName());

    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);
    ui->menuModeling->setEnabled(true);
}

bool gvle_win::closeProject()
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
            QWidget *wtool = ui->rightStack->widget( rtool.toInt() );
            ui->rightStack->removeWidget(wtool);
            delete wtool;
        }
    }

    // Update menus
    ui->actionCloseProject->setEnabled(false);
    ui->menuProject->setEnabled(false);
    // Update window title
    setWindowTitle("GVLE");

    mLogger->log(QString("Project closed"));

    delete mPackage;
    mPackage = 0;

    return true;
}

void gvle_win::onCloseProject()
{
    closeProject();
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

void gvle_win::onProjectConfigure()
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
    ui->actionConfigureProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectConfigureTimer()));
    mTimer->start(50);
}

void gvle_win::projectConfigureTimer()
{
    if (mCurrPackage.isFinish())
    {
        mTimer->stop();
        delete mTimer;
        mLogger->log(tr("Project configuration complete"));
        ui->actionConfigureProject->setEnabled(true);
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

void gvle_win::onProjectBuild()
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
    ui->actionBuildProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectBuildTimer()));
    mTimer->start(50);
}

void gvle_win::projectBuildTimer()
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
        ui->actionBuildProject->setEnabled(true);
    }
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
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectInstallTimer()));
    mTimer->start(50);
}

void gvle_win::projectInstallTimer()
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
        ui->actionBuildProject->setEnabled(true);
    }
}

/**
 * @brief gvle_win::onLaunchSimulation
 *        Handler for menu function : Simulation > Launch Simulation
 */
void gvle_win::onLaunchSimulation()
{
    QWidget *w = ui->tabWidget->currentWidget();

    vle::gvle::fileVpzView *vpzView = qobject_cast<vle::gvle::fileVpzView *>(w);
    if (vpzView == 0)
        return;

    if (mSimOpened)
        return;

    if ( ! ui->actionSimNone->isChecked())
    {
        // If plugin not already loadloed (or unloaded)
        if (mCurrentSimPlugin == 0)
        {
            // Instantiate a new loader - See QTBUG-24079
            QString pluginFile = mSimulatorPlugins.value(mCurrentSimName);
            mCurrentSimPlugin = new QPluginLoader(pluginFile);
            // If loading failed, nothing more to do :(
            if (mCurrentSimPlugin == 0)
                return;
        }
        mCurrentSimPlugin->load();
        QObject *plugin = mCurrentSimPlugin->instance();
        if ( ! mCurrentSimPlugin->isLoaded())
            return;
        PluginSimulator *sim = qobject_cast<PluginSimulator *>(plugin);
        if ( ! sim)
            return;

        mCurrentSim = sim;

        sim->setSettings(mSettings);
        //sim->setLogger(mLogger);

        QWidget *newTab = sim->getWidget();
        if (newTab)
        {
            // Configure Pluggin for the requested VPZ
            newTab->setProperty("type",   QString("simulation"));
            newTab->setProperty("plugin", QString("yes"));
            newTab->setProperty("vpz",    QVariant::fromValue((void*)vpzView));
            vpzView->usedBySim(true);
            try {
                sim->init(vpzView->vpm());
                sim->setPackage(&mCurrPackage);
                // Associate the pluggin widget with a new tab
                int n = ui->tabWidget->addTab(newTab, "Simulation");
                ui->tabWidget->setCurrentIndex(n);
                newTab->show();
            } catch(...) {
                qDebug() << "Simulation pluggin configuration error";
            }
        }

        // Search a toolbox widget from the plugin
        QWidget *newTool = sim->getWidgetToolbar();
        if (newTool)
        {
            try {
                int nid;
                nid = ui->rightStack->addWidget(newTool);
                ui->rightStack->setCurrentWidget(newTool);
                newTab->setProperty("wTool", nid);
            } catch (...) {
                qDebug() << "Simulation pluggin toolbar error";
            }
        }
    }
    else
    {
        // Create a new tab
        simulationView * newTab = new simulationView();
        newTab->setProperty("type", QString("simulation"));
        newTab->setProperty("vpz",  QVariant::fromValue((void*)vpzView));
        vpzView->usedBySim(true);
        newTab->setVpm(vpzView->vpm());
        newTab->setPackage(&mCurrPackage);
        newTab->setLogger(mLogger);
        newTab->setSettings(mSettings);
        int n = ui->tabWidget->addTab(newTab, "Simulation");
        ui->tabWidget->setCurrentIndex(n);
        newTab->show();

        // Create a new toolbox for the right column
        int nid;
        QWidget *newRTool = newTab->getTool();
        nid = ui->rightStack->addWidget(newRTool);
        ui->rightStack->setCurrentWidget(newRTool);
        newTab->setProperty("wTool", nid);
    }

    // Disable some menu entries when a tab is opened
    ui->actionLaunchSimulation->setEnabled(false);
    ui->menuSelectSimulator->setEnabled(false);
    mSimOpened = true;
}

void
gvle_win::onUndo()
{
    QWidget *w = ui->tabWidget->currentWidget();
    fileVpzView *tabVpz = (fileVpzView *)w;
    tabVpz->vpm()->undo();

}

void
gvle_win::onRedo()
{
    QWidget *w = ui->tabWidget->currentWidget();
    fileVpzView *tabVpz = (fileVpzView *)w;
    tabVpz->vpm()->redo();

}

void gvle_win::onSelectSimulator(bool isChecked)
{
    QAction *act = (QAction *)sender();

    if (isChecked)
    {
        if (mCurrentSimPlugin)
        {
            mCurrentSimPlugin->unload();
            delete mCurrentSimPlugin;
            mCurrentSimPlugin = 0;
        }
        if (act->objectName() != "actionSimNone")
            mCurrentSimName = act->objectName();
    }
}

/**
 * @brief gvle_win::onHelp Handler of menu Help > Help
 *        Open a new tab and display embedded help page
 */
void gvle_win::onHelp()
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
void gvle_win::onAbout()
{
    AboutBox box;

    box.exec();
}

/**
 * @brief gvle_win::menuRecentProjectRefresh
 *        Read the recent opened projects list and update menu
 */
void gvle_win::menuRecentProjectRefresh()
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
void gvle_win::menuRecentProjectSet(QString path, QAction *menu)
{
    QDir dir(path);
    menu->setText(dir.dirName());
    menu->setToolTip(path);
    menu->setEnabled(true);
    menu->setVisible(true);
}
void gvle_win::menuRecentProjectUpdate(QString path)
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

void gvle_win::onTabChange(int index)
{
    if (ui->tabWidget->count() == 0)
        return;

    QWidget *w = ui->tabWidget->widget(index);
    QVariant tabType = w->property("type");
    bool isVpz = (tabType.toString().compare("vpz") == 0);
    bool isSim = (tabType.toString().compare("simulation") == 0);

    if (tabType.isValid())
    {
        // Update Right Column
        if (isSim)
        {
            int toolId = w->property("wTool").toInt();
            ui->rightStack->setCurrentIndex(toolId);
        }
        else if (isVpz)
        {
            int toolId = w->property("wTool").toInt();
            ui->rightStack->setCurrentIndex(toolId);
        }
        else
            ui->rightStack->setCurrentIndex(0);

        // Update Simulation menu
        if (isVpz || isSim)
            ui->menuSimulation->setEnabled(true);
        else
            ui->menuSimulation->setEnabled(false);
    }
    else
    {
        ui->menuSimulation->setEnabled(false);
        ui->rightStack->setCurrentIndex(0);
    }
}

bool gvle_win::tabClose(int index)
{
    bool isSim = false;
    QWidget *wTool = 0;
    QWidget *w = ui->tabWidget->widget(index);
    if (w == 0) {
        return false;
    }

    bool isPlugin = false;
    QVariant vIsPlugin = w->property("plugin");
    if (vIsPlugin.isValid())
        isPlugin = (vIsPlugin.toString().compare("yes") == 0);

    if (w->property("type").toString().compare("vpz") == 0)
    {
        fileVpzView *tabVpz = (fileVpzView *)w;

        int useReason;
        bool allowClose = false;
        if (tabVpz->isUsed(&useReason))
        {
            QMessageBox msgBox;
            if (useReason == 1)
            {
                msgBox.setText(tr("This tab can't be closed (used by a simulator)"));
                msgBox.exec();
                return false;
            }
            if (useReason == 2)
            {
                msgBox.setText(tr("Modified file: ") +
                               tabVpz->vpm()->getFilename() + "\n" +
                               tr("Save before close ?"));
                msgBox.addButton(QMessageBox::Save);
                msgBox.addButton(QMessageBox::Discard);
                msgBox.addButton(QMessageBox::Cancel);
                int ret = msgBox.exec();
                switch (ret) {
                  case QMessageBox::Save:
                    tabVpz->save();
                    allowClose = true;
                    break;
                  case QMessageBox::Discard:
                    allowClose = true;
                    break;
                  case QMessageBox::Cancel:
                    allowClose = false;
                    break;
                  default:
                      // should never be reached
                      break;
                }
                if (! allowClose)
                    return false;
            }
        }
    }

    ui->tabWidget->removeTab(index);

    if (w->property("type").toString().compare("simulation") == 0)
    {
        isSim = true;

        QVariant linkedVpz = w->property("vpz");
        if (linkedVpz.isValid())
        {
            void *ptr = linkedVpz.value<void *>();
            fileVpzView *currentVpz = (fileVpzView *)ptr;
            currentVpz->usedBySim(false);
        }

        ui->actionLaunchSimulation->setEnabled(true);
        ui->menuSelectSimulator->setEnabled(true);
        mSimOpened = false;

        int toolId = w->property("wTool").toInt();
        wTool = ui->rightStack->widget(toolId);
        ui->rightStack->removeWidget(wTool);
        if (isPlugin)
            mCurrentSim->delWidgetToolbar();
        else
            delete wTool;

        if (isPlugin)
        {
            mCurrentSim->delWidget();
            w = 0;
        }
    }
    if (w)
        delete w;

    if (isSim && isPlugin)
    {
        mCurrentSimPlugin->unload();
        delete mCurrentSimPlugin;
        mCurrentSimPlugin = 0;
        //delete mCurrentSim;
        mCurrentSim = 0;
    }
    return true;
}

void gvle_win::onTabClose(int index)
{
    tabClose(index);
}

/* ---------- Manage the status bar ---------- */

/**
 * @brief gvle_win::onProjectConfigure
 *        Handler for menu function : Project > Configure Project
 */
void gvle_win::onStatusToggle()
{
    if (ui->statusLog->isVisible())
        statusWidgetClose();
    else
        statusWidgetOpen();
}

void gvle_win::statusWidgetOpen()
{
    ui->statusLog->show();
    ui->statusWidget->setMinimumSize(0, 0);
    ui->statusWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void gvle_win::statusWidgetClose()
{
    ui->statusLog->hide();
    QSize titleSize = ui->statusTitleFrame->size();
    ui->statusWidget->setFixedHeight(titleSize.height());
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

/* ---------- Manage the Project navigator ---------- */

void gvle_win::treeProjectUpdate()
{
    QTreeView* projectTreeView = ui->treeProject;
    QString projectName(vu::Path::filename(mCurrPackage.name()).c_str());

    QModelIndex projectIndex = mProjectFileSytem->setRootPath(projectName);

    projectTreeView->setRootIndex(projectIndex);

    projectTreeView->resizeColumnToContents(0);
}

QString gvle_win::treeProjectRelativePath(const QModelIndex index) const
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

void gvle_win::onTreeDblClick(QModelIndex index)
{
    if (not index.isValid()) return;

    QString fileName = mProjectFileSytem->filePath(index);
    QString currentDir = QDir::currentPath() += "/";

    fileName.replace(currentDir, "");

    QFileInfo selectedFileInfo = QFileInfo(fileName);
    if (selectedFileInfo.suffix() != "vpz")
        return;

    QString relPath;
    relPath = treeProjectRelativePath(index);

    QString basepath = mCurrPackage.getDir(vle::utils::PKG_SOURCE).c_str();
    QString relPathVpm = relPath;
    relPathVpm.replace(".vpz", ".vpm");
    vleVpm* selVpm = new vleVpm(basepath+"/"+relPath,
            basepath+"/metadata/"+relPathVpm);

    selVpm->setLogger(mLogger);
    selVpm->setBasePath(mPackage->getName());
    selVpm->setPackage(mPackage);


    // Search if the selected VPZ has already been opened
    int alreadyOpened = 0;
    int i;
    for (i = 0; i < ui->tabWidget->count(); i++)
    {
        QWidget *w = ui->tabWidget->widget(i);
        QVariant tabType = w->property("type");
        if (tabType.toString() != "vpz")
            continue;
        // Compare the tab title with the requested vpz name
        if (ui->tabWidget->tabText(i) == fileName)
        {
            alreadyOpened = i;
            break;
        }
    }

    if (alreadyOpened)
        // If the VPZ is opened, select his tab
        ui->tabWidget->setCurrentIndex(alreadyOpened);
    else
    {
        fileVpzView * newTab = new fileVpzView();
        newTab->setProperty("type", QString("vpz"));
        newTab->setVpm(selVpm);

        int n = ui->tabWidget->addTab(newTab, fileName);
        ui->tabWidget->setCurrentIndex(n);
        newTab->show();

        QObject::connect(selVpm, SIGNAL(sigChanged(QString)),
                         this,  SLOT  (setChangedVpz(QString)));

        // Create a new toolbox for the right column
        int nid;
        QWidget *newRTool = newTab->getTool();
        nid = ui->rightStack->addWidget(newRTool);
        ui->rightStack->setCurrentWidget(newRTool);
        newTab->setProperty("wTool", nid);
        //set undo redo enabled
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(true);
    }
}

void gvle_win::onCustomContextMenu(const QPoint &point)
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
        }
    }
}

void  gvle_win::onCurrentChanged(const QModelIndex& /*index*/)
{
    mProjectFileSytem->setReadOnly(true);
}


void  gvle_win::onFileRenamed(const QString & path,
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

    void gvle_win::onDataChanged(QModelIndex /*indexTL*/, QModelIndex /*indexBR*/)
{
    mProjectFileSytem->setReadOnly(true);
}

    void gvle_win::onItemChanged(QTreeWidgetItem* /*item*/, int /*col*/)
{
}


bool gvle_win::removeDir(const QString & dirName)
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

void gvle_win::setChangedVpz(QString filename)
{
    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--)
    {
        if (ui->tabWidget->tabText(i) == filename)
        {
            ui->tabWidget->setTabText(i, "* "+ filename);
            break;
        }
    }
}

/**
 * @brief gvle_win::openModeler
 *        Open (or return existing) modeler view tab
 *
 */
pluginModelerView *gvle_win::openModeler(QString filename)
{
    pluginModelerView *wview;

    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--)
    {
        QWidget *w = ui->tabWidget->widget(i);
        wview = qobject_cast<pluginModelerView *>(w);
        if (wview == 0)
            continue;
        if (wview->getFilename() == filename)
            return wview;
    }

    wview = new pluginModelerView();
    wview->setPlugin(filename);
    wview->setPackage(mPackage);

    QObject::connect(wview, SIGNAL(refreshFiles()),
                     this,  SLOT  (onRefreshFiles()));

    QString tabName = QString("Modeler: %1").arg( wview->getName() );
    int n = ui->tabWidget->addTab(wview, tabName);
    ui->tabWidget->setCurrentIndex(n);
    wview->show();

    loadModelerClasses(wview);

    return wview;
}

// Todo : this method must be move to a "package" class
void gvle_win::loadModelerClasses(pluginModelerView *modeler)
{
    QString pluginName = modeler->getName();

    QDir dir(mCurrPackage.name().c_str());
    if (dir.cd("src")) {
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        QStringList filters;
        filters << "*.cpp";
        dir.setNameFilters(filters);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            QString fname = fileInfo.fileName();

            vu::Template tpl;
            try {
                std::string tplPlugin, packagename, conf;

                tpl.open(fname.toStdString());
                tpl.tag(tplPlugin, packagename, conf);

                if (pluginName != QString(tplPlugin.c_str()))
                    continue;
                QString tplConf = QString( conf.c_str() );
                QStringList confEntries = tplConf.split(";");

                // Get the class name from conf
                QStringList classNameEntry = confEntries.filter("class:");
                QString className = classNameEntry.at(0).split(":").at(1);

                modeler->addClass(className, fname);
            } catch(...) {
                continue;
            }
        }
    }
}


/**
 * @brief gvle_win::onOpenModeler (slot)
 *        Called on "Open" menu of a Modeler plugin
 *
 */
void gvle_win::onOpenModeler()
{
    // Search the caller menu (which plugin)
    QObject *senderObject = QObject::sender();
    QAction *senderAction = qobject_cast<QAction *>(senderObject);
    // This function -must- be called by a QAction signal
    if (senderAction == 0)
        return;

    QVariant varFileName = senderAction->property("fileName");
    if ( ! varFileName.isValid())
        return;

    QString fileName = varFileName.toString();

    pluginModelerView *tab;
    tab = openModeler(fileName);

    tab->showSummary();
}

/**
 * @brief gvle_win::onNewModelerClass (slot)
 *        Called on "New class" menu of a Modeler plugin
 *
 */
void gvle_win::onNewModelerClass()
{
    // Search the caller menu (which plugin)
    QObject *senderObject = QObject::sender();
    QAction *senderAction = qobject_cast<QAction *>(senderObject);
    // This function -must- be called by a QAction signal
    if (senderAction == 0)
        return;

    QVariant varFileName = senderAction->property("fileName");
    if ( ! varFileName.isValid())
        return;

    QString fileName = varFileName.toString();

    pluginModelerView *tab;
    tab = openModeler(fileName);

    tab->addNewTab();
}

}} //namespaces
