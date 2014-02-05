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

#ifndef VLEVPZ_H
#define VLEVPZ_H

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
#include "vlepackage.h"
#include "vlevpzconn.h"
#include "vlevpzdynamic.h"
#include "vlevpzport.h"
#include "vpzexpcond.h"
#include "vle/gvle2/ui_filevpztooltip.h"

#undef USE_GRAPHICVIEW

class vleVpzModel;
class vlePackage;

namespace Ui {
class fileVpzTooltip;
}

class vleVpz : public QObject /*, public QXmlDefaultHandler */
{
    Q_OBJECT
public:
    vleVpz();
    vleVpz(const QString &filename);
    QString        getFilename();
    QString        getBasePath();
    void           setBasePath(const QString path);
    vlePackage    *getPackage();
    void           setPackage(vlePackage *package);
    bool           isAltered();
    void           save();
    vleVpzDynamic *getDynamic(QString name);
    void           addDynamic(vleVpzDynamic *dynamic);
    void           removeDynamic(vleVpzDynamic *dynamic);
    QList <vleVpzDynamic *> *getDynamicsList()
    {
        return &mDynamics;
    }

public slots:
    void focusChange(vleVpzModel *model);
    void enterModel(vleVpzModel *model);
    void addModeler(QString name);
    void addModelerDynamic(vleVpzModel *model, QString lib);
    void openModeler();

signals:
    void sigFocus(vleVpzModel *model);
    void sigEnterModel(vleVpzModel *model);
    void sigConditionsChanged();
    void sigDynamicsChanged();
    void sigOpenModeler(vleVpzModel *model);

protected:
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attributes);
public:
    QByteArray xGetXml();
private:
    void xReadDom();
    void xReadDomStructures(const QDomNode &baseNode);
    void xReadDomDynamics(const QDomNode &baseNode);
    void xReadDomExperiments(const QDomNode &baseNode);
    void xReadDomExpConditions(const QDomNode &baseNode);
    void xSaveDom(QDomDocument *doc);
    bool xSaveStructures(QDomDocument *doc, QDomNode *baseNode);
    bool xSaveModel(QDomDocument *doc, QDomElement *baseNode, vleVpzModel *model);
    bool xSaveDynamics(QDomDocument *doc, QDomNode *baseNode);
    bool xSaveExperiments(QDomDocument *doc, QDomElement *baseNode);
    bool xSaveExpCondition(QDomDocument *doc, QDomNode *baseNode, vpzExpCond *cond);
    bool xSaveExpCondValue(QDomDocument *doc, QDomNode *baseNode, vpzExpCondValue *value);
    int  removeModelDynamic(vleVpzModel *model, vleVpzDynamic *dynamic, bool recurse = false);

public:
    vpzExpCond *     addCondition(QString name = "");
    bool             removeCondition(vpzExpCond *cond);
    vpzExpCondValue *addConditionValue(vpzExpCondPort *port, vpzExpCondValue::ValueType type = vpzExpCondValue::TypeUnknown);
    bool             delConditionValue(vpzExpCondPort *port, vpzExpCondValue *value);
    vpzExpCond *     getCondition(QString name);
    vpzExpCond *     getFirstCondition();
    vpzExpCond *     getNextCondition();
    QList <vpzExpCond *> *getConditions()
    {
        return &mConditions;
    }

private:
    QString mFilename;
    QString mPath;
    QString mVleVersion;
    QString mAuthor;
    QString mDate;
    QString mExpBegin;
    QString mExpDuration;
    QString mExpCombination;
    QString mExpName;
    vlePackage *mPackage;
    int                     mConditionIteratorIndex;
    QList <vleVpzDynamic *> mDynamics;
    QList <vpzExpCond    *> mConditions;
    QList <void          *> mViews;
    QDomNode               *mClassesRaw;
    QDomNode               *mViewsRaw;
public:
    vleVpzModel * mRootModel;
};

