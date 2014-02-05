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
#ifndef VLEVPZCONN_H
#define VLEVPZCONN_H
#include <QString>
#include <QPolygon>

// Declare classes to avoid recursive inclusion problem
class vleVpzModel;
class vleVpzPort;

class vleVpzConn
{
public:
    enum ConnType { Internal, In, Out };

public:
    vleVpzConn(vleVpzModel *model = 0);
    ~vleVpzConn();
    void setType(ConnType nType);
    ConnType type();
    QString  typeName();
    void setSource(vleVpzModel *model, QString portName);
    void setSource(vleVpzPort  *port);
    void setDestination(vleVpzModel *model, QString portName);
    void setDestination(vleVpzPort *port);
    vleVpzModel *getSource();
    vleVpzModel *getDestination();
    QString getSourceModelName();
    QString getSourcePortName();
    QString getDestinationModelName();
    QString getDestinationPortName();
    bool isValid();
    void route();
    bool isLinkedWith(vleVpzModel *model);
    int  getMaxX() { return mMaxX; }
    void setHighlight(bool hl = true);
    bool isHighlighted();

public:
    QPolygon mLines;
private:
    ConnType     mType;
    vleVpzModel *mParent;
    vleVpzModel *mSource;
    vleVpzPort  *mSourcePort;
    vleVpzPort  *mDestPort;
    int          mMaxX;
    int          mMaxY;
    bool         mHighlight;
};

#endif // VLEVPZCONN_H
