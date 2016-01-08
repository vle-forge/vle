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

#include "simulation.h"
#include "ui_simulationview.h"
#include <QTimer>
#include <QColorDialog>
#include <QFileDialog>
#include <QDebug>
#include "vle/vpz/Views.hpp"
#include "vle/vpz/BaseModel.hpp"
#include "vle/vpz/AtomicModel.hpp"
#include <iostream>

namespace vle {
namespace gvle {

simulationView::simulationView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::simulationView),
    uiTool(new Ui::simulationRTool)
{
    mVpm = 0;
    mLogger = 0;
    mThread = 0;
    mSettings = 0;
    mViewsItems.clear();
    // Init the sim UI (tab widget)
    ui->setupUi(this);
    // Init the right column toolbox UI
    mWidgetTool = new QWidget();
    uiTool->setupUi(mWidgetTool);
    uiTool->widSimStyle->setVisible(false);
    // Config controls
    QObject::connect(ui->buttonGo,         SIGNAL(clicked()), this, SLOT(onButtonGo()));
}

simulationView::~simulationView()
{
    delete ui;
    delete uiTool;
    if (mThread)
    {
        log(tr("Simulation finished (closed before the end)"));
        if (mSimThread)
            mSimThread->forceQuit();
        mThread->quit();
        mThread->wait();
        delete mThread;
        mThread = 0;
        if (mSimThread)
            delete mSimThread;
    }
}

void simulationView::setVpm(vleVpm* vpm)
{
    QString fileName = vpm->getFilename();
    mVpm = new vle::vpz::Vpz(fileName.toStdString());

    //mVpm = vpz;
    vle::vpz::Observables curVpzObs;
    vle::vpz::Views       curVpzViews;
    vle::vpz::AtomicModelVector curVpzModels;

    curVpzViews  = mVpm->project().experiment().views();
    curVpzObs    = mVpm->project().experiment().views().observables();
    mVpm->project().model().getAtomicModelList(curVpzModels);

    // Update the title (Experiment Name and VPZ file name)
    QString expName = mVpm->project().experiment().name().c_str();
    QString simTitle = QString("%1 (%2)").arg(expName).arg(mVpm->filename().c_str());
    ui->modelName->setText(simTitle);

    // Initiate the view/port tree by adding the View list
    vle::vpz::Views::iterator itb = curVpzViews.begin();
    vle::vpz::Views::iterator ite = curVpzViews.end();
    for ( ; itb != ite; itb++)
    {
        vle::vpz::View& vv = itb->second;
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString(vv.name().c_str()));
        item->setIcon(0, *(new QIcon(":/icon/resources/icon/magnifier.png")));
        mViewsItems.append(item);
    }

    // Look into each Model used be current VPZ
    vle::vpz::AtomicModelVector::iterator model;
    for (model = curVpzModels.begin(); model != curVpzModels.end(); model++)
    {
        // Only model with Observable(s) are relevant
        if ((*model)->observables() == "")
            continue;

        const vle::vpz::Observable obs = curVpzObs.get((*model)->observables());
        const vle::vpz::ObservablePortList& portList = obs.observableportlist();

        // Look each port of the current model
        vle::vpz::ObservablePortList::const_iterator port;
        for (port = portList.begin(); port != portList.end(); port++)
        {
            const vle::vpz::ViewNameList& viewList = port->second.viewnamelist();
            // If the port isn't used for any view ... nothing to do
            if (viewList.begin() == viewList.end())
                continue;

            vle::vpz::ViewNameList::const_iterator view;
            for (view = viewList.begin(); view != viewList.end(); view++)
            {
                vle::vpz::View    selView = curVpzViews.get((*view));
                vle::vpz::Output& output(curVpzViews.outputs().get(selView.output()));
                QString pluginName = QString("%1/%2")
                        .arg(output.package().c_str())
                        .arg(output.plugin().c_str());
                //if (pluginName == "vle.output/storage")
                {
                    QString portViewName = QString("%1.%2")
                            .arg((*model)->getName().c_str())
                            .arg(QString::fromStdString(port->first));
                    QString portModelPath((*model)->getParentName().c_str());
                    portModelPath.replace(QChar(','), QString(" / "));
                    QString portFullName = QString("%1:%2")
                            .arg(portModelPath)
                            .arg(portViewName);
                    insertViewPort(QString::fromStdString(*view), portViewName, portFullName);
                }
            }
        }
    }

    updateViews();
}

/**
 * @brief simulationView::insertViewPort
 *        Add a model-port into the tree
 */
void simulationView::insertViewPort(QString viewName, QString portName, QString portInfo)
{
    QListIterator<QTreeWidgetItem*> items( mViewsItems );
    while( items.hasNext() )
    {
        QTreeWidgetItem *item = items.next();
        if (item->text(0) == viewName)
        {
            QTreeWidgetItem *newItem = new QTreeWidgetItem();
            newItem->setText(0, portName);
            newItem->setData(0, Qt::UserRole+0, "typePort");
            newItem->setData(0, Qt::UserRole+1, viewName);
            newItem->setData(0, Qt::UserRole+2, portName);
            newItem->setToolTip(0, portInfo);
            newItem->setIcon(0, *(new QIcon(":/icon/resources/icon/comment.png")));
            item->addChild(newItem);
            break;
        }
    }
}