class vleVpzModel : public QWidget
{
    Q_OBJECT
public:
    vleVpzModel(vleVpz *parent = 0);
    ~vleVpzModel();
    QString getName();
    vleVpz *getVpz();
    void setName(QString name);
    void copyFromModel(vleVpzModel *model);
    void addSubmodel(vleVpzModel *model);
    void delSubmodel(vleVpzModel *model);
    int  countSubmodels();
    vleVpzModel *getSubmodelAt(int i)
    {
        return mSubmodels.at(i);
    }
    QList <vleVpzModel *> *getSubmodels()
    {
        return &mSubmodels;
    }
    QList <vleVpzPort  *> *getInPorts()
    {
        return &mInPorts;
    }
    QList <vleVpzPort  *> *getOutPorts()
    {
        return &mOutPorts;
    }
    QList <vleVpzConn  *> *getConnections()
    {
        return &mConnections;
    }
    vleVpzDynamic *getDynamic();
    void setDynamic(QString dynamicName);
    void removeDynamic();

    void addCondition(vpzExpCond *cond);
    void removeCondition(vpzExpCond *cond);
    QString getConditionStringList();
    bool hasCondition(vpzExpCond *cond)
    {
        int i;
        for (i = 0; i < mConditions.count(); i++)
        {
            if (mConditions.at(i) == cond)
                return true;
        }
        return false;
    }

    QString getObservables()
    {
        return mObservables;
    }

    bool hasModeler();
    vpzExpCond *getModelerExpCond();

    bool isAltered();
    void fixWidgetSize(bool doResize = false);
    int getWidth()  { return mWidgetWidth; }
    int getHeight() { return mWidgetHeight; }
    int getRealWidth();
    int getX()      { return mWidgetX; }
    int getY()      { return mWidgetY; }
    int nextInPosY() { mWidInPortY += 16;  return mWidInPortY;  }
    int nextOutPosY(){ mWidOutPortY += 16; return mWidOutPortY; }
    void dispNormal();
    void dispMaximize();
    bool isMaximized();
    void select(bool setFocus = 0);
    void deselect();
    bool isSelected();
    void deselectSub();
    bool mouseMultipleSelect();
    vleVpzModel* getSubmodelByName(QString *name);
    vleVpzPort * getInPortByName(QString *name);
    vleVpzPort * getOutPortByName(QString *name);
protected:
    bool event(QEvent *event);
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void hideEvent (QHideEvent * event);
    void showEvent (QShowEvent * event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent * event);

signals:
     void sigFocus(vleVpzModel *model);
     void sigDblClick(vleVpzModel *model);
     void sigAddModeler(QString name);
     void sigOpenModeler();

public slots:
     void contextMenu(const QPoint & pos);
     void contextMenu(const QPoint & pos, vleVpzPort *port);
     void portConnect(vleVpzPort *port);
     void portDisconnect(vleVpzPort *port);

public:
    void xLoadNode(const QDomNode &baseNode);
private:
    void xreadSubmodels(const QDomNode &baseNode);
    void xReadInputs(const QDomNode &baseNode);
    void xReadOutputs(const QDomNode &baseNode);
    void xReadConnections(const QDomNode &baseNode);
    bool xgetXml(QDomDocument *doc, QDomElement *base);
private:
    QString mName;
    vleVpz *mVpz;
    bool    mIsAltered;
    int     mWidgetHeight;
    int     mWidgetWidth;
    int     mWidgetX;
    int     mWidgetY;
    int     mWidInPortY;
    int     mWidOutPortY;
    QList <vleVpzModel *> mSubmodels;
    QList <vleVpzPort  *> mInPorts;
    QList <vleVpzPort  *> mOutPorts;
    QList <vleVpzConn  *> mConnections;
    vleVpzDynamic *mDynamic;
    QList <vpzExpCond *>  mConditions;
    QString               mObservables;
    QLabel  mTitle;
    bool    mIsMaximized;
    QPoint  mOldPos, mStartPos;
    bool    mIsFocus;
    QWidget *mTooltip;
    Ui::fileVpzTooltip *uiTooltip;
    bool    mIsSelected;
    // Used for mouse selection into maximized model
    bool    mSelect;
    QPoint  mSelectStart;
    QPoint  mSelectCurrent;
    // Used for port selection/connection
    vleVpzPort *mPortInSel;
    vleVpzPort *mPortOutSel;
private:
    QFont   mPainterFont;
private:
    int     mSettingLine;
    int     mSettingCorner;
    int     mSettingFontSize;

public:
    sourceCpp *getModelerClass();

private:
    sourceCpp *mModelerClass;
};

#endif // VLEVPZ_H
