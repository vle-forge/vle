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

#include "vle_dom.hpp"
#include "dom_tools.hpp"
#include <QtDebug>
#include <vle/utils/Exception.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>

#include <iostream>

namespace vle {
namespace gvle {

vleDomVpz::vleDomVpz(QDomDocument* doc)
  : DomObject(doc)

{
}
vleDomVpz::~vleDomVpz()
{
}
QString
vleDomVpz::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    // element uniq in children
    if ((name == "structures") or (name == "submodels") or (name == "in") or
        (name == "out") or (name == "connections") or (name == "classes") or
        (name == "dynamics") or (name == "conditions") or
        (name == "experiment") or (name == "views") or
        (name == "observables")) {
        return getXQuery(node.parentNode()) + "/" + name;
    }
    // element identified wy attribute name
    if ((name == "port") or (name == "model") or (name == "class") or
        (name == "dynamic") or (name == "condition") or
        (name == "observable")) {
        return getXQuery(node.parentNode()) + "/" + name + "[@name=\"" +
               DomFunctions::attributeValue(node, "name") + "\"]";
    }
    if (node.nodeName() == "vle_project") {
        return "./vle_project";
    }
    return "";
}

QDomNode
vleDomVpz::getNodeFromXQuery(const QString& query, QDomNode d)
{
    // handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j = 0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j = 2;
    } else {
        curr = queryList.at(0);
        j = 1;
    }
    for (int i = j; i < queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size() - 1) {
            rest = rest + "/";
        }
    }
    // handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return (getNodeFromXQuery(rest, rootNode));
    }

    // handle recursion with uniq node
    if ((curr == "structures") or (curr == "submodels") or (curr == "in") or
        (curr == "out") or (curr == "connections") or (curr == "classes") or
        (curr == "dynamics") or (curr == "conditions") or
        (curr == "experiment") or (curr == "views") or
        (curr == "observables")) {
        return getNodeFromXQuery(rest,
                                 DomFunctions::obtainChild(d, curr, mDoc));
    }
    // handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("model"));
    nodeByNames.push_back(QString("port"));
    nodeByNames.push_back(QString("class"));
    nodeByNames.push_back(QString("dynamic"));
    nodeByNames.push_back(QString("condition"));
    nodeByNames.push_back(QString("observable"));
    QString selNodeByName = "";
    for (unsigned int i = 0; i < nodeByNames.size(); i++) {
        if (curr.startsWith(nodeByNames[i])) {
            selNodeByName = nodeByNames[i];
        }
    }
    if (not selNodeByName.isEmpty()) {
        QStringList currSplit = curr.split("\"");
        QDomNode selMod =
          DomFunctions::childWhithNameAttr(d, selNodeByName, currSplit.at(1));
        return getNodeFromXQuery(rest, selMod);
    }
    return QDomNode();
}

/************************************************************************/

vleDomVpm::vleDomVpm(QDomDocument* doc)
  : DomObject(doc)

{
}
vleDomVpm::~vleDomVpm()
{
}
QString
vleDomVpm::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    // element uniq in children
    if ((name == "condPlugins") or (name == "outputGUIplugins")) {
        return getXQuery(node.parentNode()) + "/" + name;
    }
    // element identified by attribute name
    if ((name == "condPlugin") or (name == "outputGUIplugin")) {
        return getXQuery(node.parentNode()) + "/" + name + "[@name=\"" +
               DomFunctions::attributeValue(node, "name") + "\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomVpm::getNodeFromXQuery(const QString& query, QDomNode d)
{
    // handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j = 0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j = 2;
    } else {
        curr = queryList.at(0);
        j = 1;
    }
    for (int i = j; i < queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size() - 1) {
            rest = rest + "/";
        }
    }
    // handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project_metadata" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return (getNodeFromXQuery(rest, rootNode));
    }

    // handle recursion with uniq node
    if ((curr == "condPlugins") or (curr == "outputGUIplugins")) {
        return getNodeFromXQuery(rest,
                                 DomFunctions::obtainChild(d, curr, mDoc));
    }
    // handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("condPlugin"));
    nodeByNames.push_back(QString("outputGUIplugin"));
    QString selNodeByName = "";
    for (unsigned int i = 0; i < nodeByNames.size(); i++) {
        if (curr.startsWith(nodeByNames[i])) {
            selNodeByName = nodeByNames[i];
        }
    }
    if (not selNodeByName.isEmpty()) {
        QStringList currSplit = curr.split("\"");
        QDomNode selMod =
          DomFunctions::childWhithNameAttr(d, selNodeByName, currSplit.at(1));
        return getNodeFromXQuery(rest, selMod);
    }
    return QDomNode();
}

/************************************************************************/

/******************************************************
 * Static functions
 ******************************************************/