void simulationView::updateViews()
{
    QTreeWidget *viewTree = uiTool->treeSimViews;

    viewTree->clear();
    viewTree->setColumnCount(1);
    viewTree->setHeaderLabels(QStringList("Views"));
    if (mViewsItems.count())
        viewTree->insertTopLevelItems(0, mViewsItems);

    QObject::connect(viewTree, SIGNAL(itemSelectionChanged ()),
                     this,     SLOT  (onTreeItemSelected()));
}

void simulationView::simulationStart()
{
    mSimThread = new simulationThread(mVpm);

    ui->progressBar->setTextVisible(true);

    // Load and init the simulation into VLE
    try {
        mCurrPackage->addDirectory("","output",vle::utils::PKG_SOURCE);
        std::string path = mCurrPackage->getOutputDir(vle::utils::PKG_SOURCE);
        mSimThread->setOutputPath(path.c_str());
        mSimThread->load();
        mSimThread->init();
        mOutputs = mSimThread->getOutputs();
    } catch (...) {
        ui->progressBar->setFormat(tr("Fatal Error"));
        QString logMessage;
        logMessage = tr("Project error:") + mSimThread->getError();
        log(logMessage);
        delete mSimThread;
        mSimThread = 0;
        return;
    }
    mSimDataValid = true;
    mStepCount   = 0;
    QMap<QString, int>::iterator i;
    for (i = mViewLastRow.begin(); i != mViewLastRow.end(); ++i)
        i.value() = 1;

    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_pause_blue.png")));

    QString expName = mVpm->project().experiment().name().c_str();
    mDuration = mVpm->project().experiment().duration();
    QString startMessage;
    startMessage  = tr("Simulation started. ");
    startMessage += tr("Experiment name: ") + expName + " ";
    startMessage += QString(tr("Duration=%1")).arg(mDuration, 0, 'f', 4);
    log(startMessage);

    // Start the simulation thread
    mThread = new QThread();
    mSimThread->moveToThread(mThread);
    connect(mThread,    SIGNAL(started()), mSimThread, SLOT(run()));
    connect(mSimThread, SIGNAL(step()),    this, SLOT(simulationGetStep()));
    connect(mSimThread, SIGNAL(end()),     this, SLOT(simulationFinished()));
    mThread->start();
}

void simulationView::simulationGetStep()
{
    double debugTime = mSimThread->getCurrentTime();
    double debugElapsed = debugTime - mVpm->project().experiment().begin();
    double percent = qMax(100.0,(debugElapsed / mDuration) * 100.00);
    ui->progressBar->setValue(percent);

    mStepCount++;
}

void simulationView::simulationFinished()
{
    try {
        // Call finish to close vle::devs job
        mSimThread->finish();
        // Terminate simulation thread
        mThread->quit();
        mThread->wait();
        delete mThread;
        mThread = 0;
        // Clean memory
        delete mSimThread;
        mSimThread = 0;
    } catch (...) {
        QString logMessage;
        logMessage = tr("Project error (finish):") + mSimThread->getError();
        log(logMessage);
    }
    log(tr("Simulation finished"));
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_play_blue.png")));
    ui->progressBar->setFormat("%p%");
    uiTool->treeSimViews->setEnabled(true);
}

void simulationView::onButtonGo()
{
    if (mThread == 0)
    {
        uiTool->treeSimViews->setEnabled(false);
        simulationStart();
    }
    else
    {
        if (mSimThread)
        {
            if (mSimThread->isPaused())
            {
                mSimThread->setPause(false);
                ui->progressBar->setFormat("%p%");
                ui->progressBar->setStyleSheet("");
                ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_pause_blue.png")));
            }
            else
            {
                mSimThread->setPause(true);
                ui->progressBar->setFormat(tr("%p% (Paused)"));
                ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_play_blue.png")));
            }
        }
    }
}

/**
 * @brief simulationView::onTreeItemSelected
 *        Called when the currently selected item into view/port tree change
 *
 */
void simulationView::onTreeItemSelected()
{
    // Nothing to do ...

    // This method can be used to display advanced information on the selected item ...
}

/* ------------------------- Simulation thread ------------------------- */

simulationThread::simulationThread(vleVpm* vpm)
{
    mOutputs = 0;
    mCurrentState = Wait;
    if (vpm)
    {
        // Instanciate an empty VPZ
        mVpm = new vle::vpz::Vpz();

        // Get the raw XML of vleVpz
        QByteArray xml = vpm->xGetXml();

        std::string buffer = (char *)xml.data();
        mVpm->parseMemory(buffer);

        // Instanciate the root coordinator    ToDo : Plugins
        mRoot = new vle::devs::RootCoordinator(mLoadedPlugin);
    }
    else
    {
        mVpm  = 0;
        mRoot = 0;
    }
}

