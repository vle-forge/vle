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

#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QList>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QSettings>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStandardItem>

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

#include <vle/gvle2/vlevpz.h>

#include "logger.h"
#include "ui_simulationrtool.h"

namespace Ui {
class simulationView;
class simulationRTool;
}

class simulationThread : public QObject
{
    Q_OBJECT
public:
    simulationThread(vle::vpz::Vpz *vpz = 0);
    simulationThread(vleVpz *vpz = 0);
    ~simulationThread();
    QString getError()
    {
        return mErrorMessage;
    }
    bool   isPaused();
    void   setPause(bool pause);
    void   forceQuit();
    void   setOutputPath(QString path);
    double getStartTime();
    double getDuration();
    double getCurrentTime();
    vle::value::Map    *getOutputs();
    vle::value::Matrix *getMatrix(vle::value::Value *value);
    void   load();
    void   init();
    void   finish();

public slots:
    void run();
    void restart();
signals:
     void step();
     void end();

private:
    enum State { Wait, Init, Play, Error, Finish, Close, Pause };
private:
    State                       mCurrentState;
    QString                     mErrorMessage;
    QString                     mOutputPath;
    QMutex                      mValueMutex;
    vle::vpz::Vpz              *mVpz;
    vle::devs::RootCoordinator *mRoot;
    vle::value::Map            *mOutputs;
    vle::utils::ModuleManager   mLoadedPlugin;
};

class simulationView : public QWidget
{
    Q_OBJECT
public:
    explicit simulationView(QWidget *parent = 0);
    ~simulationView();
    void setVpz(vleVpz *vpz);
    void setPackage(vle::utils::Package *pkg)
    {
        mCurrPackage = pkg;
    }
    void setLogger(Logger *logger)
    {
        mLogger = logger;
    }
    void setSettings(QSettings *s)
    {
        mSettings = s;
    }

    void simulationStart();
    void updateViews();
    QWidget *getTool()
    {
        return mWidgetTool;
    }

private:
    enum State { Wait, Init, Play, Error, Finish, Close };
    void insertViewPort(QString viewName, QString portName, QString portInfo);
    void log(QString message)
    {
        if (mLogger)
            mLogger->log(message);
    }

private:
    QThread               *  mThread;
    Logger                *  mLogger;
    QSettings             *  mSettings;
    Ui::simulationView    *  ui;
    Ui::simulationRTool   *  uiTool;
    QWidget               *  mWidgetTool;
    simulationThread      *  mSimThread;
    QList<QTreeWidgetItem *> mViewsItems;
    vle::vpz::Vpz         *  mVpz;
    vle::utils::Package   *  mCurrPackage;
    vle::utils::ModuleManager mLoadedPlugin;
    vle::value::Map       *  mOutputs;
    bool                     mSimDataValid;
    double    mDuration;
    double    mCurrentTime;
    int       mStepCount;
    QMap<QString, int>       mViewLastRow;

public slots:
     void simulationGetStep();
     void simulationFinished();
     void onTreeItemSelected();

private slots:
    void onButtonGo();
};

#endif // SIMULATIONVIEW_H