QDomElement
vleDomStatic::buildEmptyValueFromDoc(QDomDocument& domDoc,
                                     vle::value::Value::type vleType)
{
    QDomElement elem;
    switch (vleType) {
    case vle::value::Value::BOOLEAN: {
        elem = domDoc.createElement("boolean");
        break;
    }
    case vle::value::Value::INTEGER: {
        elem = domDoc.createElement("integer");
        break;
    }
    case vle::value::Value::DOUBLE: {
        elem = domDoc.createElement("double");
        break;
    }
    case vle::value::Value::STRING: {
        elem = domDoc.createElement("string");
        break;
    }
    case vle::value::Value::SET: {
        elem = domDoc.createElement("set");
        break;
    }
    case vle::value::Value::MAP: {
        elem = domDoc.createElement("map");
        break;
    }
    case vle::value::Value::TUPLE: {
        elem = domDoc.createElement("tuple");
        break;
    }
    case vle::value::Value::TABLE: {
        elem = domDoc.createElement("table");
        break;
    }
    case vle::value::Value::MATRIX: {
        elem = domDoc.createElement("matrix");
        break;
    }
    default: {
        qDebug() << "Internal error in buildEmptyValueFromDoc ";
        break;
    }
    }
    return elem;
}

std::unique_ptr<value::Value>
vleDomStatic::buildValue(const QDomNode& node, bool buildText)
{
    if (node.nodeName() == "boolean") {
        if (node.childNodes().length() != 1) {
            qDebug() << "Internal error in buildValue (6)";
            return std::unique_ptr<value::Value>();
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();
        return value::Boolean::create(qv.toBool());
    }
    if (node.nodeName() == "integer") {
        if (node.childNodes().length() != 1) {
            qDebug() << "Internal error in buildValue (5) "
                     << node.childNodes().length() << " "
                     << DomFunctions::toQString(node);
            qDebug() << "Internal error in buildValue son0 "
                     << DomFunctions::toQString(node.childNodes().at(0));
            qDebug() << "Internal error in buildValue son1 "
                     << DomFunctions::toQString(node.childNodes().at(1));
            return std::unique_ptr<value::Value>();
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();
        ;
        return value::Integer::create(qv.toInt());
    }
    if (node.nodeName() == "double") {
        if (node.childNodes().length() != 1) {
            qDebug() << "Internal error in buildValue (10)"
                     << DomFunctions::toQString(node);
            return std::unique_ptr<value::Value>();
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();
        ;
        return value::Double::create(qv.toDouble());
    }
    if (node.nodeName() == "string") {
        if (node.childNodes().length() == 0) {
            return value::String::create(std::string());
        }
        if (node.childNodes().length() == 1) {
            QString qv = node.childNodes().item(0).toText().nodeValue();
            return value::String::create(qv.toStdString());
        }
        qDebug() << "Internal error in buildValue (4.3) "
                 << node.childNodes().length();
        return std::unique_ptr<value::Value>();
    }
    if (node.nodeName() == "tuple") {
        if (node.childNodes().length() > 1) {
            qDebug() << "Internal error in buildValue (4)";
            return std::unique_ptr<value::Value>();
        }
        if (node.childNodes().length() == 0) {
            return value::Tuple::create(0, 0.0);
        } else {
            QString qv = node.childNodes().item(0).toText().nodeValue();
            QStringList vals = qv.split(" ");
            auto res = vle::value::Tuple::create(vals.length());
            auto& tpl = res->toTuple();
            for (int i = 0; i < vals.size(); i++) {
                tpl[i] = QVariant(vals.at(i)).toDouble();
            }
            return res;
        }
    }
    if (node.nodeName() == "table") {
        if (node.childNodes().length() != 1) {
            qDebug() << "Internal error in buildValue (14)"
                     << DomFunctions::toQString(node);
            qDebug() << "Internal error in buildValue (14) length"
                     << node.childNodes().length();
            return 0;
        }
        int width =
          QVariant(DomFunctions::attributeValue(node, "width")).toInt();
        int height =
          QVariant(DomFunctions::attributeValue(node, "height")).toInt();
        QString qv = node.childNodes().item(0).toText().nodeValue();

        auto res = vle::value::Table::create(width, height);
        res->toTable().fill(qv.toStdString());
        return res;
    }
    if (node.nodeName() == "#text") {
        if (buildText) {
            return value::String::create(node.nodeValue().toStdString());
        } else {
            return std::unique_ptr<value::Value>();
        }
    }

    QList<QDomNode> chs = DomFunctions::childNodesWithoutText(node);
    if (node.nodeName() == "set") {
        auto res = vle::value::Set::create();
        auto& st = res->toSet();
        for (int i = 0; i < chs.size(); i++) {
            QDomNode child = chs[i];
            st.add(vleDomStatic::buildValue(child, buildText));
        }
        return res;
    }
    if (node.nodeName() == "map") {
        auto res = vle::value::Map::create();
        for (int i = 0; i < chs.size(); i++) {
            QDomNode child = chs[i];
            if (child.nodeName() != "key") {
                qDebug() << "Internal error in buildValue (1)";
                return res;
            }
            if (not child.attributes().contains("name")) {
                qDebug() << "Internal error in buildValue (2)";
                return res;
            }
            QDomNode mapItemValue = child.childNodes().item(0);
            while (mapItemValue.isText()) {
                mapItemValue = mapItemValue.nextSibling();
            }
            res->toMap().add(
              child.attributes().namedItem("name").nodeValue().toStdString(),
              vleDomStatic::buildValue(mapItemValue, buildText));
        }
        return res;
    }
    if (node.nodeName() == "matrix") {
        int columns =
          QVariant(DomFunctions::attributeValue(node, "columns")).toInt();
        int rows =
          QVariant(DomFunctions::attributeValue(node, "rows")).toInt();
        int columnmax =
          QVariant(DomFunctions::attributeValue(node, "columnmax")).toInt();
        int rowmax =
          QVariant(DomFunctions::attributeValue(node, "rowmax")).toInt();
        int columnstep =
          QVariant(DomFunctions::attributeValue(node, "columnstep")).toInt();
        int rowstep =
          QVariant(DomFunctions::attributeValue(node, "rowstep")).toInt();

        auto res = vle::value::Matrix::create(
          columns, rows, columnmax, rowmax, columnstep, rowstep);
        if (chs.size() != (int)columns * rows) {
            qDebug() << "Internal error in buildValue (matrix)"
                     << DomFunctions::toQString(node);
            qDebug() << "Internal error in buildValue (matrix) childLength="
                     << chs.size();
            qDebug() << "Internal error in buildValue (matrix) columns*row="
                     << (unsigned int)columns * rows;

            return std::unique_ptr<value::Value>();
        }

        auto& mat = res->toMatrix();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                QDomNode child = chs[(j + (i * columns))];
                mat.set(j, i, vleDomStatic::buildValue(child, buildText));
            }
        }

        return res;
    }
    qDebug() << "Internal error in buildValue (3): " << node.nodeName() << "\n"
             << DomFunctions::toQString(node);
    throw vle::utils::InternalError("Unknown value: %s",
                                    node.nodeName().toStdString().c_str());
    return std::unique_ptr<value::Value>();
}

bool
vleDomStatic::fillWithValue(QDomDocument& domDoc,
                            QDomNode node,
                            const vle::value::Value& val)
{
    switch (val.getType()) {
    case vle::value::Value::BOOLEAN:
    case vle::value::Value::INTEGER:
    case vle::value::Value::DOUBLE:
    case vle::value::Value::STRING:

    {
        if ((val.isBoolean() and (node.nodeName() != "boolean")) or
            (val.isInteger() and (node.nodeName() != "integer")) or
            (val.isDouble() and (node.nodeName() != "double")) or
            (val.isString() and (node.nodeName() != "string"))) {
            return false;
        }
        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal =
              domDoc.createTextNode(val.writeToString().c_str());
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())) {
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(val.writeToString().c_str());
            return true;
        } else {
            qDebug()
              << "Internal error in fillWithValue (bool, string, integer)";
            return false;
        }
        break;
    }
    case vle::value::Value::SET: {
        if (node.nodeName() != "set") {
            return false;
        }
        DomFunctions::removeAllChilds(node);
        vle::value::Set::const_iterator itb = val.toSet().begin();
        vle::value::Set::const_iterator ite = val.toSet().end();
        for (; itb != ite; itb++) {
            QDomElement elemVal =
              buildEmptyValueFromDoc(domDoc, (*itb)->getType());
            fillWithValue(domDoc, elemVal, (**itb));
            node.appendChild(elemVal);
        }
        break;
    }
    case vle::value::Value::MAP: {
        if (node.nodeName() != "map") {
            return false;
        }
        DomFunctions::removeAllChilds(node);
        vle::value::Map::const_iterator itb = val.toMap().begin();
        vle::value::Map::const_iterator ite = val.toMap().end();
        for (; itb != ite; itb++) {
            QDomElement elem = domDoc.createElement("key");
            elem.setAttribute("name", QString(itb->first.c_str()));
            QDomElement elemVal =
              buildEmptyValueFromDoc(domDoc, itb->second->getType());
            fillWithValue(domDoc, elemVal, *(itb->second));
            elem.appendChild(elemVal);
            node.appendChild(elem);
        }
        break;
    }
    case vle::value::Value::TUPLE: {
        if (node.nodeName() != "tuple") {
            return false;
        }
        QString valTxt = "";
        const vle::value::Tuple& tuple = val.toTuple();
        for (unsigned int i = 0; i < tuple.size(); i++) {
            if (i > 0) {
                valTxt += " ";
            }
            valTxt += QVariant(tuple.at(i)).toString();
        }
        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal = domDoc.createTextNode(valTxt);
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())) {
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(valTxt);
            return true;
        } else {
            qDebug() << "Internal error in fillWithValue (tuple)";
            return false;
        }
        break;
    }
    case vle::value::Value::TABLE: {
        if (node.nodeName() != "table") {
            return false;
        }
        DomFunctions::setAttributeValue(
          node, "width", QVariant((int)val.toTable().width()).toString());
        DomFunctions::setAttributeValue(
          node, "height", QVariant((int)val.toTable().height()).toString());
        QString strRepr = val.toTable().writeToString().c_str();
        strRepr.remove("(");
        strRepr.remove(")");
        strRepr.replace(",", " ");
        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal = domDoc.createTextNode(strRepr);
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())) {
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(strRepr);
            return true;
        } else {

            qDebug() << "Internal error in fillWithValue (table)"
                     << DomFunctions::toQString(childs.at(0));
            qDebug() << "Internal error in fillWithValue (table)"
                     << childs.length();
            return false;
        }
        break;
    }
    case vle::value::Value::MATRIX: {
        if (node.nodeName() != "matrix") {
            return false;
        }
        const vle::value::Matrix& mat = val.toMatrix();
        int columns = mat.columns();
        int rows = mat.rows();
        int columns_max = mat.columns_max();
        int rows_max = mat.rows_max();
        int columnstep = mat.resizeColumn();
        int rowstep = mat.resizeRow();

        DomFunctions::setAttributeValue(
          node, "columns", QVariant(columns).toString());
        DomFunctions::setAttributeValue(
          node, "rows", QVariant(rows).toString());
        DomFunctions::setAttributeValue(
          node, "columnmax", QVariant(columns_max).toString());
        DomFunctions::setAttributeValue(
          node, "rowmax", QVariant(rows_max).toString());
        DomFunctions::setAttributeValue(
          node, "columnstep", QVariant(columnstep).toString());
        DomFunctions::setAttributeValue(
          node, "rowsstep", QVariant(rowstep).toString());
        DomFunctions::removeAllChilds(node);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {

                const auto& val = mat.get(j, i); // WARNING inversed
                QDomElement elemVal =
                  buildEmptyValueFromDoc(domDoc, val->getType());
                fillWithValue(domDoc, elemVal, *val);
                node.appendChild(elemVal);
            }
        }
        break;
    }
    default: {
        qDebug() << "Internal error in fillWithValue (nnn)";
        return false;
    }
    }
    return true;
}