simulationThread::simulationThread(vle::vpz::Vpz* vpz)
{
    mOutputs = 0;
    mCurrentState = Wait;
    if (vpz)
    {
        // Make a copy of current VPZ before use it for sim
        mVpm = new vle::vpz::Vpz(*vpz);

        // Instanciate the root coordinator    ToDo : Plugins
        mRoot = new vle::devs::RootCoordinator(mLoadedPlugin);
    }
    else
    {
        mVpm  = 0;
        mRoot = 0;
    }
}

/**
 * @brief simulationThread default destructor
 *
 */
simulationThread::~simulationThread()
{
    if (mRoot)
        delete mRoot;

    if (mVpm)
        delete mVpm;
}

/**
 * @brief simulationThread::getOutputs
 *        Get the map of ouputs datas
 *
 * This method take place into -simulationView- thread!
 */
vle::value::Map *simulationThread::getOutputs()
{
    if (mOutputs == 0)
        mOutputs = mRoot->outputs();

    return mOutputs;
}

vle::value::Matrix *simulationThread::getMatrix(vle::value::Value *value)
{
    vle::value::Matrix *mat;
    mValueMutex.lock();
    mat = new vle::value::Matrix(value->toMatrix());
    mValueMutex.unlock();
    return mat;
}

/**
 * @brief simulationThread::getStartTime
 *        Return the timestamp of experiment begin
 *
 */
double simulationThread::getStartTime()
{
    return mVpm->project().experiment().begin();
}

/**
 * @brief simulationThread::getDuration
 *        Return the experiment duration
 *
 */
double simulationThread::getDuration()
{
    return mVpm->project().experiment().duration();
}

/**
 * @brief simulationThread::getCurrentTime
 *        Return the current experiment time
 *
 */
double simulationThread::getCurrentTime()
{
    double currentTime = mRoot->getCurrentTime();

    return currentTime;
}

/**
 * @brief simulationThread::setOutputPath
 *        Configure output plugins for a specific location
 * @param QString Path name
 *
 * This method take place into -simulationView- thread!
 */
void simulationThread::setOutputPath(QString path)
{
    vle::vpz::Outputs::iterator itb;
    vle::vpz::Outputs::iterator ite;

    mOutputPath = path;

    if (mVpm == 0)
        return;

    itb = mVpm->project().experiment().views().outputs().begin();
    ite = mVpm->project().experiment().views().outputs().end();
    for (; itb!=ite; itb++)
    {
        vle::vpz::Output& output = itb->second;
        if (output.location().empty())
            output.setLocalStreamLocation(mOutputPath.toStdString());
    }
}

/**
 * @brief simulationThread::isPaused
 *        Test if the simulation thread is in Pause state
 */
bool simulationThread::isPaused()
{
    if (mCurrentState == Pause)
        return true;
    else
        return false;
}
/**
 * @brief simulationThread::setPause
 *        Request the simulation thread to wait some time
 */
void simulationThread::setPause(bool pause)
{
    if (pause)
        mCurrentState = Pause;
    else
        mCurrentState = Play;
}
/**
 * @brief simulationThread::forceQuit
 *        Force the current state to Finish (this will end the run loop)
 *
 */
void simulationThread::forceQuit()
{
    mCurrentState = Finish;
}

/**
 * @brief simulationThread::run
 *        Do the sim !
 *
 * This method take place into -dedicated- thread!
 */
void simulationThread::run()
{
    mCurrentState = Play;

    while((mCurrentState == Play) || (mCurrentState == Pause))
    {
        // If the simulation is paused ... just wait
        while (mCurrentState == Pause)
        {
            // QThread::msleep(100);
            QTimer::singleShot(800, this, SLOT(restart()));
            return;
        }

        try {
            if (mRoot->run() == false)
                mCurrentState = Finish;
            // Notify the view
            emit step();
        } catch (const std::exception& e) {
            mErrorMessage = QString(e.what());
        }
    }
    emit end();
}

void simulationThread::restart()
{
    if (mCurrentState == Pause)
        QTimer::singleShot(800, this, SLOT(restart()));
    else
        run();
}

/**
 * @brief simulationThread::load
 *        Load the VPZ into devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simulationThread::load()
{
    if ((mVpm == 0) || (mRoot == 0))
    {
        mErrorMessage = tr("Model or RootCoordinator missing");
        throw 10;
        return;
    }

    try {
        mRoot->load(*mVpm);
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 1;
    }
}

/**
 * @brief simulationThread::init
 *        Initialize the devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simulationThread::init()
{
    try {
        mRoot->init();
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 2;
    }
}

/**
 * @brief simulationThread::finish
 *        Terminate devs coordinator jobs
 *
 * This method take place into the -simulationView- thread!
 */
void simulationThread::finish()
{
    try {
        mRoot->finish();
    } catch (const std::exception& e) {
        mErrorMessage = QString(e.what());
        throw 3;
    }
}
}}//namepsaces
