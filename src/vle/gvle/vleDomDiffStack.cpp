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

#include <QClipboard>
#include <QFlags>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QtDebug>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include "vleDomDiffStack.h"

#include <iostream>

namespace vle {
namespace gvle {


vleDomObject::vleDomObject(QDomDocument* doc): mDoc (doc)
{
}

vleDomObject::~vleDomObject()
{
}

/**************************************************
 * Static functions
 **************************************************/

QString
vleDomObject::attributeValue(const QDomNode& node,
        const QString& attrName)
{
    if (node.attributes().contains(attrName)) {
        return node.attributes().namedItem(attrName).nodeValue();
    }
    return "";
}

void
vleDomObject::setAttributeValue(QDomNode& node, const QString& attrName,
        const QString& val)
{
    if (node.attributes().contains(attrName)) {
         node.attributes().namedItem(attrName).setNodeValue(val);
    } else {
        node.toElement().setAttribute(attrName, val);
    }
}

QDomNode
vleDomObject::obtainChild(QDomDocument& domDoc,
        QDomNode node, const QString& nodeName, bool addIfNot)
{
    QDomNodeList chs = node.childNodes();
    for(int i = 0; i<chs.size(); i++) {
        QDomNode ch = chs.at(i);
        if (ch.nodeName() == nodeName) {
            return ch;
        }
    }
    if (not addIfNot) {
        return QDomNode();
    }
    QDomNode res = domDoc.createElement(nodeName);
    node.appendChild(res);
    return res;
}

QDomNode
vleDomObject::childWhithNameAttr(const QDomNode& node,
        const QString& nodeName, const QString& nameValue)
{
    QDomNodeList childs = node.childNodes();
    QList<QDomNode> childsWithoutText;
    for (int i=0; i<childs.length();i++) {
        QDomNode ch = childs.at(i);
        if (not ch.isText() and ch.nodeName() == nodeName and
                vleDomObject::attributeValue(ch, "name") == nameValue) {
            return ch;
        }
    }
    return QDomNode();
}

QString
vleDomObject::toQString(const QDomNode& node)
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}



void
vleDomObject::removeAllChilds(QDomNode node)
{
    QDomNodeList childs = node.childNodes();
    while(node.hasChildNodes()) {
        node.removeChild(childs.item(0));
    }
}

QList<QDomNode>
vleDomObject::childNodesWithoutText(QDomNode node, const QString& nodeName)
{
    QDomNodeList childs = node.childNodes();
    QList<QDomNode> childsWithoutText;
    for (int i=0; i<childs.length();i++) {
        QDomNode ch = childs.at(i);
        if (not ch.isText()) {
            if (nodeName == "") {
                childsWithoutText.push_back(ch);
            } else if (ch.nodeName() == nodeName) {
                childsWithoutText.push_back(ch);
            }
        }
    }
    return childsWithoutText;
}



/********************************************************************/

vleDomDiffStack::DomDiff::DomDiff():node_before(), node_after(), query(""),
        merge_type("null"), merge_args(0), source(""), isDefined(false)
{
}

vleDomDiffStack::DomDiff::~DomDiff()
{
    if (merge_args) {
        delete merge_args;
        merge_args = 0;
    }
}
void
vleDomDiffStack::DomDiff::reset()
{
    node_before = QDomNode();
    node_after = QDomNode();
    query = "";
    merge_type = "null";
    if (merge_args) {
        delete merge_args;
        merge_args = 0;
    }
    source = "";
    isDefined = false;
}

vleDomDiffStack::vleDomDiffStack(vleDomObject* vdo): diffs(), prevCurr(0),
        curr(0), saved(0), mVdo(vdo), current_source(""), snapshotEnabled(true)
{
}

void
vleDomDiffStack::init(QDomNode node)
{
    diffs.resize(300);
    diffs[0].node_before = node.cloneNode();
    prevCurr = 0;
    curr = 0;
    saved = 0;
}

bool
vleDomDiffStack::enableSnapshot(bool enable)
{
    bool oldSnapshotEnabled = snapshotEnabled;
    snapshotEnabled = enable;
    return oldSnapshotEnabled;
}




void
vleDomDiffStack::snapshot(QDomNode node)
{
    snapshot(node, "null", 0);
}


void
vleDomDiffStack::snapshot (QDomNode node,
        QString mergeType,
        vle::value::Map* mergeArgs)
{

    if (not snapshotEnabled) {
        return;
    }
    bool isMerged = false;
    if (node.isNull()) {
        //false snaphot
        prevCurr = curr;
        curr ++;//TODO manage size
        if (curr >= diffs.size()) {
            qDebug() << " Internal error vleDomDiffStack::snapshot (size to big) ";
            return;
        }
        diffs[curr].query = "";
        diffs[curr].node_before = QDomNode();
        diffs[curr].merge_type = "null";
        diffs[curr].merge_args = 0;
        diffs[curr].source = current_source;
        diffs[curr].isDefined = true;

        //unvalidate all undo stack
        for (unsigned int i = curr+1; i<diffs.size(); i++) {
            diffs[i].reset();
        }
    } else {
        //check if it is possibly mergeable to top undo command
        if ((curr > 0) and (mergeType != "null") and curr != saved
                and (diffs[curr].merge_type == mergeType) and
                (diffs[curr].query == mVdo->getXQuery(node))){
            if (mergeArgs->exist("query")) {
                std::string q1 = diffs[curr].merge_args->getString("query");
                std::string q2 = mergeArgs->getString("query");
                if (q1 == q2) {
                    isMerged = true;
                }
            } else if (mergeArgs->exist("queries")) {
                const vle::value::Set& q1 =
                        diffs[curr].merge_args->getSet("queries");
                const vle::value::Set& q2 = mergeArgs->getSet("queries");
                //check if q1 and q2 contains the same model queries
                if ((q1.size() == q2.size()) and (q1.size() > 0)) {
                    bool allFound = true;
                    for (unsigned int i=0; i<q1.size(); i++) {
                        std::string q1i = q1.getString(i);
                        bool found = false;
                        for (unsigned int j=0; j<q2.size(); j++) {
                            found = found or (q1i ==  q2.getString(j));
                        }
                        allFound = allFound and found;
                    }
                    isMerged = allFound;
                }
            }
        }

        if (not isMerged) {
            prevCurr = curr;
            curr ++;//TODO manage size
            if (curr >= diffs.size()) {
                qDebug() << " Internal error vleDomDiffStack::snapshot (size to big) ";
                return;
            }
            diffs[curr].query = mVdo->getXQuery(node);
            diffs[curr].node_before = node.cloneNode();
            diffs[curr].merge_type = mergeType;
            diffs[curr].merge_args = mergeArgs;
            diffs[curr].source = current_source;
            diffs[curr].isDefined = true;
            //unvalidate all undo stack
            for (unsigned int i = curr+1; i<diffs.size(); i++) {
                diffs[i].reset();
            }
        }
    }
    emit snapshotVdo(node, isMerged);
    tryEmitUndoAvailability();
}

void
vleDomDiffStack::undo()
{
    if (curr <= 0) {
        return;
    }

    if ((diffs[curr].source == current_source) and (diffs[curr].isDefined)) {
        if (diffs[curr].query == ""){
            emit undoRedoVdo(QDomNode(), QDomNode());
        } else {
            diffs[curr].node_after =
                    mVdo->getNodeFromXQuery(diffs[curr].query).cloneNode();
            QDomNode currN = mVdo->getNodeFromXQuery(diffs[curr].query);
            QDomNode parent = currN.parentNode();
            if (parent.isNull()) {
                qDebug() << " Internal error vleDomDiffStack::undo() "
                        << diffs[curr].query;
            }
            parent.replaceChild(diffs[curr].node_before, currN);
            emit undoRedoVdo(currN, diffs[curr].node_before);
        }
        prevCurr = curr;
        curr --;
    }
    tryEmitUndoAvailability();
}

void
vleDomDiffStack::redo()
{

    if (diffs[curr+1].isDefined) {
        if ((diffs[curr+1].source == current_source) or
                (diffs[curr+1].source.isEmpty())){
            prevCurr = curr;
            curr++;
            QDomNode currN = mVdo->getNodeFromXQuery(diffs[curr].query);
            QDomNode parent = currN.parentNode();
            parent.replaceChild(diffs[curr].node_after, currN);
            emit undoRedoVdo(currN, diffs[curr].node_after);
        }
    }
    tryEmitUndoAvailability();
}

void
vleDomDiffStack::registerSaveState()
{
    saved = curr;
    tryEmitUndoAvailability();
}

void
vleDomDiffStack::clear()
{
    for (unsigned int i = 0; i<diffs.size(); i++) {
        diffs[i].reset();
    }
    prevCurr = curr;
    curr = 0;
    tryEmitUndoAvailability();
}

void
vleDomDiffStack::print(std::ostream& out) const
{
    unsigned int diffSize = 1;
    while (diffs[diffSize].isDefined){
        diffSize++;
    }
    out << " [vleDomDiffStack] nb stack size=" << diffSize
            << ", saved=" << saved << ", curr=" << curr
            << ", prevCurr=" << prevCurr
            << ", undoAvail=" << getUndoAvailability()<< "\n";
}

int
vleDomDiffStack::computeUndoAvailability(
        unsigned int prevCurr,
        unsigned int curr,
        unsigned int saved)
{
    if (prevCurr != saved and curr == saved ) {
        return -1;
    }
    if (prevCurr == saved and curr != saved ) {
        return 1;
    }
    return 0;
}

int
vleDomDiffStack::getUndoAvailability() const
{
    return computeUndoAvailability(prevCurr, curr, saved);
}

void
vleDomDiffStack::tryEmitUndoAvailability()
{
    int undoAvailability = getUndoAvailability();
    if (undoAvailability == 1) {
        emit undoAvailable(true);
    }
    if (undoAvailability == -1) {
        emit undoAvailable(false);
    }
}

}}//namespaces