QSet<QString>
vleDomStatic::dynamics(QDomNode atom)
{
    if (atom.nodeName() != "dynamics") {
        qDebug() << "Internal error in dynamics " << atom.nodeName();
        return QSet<QString>();
    }
    return DomFunctions::childNames(atom, "dynamic");
}

QString
vleDomStatic::attachedDynToAtomic(QDomNode atom)
{
    if ((atom.nodeName() != "model") or
        (DomFunctions::attributeValue(atom, "type") != "atomic")) {
        qDebug() << "Internal error in attachedDynToAtomic "
                 << atom.nodeName();
        return "";
    }
    return DomFunctions::attributeValue(atom, "dynamics");
}

QDomNode
vleDomStatic::addCond(QDomNode atom,
                      const QString& condName,
                      QDomDocument* domDoc,
                      DomDiffStack* snapObj)
{
    if (not DomFunctions::childWhithNameAttr(atom, "condition", condName)
              .isNull()) {
        return QDomNode();
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    QDomNode res = domDoc->createElement("condition");
    DomFunctions::setAttributeValue(res, "name", condName);
    atom.appendChild(res);
    return res;
}

QSet<QString>
vleDomStatic::conditions(QDomNode atom)
{
    if (atom.nodeName() != "conditions") {
        qDebug() << "Internal error in conditions " << atom.nodeName();
        return QSet<QString>();
    }
    return DomFunctions::childNames(atom, "condition");
}

QSet<QString>
vleDomStatic::attachedCondsToAtomic(const QDomNode& atom)
{
    if (atom.nodeName() != "model") {
        qDebug() << "Internal error in attachedCondsToAtomic "
                 << atom.nodeName();
        return QSet<QString>();
    }
    QString attachedConds = DomFunctions::attributeValue(atom, "conditions");
    if (attachedConds == "") {
        return QSet<QString>();
    }
    return attachedConds.split(",").toSet();
}

void
vleDomStatic::attachCondsToAtomic(QDomNode& atom, const QSet<QString>& conds)
{
    QString res = "";
    bool first = true;
    QSet<QString>::const_iterator itb = conds.begin();
    QSet<QString>::const_iterator ite = conds.end();

    for (; itb != ite; itb++) {
        if (not first) {
            res += ",";
        } else {
            first = false;
        }
        res += *itb;
    }
    DomFunctions::setAttributeValue(atom, "conditions", res);
}

bool
vleDomStatic::debuggingAtomic(const QDomNode& atom)
{
    if (atom.nodeName() != "model" and
        DomFunctions::attributeValue(atom, "type") != "atomic") {
        qDebug() << "Internal error in debuggingAtomic " << atom.nodeName();
        return false;
    }
    if (DomFunctions::attributeValue(atom, "debug") == "true") {
        return true;
    } else {
        return false;
    }
}

bool
vleDomStatic::setDebuggingToAtomic(QDomNode atom,
                                   bool val,
                                   DomDiffStack* snapObj)
{
    if (atom.nodeName() != "model" and
        DomFunctions::attributeValue(atom, "type") != "atomic") {
        qDebug() << "Internal error in setDebuggingToAtomic "
                 << atom.nodeName();
        return false;
    }
    if (val == debuggingAtomic(atom)) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }

    QString valStr = "false";
    if (val) {
        valStr = "true";
    }
    DomFunctions::setAttributeValue(atom, "debug", valStr);
    return true;
}

