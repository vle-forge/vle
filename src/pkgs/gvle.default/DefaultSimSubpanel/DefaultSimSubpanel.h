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

#ifndef GVLE_DEFAULT_SIM_SUB_PANEL_H
#define GVLE_DEFAULT_SIM_SUB_PANEL_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include "qcustomplot/qcustomplot.h"

#include <vle/gvle/plugin_simpanel.h>
#include <vle/gvle/vlevpz.hpp>

namespace Ui {
class simpanelleft;
class simpanelright;
}

namespace vle {
namespace gvle {


class DefaultSimSubpanelThread : public QObject
{
    Q_OBJECT
public:
    std::unique_ptr<vle::value::Map>     output_map;
    vleVpz*                   mvpz;
    vle::utils::Package*      mpkg;
    QString                   error_simu;
    std::vector<std::string>& log_messages;
    //parameters
    bool mdebug;
    int mnbthreads;
    int mblockSize;

    DefaultSimSubpanelThread(std::vector<std::string>& logMessages,
            bool debug, int nbthreads, int blockSize);
    ~DefaultSimSubpanelThread();
    void init(vleVpz* vpz, vle::utils::Package* pkg);
public slots:
   void onStarted();
signals:
   void simulationFinished();
};

/**
 * @brief left  widget
 */

class DefaultSimSubpanelLeftWidget : public QWidget
{
    Q_OBJECT
public:
    DefaultSimSubpanelLeftWidget();
    ~DefaultSimSubpanelLeftWidget();
    Ui::simpanelleft* ui;
    QCustomPlot* customPlot;


};

/**
 * @brief right widget
 */

class DefaultSimSubpanelRightWidget : public QWidget
{
    Q_OBJECT
public:
    DefaultSimSubpanelRightWidget();
    ~DefaultSimSubpanelRightWidget();
    Ui::simpanelright* ui;

};


struct portToPlot
{
    portToPlot() : view(), port(), color()
    {
    }
    portToPlot(QString viewN, QString portN): view(viewN), port(portN),
            color(70,130,180)
    {
    }

    portToPlot(const portToPlot& p) : view(p.view), port(p.port),
            color(p.color)
    {
    }

    QString view;
    QString port;
    QColor  color;

};

/**
 * @brief Sim panel
 */

class DefaultSimSubpanel : public PluginSimPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginSimPanel")
    Q_INTERFACES(vle::gvle::PluginSimPanel)
public:
    DefaultSimSubpanel();
    virtual ~DefaultSimSubpanel();
    void init(vleVpz* vpz, vle::utils::Package* pkg, Logger* log);
    QString  getname();
    QWidget* leftWidget();
    QWidget* rightWidget();
    void undo();
    void redo();
    PluginSimPanel* newInstance();

    void showCustomPlot(bool b);
    void addPortToPlot(QString view, QString port);
    void removePortToPlot(QString view, QString port);
    portToPlot* getPortToPlot(QString view, QString port);
    void updateCustomPlot();
    double getDouble(const vle::value::Matrix& view, unsigned int col,
            unsigned int row, bool error_message);

    //parameters
    bool debug;
    int nbthreads;
    int blockSize;

    DefaultSimSubpanelLeftWidget* left;
    DefaultSimSubpanelRightWidget* right;
    DefaultSimSubpanelThread* sim_process;
    QThread* thread;
    vleVpz* mvpz;
    vle::utils::Package* mpkg;
    Logger* mLog;
    QTimer timer;


    std::vector<portToPlot> portsToPlot;
    std::vector<std::string> log_messages;
    unsigned int index_message;


public slots:
    void onSimulationFinished();
    void onRunPressed();
    void onDebugChanged(QString);
    void onThreadsChanged(int);
    void onBlockSizeChanged(int);
    void onTreeItemChanged(QTreeWidgetItem* item, int col);
    void onTreeItemSelected();
    void onTimeout();
    void onToolColor();
};

}} //namespaces


#endif
