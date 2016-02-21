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


#include "DefaultSimSubpanel.h"

namespace vle {
namespace gvle {



DefaultSimSubpanel::DefaultSimSubpanel():
            PluginSimPanel()
{
    customPlot = new QCustomPlot();

    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);
    customPlot->replot();
}

DefaultSimSubpanel::~DefaultSimSubpanel()
{

}
void
DefaultSimSubpanel::init(vleVpm* vpm, vle::utils::Package* pkg)
{

}

QString
DefaultSimSubpanel::getname()
{
    return "Default";
}

QWidget*
DefaultSimSubpanel::leftWidget()
{
    return customPlot;
}

QWidget*
DefaultSimSubpanel::rightWidget()
{
    return 0;
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

}} //namespaces


