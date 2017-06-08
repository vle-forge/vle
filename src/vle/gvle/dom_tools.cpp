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

#include "dom_tools.hpp"
#include <QClipboard>
#include <QFlags>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QtDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>

#include <iostream>

namespace vle {
namespace gvle {

/**************************************************
 * DomFunctions static functions
 **************************************************/

DomFunctions::DomFunctions()
{
}

DomFunctions::~DomFunctions()
{
}

QString
DomFunctions::attributeValue(const QDomNode& node, const QString& attrName)
{
    if (node.attributes().contains(attrName)) {
        return node.attributes().namedItem(attrName).nodeValue();
    }
    return "";
}

void
DomFunctions::setAttributeValue(QDomNode& node,
                                const QString& attrName,
                                const QString& val)
{
    if (node.attributes().contains(attrName)) {
        node.attributes().namedItem(attrName).setNodeValue(val);
    } else {
        node.toElement().setAttribute(attrName, val);
    }
}

QDomNode
DomFunctions::obtainChild(QDomNode node,
                          const QString& nodeName,
                          QDomDocument* domDoc)
{
    QDomNodeList chs = node.childNodes();
    for (int i = 0; i < chs.size(); i++) {
        QDomNode ch = chs.at(i);
        if (ch.nodeName() == nodeName) {
            return ch;
        }
    }
    if (not domDoc) {
        return QDomNode();
    }
    QDomNode res = domDoc->createElement(nodeName);
    node.appendChild(res);
    return res;
}

QString
DomFunctions::toQString(const QDomNode& node)
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());

    return str;
}

QStringList
DomFunctions::toQStringFormat(const QDomNode& node)
{
    QString xmlIn = DomFunctions::toQString(node);
    QString xmlOut;
    QXmlStreamReader reader(xmlIn);
    QXmlStreamWriter writer(&xmlOut);
    writer.setAutoFormatting(true);

    while (!reader.atEnd()) {
        reader.readNext();
        if (!reader.isWhitespace()) {
            writer.writeCurrentToken(reader);
        }
    }
    return xmlOut.split("\n");
}

void
DomFunctions::removeAllChilds(QDomNode node)
{
    QDomNodeList childs = node.childNodes();
    while (node.hasChildNodes()) {
        node.removeChild(childs.item(0));
    }
}

QDomNode
DomFunctions::childWhithNameAttr(QDomNode node,
                                 const QString& nodeName,
                                 const QString& nameValue,
                                 QDomDocument* domDoc)
{
    QDomNodeList childs = node.childNodes();
    QList<QDomNode> childsWithoutText;
    for (int i = 0; i < childs.length(); i++) {
        QDomNode ch = childs.at(i);
        if (not ch.isText() and ch.nodeName() == nodeName and
            DomFunctions::attributeValue(ch, "name") == nameValue) {
            return ch;
        }
    }
    if (not domDoc) {
        return QDomNode();
    }
    QDomNode res = domDoc->createElement(nodeName);
    DomFunctions::setAttributeValue(res, "name", nameValue);
    node.appendChild(res);
    return res;
}
QSet<QString>
DomFunctions::childNames(QDomNode node, QString child_node)
{
    QSet<QString> child_names;
    QList<QDomNode> childNodes =
      DomFunctions::childNodesWithoutText(node, child_node);
    for (int i = 0; i < childNodes.size(); i++) {
        child_names.insert(
          DomFunctions::attributeValue(childNodes.at(i), "name"));
    }
    return child_names;
}

