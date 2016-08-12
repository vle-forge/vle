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

#ifndef gvle_VLEDOMDIFFSTACK_H
#define gvle_VLEDOMDIFFSTACK_H

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
namespace gvle {

/**
 * @brief VleDomObject is an interface to be used into a vleDomDiffStack
 * (for undo/redo)
 */
class vleDomObject
{
public:
    vleDomObject(QDomDocument* doc);
    virtual ~vleDomObject();
    virtual QString getXQuery(QDomNode node) = 0;
    virtual QDomNode getNodeFromXQuery(const QString& query,
            QDomNode d=QDomNode()) = 0;

    /**************************************************
     * Static functions
     **************************************************/

    /**
     * @brief get attribute value of a node which name is attrName
     */
    static QString attributeValue(const QDomNode& node,
            const QString& attrName);


    /**
     * @brief set an attribute value to a node
     */
    static void setAttributeValue(QDomNode& node, const QString& attrName,
            const QString& val);

    /***************************************************
     * Menber functions :
     * TODO some functions can be set to static
     ***************************************************/
    QDomNode childWhithNameAttr(QDomNode node,
            const QString& nodeName, const QString& nameValue) const;
    /**
     * @brief get the first child corresponding to the name and add it
     * if not present
     */
    QDomNode obtainChild(QDomNode node, const QString& nodeName,
            bool addIfNot=true);

    /**
     * @brief get a QString representation of a QDomNode
     */
    virtual QString toQString(const QDomNode& node) const;
    QDomDocument* mDoc;
};

class vleDomDiffStack : public QObject
{
    Q_OBJECT
public:
    struct DomDiff {
        QDomNode node_before;
        QDomNode node_after;
        QString query;
        QString merge_type;
        vle::value::Map* merge_args;
        QString source;
        bool isDefined;

        DomDiff();
        ~DomDiff();
        void reset();
    };

    std::vector<DomDiff> diffs;
    unsigned int prevCurr;
    /**
     * current state
     */
    unsigned int curr;
    /**
     * state corresponding to the save
     */
    unsigned int saved;
    vleDomObject* mVdo;
    QString current_source;
    bool snapshotEnabled;

    vleDomDiffStack(vleDomObject* vdo);

    void init (QDomNode node);
    bool enableSnapshot(bool enable);
    void snapshot (QDomNode node);
    /**
     * @brief  specific snapshot for possible merge
     * @param node, the node to save
     * @param mergeType, the id of the merge. This snapshot can be merged only
     * if the previous snapshot hase the same mergeType
     * @param a map for giving more details. It can contains either
     *  - a string "query" that identifies the query of node in the DOM object
     *  concerned with the action
     *  - a set of string "queries" that identifies the set of nodes in the
     *  DOM object concerned with the action
     *  The queries should be the same in order to merge with previous snapshot
     */
    void snapshot (QDomNode node, QString mergeType,
            vle::value::Map* mergeArgs);

    void undo();
    void redo();
    /**
     * @brief register save state, eg on save action
     */
    void registerSaveState();
    /**
     * @brief clear the stack
     */
    void clear();

    /**
     * @brief print the state of the stack
     */
    void print(std::ostream& out) const;


    /**
     * @brief tells if the last action (snapshot, undo or redo)
     * changed the state of undo availability
     * @return -1, if the stack changed undo action from available to not.
     *          0, if the stack did not changed undo availability
     *          1, if the stack changed undo action from not available to avail.
     */
    int getUndoAvailability() const;
    static int computeUndoAvailability(
            unsigned int prevCurr,
            unsigned int curr,
            unsigned int saved);
    void tryEmitUndoAvailability();
signals:
    void undoRedoVdo(QDomNode oldVal, QDomNode newVal);
    void snapshotVdo(QDomNode snapshot, bool isMerged);
    void undoAvailable(bool);


};

}}//namepsaces

#endif
