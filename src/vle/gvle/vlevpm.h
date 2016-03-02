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

#ifndef gvle_VLEVPM_H
#define gvle_VLEVPM_H

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
#include "gvle_plugins.h"
#include "vlevpz.h"
#include "vleDomDiffStack.h"


namespace vle {
namespace gvle {

class PluginOutput;
class PluginExpCond;


/**
 * @brief Class that implements vleDomObject especially for vleVpm
 */
class vleDomVpm : public vleDomObject
{
public:
    vleDomVpm(QDomDocument* doc);
    ~vleDomVpm();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

/**
 * @brief class that handles a vleVpz using metadata xml file
 * (Virtual Project Metadata - vpm)
 * The two xml files have to be coherent (same number of snapshots)
 *
 *                                             VleDomObject
 *                                                ^
 *                                      ----------|
 *                                      |         |
 *    VleVpz   <>-- VleDomDiffStack<VleDomVpz>    |
 *       ^                              -----------
 *       |                              |
 *    VleVpm   <>-- VleDomDiffStack<VleDomVpm>
 *
 */
class vleVpm : public vleVpz
{
    Q_OBJECT
public:
    vleVpm(const QString& vpzpath, const QString& vpmpath,
            gvle_plugins* plugs);
    QString toQString(const QDomNode& node) const;

    void xReadDom();

    void addConditionFromPluginToDoc(const QString& condName,
            const QString& pluginName);
    void renameConditionToDoc(const QString& oldName, const QString& newName);
    bool setOutputPluginToDoc(const QString& viewName,
            const QString& outputPlugin);
    /**
     * @brief Provides outputGUIplugin initialized with a view
     * without snapshot
     */
    PluginOutput* provideOutputGUIplugin(const QString& viewName);
    /**
     * @brief Provides condPlugin initialized with a cond
     * without snapshot
     */
    PluginExpCond* provideCondGUIplugin(const QString& condName);

    void addViewToDoc(const QString& viewName);
    void renameViewToDoc(const QString& oldName, const QString& newName);
    void rmConditionToDoc(const QString& condName);
    QString getCondGUIplugin(const QString& condName) const;
    QString getOutputGUIplugin(const QString& viewName);
    void setCurrentTab(QString tabName);

    void   save();

    void undo();
    void redo();

public slots:
    void onSnapshotVpz(QDomNode snapVpz, bool isMerged);
    void onUndoRedoVpz(QDomNode oldValVpz, QDomNode newValVpz);
    void onSnapshotVpm(QDomNode snapVpm, bool isMerged);
    void onUndoRedoVpm(QDomNode oldValVpm, QDomNode newValVpm);
    void onUndoAvailable(bool);

signals:
    void undoRedo(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);
    void undoAvailable(bool);
private:
    void xCreateDom();
    void renameCondGUIplugin(const QString& oldCond, const QString& newCond);
    void setCondGUIplugin(const QString& condName, const QString& name);
    void setOutputGUIplugin(const QString& viewName, const QString& pluginName);

    void tryEmitUndoAvailability(int prevCurr, int curr);


    QDomDocument*     mDocVpm;
    QString           mFileNameVpm;
    vleDomVpm*        mVdoVpm;
    vleDomDiffStack*  undoStackVpm;
    bool              waitUndoRedoVpz;
    bool              waitUndoRedoVpm;
    QDomNode          oldValVpz;
    QDomNode          newValVpz;
    QDomNode          oldValVpm;
    QDomNode          newValVpm;
    gvle_plugins*     mGvlePlugins;
};

}}//namespaces

#endif
