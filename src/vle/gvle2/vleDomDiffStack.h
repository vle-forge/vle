/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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

#ifndef GVLE2_VLEDOMDIFFSTACK_H
#define GVLE2_VLEDOMDIFFSTACK_H

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
#ifndef Q_MOC_RUN 
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#endif

namespace vle {
namespace gvle2 {

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
    QDomNode childWhithNameAttr(QDomNode node,
            const QString& nodeName, const QString& nameValue) const;
    /**
     * @brief get the first child corresponding to the name and add it
     * if not present
     */
    QDomNode obtainChild(QDomNode node, const QString& nodeName,
            bool addIfNot=true);
    /**
     * @brief get attribute value of a node which name is attrName
     */
    QString attributeValue(const QDomNode& node, const QString& attrName) const;
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
    unsigned int curr;
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

signals:
    void undoRedoVdo(QDomNode oldVal, QDomNode newVal);
    void snapshotVdo(QDomNode snapshot, bool isMerged);


};

}}//namepsaces

#endif
