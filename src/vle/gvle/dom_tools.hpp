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

#ifndef GVLE_DOM_TOOLS_H
#define GVLE_DOM_TOOLS_H

#include <QDebug>

#include <QDateTimeEdit>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QString>
#include <QStyleOption>
#include <QWidget>
#include <QXmlDefaultHandler>
#include <vle/value/Map.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace gvle {

/**
 * @brief DomFunctions provides static functions to handle Dom documents
 * (for undo/redo)
 */
class DomFunctions
{
public:
    DomFunctions();
    virtual ~DomFunctions();

    /**************************************************
     * Static functions, generic XML DOM
     * NOTE: no snapshot
     **************************************************/

    /**
     * @brief get attribute value of a node which name is attrName
     */
    static QString attributeValue(const QDomNode& node,
                                  const QString& attrName);
    /**
     * @brief set an attribute value to a node
     */
    static void setAttributeValue(QDomNode& node,
                                  const QString& attrName,
                                  const QString& val);
    /**
     * @brief get the first child corresponding to the name and add it
     * if not present
     * @param node, the node to which one appends a child
     * @param nodeName, the nodeName of the child name
     * @param domDoc, if not null and node is not found then the node
     * is created
     * @param the (created or found) resulting node
     */
    static QDomNode obtainChild(QDomNode node,
                                const QString& nodeName,
                                QDomDocument* domDoc = 0);

    static QString toQString(const QDomNode& node);
    static QStringList toQStringFormat(const QDomNode& node);
    /**
     * @brief Remove all childs from a QDomNode (keep attributes)
     */
    static void removeAllChilds(QDomNode node);

    /**
     * @brief get a child with a specific nodename and a specific value
     * for attribute "name".
     * @param node, the node to which one appends a child
     * @param nodeName, the name of the node to search
     * @param nameValue, used to create the QDomNode
     * @param domDoc, if not null and node is not found then the node
     * is created
     * @param the (created or found or null) resulting node
     */
    static QDomNode childWhithNameAttr(QDomNode node,
                                       const QString& nodeName,
                                       const QString& nameValue,
                                       QDomDocument* domDoc = 0);
    /**
     * @brief get values of attribute "name" of children for
     * some child_node
     * @param node, a node of the form
     * <parent_node ... >
     *   <child_node name="A"/>
     *   <child_node name="B"/>
     *   ...
     * <parent_node/>
     * @param child_node, the name of children nodes
     * @param a list ("A", "B")
     */
    static QSet<QString> childNames(QDomNode node, QString child_node);
    static QList<QDomNode> childNodesWithoutText(QDomNode node,
                                                 const QString& nodeName = "");

    /**
     * @brief get a new name for a child of QDomNode
     * @param node, a node of the form
     * <parent_node ... >
     *   <child_node name="A"/>
     *   <child_node name="B"/>
     *   ...
     * <parent_node/>
     * @param child_node, the name of child nodes
     * @param prefix, the prefix of the name to provide
     * @param exclude, a list of name to exlude
     * @retrun a name starting from prefix and different from "A", "B", etc..
     */
    static QString childNameProvider(QDomNode node,
                                     QString child_node,
                                     QString prefix,
                                     const QSet<QString>& exclude);
    static QString childNameProvider(QDomNode node,
                                     QString child_node,
                                     QString prefix);
};

/**
 * @brief DomObject is an interface to be used into a DomDiffStack
 * (for undo/redo)
 */
class DomObject
{
public:
    DomObject(QDomDocument* doc)
      : mDoc(doc)
    {
    }
    virtual ~DomObject()
    {
    }
    virtual QString getXQuery(QDomNode node) = 0;
    virtual QDomNode getNodeFromXQuery(const QString& query,
                                       QDomNode d = QDomNode()) = 0;
    QDomDocument* mDoc;
};

/**
 * @brief DomDiffStack handle a stack of undo/redo objects based on the copy
 * of registered QDomNode (via snapshots)
 */
class DomDiffStack : public QObject
{
    Q_OBJECT
public:
    struct DomDiff
    {
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
    DomObject* mVdo;
    QString current_source;
    bool snapshotEnabled;

    DomDiffStack(DomObject* vdo);
    virtual ~DomDiffStack();

    void init(QDomNode node);
    bool enableSnapshot(bool enable);
    void snapshot(QDomNode node);
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
    void snapshot(QDomNode node,
                  QString mergeType,
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
     *          1, if the stack changed undo action from not available to
     * avail.
     */
    static int computeUndoAvailability(unsigned int prevCurr,
                                       unsigned int curr,
                                       unsigned int saved);
    void tryEmitUndoAvailability();
signals:
    void undoRedoVdo(QDomNode oldVal, QDomNode newVal);
    void snapshotVdo(QDomNode snapshot, bool isMerged);
    void undoAvailable(bool);
};
}
} // namepsaces

#endif