bool
vleDomStatic::existCondFromConds(QDomNode conds, const QString& condName)
{
    if (conds.nodeName() != "conditions") {
        qDebug() << "Internal error in existCondFromConds "
                 << conds.nodeName();
        return false;
    }
    QDomNode node =
      DomFunctions::childWhithNameAttr(conds, "condition", condName);
    return not node.isNull();
}

bool
vleDomStatic::existPortFromCond(QDomNode atom, const QString& portName)
{
    if (atom.nodeName() != "condition") {
        qDebug() << "Internal error in existPortFromCond " << atom.nodeName();
        return false;
    }
    QDomNode node = DomFunctions::childWhithNameAttr(atom, "port", portName);
    return not node.isNull();
}

std::unique_ptr<value::Value>
vleDomStatic::getValueFromPortCond(QDomNode atom,
                                   const QString& portName,
                                   int index)
{
    std::unique_ptr<vle::value::Value> val;
    if (atom.nodeName() != "condition") {
        qDebug() << "Internal error in getValueFromPortCond "
                 << atom.nodeName();
        return val;
    }
    QDomNode port = DomFunctions::childWhithNameAttr(atom, "port", portName);
    QDomNodeList valueList = port.childNodes();
    int ii = 0;
    val.reset(nullptr);
    // note: how to detecte the number of values. For the moment each time one
    // succeeds in buildding a vle value.
    for (int k = 0; k < valueList.length(); k++) {
        val = buildValue(valueList.at(k), false);
        if (val) {
            if (index == ii) {
                return val;
            } else {
                ii += 1;
            }
        }
    }
    val.reset(nullptr);
    return val;
}

