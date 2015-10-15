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

#ifndef GVLE2_VLEVPM_H
#define GVLE2_VLEVPM_H

#include <QDebug>


#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QStyleOption>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlDefaultHandler>
#include <QDateTimeEdit>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>


namespace vle {
namespace gvle2 {

class vpzVpm : public QObject
{
    Q_OBJECT
public:
    vpzVpm();
    vpzVpm(const QString& filename);
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    void xSaveDom();
    void setCondPlugin(const QString& condName, const QString& name);
    QString getCondPlugin(const QString& condName);


    QDomDocument* mDoc;
    QString       mFileName;
};

}}//namespaces

#endif
