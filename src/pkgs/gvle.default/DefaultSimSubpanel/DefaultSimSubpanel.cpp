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
#include <unistd.h>

#include "DefaultSimSubpanel.h"
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Matrix.hpp>

#include "ui_simpanelleft.h"
#include "ui_simpanelright.h"

namespace vle {
namespace gvle {

struct sim_log : utils::Context::LogFunctor
{
    std::vector<std::string>& log_messages;
    sim_log(std::vector<std::string>& logMessages)
      : log_messages(logMessages)
    {
    }

    virtual void write(const utils::Context& ctx,
                       int priority,
                       const char* file,
                       int line,
                       const char* fn,
                       const char* format,
                       va_list args) noexcept override
    {
        (void)ctx;
        (void)priority;
        (void)file;
        (void)line;
        (void)fn;
        log_messages.emplace_back(vle::utils::vformat(format, args));
    }
    virtual ~sim_log(){};
};

DefaultSimSubpanelThread::DefaultSimSubpanelThread(
  std::vector<std::string>& logMessages,
  bool debug,
  int nbthreads,
  int blockSize)
  : output_map(nullptr)
  , mvpz(0)
  , mpkg(0)
  , error_simu("")
  , log_messages(logMessages)
  , mdebug(debug)
  , mnbthreads(nbthreads)
  , mblockSize(blockSize)
{
}

DefaultSimSubpanelThread::~DefaultSimSubpanelThread()
{
}
void
DefaultSimSubpanelThread::init(vleVpz* vpz, vle::utils::Package* pkg)
{
    mvpz = vpz;
    mpkg = pkg;
}

void
DefaultSimSubpanelThread::onStarted()
{
    auto ctx = utils::make_context();
    if (mdebug) {
        ctx->set_log_priority(7); // VLE_LOG_DEBUG
        ctx->set_log_function(
          std::unique_ptr<sim_log>(new sim_log(log_messages)));
    } else {
        ctx->set_log_priority(3); // VLE_LOG_ERROR
    }
    if (mnbthreads > 0) {
        ctx->set_setting("vle.simulation.thread", (long)mnbthreads);
    }
    if (mblockSize != 8) {
        ctx->set_setting("vle.simulation.block-size", (long)mblockSize);
    }

    vle::manager::SimulationOptions doSpawn;
    if (mdebug) {
        doSpawn = vle::manager::SIMULATION_NONE;
    } else {
        doSpawn = vle::manager::SIMULATION_SPAWN_PROCESS;
    }

    vle::manager::Simulation sim(ctx,
                                 vle::manager::LOG_RUN,
                                 doSpawn,
                                 std::chrono::milliseconds::zero(),
                                 &std::cout);
    vle::manager::Error manerror;
    output_map.reset(nullptr);
    std::unique_ptr<vle::vpz::Vpz> vpz(nullptr);
    try {
        vpz.reset(new vle::vpz::Vpz(mvpz->getFilename().toStdString()));
        // set default location of outputs
        vle::vpz::Outputs::iterator itb =
          vpz->project().experiment().views().outputs().begin();
        vle::vpz::Outputs::iterator ite =
          vpz->project().experiment().views().outputs().end();
        for (; itb != ite; itb++) {
            vle::vpz::Output& output = itb->second;
            if (output.location().empty()) {
                mpkg->addDirectory("", "output", vle::utils::PKG_SOURCE);
                output.setStreamLocation(
                  mpkg->getOutputDir(vle::utils::PKG_SOURCE));
            }
        }
    } catch (const vle::utils::SaxParserError& e) {
        error_simu = QString("Error before simulation '%1'").arg(e.what());
    }
    if (vpz) {
        output_map = sim.run(std::move(vpz), &manerror);
    }
    if (manerror.code != 0) {
        error_simu = QString("Error during simulation '%1'")
                       .arg(manerror.message.c_str());
        output_map.reset();
    }

    emit simulationFinished();
}

/*************** Left widget ***************************/

DefaultSimSubpanelLeftWidget::DefaultSimSubpanelLeftWidget()
  : ui(new Ui::simpanelleft)
  , customPlot(0)
{
    ui->setupUi(this);
}
DefaultSimSubpanelLeftWidget::~DefaultSimSubpanelLeftWidget()
{
    delete customPlot;
    delete ui;
}

/*************** Right widget ***************************/

DefaultSimSubpanelRightWidget::DefaultSimSubpanelRightWidget()
  : ui(new Ui::simpanelright)
{
    ui->setupUi(this);
}

DefaultSimSubpanelRightWidget::~DefaultSimSubpanelRightWidget()
{
    delete ui;
}

/*************** Default Sim panel ***************************/

DefaultSimSubpanel::DefaultSimSubpanel()
  : PluginSimPanel()
  , debug(false)
  , nbthreads(0)
  , blockSize(8)
  , left(new DefaultSimSubpanelLeftWidget)
  , right(new DefaultSimSubpanelRightWidget)
  , sim_process(0)
  , thread(0)
  , mvpz(0)
  , mpkg(0)
  , mLog(0)
  , timer(this)
  , portsToPlot()
  , log_messages()
  , index_message(0)
{
    QObject::connect(
      left->ui->runButton, SIGNAL(pressed()), this, SLOT(onRunPressed()));
    QObject::connect(left->ui->debugCombo,
                     SIGNAL(currentTextChanged(QString)),
                     this,
                     SLOT(onDebugChanged(QString)));
    QObject::connect(left->ui->threadsSpinBox,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(onThreadsChanged(int)));
    QObject::connect(left->ui->blockSizeSpinBox,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(onBlockSizeChanged(int)));
    QObject::connect(
      right->ui->butSimColor, SIGNAL(clicked()), this, SLOT(onToolColor()));
    QObject::connect(right->ui->treeSimViews,
                     SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this,
                     SLOT(onTreeItemChanged(QTreeWidgetItem*, int)));
    QObject::connect(right->ui->treeSimViews,
                     SIGNAL(itemSelectionChanged()),
                     this,
                     SLOT(onTreeItemSelected()));
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

DefaultSimSubpanel::~DefaultSimSubpanel()
{
    delete left;
    delete right;
    delete sim_process;
    if (thread) {
        thread->quit();
        thread->wait();
        delete thread;
    }
}
void
DefaultSimSubpanel::init(vleVpz* vpz, vle::utils::Package* pkg, Logger* log)
{
    mvpz = vpz;
    mpkg = pkg;
    mLog = log;
    onTreeItemSelected();
}

QString
DefaultSimSubpanel::getname()
{
    return "Default";
}

QWidget*
DefaultSimSubpanel::leftWidget()
{
    return left;
}

QWidget*
DefaultSimSubpanel::rightWidget()
{
    return right;
}

void
DefaultSimSubpanel::undo()
{
}

void
DefaultSimSubpanel::redo()
{
}

PluginSimPanel*
DefaultSimSubpanel::newInstance()
{
    return new DefaultSimSubpanel;
}

void
DefaultSimSubpanel::showCustomPlot(bool b)
{
    int stackSize = left->ui->stackPlot->count();
    if (stackSize < 1 or stackSize > 2) {
        return;
    }
    if (stackSize == 2) {
        left->ui->stackPlot->removeWidget(left->ui->stackPlot->widget(1));
    }
    if (b) {
        left->ui->stackPlot->addWidget(left->customPlot);
        left->ui->stackPlot->setCurrentIndex(1);
    }
}

void
DefaultSimSubpanel::addPortToPlot(QString view, QString port)
{
    for (unsigned int i = 0; i < portsToPlot.size(); i++) {
        const portToPlot& p = portsToPlot[i];
        if (p.view == view and p.port == port) {
            return;
        }
    }
    portsToPlot.push_back(portToPlot(view, port));
    onTreeItemSelected();
}

void
DefaultSimSubpanel::removePortToPlot(QString view, QString port)
{
    std::vector<portToPlot>::iterator itb = portsToPlot.begin();
    std::vector<portToPlot>::iterator ite = portsToPlot.end();
    std::vector<portToPlot>::iterator itf = ite;
    for (; itb != ite; itb++) {
        if (itb->view == view and itb->port == port) {
            itf = itb;
        }
    }
    if (itf != ite) {
        portsToPlot.erase(itf);
    }
    onTreeItemSelected();
}

portToPlot*
DefaultSimSubpanel::getPortToPlot(QString view, QString port)
{
    std::vector<portToPlot>::iterator itb = portsToPlot.begin();
    std::vector<portToPlot>::iterator ite = portsToPlot.end();
    std::vector<portToPlot>::iterator itf = ite;
    for (; itb != ite; itb++) {
        if (itb->view == view and itb->port == port) {
            itf = itb;
        }
    }
    if (itf != ite) {
        return &(*itf);
    }
    return 0;
}

void
DefaultSimSubpanel::updateCustomPlot()
{
    // get results
    left->customPlot->clearGraphs();
    if (not sim_process or not sim_process->output_map) {
        return;
    }
    const vle::value::Map& simu = *sim_process->output_map;

    unsigned int nbGraphs = 0;
    std::vector<portToPlot>::iterator itb = portsToPlot.begin();
    std::vector<portToPlot>::iterator ite = portsToPlot.end();
    // compute bounds
    double minyi = 99999999;
    double maxyi = -99999999;
    double minxi = 99999999;
    double maxxi = -99999999;
    for (; itb != ite; itb++) {
        const vle::value::Matrix& view =
          simu.getMatrix(itb->view.toStdString());
        QString portName = itb->port;
        int index = -1;
        for (unsigned int j = 0; j < view.columns(); ++j) {
            if (view.getString(j, 0) == portName.toStdString()) {
                index = j;
            }
        }
        if (index != -1) {
            for (unsigned int i = 1; i < view.rows(); ++i) {
                double xi = view.getDouble(0, i); // time column
                double yi = getDouble(view, index, i, i == 1);
                if (yi < minyi) {
                    minyi = yi;
                }
                if (yi > maxyi) {
                    maxyi = yi;
                }
                if (xi < minxi) {
                    minxi = xi;
                }
                if (xi > maxxi) {
                    maxxi = xi;
                }
            }
        }
    }
    // enlarge bounds to get points on the sides
    if (maxxi <= minxi) {
        minxi = minxi - 0.5;
        maxxi = maxxi + 0.5;
    }
    minxi = minxi - (maxxi - minxi) / 100;
    maxxi = maxxi + (maxxi - minxi) / 100;

    if (maxyi <= minyi) {
        minyi = minyi - 0.5;
        maxyi = maxyi + 0.5;
    }
    minyi = minyi - (maxyi - minyi) / 100;
    maxyi = maxyi + (maxyi - minyi) / 100;

    // plot graphs
    left->customPlot->xAxis->setRange(minxi, maxxi);
    left->customPlot->yAxis->setRange(minyi, maxyi);
    left->customPlot->xAxis->setLabel("time");
    itb = portsToPlot.begin();
    for (; itb != ite; itb++) {
        const vle::value::Matrix& view =
          simu.getMatrix(itb->view.toStdString());
        QString portName = itb->port;
        int index = -1;
        for (unsigned int j = 0; j < view.columns(); ++j) {
            if (view.getString(j, 0) == portName.toStdString()) {
                index = j;
            }
        }
        if (index != -1) {
            QVector<double> x(view.rows() - 1), y(view.rows() - 1);
            for (unsigned int i = 1; i < view.rows(); ++i) {
                x[i - 1] = view.getDouble(0, i); // time
                y[i - 1] = getDouble(view, index, i, false);
            }
            // create graph and assign data to it:
            left->customPlot->addGraph();
            if (x.size() == 1) {
                left->customPlot->graph(nbGraphs)->setLineStyle(
                  QCPGraph::lsNone);
                left->customPlot->graph(nbGraphs)->setScatterStyle(
                  QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
            }
            left->customPlot->graph(nbGraphs)->setPen(QPen(itb->color));
            left->customPlot->graph(nbGraphs)->setData(x, y);
            nbGraphs++;
        }
    }
    left->customPlot->replot();
    showCustomPlot((nbGraphs > 0));
}

double
DefaultSimSubpanel::getDouble(const vle::value::Matrix& view,
                              unsigned int col,
                              unsigned int row,
                              bool error_message)
{
    if (not view.get(col, row)) {
        if (error_message) {
            mLog->logExt(QString("output '%1' is null")
                           .arg(view.getString(col, 0).c_str()),
                         true);
        }
        return 0;
    } else if (view.get(col, row)->isInteger()) {
        return (double)view.getInt(col, row);
    } else if (view.get(col, row)->isDouble()) {
        return (double)view.getDouble(col, row);
    } else {
        if (error_message) {
            mLog->logExt(QString("output '%1' is neither an int nor a double")
                           .arg(view.getString(col, 0).c_str()),
                         true);
        }
        return 0;
    }
}

void
DefaultSimSubpanel::onSimulationFinished()
{
    bool oldBlockTree = right->ui->treeSimViews->blockSignals(true);
    right->ui->treeSimViews->clear();
    onTimeout();
    timer.stop();
    if (sim_process and not sim_process->error_simu.isEmpty()) {
        mLog->logExt(sim_process->error_simu, true);
    } else if (sim_process and sim_process->output_map) {
        const vle::value::Map& simu = *sim_process->output_map;
        QList<QTreeWidgetItem*> views_items;
        vle::value::Map::const_iterator itb = simu.begin();
        vle::value::Map::const_iterator ite = simu.end();
        for (; itb != ite; itb++) {
            QString viewName(itb->first.c_str());
            QStringList viewType = mvpz->getViewTypeFromDoc(viewName);
            if (viewType.contains("timed") or viewType.contains("finish")) {

                QTreeWidgetItem* vItem = new QTreeWidgetItem();
                if (viewType.contains("timed")) {
                    double ts = mvpz->timeStepFromDoc(viewName);
                    vItem->setText(0,
                                   QString("%1 (%2:%3)")
                                     .arg(viewName)
                                     .arg("timed")
                                     .arg(QVariant(ts).toString()));
                } else {
                    vItem->setText(
                      0, QString("%1 (%2)").arg(viewName).arg("finish"));
                }
                vItem->setData(0, Qt::UserRole + 0, "view");
                vItem->setData(0, Qt::UserRole + 1, viewName);
                const vle::value::Matrix& res = itb->second->toMatrix();
                for (unsigned int i = 0; i < res.columns(); i++) {
                    if (res.get(i, 0) and res.get(i, 0)->isString()) {
                        QString portName = res.getString(i, 0).c_str();
                        if (portName != "time") {
                            QTreeWidgetItem* pItem = new QTreeWidgetItem();
                            pItem->setText(0, portName);
                            pItem->setFlags(vItem->flags() |
                                            Qt::ItemIsUserCheckable);
                            pItem->setCheckState(0, Qt::Unchecked);
                            pItem->setData(0, Qt::UserRole + 0, "port");
                            pItem->setData(0, Qt::UserRole + 1, viewName);
                            pItem->setData(0, Qt::UserRole + 2, portName);
                            vItem->addChild(pItem);
                        }
                    }
                }
                views_items.append(vItem);
            }
        }
        right->ui->treeSimViews->addTopLevelItems(views_items);
    }

    updateCustomPlot();
    right->ui->treeSimViews->blockSignals(oldBlockTree);

    thread->quit();
    thread->wait();
    delete thread;
    thread = 0;

    left->ui->runButton->setEnabled(true);
}

void
DefaultSimSubpanel::onRunPressed()
{
    // disable buttons and menu
    left->ui->runButton->setEnabled(false);
    right->ui->widSimStyle->setVisible(false);

    // delete custom plot
    delete left->customPlot;
    left->customPlot = new QCustomPlot();
    showCustomPlot(false);
    portsToPlot.erase(portsToPlot.begin(), portsToPlot.end());

    // prepare new thread for simulation
    log_messages.clear();
    index_message = 0;
    delete sim_process;
    sim_process =
      new DefaultSimSubpanelThread(log_messages, debug, nbthreads, blockSize);
    sim_process->init(mvpz, mpkg);

    // delete set
    delete thread;
    thread = new QThread();

    // move to thread
    timer.start(1000);
    sim_process->moveToThread(thread);
    connect(thread, SIGNAL(started()), sim_process, SLOT(onStarted()));
    connect(sim_process,
            SIGNAL(simulationFinished()),
            this,
            SLOT(onSimulationFinished()));
    thread->start();
}

void
DefaultSimSubpanel::onDebugChanged(QString val)
{
    if (val == "true") {
        debug = true;
    } else {
        debug = false;
    }
}

void
DefaultSimSubpanel::onThreadsChanged(int val)
{
    if (val > 0) {
        // set default debug value and disble it
        int index = left->ui->debugCombo->findText("false");
        if (index != -1) {
            left->ui->debugCombo->setCurrentIndex(index);
        }
        left->ui->debugCombo->setEnabled(false);
        // enable block-size
        left->ui->blockSizeSpinBox->setEnabled(true);

    } else {
        // enable debug
        left->ui->debugCombo->setEnabled(true);
        // set default block-size value and disable it
        left->ui->blockSizeSpinBox->setValue(8);
        left->ui->blockSizeSpinBox->setEnabled(false);
    }
    nbthreads = val;
}

void
DefaultSimSubpanel::onBlockSizeChanged(int val)
{
    blockSize = val;
}

void
DefaultSimSubpanel::onTreeItemChanged(QTreeWidgetItem* item, int col)
{
    int state = item->checkState(col);
    QString viewName = item->data(0, Qt::UserRole + 1).toString();
    QString portName = item->data(0, Qt::UserRole + 2).toString();

    if (state == Qt::Checked) {
        addPortToPlot(viewName, portName);
    } else {
        removePortToPlot(viewName, portName);
    }
    updateCustomPlot();
}

void
DefaultSimSubpanel::onTreeItemSelected()
{
    QTreeWidgetItem* item = right->ui->treeSimViews->currentItem();
    bool itemVisility = false;

    if (item) {
        QString viewName = item->data(0, Qt::UserRole + 1).toString();
        QString portName = item->data(0, Qt::UserRole + 2).toString();
        if (portName == "") {
            itemVisility = false;
        } else {
            int state = item->checkState(0);
            if (state == Qt::Checked) {
                itemVisility = true;
                portToPlot* port = getPortToPlot(viewName, portName);
                QString style = "border:1px solid; ";
                style += QString("background-color: rgb(%1, %2, %3);")
                           .arg(port->color.red())
                           .arg(port->color.green())
                           .arg(port->color.blue());
                right->ui->butSimColor->setStyleSheet(style);
            } else {
                itemVisility = false;
            }
        }
    }
    right->ui->widSimStyle->setVisible(itemVisility);
}

void
DefaultSimSubpanel::onTimeout()
{
    for (unsigned int i = index_message; i < log_messages.size(); i++) {
        mLog->logExt(QString(log_messages[i].c_str()), false);
    }
    index_message = log_messages.size();
}

void
DefaultSimSubpanel::onToolColor()
{
    QTreeWidgetItem* item = right->ui->treeSimViews->currentItem();
    QString viewName = item->data(0, Qt::UserRole + 1).toString();
    QString portName = item->data(0, Qt::UserRole + 2).toString();
    if (portName == "") {
        return;
    } else {
        QColorDialog* colorDialog = new QColorDialog(left);
        colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
        if (colorDialog->exec()) {
            portToPlot* p = getPortToPlot(viewName, portName);
            p->color = colorDialog->selectedColor();
            onTreeItemSelected();
            updateCustomPlot();
        }
    }
}
}
} // namespaces