bool
vleDomStatic::rmPortFromCond(QDomNode atom,
                             const QString& portName,
                             DomDiffStack* snapObj)
{
    if (atom.nodeName() != "condition") {
        qDebug() << "Internal error in rmPortFromCond " << atom.nodeName();
        return false;
    }
    QString condName = DomFunctions::attributeValue(atom, "name");
    if (condName == "simulation_engine") {
        if (portName == "begin" or portName == "duration") {
            return false;
        }
    }
    QDomNode toRm = DomFunctions::childWhithNameAttr(atom, "port", portName);
    if (!toRm.isNull()) {
        if (snapObj) {
            snapObj->snapshot(atom);
        }
        atom.removeChild(toRm);
        return true;
    }
    return false;
}

bool
vleDomStatic::renamePortFromCond(QDomNode atom,
                                 const QString& oldName,
                                 const QString& newName,
                                 DomDiffStack* snapObj)
{
    if (atom.nodeName() != "condition") {
        qDebug() << "Internal error in renamePortFromCond " << atom.nodeName();
        return false;
    }
    QString condName = DomFunctions::attributeValue(atom, "name");
    if (condName == "simulation_engine") {
        if (oldName == "begin" or oldName == "duration") {
            return false;
        }
    }
    QDomNode toRename =
      DomFunctions::childWhithNameAttr(atom, "port", oldName);
    if (toRename.isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    DomFunctions::setAttributeValue(toRename, "name", newName);
    return true;
}

bool
vleDomStatic::fillConditionWithMap(QDomDocument& domDoc,
                                   QDomNode atom,
                                   const vle::value::Map& val,
                                   DomDiffStack* snapObj)
{
    if (atom.nodeName() != "condition") {
        qDebug() << "Internal error in fillConditionWithPort "
                 << atom.nodeName();
        return false;
    }
    if (val.size() == 0) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    QDomNodeList ports = atom.toElement().elementsByTagName("port");
    for (auto& vi : val.value()) {
        QString portName = vi.first.c_str();
        QDomNode portToFill =
          DomFunctions::childWhithNameAttr(atom, "port", portName);
        if (portToFill.isNull()) {
            portToFill = domDoc.createElement("port");
            DomFunctions::setAttributeValue(portToFill, "name", portName);
            atom.appendChild(portToFill);
        }
        DomFunctions::removeAllChilds(portToFill);
        QDomNode node =
          vleDomStatic::buildEmptyValueFromDoc(domDoc, vi.second->getType());
        vleDomStatic::fillWithValue(domDoc, node, *vi.second);
        portToFill.appendChild(node);
    }
    return true;
}

QSet<QString>
vleDomStatic::observables(QDomNode atom)
{
    if (atom.nodeName() != "observables") {
        qDebug() << "Internal error in observables " << atom.nodeName();
        return QSet<QString>();
    }
    return DomFunctions::childNames(atom, "observable");
}

QString
vleDomStatic::attachedObsToAtomic(QDomNode atom)
{
    if ((atom.nodeName() != "model") or
        (DomFunctions::attributeValue(atom, "type") != "atomic")) {
        qDebug() << "Internal error in attachedObsToAtomic "
                 << atom.nodeName();
        return "";
    }
    return DomFunctions::attributeValue(atom, "observables");
}

bool
vleDomStatic::addObservablePort(QDomDocument& domDoc,
                                QDomNode atom,
                                const QString& portName,
                                DomDiffStack* snapObj)
{
    if (atom.nodeName() != "observable") {
        qDebug() << "Internal error in addObservablePort " << atom.nodeName();
        return false;
    }
    if (not DomFunctions::childWhithNameAttr(atom, "port", portName)
              .isNull()) {
        return false;
    }

    if (snapObj) {
        snapObj->snapshot(atom);
    }
    QDomNode portNode =
      DomFunctions::childWhithNameAttr(atom, "port", portName, &domDoc);
    DomFunctions::setAttributeValue(portNode, "name", portName);
    return true;
}

bool
vleDomStatic::rmObservablePort(QDomNode atom,
                               const QString& portName,
                               DomDiffStack* snapObj)
{
    if (atom.nodeName() != "observable") {
        qDebug() << "Internal error in rmObservablePort " << atom.nodeName();
        return false;
    }
    QDomNode toRm = DomFunctions::childWhithNameAttr(atom, "port", portName);
    if (not toRm.isNull()) {
        if (snapObj) {
            snapObj->snapshot(atom);
        }
        atom.removeChild(toRm);
        return true;
    }
    return false;
}

bool
vleDomStatic::renameObservablePort(QDomNode atom,
                                   const QString& oldName,
                                   const QString& newName,
                                   DomDiffStack* snapObj)
{
    if (atom.nodeName() != "observable") {
        qDebug() << "Internal error in renameObservablePort "
                 << atom.nodeName();
        return false;
    }
    QDomNode toRename =
      DomFunctions::childWhithNameAttr(atom, "port", oldName);
    if (toRename.isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    DomFunctions::setAttributeValue(toRename, "name", newName);
    return true;
}

bool
vleDomStatic::addPortToInNode(QDomDocument& domDoc,
                              QDomNode atom,
                              const QString& portName,
                              DomDiffStack* snapObj)
{
    if (atom.nodeName() != "in") {
        qDebug() << "Internal error in addPortToInNode " << atom.nodeName();
        return false;
    }
    if (not DomFunctions::childWhithNameAttr(atom, "port", portName)
              .isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    QDomNode portNode =
      DomFunctions::childWhithNameAttr(atom, "port", portName, &domDoc);
    DomFunctions::setAttributeValue(portNode, "name", portName);
    return true;
}

bool
vleDomStatic::rmPortToInNode(QDomNode atom,
                             const QString& portName,
                             DomDiffStack* snapObj)
{
    if (atom.nodeName() != "in") {
        qDebug() << "Internal error in rmPortToInNode " << atom.nodeName();
        return false;
    }
    QDomNode toRm = DomFunctions::childWhithNameAttr(atom, "port", portName);
    if (!toRm.isNull()) {
        if (snapObj) {
            snapObj->snapshot(atom);
        }
        atom.removeChild(toRm);
        return true;
    }
    return false;
}

bool
vleDomStatic::renamePortToInNode(QDomNode atom,
                                 const QString& oldName,
                                 const QString& newName,
                                 DomDiffStack* snapObj)
{
    if (atom.nodeName() != "in") {
        qDebug() << "Internal error in renamePortToInNode " << atom.nodeName();
        return false;
    }
    QDomNode toRename =
      DomFunctions::childWhithNameAttr(atom, "port", oldName);
    if (toRename.isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    DomFunctions::setAttributeValue(toRename, "name", newName);
    return true;
}

bool
vleDomStatic::addPortToOutNode(QDomDocument& domDoc,
                               QDomNode atom,
                               const QString& portName,
                               DomDiffStack* snapObj)
{
    if (atom.nodeName() != "out") {
        qDebug() << "Internal error in addPortToOutNode " << atom.nodeName();
        return false;
    }
    if (not DomFunctions::childWhithNameAttr(atom, "port", portName)
              .isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }

    QDomNode portNode =
      DomFunctions::childWhithNameAttr(atom, "port", portName, &domDoc);
    DomFunctions::setAttributeValue(portNode, "name", portName);
    return true;
}

bool
vleDomStatic::rmPortToOutNode(QDomNode atom,
                              const QString& portName,
                              DomDiffStack* snapObj)
{
    if (atom.nodeName() != "out") {
        qDebug() << "Internal error in rmPortToOutNode " << atom.nodeName();
        return false;
    }
    QDomNode toRm = DomFunctions::childWhithNameAttr(atom, "port", portName);
    if (!toRm.isNull()) {
        if (snapObj) {
            snapObj->snapshot(atom);
        }
        atom.removeChild(toRm);
        return true;
    }
    return false;
}

bool
vleDomStatic::renamePortToOutNode(QDomNode atom,
                                  const QString& oldName,
                                  const QString& newName,
                                  DomDiffStack* snapObj)
{
    if (atom.nodeName() != "out") {
        qDebug() << "Internal error in renamePortToOutNode "
                 << atom.nodeName();
        return false;
    }
    QDomNode toRename =
      DomFunctions::childWhithNameAttr(atom, "port", oldName);
    if (toRename.isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    DomFunctions::setAttributeValue(toRename, "name", newName);
    return true;
}

QList<QDomNode>
vleDomStatic::getConnectionsFromCoupled(QDomNode coupled,
        QString connType, bool origin, QString submodel, QString port)
{
    QList<QDomNode> ret;
    QDomNode conns = DomFunctions::obtainChild(coupled, "connections");
    if (conns.isNull()) {
        return ret;
    }
    QList<QDomNode> connList = DomFunctions::childNodesWithoutText(
            conns, "connection");
    for (auto conn : connList) {
        if (DomFunctions::attributeValue(conn, "type") == connType) {
            QString toget = origin ? "origin" : "destination";
            QDomNode el = DomFunctions::obtainChild(conn, toget);
            if ((DomFunctions::attributeValue(el, "model") == submodel) and
                    (DomFunctions::attributeValue(el, "port") == port)) {
                ret.append(conn);
            }
        }
    }
    return ret;
}

bool
vleDomStatic::renameModelIntoCoupled(QDomDocument& domDoc,
                                     QDomNode atom,
                                     QString old_model,
                                     QString new_model,
                                     DomDiffStack* snapObj)
{
    if ((atom.nodeName() != "model") or
        (DomFunctions::attributeValue(atom, "type") != "coupled")) {
        qDebug() << "Internal error in vleDomStatic::renameModelIntoCoupled "
                 << atom.nodeName();
        return false;
    }
    QDomNode oldMod = subModel(atom, old_model);
    if (oldMod.isNull()) {
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    if (DomFunctions::attributeValue(oldMod, "type") == "coupled") {
        // rename connections at the submodels level
        QList<QDomNode> consTag =
          DomFunctions::childNodesWithoutText(oldMod, "connections");
        if (consTag.size() != 1) {
            qDebug() << "Internal error (2) in "
                        "vleDomStatic::renameModelIntoCoupled ";
        }
        QList<QDomNode> cons =
          DomFunctions::childNodesWithoutText(consTag.at(0), "connection");
        for (int i = 0; i < cons.size(); i++) {
            QDomNode con = cons.at(i);
            QDomNode orig = con.toElement().elementsByTagName("origin").at(0);
            QDomNode dest =
              con.toElement().elementsByTagName("destination").at(0);
            if ((DomFunctions::attributeValue(con, "type") == "input") and
                (DomFunctions::attributeValue(orig, "model") == old_model)) {
                DomFunctions::setAttributeValue(orig, "model", new_model);
            } else if ((DomFunctions::attributeValue(con, "type") ==
                        "output") and
                       (DomFunctions::attributeValue(dest, "model") ==
                        old_model)) {
                DomFunctions::setAttributeValue(dest, "model", new_model);
            }
            //            else if (DomFunctions::attributeValue(con, "type") ==
            //            "internal") {
            //                if (DomFunctions::attributeValue(dest, "model")
            //                == old_model) {
            //                    DomFunctions::setAttributeValue(dest,
            //                    "model", new_model);
            //                }
            //                if (DomFunctions::attributeValue(orig, "model")
            //                == old_model) {
            //                    DomFunctions::setAttributeValue(orig,
            //                    "model", new_model);
            //                }
            //            }
        }
    }
    // rename connections at the coupled level
    QDomNode consNode =
      DomFunctions::obtainChild(atom, "connections", &domDoc);
    QList<QDomNode> cons =
      DomFunctions::childNodesWithoutText(consNode, "connection");
    for (int i = 0; i < cons.size(); i++) {
        QDomNode con = cons.at(i);
        QDomNode orig = con.toElement().elementsByTagName("origin").at(0);
        QDomNode dest = con.toElement().elementsByTagName("destination").at(0);
        if ((DomFunctions::attributeValue(con, "type") == "input") and
            (DomFunctions::attributeValue(dest, "model") == old_model)) {
            DomFunctions::setAttributeValue(dest, "model", new_model);
        } else if ((DomFunctions::attributeValue(con, "type") == "output") and
                   (DomFunctions::attributeValue(orig, "model") ==
                    old_model)) {
            DomFunctions::setAttributeValue(orig, "model", new_model);
        } else if (DomFunctions::attributeValue(con, "type") == "internal") {
            if (DomFunctions::attributeValue(dest, "model") == old_model) {
                DomFunctions::setAttributeValue(dest, "model", new_model);
            }
            if (DomFunctions::attributeValue(orig, "model") == old_model) {
                DomFunctions::setAttributeValue(orig, "model", new_model);
            }
        }
    }
    DomFunctions::setAttributeValue(oldMod, "name", new_model);
    return true;
}

bool
vleDomStatic::renameModelIntoStructures(QDomDocument& domDoc,
                                        QDomNode atom,
                                        QString old_model,
                                        QString new_model,
                                        DomDiffStack* snapObj)
{
    if (atom.nodeName() != "structures") {
        qDebug()
          << "Internal error in vleDomStatic::renameModelIntoStructures "
          << atom.nodeName();
        return false;
    }
    QDomNode mainMod =
      DomFunctions::childWhithNameAttr(atom, "model", old_model);
    if (mainMod.isNull()) {
        qDebug() << "Internal error(2) in "
                    "vleDomStatic::renameModelIntoStructures ";
        return false;
    }
    if (snapObj) {
        snapObj->snapshot(atom);
    }
    if (DomFunctions::attributeValue(mainMod, "type") == "coupled") {
        // rename connections at the submodels level
        QDomNode consNode =
          DomFunctions::obtainChild(mainMod, "connections", &domDoc);
        QList<QDomNode> cons =
          DomFunctions::childNodesWithoutText(consNode, "connection");
        for (int i = 0; i < cons.size(); i++) {
            QDomNode con = cons.at(i);
            QDomNode orig = con.toElement().elementsByTagName("origin").at(0);
            QDomNode dest =
              con.toElement().elementsByTagName("destination").at(0);
            if ((DomFunctions::attributeValue(con, "type") == "input") and
                (DomFunctions::attributeValue(orig, "model") == old_model)) {
                DomFunctions::setAttributeValue(orig, "model", new_model);
            } else if ((DomFunctions::attributeValue(con, "type") ==
                        "output") and
                       (DomFunctions::attributeValue(dest, "model") ==
                        old_model)) {
                DomFunctions::setAttributeValue(dest, "model", new_model);
            }
        }
    }
    DomFunctions::setAttributeValue(mainMod, "name", new_model);
    return true;
}

QStringList
vleDomStatic::subModels(QDomNode atom)
{
    QStringList submodels;
    if ((atom.nodeName() != "model") or
        (DomFunctions::attributeValue(atom, "type") != "coupled")) {
        qDebug() << "Internal error in vleDomStatic::subModels "
                 << atom.nodeName();
        return submodels;
    }
    QDomNode submod_node = DomFunctions::obtainChild(atom, "submodels");
    if (submod_node.isNull()) {
        return submodels;
    }
    QList<QDomNode> sub =
      DomFunctions::childNodesWithoutText(submod_node, "model");
    for (int i = 0; i < sub.size(); i++) {
        submodels.append(DomFunctions::attributeValue(sub.at(i), "name"));
    }
    return submodels;
}

QDomNode
vleDomStatic::subModel(QDomNode atom, QString model_name)
{
    if ((atom.nodeName() != "model") or
        (DomFunctions::attributeValue(atom, "type") != "coupled")) {
        qDebug() << "Internal error in vleDomStatic::subModel "
                 << atom.nodeName();
        return QDomNode();
    }
    QDomNode submod_node = DomFunctions::obtainChild(atom, "submodels");
    if (submod_node.isNull()) {
        return QDomNode();
    }
    return DomFunctions::childWhithNameAttr(submod_node, "model", model_name);
}

QString
vleDomStatic::connectionModOrig(QDomNode atom)
{
    if (atom.nodeName() != "connection") {
        qDebug() << "Internal error in vleDomStatic::connectionOrig "
                 << atom.nodeName();
        return "";
    }
    QDomNode orig = DomFunctions::obtainChild(atom, "origin");
    return DomFunctions::attributeValue(orig, "model");
}

QString
vleDomStatic::connectionModDest(QDomNode atom)
{
    if (atom.nodeName() != "connection") {
        qDebug() << "Internal error in vleDomStatic::connectionDest "
                 << atom.nodeName();
        return "";
    }
    QDomNode dest = DomFunctions::obtainChild(atom, "destination");
    return DomFunctions::attributeValue(dest, "model");
}

QStringList
vleDomStatic::getViewTypeToView(const QDomNode& atom)
{
    if (atom.nodeName() != "view") {
        qDebug() << "Internal error in getViewTypeToView " << atom.nodeName();
        return QStringList();
    }
    return DomFunctions::attributeValue(atom, "type").split(",");
}

//{
//    int undoAvailability = computeUndoAvailability(prevCurr, curr, saved);
//    if (undoAvailability == 1) {
//        emit undoAvailable(true);
//    }
//    if (undoAvailability == -1) {
//        emit undoAvailable(false);
//    }
//}
}
} // namespaces
