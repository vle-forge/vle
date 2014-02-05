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

#include <QPoint>
#include <QDebug>
#include "vlevpzconn.h"
#include "vlevpz.h"

/**
 * @brief vleVpzConn::vleVpzConn
 *        Default constructor
 *
 */
vleVpzConn::vleVpzConn(vleVpzModel *model)
{
    mType   = Internal;
    mParent = model;
    mSourcePort = 0;
    mDestPort   = 0;
    mMaxX = 0;
    mMaxY = 0;
    mHighlight = false;
}

/**
 * @brief vleVpzConn::~vleVpzConn
 *        Default destructor
 *
 */
vleVpzConn::~vleVpzConn()
{
    // If a source port is defined
    if (mSourcePort)
        // Disconnect the port
        mSourcePort->unConn(this);
    // If a destination port is defined
    if (mDestPort)
        // Disconnect the port
        mDestPort->unConn(this);
}

/**
 * @brief vleVpzConn::setType
 *        Set or change the type of the connection
 *
 */
void vleVpzConn::setType(vleVpzConn::ConnType nType)
{
    mType = nType;
}

/**
 * @brief vleVpzConn::type
 *        Get the type of the connection
 *
 */
vleVpzConn::ConnType vleVpzConn::type()
{
    return mType;
}

/**
 * @brief vleVpzConn::typeName
 *        Return a human readable version of the type
 *
 */
QString vleVpzConn::typeName()
{
    if (mType == Internal)
        return "internal";
    if (mType == In)
        return "input";
    if (mType == Out)
        return "output";
    return "unknown";
}

/**
 * @brief vleVpzConn::setSource
 *        Set the source port of the connection
 *
 */
void vleVpzConn::setSource(vleVpzPort *port)
{
    if (port == 0)
    {
        qDebug() << QObject::tr("vleVpzConn::setSource Invalid port specified");
        throw -1;
    }

    mSourcePort = port;

    mSourcePort->setConn(this);
}

/**
 * @brief vleVpzConn::getSource
 *        Return the vpzModel source
 *
 */
vleVpzModel *vleVpzConn::getSource()
{
    if (mSourcePort == 0)
        return 0;

    return mSourcePort->getModel();
}

/**
 * @brief vleVpzConn::getSourceModelName
 *        Return the name of the source model
 *
 */
QString vleVpzConn::getSourceModelName()
{
    if (mSourcePort == 0)
        return "";

    return mSourcePort->getModel()->getName();
}

/**
 * @brief vleVpzConn::getSourcePortName
 *        Return the name of the source port
 *
 */
QString vleVpzConn::getSourcePortName()
{
    if (mSourcePort == 0)
        return "";

    return mSourcePort->getName();
}

/**
 * @brief vleVpzConn::getDestinationModelName
 *        Return the name of the destination model
 *
 */
QString vleVpzConn::getDestinationModelName()
{
    if (mDestPort == 0)
        return "";

    return mDestPort->getModel()->getName();
}

/**
 * @brief vleVpzConn::getDestinationPortName
 *        Return the name of the destination port
 *
 */
QString vleVpzConn::getDestinationPortName()
{
    if (mDestPort == 0)
        return "";

    return mDestPort->getName();
}

/**
 * @brief vleVpzConn::setDestination
 *        Set the destination port of the connection
 *
 */
void vleVpzConn::setDestination(vleVpzPort *port)
{
    if (port == 0)
    {
        qDebug() << QObject::tr("vleVpzConn::setDestination Invalid port specified");
        throw -1;
    }

    mDestPort = port;

    mDestPort->setConn(this);
}

/**
 * @brief vleVpzConn::getSource
 *        Return the vpzModel source
 *
 */
vleVpzModel *vleVpzConn::getDestination()
{
    if (mDestPort == 0)
        return 0;

    return mDestPort->getModel();
}

/**
 * @brief vleVpzConn::isValid
 *        Return true if the connection is fully defined and valid
 *
 */
bool vleVpzConn::isValid()
{
    if (mSourcePort && mDestPort)
        return true;

    if ( ! mSourcePort)
        qDebug() << "vleVpzConn::isValid() no src";

    if ( ! mDestPort)
        qDebug() << "vleVpzConn::isValid() no dst";

    return false;
}

/**
 * @brief vleVpzConn::isLinkedWith
 *        Return true if the specified model is used by the connection (as source or dest)
 *
 */
bool vleVpzConn::isLinkedWith(vleVpzModel *model)
{
    if (mSourcePort->getModel() == model)
        return true;

    if (mDestPort->getModel() == model)
        return true;

    return false;
}

/**
 * @brief vleVpzConn::setHighlight
 *        Mark/unmark the connection as highlighted
 *
 */
void vleVpzConn::setHighlight(bool hl)
{
    mHighlight = hl;
}

/**
 * @brief vleVpzConn::isHighlighted
 *        Test the connection is currently highlighted
 *
 */
bool vleVpzConn::isHighlighted()
{
    return mHighlight;
}

/**
 * @brief vleVpzConn::route
 *        Compute and trace the polyline of the conn
 *
 */
void vleVpzConn::route()
{
    int xpoint, ypoint;
    int xsrc, ysrc;
    int xdst, ydst;
    mLines.clear();

    vleVpzModel *srcModel = mSourcePort->getModel();
    vleVpzModel *dstModel = mDestPort->getModel();

    if (mType == vleVpzConn::In)
    {
        xsrc = 10;
        ysrc = mSourcePort->getPosY();
    }
    else
    {
        xsrc = srcModel->getX() + srcModel->getWidth();
        ysrc = srcModel->getY() + mSourcePort->getPosY();
    }

    if (mType == vleVpzConn::Out)
    {
        xdst = dstModel->getRealWidth() - 10;
        ydst = mDestPort->getPosY();
    }
    else
    {
        xdst = dstModel->getX();
        ydst = dstModel->getY()   + mDestPort->getPosY();
    }

    // Add the origin point
    mLines << QPoint(xsrc, ysrc);
    mMaxX = xsrc;
    // Add an horizontal seg (use port Y coordinate as offset)
    xpoint = xsrc + mSourcePort->y();
    if (xpoint > mMaxX)
        mMaxX = xpoint;
    mLines << QPoint(xpoint, ysrc);
    // Add a vertical seg : 50% of the Y distance
    int yTmp = ysrc + ( (ydst - ysrc) / 2);
    if (xdst < xsrc)
    {
        if (ydst < ysrc)
        {
            if (yTmp > srcModel->getY())
            {
                yTmp = srcModel->getY() - mSourcePort->y();
            }
        }
        else
        {
            if (yTmp < (srcModel->getY() + srcModel->getHeight() + 10))
            {
                yTmp = srcModel->getY() + srcModel->getHeight() + 10 + mSourcePort->y();
            }
        }
    }
    ypoint = yTmp;
    mLines << QPoint(xpoint, ypoint);
    // Horizontal line to destination model
    xpoint = xdst - mDestPort->getPosY();
    if (xpoint > mMaxX)
        mMaxX = xpoint;
    mLines << QPoint(xpoint, ypoint);
    // Vertical line to input port Y
    ypoint = ydst;
    mLines << QPoint(xpoint, ypoint);
    // Last horizontal line to port
    xpoint = xdst;
    if (xpoint > mMaxX)
        mMaxX = xpoint;
    mLines << QPoint(xpoint, ypoint);
}