QList<QDomNode>
DomFunctions::childNodesWithoutText(QDomNode node, const QString& nodeName)
{
    QDomNodeList childs = node.childNodes();
    QList<QDomNode> childsWithoutText;
    for (int i = 0; i < childs.length(); i++) {
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

QString
DomFunctions::childNameProvider(QDomNode node,
                                QString child_node,
                                QString prefix,
                                const QSet<QString>& exclude)
{
    QString new_name = prefix;
    unsigned int id_name = 0;
    while (true) {
        if (id_name > 0) {
            new_name = prefix;
            new_name += "_";
            new_name += QVariant(id_name).toString();
        }
        QString name = childNameProvider(node, child_node, new_name);
        if (not exclude.contains(name)) {
            return name;
        }
        id_name++;
    }
    return "";
}

QString
DomFunctions::childNameProvider(QDomNode node,
                                QString child_node,
                                QString prefix)
{
    QList<QDomNode> children = childNodesWithoutText(node, child_node);
    QString new_name = prefix;
    unsigned int id_name = 0;
    bool new_name_found = false;
    while (not new_name_found) {
        bool new_name_found_i = false;
        for (int j = 0; j < children.length(); j++) {
            new_name_found_i =
              new_name_found_i or
              (attributeValue(children.at(j), "name") == new_name);
        }
        if (new_name_found_i) {
            id_name++;
            new_name = prefix;
            new_name += "_";
            new_name += QVariant(id_name).toString();
        } else {
            new_name_found = true;
        }
    }
    return new_name;
}

/**************************************************
 * DomDiffStack implementation
 **************************************************/

DomDiffStack::DomDiff::DomDiff()
  : node_before()
  , node_after()
  , query("")
  , merge_type("null")
  , merge_args(0)
  , source("")
  , isDefined(false)
{
}

DomDiffStack::DomDiff::~DomDiff()
{
    if (merge_args) {
        delete merge_args;
        merge_args = 0;
    }
}
void
DomDiffStack::DomDiff::reset()
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

DomDiffStack::DomDiffStack(DomObject* vdo)
  : diffs()
  , prevCurr(0)
  , curr(0)
  , saved(0)
  , mVdo(vdo)
  , current_source("")
  , snapshotEnabled(true)
{
}

DomDiffStack::~DomDiffStack()
{
}

void
DomDiffStack::init(QDomNode node)
{
    diffs.resize(300);
    diffs[0].node_before = node.cloneNode();
    prevCurr = 0;
    curr = 0;
    saved = 0;
}

bool
DomDiffStack::enableSnapshot(bool enable)
{
    bool oldSnapshotEnabled = snapshotEnabled;
    snapshotEnabled = enable;
    return oldSnapshotEnabled;
}

void
DomDiffStack::snapshot(QDomNode node)
{
    snapshot(node, "null", 0);
}

void
DomDiffStack::snapshot(QDomNode node,
                       QString mergeType,
                       vle::value::Map* mergeArgs)
{
    if (not snapshotEnabled) {
        return;
    }
    bool isMerged = false;
    if (node.isNull()) {
        // false snaphot
        prevCurr = curr;
        curr++; // TODO manage size
        if (curr >= diffs.size()) {
            qDebug()
              << " Internal error DomDiffStack::snapshot (size to big) ";
            return;
        }
        diffs[curr].query = "";
        diffs[curr].node_before = QDomNode();
        diffs[curr].merge_type = "null";
        if (diffs[curr].merge_args) {
            delete diffs[curr].merge_args;
            diffs[curr].merge_args = 0;
        }
        diffs[curr].source = current_source;
        diffs[curr].isDefined = true;

        // unvalidate all undo stack
        for (unsigned int i = curr + 1; i < diffs.size(); i++) {
            diffs[i].reset();
        }
    } else {
        // check if it is possibly mergeable to top undo command
        if ((curr > 0) and (mergeType != "null") and curr != saved and
            (diffs[curr].merge_type == mergeType) and
            (diffs[curr].query == mVdo->getXQuery(node))) {
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
                // check if q1 and q2 contains the same model queries
                if ((q1.size() == q2.size()) and (q1.size() > 0)) {
                    bool allFound = true;
                    for (unsigned int i = 0; i < q1.size(); i++) {
                        std::string q1i = q1.getString(i);
                        bool found = false;
                        for (unsigned int j = 0; j < q2.size(); j++) {
                            found = found or (q1i == q2.getString(j));
                        }
                        allFound = allFound and found;
                    }
                    isMerged = allFound;
                }
            }
        }

        if (not isMerged) {
            prevCurr = curr;
            curr++; // TODO manage size
            if (curr >= diffs.size()) {
                qDebug()
                  << " Internal error DomDiffStack::snapshot (size to big) ";
                return;
            }
            diffs[curr].query = mVdo->getXQuery(node);
            diffs[curr].node_before = node.cloneNode();
            diffs[curr].merge_type = mergeType;
            diffs[curr].merge_args = mergeArgs;
            diffs[curr].source = current_source;
            diffs[curr].isDefined = true;
            // unvalidate all undo stack
            for (unsigned int i = curr + 1; i < diffs.size(); i++) {
                diffs[i].reset();
            }
        }
    }
    emit snapshotVdo(node, isMerged);
    tryEmitUndoAvailability();
}

void
DomDiffStack::undo()
{
    if (curr <= 0) {
        return;
    }

    if ((diffs[curr].source == current_source) and (diffs[curr].isDefined)) {
        if (diffs[curr].query == "") {
            emit undoRedoVdo(QDomNode(), QDomNode());
        } else {
            diffs[curr].node_after =
              mVdo->getNodeFromXQuery(diffs[curr].query).cloneNode();
            QDomNode currN = mVdo->getNodeFromXQuery(diffs[curr].query);
            QDomNode parent = currN.parentNode();
            if (parent.isNull()) {
                qDebug() << " Internal error DomDiffStack::undo() "
                         << diffs[curr].query;
            }
            parent.replaceChild(diffs[curr].node_before, currN);
            emit undoRedoVdo(currN, diffs[curr].node_before);
        }
        prevCurr = curr;
        curr--;
    }
    tryEmitUndoAvailability();
}

void
DomDiffStack::redo()
{

    if (diffs[curr + 1].isDefined) {
        if ((diffs[curr + 1].source == current_source) or
            (diffs[curr + 1].source.isEmpty())) {
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
DomDiffStack::registerSaveState()
{
    saved = curr;
    tryEmitUndoAvailability();
}

void
DomDiffStack::clear()
{
    for (unsigned int i = 0; i < diffs.size(); i++) {
        diffs[i].reset();
    }
    prevCurr = curr;
    curr = 0;
    tryEmitUndoAvailability();
}

void
DomDiffStack::print(std::ostream& out) const
{
    unsigned int diffSize = 1;
    while (diffs[diffSize].isDefined) {
        out << " [DomDiffStack] diff " << diffSize << ": "
            << " query=" << diffs[diffSize].query.toStdString()
            << " source=" << diffs[diffSize].source.toStdString() << "\n";
        diffSize++;
    }
    out << " [DomDiffStack] nb stack size=" << diffSize << ", saved=" << saved
        << ", curr=" << curr << ", prevCurr=" << prevCurr
        << ", undoAvail=" << computeUndoAvailability(prevCurr, curr, saved)
        << "\n";
}

int
DomDiffStack::computeUndoAvailability(unsigned int prevCurr,
                                      unsigned int curr,
                                      unsigned int saved)
{
    if (prevCurr != saved and curr == saved) {
        return -1;
    }
    if (prevCurr == saved and curr != saved) {
        return 1;
    }
    return 0;
}

void
DomDiffStack::tryEmitUndoAvailability()
{
    int undoAvailability = computeUndoAvailability(prevCurr, curr, saved);
    if (undoAvailability == 1) {
        emit undoAvailable(true);
    }
    if (undoAvailability == -1) {
        emit undoAvailable(false);
    }
}
}
} // namespaces
