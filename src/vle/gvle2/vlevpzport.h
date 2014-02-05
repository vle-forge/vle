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

#ifndef VLEVPZPORT_H
#define VLEVPZPORT_H

#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QWidget>
#include "vlevpz.h"
#include "vlevpzconn.h"

class vleVpzPort : public QWidget
{
    Q_OBJECT
public:
    enum portType { TypeUnknown, TypeInput, TypeOutput };
public:
    explicit vleVpzPort(QWidget *parent = 0);
    ~vleVpzPort();
    vleVpzModel *getModel();
    QString  getName(QString *name = 0);
    void     setName(QString name);
    void     setType(portType type);
    portType getType();
    int      getPosY();
    void     select(bool status);
    vleVpzConn* getConn();
    void        setConn(vleVpzConn *conn);
    void        unConn(vleVpzConn *conn);
    bool        isConnected(vleVpzConn *conn);
    void        edit(bool doEdit = true);

public slots:
    void contextMenu(const QPoint & pos);
    void editEnd();

protected:
    void hideEvent  (QHideEvent *event);
    void showEvent  (QShowEvent *event);
    void enterEvent (QEvent     *event);
    void leaveEvent (QEvent     *event);
    void mousePressEvent (QMouseEvent *event);
    void updLabel();
private:
    vleVpzModel *getTopModel();

private:
    QString  mName;
    portType mType;
    bool     mIsSelected;
    vleVpzModel *mModel;
    QList <vleVpzConn *> mConnections;
    int      mSavedPosX;

    QLabel   mIcon;
    QLabel   mTitle;
    QLineEdit mEdit;
};

#endif // VLEVPZPORT_H
