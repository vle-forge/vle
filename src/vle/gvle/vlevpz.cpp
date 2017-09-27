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

#include <iostream>

#include "plugin_cond.h"
#include "plugin_output.h"
#include "vlevpz.hpp"
#include <QApplication>
#include <QClipboard>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QtDebug>
#include <vle/utils/Exception.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

namespace vle {
namespace gvle {

vleVpz::vleVpz(const QString& filename)
  : hasMeta(false)
  , mFilename(filename)
  , mFileNameVpm("")
  , mDoc("vle_project")
  , mDocVpm(0)
  , mVdo(0)
  , mVdoVpm(0)
  , undoStack(0)
  , undoStackVpm(0)
  , waitUndoRedoVpz(false)
  , waitUndoRedoVpm(false)
  , oldValVpz()
  , newValVpz()
  , oldValVpm()
  , newValVpm()
  , mLogger(0)
  , mGvlePlugins(0)
  , maxPrecision(std::numeric_limits<double>::digits10)
{
    xReadDom();
    mVdo = new vleDomVpz(&mDoc);
    undoStack = new DomDiffStack(mVdo);
    undoStack->init(getDomDoc());
}

vleVpz::vleVpz(QXmlInputSource& source)
  : hasMeta(false)
  , mFilename("")
  , mFileNameVpm("")
  , mDoc("vle_project")
  , mDocVpm(0)
  , mVdo(0)
  , mVdoVpm(0)
  , undoStack(0)
  , undoStackVpm(0)
  , waitUndoRedoVpz(false)
  , waitUndoRedoVpm(false)
  , oldValVpz()
  , newValVpz()
  , oldValVpm()
  , newValVpm()
  , mLogger(0)
  , mGvlePlugins(0)
  , maxPrecision(std::numeric_limits<double>::digits10)
{
    QXmlSimpleReader reader;
    mDoc.setContent(&source, &reader);
    mVdo = new vleDomVpz(&mDoc);
    undoStack = new DomDiffStack(mVdo);
    undoStack->init(getDomDoc());
}

vleVpz::~vleVpz()
{
    delete mDocVpm;
    delete mVdo;
    delete mVdoVpm;
    delete undoStack;
    delete undoStackVpm;
    mLogger = 0;
    mGvlePlugins = 0;
}

vleVpz::vleVpz(const QString& vpzpath,
               const QString& vpmpath,
               gvle_plugins* plugs)
  : hasMeta(true)
  , mFilename(vpzpath)
  , mFileNameVpm(vpmpath)
  , mDoc("vle_project")
  , mDocVpm(0)
  , mVdo(0)
  , mVdoVpm(0)
  , undoStack(0)
  , undoStackVpm(0)
  , waitUndoRedoVpz(false)
  , waitUndoRedoVpm(false)
  , oldValVpz()
  , newValVpz()
  , oldValVpm()
  , newValVpm()
  , mLogger(0)
  , mGvlePlugins(plugs)
  , maxPrecision(std::numeric_limits<double>::digits10)
{
    xReadDom();
    mVdo = new vleDomVpz(&mDoc);
    undoStack = new DomDiffStack(mVdo);
    undoStack->init(getDomDoc());
    QFile file(mFileNameVpm);
    if (file.exists()) {
        mDocVpm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocVpm->setContent(&source, &reader);
    } else {
        xCreateDomMetadata();
    }

    mVdoVpm = new vleDomVpm(mDocVpm);
    undoStackVpm = new DomDiffStack(mVdoVpm);
    undoStackVpm->init(*mDocVpm);

    QObject::connect(vleVpz::undoStack,
                     SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                     this,
                     SLOT(onUndoRedoStackVpz(QDomNode, QDomNode)));
    QObject::connect(undoStackVpm,
                     SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                     this,
                     SLOT(onUndoRedoStackVpm(QDomNode, QDomNode)));
    QObject::connect(vleVpz::undoStack,
                     SIGNAL(undoAvailable(bool)),
                     this,
                     SLOT(onUndoAvailable(bool)));
}

/******************************************************
 * Access to specific nodes in the vpz from Doc
 ******************************************************/

const QDomDocument&
vleVpz::getDomDoc() const
{
    return mDoc;
}

QDomDocument&
vleVpz::getDomDoc()
{
    return mDoc;
}

QDomNode
vleVpz::classesFromDoc()
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("classes");
    QDomNode classesElem = nodeList.item(0);
    return classesElem;
}

QDomNode
vleVpz::classFromDoc(const QString& className)
{
    QDomNode classesElem = classesFromDoc();
    QDomNodeList list = classesElem.toElement().elementsByTagName("class");
    for (int i = 0; i < list.length(); i++) {
        if (DomFunctions::attributeValue(list.at(i), "name") == className) {
            return list.at(i);
        }
    }
    qDebug() << ("Internal error in classFromDoc (class not found): ")
             << className;
    return QDomNode();
}

QDomNode
vleVpz::classModelFromDoc(const QString& className)
{
    if (className.isEmpty()) {
        return modelFromDoc();
    } else {
        QDomNode classNode = classFromDoc(className);
        QDomNodeList list = classNode.toElement().elementsByTagName("model");
        return list.at(0);
    }
}

bool
vleVpz::existClassFromDoc(const QString& className)
{
    QDomNode classesElem = classesFromDoc();
    QDomNodeList list = classesElem.toElement().elementsByTagName("class");
    for (int i = 0; i < list.length(); i++) {
        if (DomFunctions::attributeValue(list.at(i), "name") == className) {
            return true;
        }
    }
    return false;
}

QString
vleVpz::addClassToDoc(bool atomic)
{
    QDomNode classesElem =
      DomFunctions::obtainChild(vleProjectFromDoc(), "classes", &mDoc);
    undoStack->snapshot(classesElem);
    QString prefix = "NewClass";
    QString name;
    int k = 0;
    bool found = false;
    while (not found) {
        name = prefix;
        if (k > 0) {
            name += "_";
            name += QVariant(k).toString();
        }
        if (existClassFromDoc(name)) {
            k++;
        } else {
            found = true;
        }
    }
    QDomNode newClass = getDomDoc().createElement("class");
    DomFunctions::setAttributeValue(newClass, "name", name);
    QDomNode newModel = getDomDoc().createElement("model");
    DomFunctions::setAttributeValue(newModel, "name", "NewModel");
    if (atomic) {
        DomFunctions::setAttributeValue(newModel, "type", "atomic");
        DomFunctions::setAttributeValue(newModel, "conditions", "");
        DomFunctions::setAttributeValue(newModel, "dynamics", "");
        DomFunctions::setAttributeValue(newModel, "observables", "");
    } else {
        DomFunctions::setAttributeValue(newModel, "type", "coupled");
    }
    DomFunctions::setAttributeValue(newModel, "x", "0");
    DomFunctions::setAttributeValue(newModel, "y", "0");
    DomFunctions::setAttributeValue(newModel, "width", "50");
    DomFunctions::setAttributeValue(newModel, "height", "50");
    newClass.appendChild(newModel);
    classesElem.appendChild(newClass);
    synchronizeUndoStack();
    emit modelsUpdated();
    return name;
}

void
vleVpz::renameClassToDoc(const QString& oldClass, const QString& newClass)
{
    QDomNode cl = classFromDoc(oldClass);
    if (cl.isNull()) {
        qDebug() << "Internal error in renameClassToDoc (class not found)";
        return;
    }
    undoStack->snapshot(classesFromDoc());
    DomFunctions::setAttributeValue(cl, "name", newClass);
    synchronizeUndoStack();
}

QString
vleVpz::copyClassToDoc(const QString& className)
{
    QDomNode classesElem = classesFromDoc();
    QDomNode classNode = classFromDoc(className);
    QDomNode copy = classNode.cloneNode(true);
    QString name_prefix = className;
    QString name;
    int k = 1;
    bool found = false;
    while (not found) {
        name = name_prefix;
        name += "_";
        name += QVariant(k).toString();
        if (existClassFromDoc(name)) {
            k++;
        } else {
            found = true;
        }
    }
    DomFunctions::setAttributeValue(copy, "name", name);
    classesElem.appendChild(copy);
    return name;
}

void
vleVpz::removeClassToDoc(const QString& className)
{
    QDomNode classesElem = classesFromDoc();
    undoStack->snapshot(classesElem);
    QDomNode classNode = classFromDoc(className);
    classesElem.removeChild(classNode);
    synchronizeUndoStack();
}

QDomNode
vleVpz::vleProjectFromDoc() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("vle_project");
    return nodeList.item(0);
}

QDomNode
vleVpz::experimentFromDoc() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomNode expElem = nodeList.item(0);
    return expElem;
}

QDomNode
vleVpz::obsFromDoc() const
{
    QDomNode views = viewsFromDoc();
    if (views.isNull()) {
        return QDomNode();
    }
    return obsFromViews(views);
}

QDomNode
vleVpz::viewsFromDoc() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    if (nodeList.size() == 0) {
        return QDomNode();
    }
    QDomElement expElem = nodeList.item(0).toElement();
    nodeList = expElem.elementsByTagName("views");
    if (nodeList.size() == 0) {
        return QDomNode();
    }
    return nodeList.item(0);
}

QDomNode
vleVpz::condsFromDoc() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement expElem = nodeList.item(0).toElement();
    nodeList = expElem.elementsByTagName("conditions");
    return nodeList.item(0);
}

QDomNode
vleVpz::structuresFromDoc() const
{
    QDomNodeList dynList = getDomDoc().elementsByTagName("structures");
    return dynList.item(0);
}

QDomNode
vleVpz::dynamicsFromDoc() const
{
    QDomNodeList dynList = getDomDoc().elementsByTagName("dynamics");
    return dynList.item(0);
}

QDomNodeList
vleVpz::dynListFromDoc() const
{
    QDomNodeList dynList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynElem = dynamicsFromDoc().toElement();
    dynList = dynElem.elementsByTagName("dynamic");
    return dynList;
}

QDomNode
vleVpz::dynamicFromDoc(const QString& dyn) const
{
    QDomNodeList dynList = dynListFromDoc();
    for (int i = 0; i < dynList.length(); i++) {
        if (DomFunctions::attributeValue(dynList.at(i), "name") == dyn) {
            return dynList.at(i);
        }
    }
    return QDomNode();
}

QDomNodeList
vleVpz::portsListFromDoc(const QString& condName) const
{
    return portsListFromCond(condFromConds(condsFromDoc(), condName));
}

QDomNode
vleVpz::portFromDoc(const QString& condName, const QString& portName) const
{
    return portFromCond(condFromConds(condsFromDoc(), condName), portName);
}

QDomNode
vleVpz::modelFromDoc()
{
    QDomNode structures =
      getDomDoc().documentElement().elementsByTagName("structures").at(0);
    return DomFunctions::obtainChild(structures, "model", &mDoc);
}

QDomNode
vleVpz::modelConnectionsFromDoc(const QString& mod_query)
{
    QDomNode mod = mVdo->getNodeFromXQuery(mod_query);
    QDomNodeList childList = mod.childNodes();
    for (int i = 0; i < childList.length(); i++) {
        QDomNode item = childList.item(i);
        if (item.nodeName() == "connections") {
            return item;
        }
    }
    qDebug() << "Internal Error in modelConnectionsFromDoc\n";
    return QDomNode();
}

QList<QDomNode>
vleVpz::listOfAtomicFromDoc() const
{
    QDomNodeList modelList = getDomDoc().elementsByTagName("model");
    QList<QDomNode> atomList;
    for (int i = 0; i < modelList.size(); i++) {
        QDomNode model = modelList.at(i);
        if (DomFunctions::attributeValue(model, "type") == "atomic") {
            atomList.push_back(model);
        }
    }
    return atomList;
}

QDomNode
vleVpz::modelConnectionFromDoc(const QString& sourceFullPath,
                               const QString& destinationFullPath,
                               const QString& sourcePort,
                               const QString& destinationPort)
{
    QString coupledModel("");
    QStringList sourcePathSplit = sourceFullPath.split(".");
    QStringList destinationPathSplit = destinationFullPath.split(".");
    if (sourcePathSplit.length() == destinationPathSplit.length() - 1) {
        // source is a coupled
        for (int i = 0; i < sourcePathSplit.length(); i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += sourcePathSplit[i];
        }
    } else if (sourcePathSplit.length() - 1 == destinationPathSplit.length()) {
        // destination is the coupled
        for (int i = 0; i < destinationPathSplit.length(); i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += destinationPathSplit[i];
        }
    } else if (sourcePathSplit.length() == destinationPathSplit.length()) {
        // the two are in common coupled model
        for (int i = 0; i < destinationPathSplit.length() - 1; i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += destinationPathSplit[i];
        }
    } else {
        qDebug() << "Internal error in modelConnectionFromDoc 1";
        return QDomNode();
    }
    QString sourceModel = sourcePathSplit[sourcePathSplit.length() - 1];
    QString destModel =
      destinationPathSplit[destinationPathSplit.length() - 1];
    QDomNode connexions = modelConnectionsFromDoc(coupledModel);
    QDomNodeList conList =
      connexions.toElement().elementsByTagName("connection");
    for (int j = 0; j < conList.length(); j++) {
        QDomNode con = conList.at(j);
        if ((DomFunctions::attributeValue(
               con.toElement().elementsByTagName("origin").item(0), "model") ==
             sourceModel) and
            (DomFunctions::attributeValue(
               con.toElement().elementsByTagName("origin").item(0), "port") ==
             sourcePort) and
            (DomFunctions::attributeValue(
               con.toElement().elementsByTagName("destination").item(0),
               "model") == destModel) and
            (DomFunctions::attributeValue(
               con.toElement().elementsByTagName("destination").item(0),
               "port") == destinationPort)) {
            return con;
        }
    }
    qDebug() << "Internal error in modelConnectionFromDoc 2";
    return QDomNode();
}

QDomNode
vleVpz::baseModelFromModelQuery(const QString& model_query)
{
    QDomNode model_node = mVdo->getNodeFromXQuery(model_query);
    QDomNode base = model_node;
    bool found_base = false;
    while (not found_base) {
        if (base.parentNode().nodeName() == "submodels") {
            base = base.parentNode().parentNode();
        } else {
            found_base = true;
        }
    }
    return base;
}

/******************************************************
 * Access to specific nodes in the vpz from internal nodes
 ******************************************************/

QDomNode
vleVpz::obsFromViews(QDomNode node) const
{
    if (node.isNull()) {
        return QDomNode();
    }
    if (node.nodeName() != "views") {
        qDebug() << ("Internal error in obsFromView (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "observables") {
            return child;
        }
    }
    return QDomNode();
}

QDomNode
vleVpz::outputsFromViews(QDomNode node)
{
    if (node.nodeName() != "views") {

        qDebug() << ("Internal error in outputsFromView (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "outputs") {
            return child;
        }
    }
    qDebug() << ("Internal error in outputsFromView (outputs not found)");
    return QDomNode();
}

QDomNode
vleVpz::mapFromOutput(QDomNode node)
{
    if (node.nodeName() != "output") {
        QString mess = "Internal error in mapFromOutput (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return QDomNode();
    }
    QDomNodeList childs = node.childNodes();
    if (childs.length() != 1) {
        QString mess = "Internal error in mapFromOutput (wrong nb of childs)";
        qDebug() << (mess);
        return QDomNode();
    }
    QDomNode child = childs.item(0);
    if (child.nodeName() != "map") {
        QString mess =
          "Internal error in mapFromOutput (unexpected node type)";
        qDebug() << (mess);
        return QDomNode();
    }
    return child;
}

QDomNode
vleVpz::atomicModelFromModel(const QDomNode& node, const QString& atom) const
{
    QDomNode res;
    if (node.nodeName() != "model") {
        qDebug()
          << ("Internal error in atomicModelFromModel (wrong main tag)");
        return res;
    }
    QDomNode currSubModelTag = node.firstChildElement("submodels");
    if (currSubModelTag.nodeName() != "submodels") {
        if ((DomFunctions::attributeValue(node, "type") == "atomic") and
            (DomFunctions::attributeValue(node, "name") == atom)) {
            return node;
        }
        return res;
    }
    QDomNodeList currSubModels = currSubModelTag.childNodes();
    bool found = false;
    for (int i = 0; i < currSubModels.length(); i++) {
        QDomNode child = currSubModels.at(i);
        if (child.nodeName() == "model") {
            QDomNode n = atomicModelFromModel(currSubModels.at(i), atom);
            if (not n.isNull()) {
                if (found) {
                    qDebug() << ("Internal error in atomicModelFromModel "
                                 "(ambiguous)");
                    return QDomNode();
                }
                found = true;
                res = n;
            }
        }
    }
    return res;
}

QDomNode
vleVpz::connectionsFromModel(const QDomNode& node) const
{
    if (node.nodeName() != "model") {
        qDebug()
          << ("Internal error in connectionsFromModel (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList children = node.toElement().childNodes();
    for (int i = 0; i < children.length(); i++) {
        QDomNode conns = children.at(i);
        if (conns.nodeName() == "connections") {
            return conns;
        }
    }
    return QDomNode();
}

QList<QDomNode>
vleVpz::connectionListFromModel(const QDomNode& node,
                                const QList<QString>& submodel_names) const
{
    QList<QDomNode> ret;
    if (node.nodeName() != "model") {
        qDebug()
          << ("Internal error in connectionListFromModel (wrong main tag)");
        return ret;
    }
    QDomNode conns = DomFunctions::obtainChild(node, "connections");
    if (conns.isNull()) {
        return ret;
    }
    QList<QDomNode> all_conns =
      DomFunctions::childNodesWithoutText(conns, "connection");
    if (submodel_names.empty()) {
        return all_conns;
    }

    for (int i = 0; i < all_conns.size(); i++) {
        QDomNode conn = all_conns.at(i);
        if (isConnectionAssociatedTo(conn, submodel_names)) {
            ret.append(conn);
        }
    }
    return ret;
}

QList<QDomNode>
vleVpz::submodelsFromModel(const QDomNode& node)
{
    QDomNode sub = DomFunctions::obtainChild(node, "submodels");
    if (sub.isNull()) {
        return QList<QDomNode>();
    } else {
        return DomFunctions::childNodesWithoutText(sub, "model");
    }
}

vleDomVpz*
vleVpz::vdo()
{
    return mVdo;
}

void
vleVpz::removeTextChilds(QDomNode node, bool recursively)
{
    QDomNodeList chs;
    QList<QDomNode> torm;
    bool stop = false;
    bool first = true;
    while (not stop) {
        torm.clear();
        chs = node.childNodes();
        for (int i = 0; i < chs.length(); i++) {
            QDomNode ch = chs.at(i);
            if (ch.nodeName() == "#text" or ch.isText()) {
                torm.push_back(ch);
            }
            if (first and recursively) {
                removeTextChilds(ch);
            }
        }
        first = false;
        if (torm.size() == 0) {
            stop = true;
        } else {
            for (int i = 0; i < torm.size(); i++) {
                QDomNode ch = torm[i];
                node.removeChild(ch);
            }
        }
    }
}

QString
vleVpz::mergeQueries(const QString& query1, const QString& query2)
{

    QStringList qList1 = query1.split("/");
    QStringList qList2 = query2.split("/");
    if ((qList2.size() >= 2) and
        (qList1.at(qList1.size() - 1) == qList2.at(1))) {
        QString res = "";
        QStringList resList = qList1;
        qList2.removeFirst();
        qList2.removeFirst();
        resList.append(qList2);
        int nbElem = resList.size();
        for (int i = 0; i < nbElem; i++) {
            if (i == 0) {
                res = resList.at(i);
            } else {
                res = res + "/" + resList.at(i);
            }
        }
        return res;
    } else {
        return "";
    }
}

QString
vleVpz::subQuery(const QString& query, int begin, int end)
{
    QStringList qList = query.split("/");
    int end2 = end;
    if (end2 < 0) {
        end2 = qList.size() + end;
    }
    QString res = "";
    for (int i = begin; i < end2; i++) {
        if (i == begin) {
            res = qList.at(i);
        } else {
            res = res + "/" + qList.at(i);
        }
    }
    return res;
}

/*****************************************************
 * TODO A TRIER
 *****************************************************/

QString
vleVpz::getFilename() const
{
    return mFilename;
}

QString
vleVpz::getAuthor() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("author")) {
        QDomNode xAuthor = attrMap.namedItem("author");
        return xAuthor.nodeValue();
    }
    return QString("");
}

void
vleVpz::setAuthor(const QString author)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("author", author);
    synchronizeUndoStack();
}

QString
vleVpz::getDate() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("date")) {
        QDomNode xDate = attrMap.namedItem("date");
        return xDate.nodeValue();
    }
    return QString("");
}

void
vleVpz::setDate(const QString date)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("date", date);
    synchronizeUndoStack();
}

QString
vleVpz::getVersion() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("version")) {
        QDomNode xVersion = attrMap.namedItem("version");
        return xVersion.nodeValue();
    }
    return QString("");
}

void
vleVpz::setVersion(const QString version)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("version", version);
    synchronizeUndoStack();
}

QString
vleVpz::getExpName() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("name")) {
        QDomNode xName = attrMap.namedItem("name");
        return xName.nodeValue();
    }
    return QString("");
}

void
vleVpz::setExpName(const QString name)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("name", name);
    synchronizeUndoStack();
}

QString
vleVpz::getExpDuration() const
{
    QDomNode xDuration = portFromDoc("simulation_engine", "duration");
    std::vector<std::unique_ptr<value::Value>> valuesToFill;
    fillWithMultipleValue(xDuration, valuesToFill);
    std::unique_ptr<value::Value> durationValue = std::move(valuesToFill[0]);
    return QLocale().toString(
      durationValue->toDouble().value(), 'g', maxPrecision);
}

void
vleVpz::setExpDuration(const QString duration)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    synchronizeUndoStack();
    docElem.setAttribute("duration", duration);
    bool ok;
    double durationNum = QLocale().toDouble(duration, &ok);
    if (ok) {
        vle::value::Value* durationValue = new vle::value::Double(durationNum);
        fillWithValue("simulation_engine", "duration", 0, *durationValue);
        delete durationValue;

        emit experimentUpdated();
    }
}

QString
vleVpz::getExpBegin() const
{
    QDomNode xBegin = portFromDoc("simulation_engine", "begin");
    std::vector<std::unique_ptr<value::Value>> valuesToFill;
    fillWithMultipleValue(xBegin, valuesToFill);
    std::unique_ptr<value::Value> beginValue = std::move(valuesToFill[0]);
    return QLocale().toString(
      beginValue->toDouble().value(), 'g', maxPrecision);
}

void
vleVpz::setExpBegin(const QString begin)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    synchronizeUndoStack();
    docElem.setAttribute("begin", begin);
    bool ok;
    double beginNum = QLocale().toDouble(begin, &ok);
    if (ok) {
        vle::value::Value* beginValue = new vle::value::Double(beginNum);
        fillWithValue("simulation_engine", "begin", 0, *beginValue);
        delete beginValue;

        emit experimentUpdated();
    }
}

void
vleVpz::renameObservableToDoc(const QString& oldName, const QString& newName)
{
    if (not existObsFromDoc(oldName) or existObsFromDoc(newName) or
        oldName == newName) {
        return;
    }

    bool bigSnapshotDone = false;
    // update models attached with this observable
    QDomNode atom;
    QDomNodeList modList = getDomDoc().elementsByTagName("model");
    for (int i = 0; i < modList.length(); i++) {
        atom = modList.at(i);
        if ((DomFunctions::attributeValue(atom, "type") == "atomic") and
            (DomFunctions::attributeValue(atom, "observables") == oldName)) {
            if (not bigSnapshotDone) {
                undoStack->snapshot(vleProjectFromDoc());
                synchronizeUndoStack();
                bigSnapshotDone = true;
            }
            DomFunctions::setAttributeValue(atom, "observables", newName);
        }
    }

    // rename observable
    QDomNode obss = obsFromDoc();
    if (not bigSnapshotDone) {
        undoStack->snapshot(obss);
        synchronizeUndoStack();
    }
    QDomNode toRename = obsFromObss(obss, oldName);
    QDomElement docElem = toRename.toElement();
    docElem.setAttribute("name", newName);

    emit observablesUpdated();
}

void
vleVpz::renameObsPortToDoc(const QString& obsName,
                           const QString& oldPortName,
                           const QString& newPortName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    synchronizeUndoStack();
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode portToRename =
      DomFunctions::childWhithNameAttr(obs, "port", oldPortName);
    DomFunctions::setAttributeValue(portToRename, "name", newPortName);

    emit observablesUpdated();
}

QDomElement
vleVpz::buildEmptyValueFromDoc(vle::value::Value::type vleType)
{
    return vleDomStatic::buildEmptyValueFromDoc(getDomDoc(), vleType);
}

bool
vleVpz::existObsFromDoc(const QString& obsName) const
{
    QDomNode obss = obsFromViews(viewsFromDoc());
    if (obss.isNull()) {
        return false;
    }
    QDomNodeList obsList = obss.childNodes();
    for (int i = 0; i < obsList.length(); i++) {
        QDomNode obs = obsList.at(i);
        if (DomFunctions::attributeValue(obs, "name") == obsName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existViewFromDoc(const QString& viewName) const
{
    QDomNode views = viewsFromDoc();
    QDomNodeList viewList = views.childNodes();
    for (int i = 0; i < viewList.length(); i++) {
        QDomNode view = viewList.at(i);
        if (DomFunctions::attributeValue(view, "name") == viewName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existDynamicFromDoc(const QString& dynName) const
{
    QDomNode dyns = dynamicsFromDoc();
    QDomNodeList dynList = dyns.childNodes();
    for (int i = 0; i < dynList.length(); i++) {
        QDomNode view = dynList.at(i);
        if (DomFunctions::attributeValue(view, "name") == dynName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existCondFromDoc(const QString& condName) const
{
    return vleDomStatic::existCondFromConds(condsFromDoc(), condName);
}

bool
vleVpz::existObsPortFromDoc(const QString& obsName,
                            const QString& portName) const
{
    QDomNodeList ports = portsListFromObs(obsFromObss(obsFromDoc(), obsName));
    for (int i = 0; i < ports.length(); i++) {
        QDomNode port = ports.at(i);
        if (DomFunctions::attributeValue(port, "name") == portName) {
            return true;
        }
    }
    return false;
}

void
vleVpz::renameViewToDoc(const QString& oldName, const QString& newName)
{
    QDomNode views = viewsFromDoc();

    undoStack->snapshot(views);
    QList<QDomNode> viewList =
      DomFunctions::childNodesWithoutText(views, "view");
    for (int i = 0; i < viewList.size(); i++) {
        QDomNode v = viewList[i];
        if (DomFunctions::attributeValue(v, "name") == oldName) {
            DomFunctions::setAttributeValue(v, "name", newName);
        }
        if (DomFunctions::attributeValue(v, "output") == oldName) {
            DomFunctions::setAttributeValue(v, "output", newName);
        }
    }
    // rename output (view and output names are set to be equal)
    QDomNode outputs = DomFunctions::obtainChild(views, "outputs", &mDoc);
    QList<QDomNode> outList =
      DomFunctions::childNodesWithoutText(outputs, "output");
    for (int i = 0; i < outList.size(); i++) {
        QDomNode o = outList[i];
        if (DomFunctions::attributeValue(o, "name") == oldName) {
            DomFunctions::setAttributeValue(o, "name", newName);
        }
    }
    // rename view names into observables (ie. attached views to ports)
    QDomNode obss = DomFunctions::obtainChild(views, "observables", &mDoc);
    QList<QDomNode> obsList =
      DomFunctions::childNodesWithoutText(obss, "observable");
    for (int i = 0; i < obsList.size(); i++) {
        QDomNode o = obsList[i];
        QList<QDomNode> portList =
          DomFunctions::childNodesWithoutText(o, "port");
        for (int j = 0; j < portList.size(); j++) {
            QDomNode p = portList[j];
            QList<QDomNode> attViewL =
              DomFunctions::childNodesWithoutText(p, "attachedview");
            for (int k = 0; k < attViewL.size(); k++) {
                QDomNode av = attViewL[k];
                if (DomFunctions::attributeValue(av, "name") == oldName) {
                    DomFunctions::setAttributeValue(av, "name", newName);
                }
            }
        }
    }
    QString gui_plugin = getOutputGUIplugin(oldName);
    setOutputGUIplugin(oldName, "", true);
    setOutputGUIplugin(newName, gui_plugin, false);

    emit viewsUpdated();
}

void
vleVpz::renameConditionToDoc(const QString& oldName, const QString& newName)
{
    if (oldName == "simulation_engine" or existCondFromDoc(newName) or
        not existCondFromDoc(oldName) or oldName == newName) {
        return;
    }
    int prev_curr = undoStack->curr;
    bool bigSnapshotDone = false;

    // update models attached with this condition
    QDomNode atom;
    QDomNodeList modList = getDomDoc().elementsByTagName("model");
    for (int i = 0; i < modList.length(); i++) {
        atom = modList.at(i);
        if (DomFunctions::attributeValue(atom, "type") == "atomic") {
            QSet<QString> attachedConds =
              vleDomStatic::attachedCondsToAtomic(atom);
            if (attachedConds.contains(oldName)) {
                attachedConds.remove(oldName);
                attachedConds.insert(newName);
                if (not bigSnapshotDone) {
                    undoStack->snapshot(vleProjectFromDoc());
                    synchronizeUndoStack();
                    bigSnapshotDone = true;
                }
                vleDomStatic::attachCondsToAtomic(atom, attachedConds);
            }
        }
    }
    // rename condition
    QDomNode conds = condsFromDoc();
    if (not bigSnapshotDone) {
        undoStack->snapshot(conds);
        synchronizeUndoStack();
    }
    QDomNodeList condList = condsListFromConds(conds);
    for (int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.at(i);
        if (DomFunctions::attributeValue(cond, "name") == oldName) {
            DomFunctions::setAttributeValue(cond, "name", newName);
        }
    }

    renameCondGUIplugin(oldName, newName);
    synchronizeUndoStack();
    tryEmitUndoAvailability(prev_curr, undoStack->curr, undoStack->saved);
    emit conditionsUpdated();
}

void
vleVpz::renameDynamicToDoc(const QString& oldName, const QString& newName)
{
    if (not existDynamicFromDoc(oldName) or existDynamicFromDoc(newName) or
        oldName == newName) {
        return;
    }
    bool bigSnapshotDone = false;

    // update models attached with this dynamic
    QDomNodeList mods = getDomDoc().elementsByTagName("model");
    for (int i = 0; i < mods.size(); i++) {
        QDomNode mod = mods.at(i);
        if ((DomFunctions::attributeValue(mod, "type") == "atomic") and
            (DomFunctions::attributeValue(mod, "dynamics") == oldName)) {
            if (not bigSnapshotDone) {
                undoStack->snapshot(vleProjectFromDoc());
                synchronizeUndoStack();
                bigSnapshotDone = true;
            }
            DomFunctions::setAttributeValue(mod, "dynamics", newName);
        }
    }

    // rename dynamic
    QDomNode dyns = dynamicsFromDoc();
    if (not bigSnapshotDone) {
        undoStack->snapshot(dyns);
        synchronizeUndoStack();
    }
    QDomNode dyn = DomFunctions::childWhithNameAttr(dyns, "dynamic", oldName);
    DomFunctions::setAttributeValue(dyn, "name", newName);

    emit dynamicsUpdated();
}

void
vleVpz::renameCondPortToDoc(const QString& condName,
                            const QString& oldName,
                            const QString& newName)
{
    if (condName == "simulation_engine" and
        (oldName == "begin" or oldName == "duration")) {
        return;
    }
    QDomNode cond = condFromConds(condsFromDoc(), condName);
    QDomNodeList portList = portsListFromCond(cond);
    int oldNameIndex = -1;
    for (int i = 0; i < portList.length(); i++) {
        QDomNode port = portList.at(i);
        if (DomFunctions::attributeValue(port, "name") == newName) {
            // already exists do nothing.
            return;
        }
        if (DomFunctions::attributeValue(port, "name") == oldName) {
            oldNameIndex = i;
        }
    }
    if (oldNameIndex != -1) {
        undoStack->snapshot(cond);
        synchronizeUndoStack();
        QDomNode port = portList.at(oldNameIndex);
        DomFunctions::setAttributeValue(port, "name", newName);
        emit conditionsUpdated();
    } else {
        qDebug() << "Internal error in renameCondPortToDoc (not found)";
    }
}

QString
vleVpz::newDynamicNameToDoc(QString prefix) const
{
    return DomFunctions::childNameProvider(
      dynamicsFromDoc(), "dynamic", prefix);
}

QString
vleVpz::newViewNameToDoc(QString prefix) const
{
    return DomFunctions::childNameProvider(viewsFromDoc(), "view", prefix);
}

QString
vleVpz::newCondNameToDoc(QString prefix) const
{
    return DomFunctions::childNameProvider(
      condsFromDoc(), "condition", prefix);
}

QString
vleVpz::newCondPortNameToDoc(QString cond, QString prefix) const
{
    QDomNode condAtom = condFromConds(condsFromDoc(), cond);
    return DomFunctions::childNameProvider(condAtom, "port", prefix);
}

QString
vleVpz::newObsNameToDoc(QString prefix) const
{
    QDomNode observables = obsFromDoc();
    return DomFunctions::childNameProvider(observables, "observables", prefix);
}

QString
vleVpz::newObsPortNameToDoc(QString obsName, QString prefix) const
{
    QDomNode observables = obsFromDoc();
    QDomNode obs = obsFromObss(observables, obsName);
    return DomFunctions::childNameProvider(obs, "port", prefix);
}

QDomNodeList
vleVpz::obsPortsListFromDoc(const QString& obsName) const
{
    return portsListFromObs(
      obsFromObss(obsFromViews(viewsFromDoc()), obsName));
}

void
vleVpz::addObservableToDoc(const QString& obsName)
{
    QDomNode expe = experimentFromDoc();
    QDomNode observables = obsFromDoc();
    QDomElement elem;
    if (observables.isNull()) {
        undoStack->snapshot(expe);
        synchronizeUndoStack();
        QDomNode views;
        QDomNodeList nodeList = expe.toElement().elementsByTagName("views");
        if (nodeList.size() == 0) {
            views = getDomDoc().createElement("views");
            expe.appendChild(views);
        } else {
            views = nodeList.at(0);
        }
        observables = obsFromDoc();
        if (observables.isNull()) {
            observables = getDomDoc().createElement("observables");
            views.appendChild(observables);
        }
    } else {
        undoStack->snapshot(observables);
        synchronizeUndoStack();
    }
    QDomNodeList obss = obssListFromObss(observables);
    for (int i = 0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == obsName)) {
            qDebug() << ("Internal error in addObservable (already here)");
        }
    }
    elem = getDomDoc().createElement("observable");
    elem.setAttribute("name", obsName);
    observables.appendChild(elem);

    emit observablesUpdated();
}

void
vleVpz::addViewToDoc(const QString& viewName)
{
    QDomNode views = viewsFromDoc();
    int prevCurr = undoStack->curr;
    if (views.isNull()) {
        QDomNode exp = experimentFromDoc();
        QDomElement viewsTag = getDomDoc().createElement("views");
        exp.appendChild(viewsTag);
        undoStack->snapshot(experimentFromDoc());
    } else {
        undoStack->snapshot(views);
    }
    addView(viewsFromDoc(), viewName);
    if (hasMeta) {
        // update vpm with snapshot but without signal
        QString outputPlugin = vleVpz::getOutputPluginFromDoc(viewName);
        QString guiPluginName("");
        if (outputPlugin == "vle.output/storage") {
            guiPluginName = "gvle.output/storage";
        }
        if (outputPlugin == "vle.output/file") {
            guiPluginName = "gvle.output/file";
        }
        setOutputGUIplugin(viewName, guiPluginName);
        // update vpz with snapshot but without signal

        bool oldsnap = undoStack->enableSnapshot(false);
        provideOutputGUIplugin(viewName);

        undoStack->enableSnapshot(oldsnap);
        synchronizeUndoStack();
    }
    synchronizeUndoStack();
    emit viewsUpdated();
    tryEmitUndoAvailability(prevCurr, undoStack->curr, undoStack->saved);
}

void
vleVpz::rmViewToDoc(const QString& viewName)
{
    QDomNode views = viewsFromDoc();
    undoStack->snapshot(views);
    synchronizeUndoStack();
    if (views.isNull()) {
        return;
    }
    QList<QDomNode> viewList =
      DomFunctions::childNodesWithoutText(views, "view");
    for (int i = 0; i < viewList.size(); i++) {
        QDomNode v = viewList[i];
        if (DomFunctions::attributeValue(v, "name") == viewName) {
            views.removeChild(v);
        }
    }
    QDomNode outputs = DomFunctions::obtainChild(views, "outputs");
    if (outputs.isNull()) {
        return;
    }
    QList<QDomNode> outList =
      DomFunctions::childNodesWithoutText(outputs, "output");
    for (int i = 0; i < outList.size(); i++) {
        QDomNode o = outList[i];
        if (DomFunctions::attributeValue(o, "name") == viewName) {
            outputs.removeChild(o);
        }
    }
    QDomNode observables = DomFunctions::obtainChild(views, "observables");
    if (observables.isNull()) {
        return;
    }
    QList<QDomNode> obsList =
      DomFunctions::childNodesWithoutText(observables, "observable");
    for (int i = 0; i < obsList.size(); i++) {
        QDomNode o = obsList[i];
        QList<QDomNode> portList =
          DomFunctions::childNodesWithoutText(o, "port");
        for (int j = 0; j < portList.size(); j++) {
            QDomNode p = portList[j];
            QList<QDomNode> atList =
              DomFunctions::childNodesWithoutText(p, "attachedview");
            for (int k = 0; k < atList.size(); k++) {
                QDomNode a = atList[k];
                if (DomFunctions::attributeValue(a, "name") == viewName) {
                    p.removeChild(a);
                }
            }
        }
    }
    emit viewsUpdated();
}

bool
vleVpz::enableViewToDoc(const QString& viewName, bool enable)
{
    QDomNode views = viewsFromDoc();
    QDomNode atom = viewFromDoc(viewName);
    if (atom.isNull()) {
        qDebug() << "Internal error in enableViewToDoc " << viewName;
        return false;
    }
    bool disable_in_place =
      (DomFunctions::attributeValue(atom, "enable") == "false");
    if (disable_in_place and enable) {
        undoStack->snapshot(views);
        synchronizeUndoStack();
        DomFunctions::setAttributeValue(atom, "enable", "true");
        return true;
    }
    if (not disable_in_place and not enable) {
        undoStack->snapshot(views);
        synchronizeUndoStack();
        DomFunctions::setAttributeValue(atom, "enable", "false");
        return true;
    }
    return false;
}

bool
vleVpz::enabledViewFromDoc(const QString& viewName)
{
    QDomNode atom = viewFromDoc(viewName);
    if (atom.isNull()) {
        return false;
    }
    return (not(DomFunctions::attributeValue(atom, "enable") == "false"));
}

QDomNode
vleVpz::addConditionToDoc(const QString& condName)
{
    QDomNode added =
      vleDomStatic::addCond(condsFromDoc(), condName, &mDoc, undoStack);
    if (not added.isNull()) {
        synchronizeUndoStack();
        emit conditionsUpdated();
    }
    return added;
}

void
vleVpz::addConditionFromPluginToDoc(const QString& condName,
                                    const QString& pluginName)
{
    int prev_curr = undoStack->curr;
    QDomNode added =
      vleDomStatic::addCond(condsFromDoc(), condName, &mDoc, undoStack);
    if (added.isNull()) {
        return;
    }
    if (not hasMeta) {
        return;
    }
    // update vpm
    setCondGUIplugin(condName, pluginName);

    // update vpz with snapshot but without signal
    bool oldsnap = undoStack->enableSnapshot(false);
    provideCondGUIplugin(condName);
    undoStack->enableSnapshot(oldsnap);
    synchronizeUndoStack();
    tryEmitUndoAvailability(prev_curr, undoStack->curr, undoStack->saved);
}

QDomElement
vleVpz::createDynamic(const QString& dyn)
{
    QDomElement elem = getDomDoc().createElement("dynamic");
    elem.setAttribute("name", dyn);
    return elem;
}

QDomNode
vleVpz::addDynamicToDoc(const QString& dyn)
{
    QDomNodeList dynList = dynListFromDoc();
    if (existDynamicIntoDynList(dyn, dynList)) {
        qDebug() << ("Internal error in addDynamicToDoc (already here)");
        return QDomNode();
    }
    QDomElement elem = createDynamic(dyn);
    dynamicsFromDoc().appendChild(elem);
    return elem;
}

QDomNode
vleVpz::addDynamicToDoc(const QString& dyn,
                        const QString& pkgName,
                        const QString& libName)
{
    undoStack->snapshot(dynamicsFromDoc());
    synchronizeUndoStack();
    QDomNode newNode = addDynamicToDoc(dyn);
    DomFunctions::setAttributeValue(newNode, "package", pkgName);
    DomFunctions::setAttributeValue(newNode, "library", libName);
    emit dynamicsUpdated();
    return newNode;
}

void
vleVpz::configDynamicToDoc(const QString& dyn,
                           const QString& pkgName,
                           const QString& libName)
{
    QDomNode dynNode = dynamicFromDoc(dyn);
    if (dynNode.isNull()) {
        qDebug() << "Internal error in configDynamicToDoc (not found)";
        return;
    }
    undoStack->snapshot(dynNode);
    synchronizeUndoStack();
    DomFunctions::setAttributeValue(dynNode, "package", pkgName);
    DomFunctions::setAttributeValue(dynNode, "library", libName);
    emit dynamicsUpdated();
}

QDomNode
vleVpz::copyDynamicToDoc(const QString& dyn, const QString& newDyn)
{
    QDomNode dynNode = dynamicFromDoc(dyn);
    QString pkg = DomFunctions::attributeValue(dynNode, "package");
    QString lib = DomFunctions::attributeValue(dynNode, "library");
    QDomNode newNode = addDynamicToDoc(newDyn, pkg, lib);
    return newNode;
}

void
vleVpz::addObsPortToDoc(const QString& obsName, const QString& portName)
{
    QDomNode observables = obsFromDoc();
    undoStack->snapshot(observables);
    synchronizeUndoStack();
    QDomNode obs = obsFromObss(observables, obsName);
    vleDomStatic::addObservablePort(getDomDoc(), obs, portName);
    emit observablesUpdated();
}

QDomNode
vleVpz::addCondPortToDoc(const QString& condName, const QString& portName)
{
    QDomNode node = condFromConds(condsFromDoc(), condName);
    undoStack->snapshot(node);
    synchronizeUndoStack();
    QDomNodeList conds = portsListFromCond(node);
    for (int i = 0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)) {
            qDebug() << ("Internal error in addPort (already here)");
            return QDomNode();
        }
    }
    QDomElement elem = getDomDoc().createElement("port");
    elem.setAttribute("name", portName);
    node.appendChild(elem);
    return elem;
}

void
vleVpz::rmConditionToDoc(const QString& condName)
{
    if (not existCondFromDoc(condName) or condName == "simulation_engine") {
        return;
    }
    int prevCurr = undoStack->curr;
    bool bigSnapshotDone = false;

    // detach the condition to all the models
    QDomNode atom;
    QDomNodeList modList = getDomDoc().elementsByTagName("model");
    for (int i = 0; i < modList.length(); i++) {
        atom = modList.at(i);
        if (DomFunctions::attributeValue(atom, "type") == "atomic") {
            QSet<QString> attachedConds =
              vleDomStatic::attachedCondsToAtomic(atom);
            if (attachedConds.contains(condName)) {
                attachedConds.remove(condName);
                if (not bigSnapshotDone) {
                    undoStack->snapshot(vleProjectFromDoc());
                    synchronizeUndoStack();
                    bigSnapshotDone = true;
                }
                vleDomStatic::attachCondsToAtomic(atom, attachedConds);
            }
        }
    }
    // remove cond from experiment
    QDomNode conds = condsFromDoc();
    if (not bigSnapshotDone) {
        undoStack->snapshot(conds);
        synchronizeUndoStack();
    }
    QDomNode toRm = condFromConds(conds, condName);
    conds.removeChild(toRm);

    // update vpm
    QString condPlugin = getCondGUIplugin(condName);
    if (condPlugin != "") {
        setCondGUIplugin(condName, "");
    }
    emit conditionsUpdated();
    tryEmitUndoAvailability(prevCurr, undoStack->curr, undoStack->saved);
}

bool
vleVpz::existViewFromObsPortDoc(const QString& obsName,
                                const QString& portName,
                                const QString& viewName) const
{
    QDomNode obss = obsFromDoc();
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode toBeChecked = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(toBeChecked);
    for (int i = 0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)) {
            return true;
        }
    }
    return false;
}

void
vleVpz::rmViewToObsPortDoc(const QString& obsName,
                           const QString& portName,
                           const QString& viewName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    synchronizeUndoStack();
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode port = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(port);
    for (int i = 0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)) {
            port.removeChild(child);
        }
    }

    emit observablesUpdated();
}

void
vleVpz::attachViewToObsPortDoc(const QString& obsName,
                               const QString& portName,
                               const QString& viewName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    synchronizeUndoStack();
    QDomNode obs = obsFromObss(obss, obsName);

    QDomNode toBeAttachedTo = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(toBeAttachedTo);
    for (int i = 0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)) {
            qDebug() << ("Internal error in Port (already here)");
            return;
        }
    }
    QDomElement elem = getDomDoc().createElement("attachedview");
    elem.setAttribute("name", viewName);
    toBeAttachedTo.appendChild(elem);

    emit observablesUpdated();
}

void
vleVpz::rmObsPortToDoc(const QString& obsName, const QString& portName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    synchronizeUndoStack();
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode toRm = portFromObs(obs, portName);
    obs.removeChild(toRm);

    emit observablesUpdated();
}

void
vleVpz::rmCondPortToDoc(const QString& condName, const QString& portName)
{
    vleDomStatic::rmPortFromCond(
      condFromConds(condsFromDoc(), condName), portName, undoStack);
}

void
vleVpz::addValuePortCondToDoc(const QString& condName,
                              const QString& portName,
                              const vle::value::Value& val)
{
    QDomNode port = portFromDoc(condName, portName);
    undoStack->snapshot(port);
    synchronizeUndoStack();
    QString tagName = "";
    switch (val.getType()) {
    case vle::value::Value::BOOLEAN:
        tagName = "boolean";
        break;
    case vle::value::Value::INTEGER:
        tagName = "integer";
        break;
    case vle::value::Value::DOUBLE:
        tagName = "double";
        break;
    case vle::value::Value::STRING:
        tagName = "string";
        break;
    case vle::value::Value::SET:
        tagName = "set";
        break;
    case vle::value::Value::MAP:
        tagName = "map";
        break;
    case vle::value::Value::TUPLE:
        tagName = "tuple";
        break;
    case vle::value::Value::TABLE:
        tagName = "table";
        break;
    case vle::value::Value::XMLTYPE:
        tagName = "xml";
        break;
    case vle::value::Value::NIL:
        tagName = "nil";
        break;
    case vle::value::Value::MATRIX:
        tagName = "matrix";
        break;
    case vle::value::Value::USER:
        tagName = "user";
        break;
    }
    QDomNode child = getDomDoc().createElement(tagName);
    fillWithValue(child, val);
    port.appendChild(child);
}

bool
vleVpz::fillConditionWithMapToDoc(const QString& condName,
                                  const vle::value::Map& val,
                                  bool rm)
{
    QDomNode cond = condFromConds(condsFromDoc(), condName);
    undoStack->snapshot(cond);
    if (rm) {
        QDomNodeList portList = portsListFromDoc(condName);
        for (auto i = 0; i < portList.length(); i++) {
            QDomNode node = portList.at(i);
            node.parentNode().removeChild(node);
        }
    }
    synchronizeUndoStack();
    return vleDomStatic::fillConditionWithMap(mDoc, cond, val);
}

void
vleVpz::rmValuePortCondToDoc(const QString& condName,
                             const QString& portName,
                             int index)
{
    if (index >= 0) {
        QDomNode port = portFromDoc(condName, portName);
        undoStack->snapshot(port);
        synchronizeUndoStack();
        if (port.childNodes().length() > index) {
            QDomNode n = port.childNodes().at(index);
            port.removeChild(n);
        }
    }
}

void
vleVpz::rmObservableFromDoc(const QString& obsName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    synchronizeUndoStack();
    QDomNode toRm = obsFromObss(obss, obsName);
    obss.removeChild(toRm);

    QDomNode structures =
      getDomDoc().documentElement().elementsByTagName("structures").at(0);
    QDomNode currModel =
      structures.toElement().elementsByTagName("model").at(0);
    rmObservableFromModel(currModel, obsName);

    emit observablesUpdated();
}

void
vleVpz::rmObservableFromModel(QDomNode& node, const QString& obsName)
{
    if (node.nodeName() != "model") {
        qDebug()
          << ("Internal error in rmObservableFromModel (wrong main tag)");
        return;
    }
    QDomNode currModel = node.toElement().elementsByTagName("model").at(0);
    QString currName = DomFunctions::attributeValue(currModel, "name");

    QDomNodeList list = currModel.childNodes();
    for (int j = 0; j < list.length(); j++) {
        QDomNode item = list.item(j);
        if (item.nodeName() == "submodels") {
            QDomNodeList list = item.childNodes();
            for (int k = 0; k < list.length(); k++) {
                QDomNode item = list.item(k);
                if (item.nodeName() == "model") {
                    rmObservableFromModel(item, obsName);
                }
            }
        } else {
            if (DomFunctions::attributeValue(currModel, "observables") ==
                obsName) {
                DomFunctions::setAttributeValue(currModel, "observables", "");
            }
        }
    }

    emit observablesUpdated();
}

QDomNodeList
vleVpz::obssListFromObss(const QDomNode& node) const
{
    if (node.isNull()) {
        return QDomNodeList();
    }
    if (node.nodeName() != "observables") {
        qDebug() << ("Internal error in obssListFromObss (wrong main tag)");
        return QDomNodeList();
    }
    return node.toElement().elementsByTagName("observable");
}

QDomNodeList
vleVpz::condsListFromConds(const QDomNode& node) const
{
    if (node.isNull()) {
        return QDomNodeList();
    }
    if (node.nodeName() != "conditions") {
        qDebug() << ("Internal error in condsListFromConds (wrong main tag)");
        return QDomNodeList();
    }
    return node.toElement().elementsByTagName("condition");
}

QDomNodeList
vleVpz::portsListFromObs(const QDomNode& node) const
{
    if (node.nodeName() != "observable") {
        qDebug() << ("Internal error in portsListFromObs (wrong main tag)");
        return QDomNodeList();
    }
    return node.toElement().elementsByTagName("port");
}

QDomNodeList
vleVpz::viewsListFromObsPort(const QDomNode& node) const
{
    if (node.nodeName() != "port") {
        qDebug()
          << ("Internal error in viewsListFromObsPort (wrong main tag)");
        return QDomNodeList();
    }
    return node.toElement().elementsByTagName("attachedview");
}

QDomNodeList
vleVpz::portsListFromCond(const QDomNode& node) const
{
    if (node.nodeName() != "condition") {
        qDebug() << ("Internal error in portsListFromCond (wrong main tag)");
        return QDomNodeList();
    }
    return node.toElement().elementsByTagName("port");
}

QDomNode
vleVpz::obsFromObss(const QDomNode& node, const QString& obsName) const
{
    QDomNodeList obss = obssListFromObss(node);
    for (int i = 0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == obsName)) {
            return child;
        }
    }
    return QDomNode();
}

QDomNode
vleVpz::condFromConds(const QDomNode& node, const QString& condName) const
{
    QDomNodeList conds = condsListFromConds(node);
    for (int i = 0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == condName)) {
            return child;
        }
    }
    return QDomNode();
}

QDomNode
vleVpz::portFromObs(const QDomNode& node, const QString& portName) const
{
    QDomNodeList obss = portsListFromObs(node);
    for (int i = 0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)) {
            return child;
        }
    }
    return QDomNode();
}

QDomNode
vleVpz::portFromCond(const QDomNode& node, const QString& portName) const
{
    QDomNodeList conds = portsListFromCond(node);
    for (int i = 0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)) {
            return child;
        }
    }
    return QDomNode();
}

bool
vleVpz::existPortFromObs(const QDomNode& node, const QString& portName) const
{
    QDomNodeList portList = portsListFromObs(node);
    for (int i = 0; i < portList.length(); i++) {
        QDomNode port = portList.at(i);
        if (DomFunctions::attributeValue(port, "name") == portName) {
            return true;
        }
    }
    return false;
}

void
vleVpz::renameModel(QDomNode node, const QString& newName)
{
    if (node.nodeName() != "model") {
        qDebug() << "Internal error in renameModel " << node.nodeName();
        return;
    }
    QString oldName = DomFunctions::attributeValue(node, "name");
    bool modif = false;
    if (node.parentNode().nodeName() == "structures") {
        modif = vleDomStatic::renameModelIntoStructures(
          mDoc, node.parentNode(), oldName, newName, undoStack);
    } else {
        modif = vleDomStatic::renameModelIntoCoupled(
          mDoc, node.parentNode().parentNode(), oldName, newName, undoStack);
    }
    if (modif) {
        synchronizeUndoStack();
        emit modelsUpdated();
    }
}

void
vleVpz::rmModel(QDomNode node, bool withEmit)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in rmModel (wrong main tag)");
        return;
    }
    QString modelName = DomFunctions::attributeValue(node, "name");
    QDomNode parent =
      node.parentNode(); // either structures, class or submodels
    if (parent.nodeName() == "submodels") {
        undoStack->snapshot(parent.parentNode());
        synchronizeUndoStack();
    } else {
        undoStack->snapshot(parent);
        synchronizeUndoStack();
    }

    QDomNode conTag = connectionsFromModel(parent.parentNode());
    QList<QDomNode> toRemove;
    if (not conTag.isNull()) {
        // remove connections at the coupled level
        QDomNodeList conList =
          conTag.toElement().elementsByTagName("connection");
        for (int i = 0; i < conList.length(); i++) {
            QDomNode con = conList.at(i);
            QDomNode orig = con.toElement().elementsByTagName("origin").at(0);
            QDomNode dest =
              con.toElement().elementsByTagName("destination").at(0);
            if ((DomFunctions::attributeValue(con, "type") == "input") and
                (DomFunctions::attributeValue(dest, "model") == modelName)) {
                toRemove.append(con);
            } else if ((DomFunctions::attributeValue(con, "type") ==
                        "output") and
                       (DomFunctions::attributeValue(orig, "model") ==
                        modelName)) {
                toRemove.append(con);
            } else if ((DomFunctions::attributeValue(con, "type") ==
                        "internal") and
                       ((DomFunctions::attributeValue(orig, "model") ==
                         modelName) or
                        (DomFunctions::attributeValue(dest, "model") ==
                         modelName))) {
                toRemove.append(con);
            }
        }
    }

    for (int i = 0; i < toRemove.length(); i++) {
        toRemove.at(i).parentNode().removeChild(toRemove.at(i));
    }
    node.parentNode().removeChild(node);
    removeTextChilds(parent);
    removeTextChilds(parent.parentNode());
    if (withEmit) {
        emit modelsUpdated();
    }
}

void
vleVpz::rmModelsFromCoupled(QList<QDomNode> nodes)
{
    if (nodes.empty()) {
        return;
    }
    QString query = "";
    QString queryi = "";
    bool oldSnapshotAvailibility = true;
    for (int i = 0; i < nodes.size(); i++) {
        QDomNode modi = nodes.at(i);
        QDomNode parent = modi.parentNode(); // either structures, class or
        // submodels
        queryi = mVdo->getXQuery(parent);
        if (i == 0) {
            query = queryi;
            if (parent.nodeName() == "submodels") {
                undoStack->snapshot(parent.parentNode());
                synchronizeUndoStack();
            } else {
                undoStack->snapshot(parent);
                synchronizeUndoStack();
            }
            oldSnapshotAvailibility = undoStack->enableSnapshot(false);
        } else {
            if (query != queryi) {
                qDebug() << ("Internal error in rmModelsFromCoupled ");
            }
        }
        rmModel(modi);
    }
    undoStack->enableSnapshot(oldSnapshotAvailibility);
    emit modelsUpdated();
}

bool
vleVpz::existSubModel(QDomNode node, const QString& modName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in existSubModel (wrong main tag)");
        return false;
    }
    QList<QDomNode> chs =
      DomFunctions::childNodesWithoutText(node, "submodels");
    if (chs.size() == 1) {
        QList<QDomNode> mods =
          DomFunctions::childNodesWithoutText(chs[0], "model");
        for (int i = 0; i < mods.size(); i++) {

            if (DomFunctions::attributeValue(mods[i], "name") == modName) {
                return true;
            }
        }
    }
    return false;
}

bool
vleVpz::existSiblingModel(QDomNode node, const QString& modName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in existSiblingModel (wrong main tag)");
        return false;
    }
    if (node.parentNode().nodeName() == "structures") { // main model
        return false;
    }
    if (node.parentNode().nodeName() == "class") { // class main model
        return false;
    }
    if (node.parentNode().nodeName() != "submodels") {
        qDebug() << ("Internal error in existSiblingModel (wrong main tag 2)");
        return false;
    }
    QList<QDomNode> mods =
      DomFunctions::childNodesWithoutText(node.parentNode(), "model");
    for (int i = 0; i < mods.size(); i++) {
        if (DomFunctions::attributeValue(mods[i], "name") == modName) {
            return true;
        }
    }
    return false;
}

void
vleVpz::addModel(QDomNode node, const QString& type, QPointF pos)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in addModel (wrong main tag)");
        return;
    }
    QDomElement subModels;
    QDomNodeList subTag = node.toElement().elementsByTagName("submodels");
    if (subTag.length() == 0) {
        subModels = getDomDoc().createElement("submodels");
        node.appendChild(subModels);
    } else {
        subModels = subTag.at(0).toElement();
    }
    undoStack->snapshot(subModels);
    synchronizeUndoStack();

    QString newModelName =
      DomFunctions::childNameProvider(subModels, "model", type);

    QDomNode newModel = getDomDoc().createElement("model");
    if (type == "coupled") {
        DomFunctions::setAttributeValue(newModel, "height", "50");
        DomFunctions::setAttributeValue(newModel, "width", "50");
        DomFunctions::setAttributeValue(
          newModel, "y", QVariant(pos.y()).toString());
        DomFunctions::setAttributeValue(
          newModel, "x", QVariant(pos.x()).toString());
        DomFunctions::setAttributeValue(newModel, "type", "coupled");
        DomFunctions::setAttributeValue(newModel, "name", newModelName);
    } else {
        DomFunctions::setAttributeValue(newModel, "height", "50");
        DomFunctions::setAttributeValue(newModel, "width", "50");
        DomFunctions::setAttributeValue(
          newModel, "y", QVariant(pos.y()).toString());
        DomFunctions::setAttributeValue(
          newModel, "x", QVariant(pos.x()).toString());
        DomFunctions::setAttributeValue(newModel, "observables", "");
        DomFunctions::setAttributeValue(newModel, "dynamics", "");
        DomFunctions::setAttributeValue(newModel, "conditions", "");
        DomFunctions::setAttributeValue(newModel, "type", "atomic");
        DomFunctions::setAttributeValue(newModel, "name", newModelName);
    }
    QDomNode inPort = getDomDoc().createElement("in");
    QDomNode outPort = getDomDoc().createElement("out");
    newModel.appendChild(inPort);
    newModel.appendChild(outPort);
    if (type == "coupled") {
        QDomNode sub = getDomDoc().createElement("submodels");
        newModel.appendChild(sub);
        QDomNode con = getDomDoc().createElement("connections");
        newModel.appendChild(con);
    }
    subModels.appendChild(newModel);
    emit modelsUpdated();
}

void
vleVpz::copyModelsToModel(QList<QDomNode> modsToCopy,
                          QDomNode modDest,
                          qreal xtranslation,
                          qreal ytranslation)
{
    QDomNode parent;
    // check all parents of modsToCopy are the same
    for (int i = 0; i < modsToCopy.size(); i++) {
        if (i == 0) {
            parent = modsToCopy.at(i).parentNode().parentNode();
        } else {
            if (not(modsToCopy.at(i).parentNode().parentNode() == parent)) {
                qDebug() << ("Internal error in copyModelsToModel");
                return;
            }
        }
    }
    // check destination is a coupled model
    if (modDest.nodeName() != "model" or
        DomFunctions::attributeValue(modDest, "type") != "coupled") {
        qDebug() << ("Internal error in copyModelsToModel (wrong main tag)");
        return;
    }
    // build copies of models
    QList<QString> storeCopyNames;
    QList<QString> storeOrigNames;
    {
        for (int i = 0; i < modsToCopy.size(); i++) {
            QDomNode sub = modsToCopy.at(i).cloneNode(true);
            QString subNamePrefix = DomFunctions::attributeValue(sub, "name");
            storeOrigNames.append(subNamePrefix);
            QString subName;
            int k = 0;
            bool found = false;
            while (not found) {
                subName = subNamePrefix;
                if (k > 0) {
                    subName += "_";
                    subName += QVariant(k).toString();
                }
                if (not existSubModel(modDest, subName)) {
                    found = true;
                    renameModel(sub, subName);
                    DomFunctions::setAttributeValue(
                      sub,
                      "x",
                      QVariant(
                        DomFunctions::attributeValue(sub, "x").toDouble() +
                        xtranslation)
                        .toString());
                    DomFunctions::setAttributeValue(
                      sub,
                      "y",
                      QVariant(
                        DomFunctions::attributeValue(sub, "y").toDouble() +
                        ytranslation)
                        .toString());
                } else {
                    k++;
                }
            }
            storeCopyNames.append(subName);
            QDomNode subs =
              DomFunctions::obtainChild(modDest, "submodels", &mDoc);
            subs.appendChild(sub);
        }
    }
    // add Internal connections to destination model
    {
        QDomNode consTagDest = connectionsFromModel(modDest);
        if (consTagDest.isNull()) {
            consTagDest = getDomDoc().createElement("connections");
            modDest.appendChild(consTagDest);
        }
        QDomNode consTagOrig = connectionsFromModel(parent);
        if (consTagOrig.isNull()) {
            consTagOrig = getDomDoc().createElement("connections");
            parent.appendChild(consTagOrig);
        }
        QList<QDomNode> consOrig =
          DomFunctions::childNodesWithoutText(consTagOrig, "connection");
        for (int i = 0; i < consOrig.size(); i++) {
            QDomNode con = consOrig[i];
            if (DomFunctions::attributeValue(con, "type") == "internal") {
                QDomNode origPort =
                  con.toElement().elementsByTagName("origin").at(0);
                QDomNode destPort =
                  con.toElement().elementsByTagName("destination").at(0);
                if (storeOrigNames.contains(
                      DomFunctions::attributeValue(origPort, "model")) and
                    storeOrigNames.contains(
                      DomFunctions::attributeValue(destPort, "model"))) {
                    QDomNode conClone = con.cloneNode(true);
                    origPort =
                      conClone.toElement().elementsByTagName("origin").at(0);
                    destPort = conClone.toElement()
                                 .elementsByTagName("destination")
                                 .at(0);
                    int idOrig = storeOrigNames.indexOf(
                      DomFunctions::attributeValue(origPort, "model"), 0);
                    int idDest = storeOrigNames.indexOf(
                      DomFunctions::attributeValue(destPort, "model"), 0);
                    DomFunctions::setAttributeValue(
                      origPort, "model", storeCopyNames[idOrig]);
                    DomFunctions::setAttributeValue(
                      destPort, "model", storeCopyNames[idDest]);
                    consTagDest.appendChild(conClone);
                }
            }
        }
    }
    emit modelsUpdated();
}
void
vleVpz::renameModelPort(QDomNode node, const QString& newName)
{
    if (node.nodeName() != "port") {
        qDebug() << ("Internal error in renameModelPort (wrong main tag)");
        return;
    }

    QString oldName = DomFunctions::attributeValue(node, "name");
    QString portType = node.parentNode().nodeName();
    QDomNode mod = node.parentNode().parentNode();
    if (mod.nodeName() != "model") {
        qDebug() << ("Internal error in renameModelPort (wrong parent)");
        return;
    }
    QString modName = DomFunctions::attributeValue(mod, "name");
    if (mod.parentNode().nodeName() == "submodels") {
        // adapt connections at the coupled level
        undoStack->snapshot(mod.parentNode().parentNode());
        synchronizeUndoStack();
        QDomNode parConns =
          connectionsFromModel(mod.parentNode().parentNode());
        if (not parConns.isNull()) {
            QList<QDomNode> conList =
              DomFunctions::childNodesWithoutText(parConns, "connection");
            for (int i = 0; i < conList.size(); i++) {
                QDomNode con = conList[i];
                if (portType == "in") {
                    if ((DomFunctions::attributeValue(con, "type") ==
                         "input") or
                        (DomFunctions::attributeValue(con, "type") ==
                         "internal")) {
                        QDomNode dest = con.toElement()
                                          .elementsByTagName("destination")
                                          .at(0);
                        if ((DomFunctions::attributeValue(dest, "model") ==
                             modName) and
                            (DomFunctions::attributeValue(dest, "port") ==
                             oldName)) {
                            DomFunctions::setAttributeValue(
                              dest, "port", newName);
                        }
                    }
                } else if (portType == "out") {
                    if ((DomFunctions::attributeValue(con, "type") ==
                         "output") or
                        (DomFunctions::attributeValue(con, "type") ==
                         "internal")) {
                        QDomNode orig =
                          con.toElement().elementsByTagName("origin").at(0);
                        if ((DomFunctions::attributeValue(orig, "model") ==
                             modName) and
                            (DomFunctions::attributeValue(orig, "port") ==
                             oldName)) {
                            DomFunctions::setAttributeValue(
                              orig, "port", newName);
                        }
                    }
                }
            }
        }
    } else {
        undoStack->snapshot(node.parentNode().parentNode());
        synchronizeUndoStack();
    }
    if (not connectionsFromModel(mod).isNull()) {
        // adapt the connections at internal level (if submodels)
        QList<QDomNode> conList = DomFunctions::childNodesWithoutText(
          connectionsFromModel(mod), "connection");
        for (int i = 0; i < conList.size(); i++) {
            QDomNode con = conList[i];
            if (portType == "in") {
                if (DomFunctions::attributeValue(con, "type") == "input") {
                    QDomNode orig =
                      con.toElement().elementsByTagName("origin").at(0);
                    if ((DomFunctions::attributeValue(orig, "model") ==
                         modName) and
                        (DomFunctions::attributeValue(orig, "port") ==
                         oldName)) {
                        DomFunctions::setAttributeValue(orig, "port", newName);
                    }
                }
            } else if (portType == "out") {
                if (DomFunctions::attributeValue(con, "type") == "output") {
                    QDomNode dest =
                      con.toElement().elementsByTagName("destination").at(0);
                    if ((DomFunctions::attributeValue(dest, "model") ==
                         modName) and
                        (DomFunctions::attributeValue(dest, "port") ==
                         oldName)) {
                        DomFunctions::setAttributeValue(dest, "port", newName);
                    }
                }
            }
        }
    }
    DomFunctions::setAttributeValue(node, "name", newName);
    emit modelsUpdated();
}

void
vleVpz::rmModelPort(QDomNode node)
{
    if (node.nodeName() != "port") {
        qDebug() << ("Internal error in rmModelPort (wrong main tag)");
        return;
    }
    QString oldName = DomFunctions::attributeValue(node, "name");
    QString portType = node.parentNode().nodeName();
    QDomNode parent = node.parentNode().parentNode();
    if (parent.nodeName() != "model") {
        qDebug() << ("Internal error in rmModelPort (wrong parent)");
        return;
    }
    QString ownerModel = DomFunctions::attributeValue(parent, "name");
    if (parent.parentNode().nodeName() == "submodels") {
        // adapt connections at the coupled level (grand_parent)
        QDomNode grand_parent = parent.parentNode().parentNode();
        undoStack->snapshot(grand_parent);
        synchronizeUndoStack();
        // delete internal connections
        for (auto conn : vleDomStatic::getConnectionsFromCoupled(grand_parent,
                "internal",(portType=="out"), ownerModel, oldName)) {
            rmModelConnection(conn, false);
        }
        if (portType == "in") {
            //delete input connections
            for (auto conn : vleDomStatic::getConnectionsFromCoupled(
                    grand_parent, "input", false, ownerModel, oldName)) {
                rmModelConnection(conn, false);
            }

        } else if (portType == "out") {
            //delete output connections
            for (auto conn : vleDomStatic::getConnectionsFromCoupled(
                    grand_parent, "output", true, ownerModel, oldName)) {
                rmModelConnection(conn, false);
            }
        }
    } else {
        undoStack->snapshot(parent);
        synchronizeUndoStack();
    }
    // adapt connections at the parent level (if it's a coupled model)
    if (portType == "in") {
        //delete input connections
        for (auto conn : vleDomStatic::getConnectionsFromCoupled(
                parent, "input", true, ownerModel, oldName)) {
            rmModelConnection(conn, false);
        }
    } else if (portType == "out") {
        //delete output connections
        for (auto conn : vleDomStatic::getConnectionsFromCoupled(
                parent, "output", false, ownerModel, oldName)) {
            rmModelConnection(conn, false);
        }
    }

    QDomNode outsTag = node.parentNode();
    outsTag.removeChild(node);
    removeTextChilds(outsTag);
    emit modelsUpdated();
}

void
vleVpz::addModelPort(QDomNode node, const QString& type)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in addModelPort (wrong main tag)");
        return;
    }
    QString newPortName = "NewPort";

    int nbChildren = node.childNodes().size();
    QDomNode portsType = DomFunctions::obtainChild(node, type, &mDoc);
    if (nbChildren != node.childNodes().size()) {
        if (portsType != node.firstChild()) {
            node.insertBefore(portsType, node.firstChild());
        }
    }

    undoStack->snapshot(portsType);
    synchronizeUndoStack();

    QDomNodeList ports = portsType.toElement().elementsByTagName("port");
    bool found = false;
    unsigned int idInPort = 0;
    while (not found) {
        found = true;
        for (int i = 0; i < ports.length(); i++) {
            if (DomFunctions::attributeValue(ports.at(i), "name") ==
                newPortName) {
                found = false;
            }
        }
        if (not found) {
            idInPort++;
            newPortName = "NewPort";
            newPortName += "_";
            newPortName += QVariant(idInPort).toString();
        }
    }
    QDomElement inPort = getDomDoc().createElement("port");
    inPort.setAttribute("name", newPortName);
    portsType.appendChild(inPort);
    emit modelsUpdated();
}

void
vleVpz::addModelInputPort(QDomNode node)
{
    addModelPort(node, "in");
}

void
vleVpz::addModelOutputPort(QDomNode node)
{
    addModelPort(node, "out");
}

bool
vleVpz::hasModelPort(QDomNode node, const QString& type)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in addModelPort (wrong main tag)");
        return false;
    }

    QDomNode portsType = DomFunctions::obtainChild(node, type, &mDoc);

    QDomNodeList ports = portsType.toElement().elementsByTagName("port");
    return ports.length() != 0;
}

bool
vleVpz::hasModelInputPort(QDomNode node)
{
    return hasModelPort(node, "in");
}

bool
vleVpz::hasModelOutputPort(QDomNode node)
{
    return hasModelPort(node, "out");
}

bool
vleVpz::existConnection(QDomNode connections,
                        QString modelOrig,
                        QString portOrig,
                        QString modelDest,
                        QString portDest,
                        QString connType)
{
    if (connections.nodeName() != "connections") {
        qDebug() << ("Internal error in existConnection (wrong main tag)");
        return false;
    }
    QDomNodeList currConns = connections.childNodes();
    for (int i = 0; i < currConns.length(); i++) {
        QDomNode currCon = currConns.at(i);
        if (currCon.nodeName() == "connection" and
            DomFunctions::attributeValue(currCon, "type") == connType) {
            QDomNode currOrig =
              currCon.toElement().elementsByTagName("origin").at(0);
            QDomNode currDest =
              currCon.toElement().elementsByTagName("destination").at(0);
            if ((DomFunctions::attributeValue(currOrig, "model") ==
                 modelOrig) and
                (DomFunctions::attributeValue(currDest, "model") ==
                 modelDest) and
                (DomFunctions::attributeValue(currOrig, "port") ==
                 portOrig) and
                (DomFunctions::attributeValue(currDest, "port") == portDest)) {
                return true;
            }
        }
    }
    return false;
}

bool
vleVpz::isConnectionAssociatedTo(QDomNode connection,
                                 const QList<QString>& submodel_names) const
{
    if (connection.nodeName() != "connection") {
        qDebug()
          << ("Internal error in isConnectionAssociatedTo (wrong main tag)");
        return false;
    }

    QDomNode orig = connection.toElement().elementsByTagName("origin").at(0);
    QDomNode dest =
      connection.toElement().elementsByTagName("destination").at(0);
    QString model_a = DomFunctions::attributeValue(orig, "model");
    QString model_b = DomFunctions::attributeValue(dest, "model");
    QString conn_type = DomFunctions::attributeValue(connection, "type");
    if (conn_type == "internal" and (submodel_names.contains(model_a) or
                                     submodel_names.contains(model_b))) {
        return true;
    } else if (conn_type == "output" and submodel_names.contains(model_a)) {
        return true;
    } else if (conn_type == "input" and submodel_names.contains(model_b)) {
        return true;
    }
    return false;
}

void
vleVpz::addModelConnection(QDomNode node1, QDomNode node2)
{
    if (node1.nodeName() != "port") {
        qDebug() << ("Internal error in addModelConnection (wrong main tag)");
        return;
    }
    if (node2.nodeName() != "port") {
        qDebug() << ("Internal error in addModelConnection (wrong main tag)");
        return;
    }
    QString node1type = node1.parentNode().nodeName();
    QString node2type = node2.parentNode().nodeName();
    QDomNode model1 = node1.parentNode().parentNode();
    QDomNode model2 = node2.parentNode().parentNode();
    QString model1name = DomFunctions::attributeValue(model1, "name");
    QString model2name = DomFunctions::attributeValue(model2, "name");
    QString port1name = DomFunctions::attributeValue(node1, "name");
    QString port2name = DomFunctions::attributeValue(node2, "name");
    QDomNode coupled1;
    QDomNode coupled2;
    if (model1.parentNode().nodeName() == "submodels") {
        coupled1 = model1.parentNode().parentNode();
    }
    if (model2.parentNode().nodeName() == "submodels") {
        coupled2 = model2.parentNode().parentNode();
    }
    if ((coupled1 == coupled2) and (node1type == "out") and
        (node2type == "in")) {
        QDomNode cons = connectionsFromModel(coupled1);
        if (cons.isNull()) {
            undoStack->snapshot(coupled1);
            synchronizeUndoStack();
            QDomElement c = getDomDoc().createElement("connections");
            coupled1.appendChild(c);
            cons = connectionsFromModel(coupled1);
        } else {
            if (existConnection(cons,
                                model1name,
                                port1name,
                                model2name,
                                port2name,
                                "internal")) {
                return;
            }
            undoStack->snapshot(cons);
            synchronizeUndoStack();
        }
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "internal");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model", model1name);
        orig.setAttribute("port", port1name);
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model", model2name);
        dest.setAttribute("port", port2name);
        con.appendChild(dest);
        cons.appendChild(con);
    } else if ((model1 == coupled2) and (node1type == "in") and
               (node2type == "in")) {
        QDomNode cons = connectionsFromModel(coupled2);
        if (cons.isNull()) {
            undoStack->snapshot(coupled2);
            synchronizeUndoStack();
            QDomElement c = getDomDoc().createElement("connections");
            coupled2.appendChild(c);
            cons = connectionsFromModel(coupled2);
        } else {
            if (existConnection(cons,
                                model1name,
                                port1name,
                                model2name,
                                port2name,
                                "input")) {
                return;
            }
            undoStack->snapshot(cons);
            synchronizeUndoStack();
        }
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "input");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model", model1name);
        orig.setAttribute("port", port1name);
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model", model2name);
        dest.setAttribute("port", port2name);
        con.appendChild(dest);
        cons.appendChild(con);
    } else if ((coupled1 == model2) and (node1type == "out") and
               (node2type == "out")) {
        QDomNode cons = connectionsFromModel(coupled1);
        if (cons.isNull()) {
            undoStack->snapshot(coupled1);
            synchronizeUndoStack();
            QDomElement c = getDomDoc().createElement("connections");
            coupled1.appendChild(c);
            cons = connectionsFromModel(coupled1);
        } else {
            if (existConnection(cons,
                                model1name,
                                port1name,
                                model2name,
                                port2name,
                                "output")) {
                return;
            }
            undoStack->snapshot(cons);
            synchronizeUndoStack();
        }
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "output");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model", model1name);
        orig.setAttribute("port", port1name);
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model", model2name);
        dest.setAttribute("port", port2name);
        con.appendChild(dest);
        cons.appendChild(con);
    }
    emit modelsUpdated();
}

void
vleVpz::rmModelConnection(QDomNode node, bool undo)
{
    if (node.nodeName() != "connection") {
        qDebug() << ("Internal error in rmModelConnection (wrong main tag)");
        return;
    }
    QDomNode parent = node.parentNode();
    if (undo) {
        undoStack->snapshot(parent);
        synchronizeUndoStack();
    }
    parent.removeChild(node);
    removeTextChilds(parent);
}

void
vleVpz::setWidthToModel(QDomNode node, int width)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setWidthToModel");
        return;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setWidthToModel", forUndoMerge);
        synchronizeUndoStack();
    }
    DomFunctions::setAttributeValue(node, "width", QVariant(width).toString());
}

void
vleVpz::setHeightToModel(QDomNode node, int height)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setHeightToModel");
        return;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setHeightToModel", forUndoMerge);
        synchronizeUndoStack();
    }
    DomFunctions::setAttributeValue(
      node, "height", QVariant(height).toString());
}

void
vleVpz::setPositionToModel(QDomNode node, int x, int y)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setPositionToModel");
        return;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        vle::value::Set& qs = forUndoMerge->addSet("queries");
        qs.addString(mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setPositionToModel", forUndoMerge);
        synchronizeUndoStack();
    }
    DomFunctions::setAttributeValue(node, "x", QVariant(x).toString());
    DomFunctions::setAttributeValue(node, "y", QVariant(y).toString());
}
void
vleVpz::setPositionToModel(QList<QDomNode>& nodes,
                           QList<int> xs,
                           QList<int> ys)
{
    int size = nodes.size();
    if (size > 0) {
        if ((size != xs.size()) or (size != ys.size())) {
            qDebug() << ("Internal error setPositionToModel (wrong size)");
            return;
        }
        if (nodes[0].nodeName() != "model") {
            qDebug() << ("Internal error setPositionToModel (main tag)");
            return;
        }
        QString nodeLocation = mVdo->getXQuery(nodes[0]);
        nodeLocation = subQuery(nodeLocation, 0, -1);
        for (int i = 1; i < size; i++) {
            QDomNode nodei = nodes[i];
            QString nodeLoci = mVdo->getXQuery(nodei);
            nodeLoci = subQuery(nodeLoci, 0, -1);
            if (nodeLocation != nodeLoci) {
                qDebug() << ("Internal error setPositionToModel (location)");
                return;
            }
        }
        { // Undo command building
            vle::value::Map* forUndoMerge = new vle::value::Map();
            vle::value::Set& qs = forUndoMerge->addSet("queries");
            for (int i = 0; i < size; i++) {
                QDomNode nodei = nodes[i];
                qs.addString(mVdo->getXQuery(nodei).toStdString());
            }
            undoStack->snapshot(
              nodes[0].parentNode(), "setPositionToModel", forUndoMerge);
            synchronizeUndoStack();
        }

        for (int i = 0; i < size; i++) {
            QDomNode nodei = nodes[i];
            DomFunctions::setAttributeValue(
              nodei, "x", QVariant(xs[i]).toString());
            DomFunctions::setAttributeValue(
              nodei, "y", QVariant(ys[i]).toString());
        }
    }
}

QDomNode
vleVpz::addView(QDomNode node, const QString& viewName)
{
    if (node.isNull()) {
        qDebug() << ("Internal error in addView (null conds)");
        return QDomNode();
    }
    if (existViewFromDoc(viewName)) {
        qDebug() << ("Internal error in addView (already here)");
        return QDomNode();
    }
    QDomElement elem = getDomDoc().createElement("view");
    elem.setAttribute("name", viewName);
    // add output and configure it with vle.output/storage
    elem.setAttribute("output", viewName);
    elem.setAttribute("type", "timed");
    elem.setAttribute("timestep", "1");
    QDomNode outputsNode = DomFunctions::obtainChild(
      DomFunctions::obtainChild(
        DomFunctions::obtainChild(
          DomFunctions::obtainChild(mDoc, "vle_project", &mDoc),
          "experiment",
          &mDoc),
        "views",
        &mDoc),
      "outputs",
      &mDoc);
    QDomElement elemOut = getDomDoc().createElement("output");
    elemOut.setAttribute("name", viewName);
    elemOut.setAttribute("location", "");
    elemOut.setAttribute("format", "local");
    elemOut.setAttribute("plugin", "file");
    elemOut.setAttribute("package", "vle.output");
    outputsNode.appendChild(elemOut);
    node.appendChild(elem);
    return elem;
}

QDomNode
vleVpz::addCondition(QDomNode node, const QString& condName)
{
    if (node.isNull()) {
        qDebug() << ("Internal error in addCondition (null conds)");
        return QDomNode();
    }
    QDomNodeList conds = condsListFromConds(node);
    for (int i = 0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == condName)) {
            qDebug() << ("Internal error in addCondition (already here)");
            return QDomNode();
        }
    }
    QDomElement elem = getDomDoc().createElement("condition");
    elem.setAttribute("name", condName);
    node.appendChild(elem);
    return elem;
}

QString
vleVpz::modelCondsFromDoc(const QString& mod_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(mod_query);
    return DomFunctions::attributeValue(atom, "conditions");
}

bool
vleVpz::isAttachedCond(const QString& model_query, const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    QSet<QString> attachedConds = vleDomStatic::attachedCondsToAtomic(atom);
    return attachedConds.contains(condName);
}

QSet<QString>
vleVpz::sharedAttachedConds(const QString& atom_query) const
{
    QDomNode selAtom = mVdo->getNodeFromXQuery(atom_query);
    return sharedAttachedConds(selAtom);
}

QSet<QString>
vleVpz::sharedAttachedConds(const QDomNode& atom) const
{
    QSet<QString> sharedConds;
    QSet<QString> condsAtom = vleDomStatic::attachedCondsToAtomic(atom);
    if (condsAtom.count() == 0) {
        return sharedConds;
    }
    QList<QDomNode> atoms = listOfAtomicFromDoc();
    for (int i = 0; i < atoms.length(); i++) {
        QDomNode other = atoms.at(i);
        if (DomFunctions::attributeValue(other, "name") !=
            DomFunctions::attributeValue(atom, "name")) {
            QSet<QString> condsOther =
              vleDomStatic::attachedCondsToAtomic(other);
            sharedConds += condsOther.intersect(condsAtom);
        }
    }
    return sharedConds;
}

QString
vleVpz::sharedDynamic(const QString& atom_query) const
{
    QDomNode selAtom = mVdo->getNodeFromXQuery(atom_query);
    return sharedDynamic(selAtom);
}
QString
vleVpz::sharedDynamic(const QDomNode& atom) const
{
    QString dyn = DomFunctions::attributeValue(atom, "dynamics");
    if (dyn == "") {
        return "";
    }
    QList<QDomNode> atoms = listOfAtomicFromDoc();
    for (int i = 0; i < atoms.length(); i++) {
        QDomNode other = atoms.at(i);
        if (DomFunctions::attributeValue(other, "name") !=
            DomFunctions::attributeValue(atom, "name")) {
            QString dynOther = DomFunctions::attributeValue(other, "dynamics");
            if (dyn == dynOther) {
                return dyn;
            }
        }
    }
    return "";
}

QString
vleVpz::sharedObservable(const QString& atom_query) const
{
    QDomNode selAtom = mVdo->getNodeFromXQuery(atom_query);
    return sharedObservable(selAtom);
}
QString
vleVpz::sharedObservable(const QDomNode& atom) const
{
    QString obs = DomFunctions::attributeValue(atom, "observables");
    if (obs == "") {
        return "";
    }
    QList<QDomNode> atoms = listOfAtomicFromDoc();
    for (int i = 0; i < atoms.length(); i++) {
        QDomNode other = atoms.at(i);
        if (DomFunctions::attributeValue(other, "name") !=
            DomFunctions::attributeValue(atom, "name")) {
            QString obsOther =
              DomFunctions::attributeValue(other, "observables");
            if (obs == obsOther) {
                return obs;
            }
        }
    }
    return "";
}

QString
vleVpz::getAtomicModelConds(const QDomNode atom)
{
    return DomFunctions::attributeValue(atom, "conditions");
}

bool
vleVpz::isAtomicModelCondsSet(const QDomNode atom)
{
    return DomFunctions::attributeValue(atom, "conditions") != "";
}

QString
vleVpz::modelDynFromDoc(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    return DomFunctions::attributeValue(atom, "dynamics");
}

QString
vleVpz::modelObsFromDoc(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    return DomFunctions::attributeValue(atom, "observables");
}

bool
vleVpz::isAtomicModelObsSet(QDomNode atom)
{
    return vleDomStatic::attachedObsToAtomic(atom) != "";
}

void
vleVpz::setDynToAtomicModel(const QString& model_query,
                            const QString& dyn,
                            bool undo)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    if (undo) {
        undoStack->snapshot(atom);
        synchronizeUndoStack();
    }
    DomFunctions::setAttributeValue(atom, "dynamics", dyn);
}

QString
vleVpz::getAtomicModelDyn(QDomNode atom)
{
    return DomFunctions::attributeValue(atom, "dynamics");
}

bool
vleVpz::isAtomicModelDynSet(QDomNode atom)
{
    return getAtomicModelDyn(atom) != "";
}

void
vleVpz::removeDyn(const QString& dyn)
{
    if (not existDynamicFromDoc(dyn)) {
        return;
    }

    bool bigSnapshot = false;

    // detach dynamic to models
    QDomNodeList mods = getDomDoc().elementsByTagName("model");
    for (int i = 0; i < mods.size(); i++) {
        QDomNode mod = mods.at(i);
        if ((DomFunctions::attributeValue(mod, "type") == "atomic") and
            (DomFunctions::attributeValue(mod, "dynamics") == dyn)) {
            if (not bigSnapshot) {
                undoStack->snapshot(vleProjectFromDoc());
                synchronizeUndoStack();
                bigSnapshot = true;
            }
            DomFunctions::setAttributeValue(mod, "dynamics", "");
        }
    }

    // remove dynamic
    QDomNode dyns = dynamicsFromDoc();
    if (not bigSnapshot) {
        undoStack->snapshot(dyns);
        synchronizeUndoStack();
    }
    dyns.removeChild(dynamicFromDoc(dyn));

    emit dynamicsUpdated();
}

void
vleVpz::attachCondToAtomicModel(const QString& model_query,
                                const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    QSet<QString> attachedConds = vleDomStatic::attachedCondsToAtomic(atom);
    if (not attachedConds.contains(condName)) {
        undoStack->snapshot(atom);
        synchronizeUndoStack();
        attachedConds.insert(condName);
        vleDomStatic::attachCondsToAtomic(atom, attachedConds);
        emit conditionsUpdated();
    }
}

void
vleVpz::detachCondToAtomicModel(const QString& model_query,
                                const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    undoStack->snapshot(atom);
    synchronizeUndoStack();
    QSet<QString> attachedConds = vleDomStatic::attachedCondsToAtomic(atom);
    if (attachedConds.contains(condName)) {
        attachedConds.remove(condName);
        vleDomStatic::attachCondsToAtomic(atom, attachedConds);
        emit conditionsUpdated();
    }
}

void
vleVpz::setObsToAtomicModel(const QString& model_query, const QString& obsName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    undoStack->snapshot(atom);
    synchronizeUndoStack();

    DomFunctions::setAttributeValue(atom, "observables", obsName);

    emit observablesUpdated();
}

void
vleVpz::unsetObsFromAtomicModel(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    undoStack->snapshot(atom);
    synchronizeUndoStack();
    DomFunctions::setAttributeValue(atom, "observables", "");

    emit observablesUpdated();
}

QDomNode
vleVpz::outputFromOutputs(QDomNode node, const QString& outputName)
{
    if (node.nodeName() != "outputs") {
        qDebug() << ("Internal error in outputFromOutputs (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "output") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == outputName)) {
            return child;
        }
    }
    qDebug() << ("Internal error in outputFromOutputs (output not found)");
    return QDomNode();
}

std::unique_ptr<vle::value::Map>
vleVpz::buildOutputConfigMap(const QString& outputName)
{
    QDomNode outputNode =
      outputFromOutputs(outputsFromViews(viewsFromDoc()), outputName);
    QList<QDomNode> nodes =
      DomFunctions::childNodesWithoutText(outputNode, "map");
    if (nodes.size() == 1) {
        std::unique_ptr<value::Value> val =
          vleDomStatic::buildValue(nodes.at(0), false);
        if (val->isMap()) {
            value::Value* m = val.release();
            return std::unique_ptr<value::Map>(static_cast<value::Map*>(m));
        } else {
            qDebug() << ("Internal error in buildOutputConfigMap (not a map)");
        }
    }
    return std::unique_ptr<value::Map>(new value::Map());
}

void
vleVpz::fillOutputConfigMap(const QString& outputName,
                            const vle::value::Map& mapConfig)
{
    undoStack->snapshot(viewsFromDoc());
    synchronizeUndoStack();

    QDomNode outputNode =
      outputFromOutputs(outputsFromViews(viewsFromDoc()), outputName);
    QDomNode nodeMap = DomFunctions::obtainChild(outputNode, "map", &mDoc);
    fillWithValue(nodeMap, mapConfig);
}

void
vleVpz::viewObservableNames(std::vector<std::string>& v) const
{
    v.clear();
    QDomNode outputs = viewsFromDoc();
    QDomNodeList outputList =
      outputs.toElement().elementsByTagName("observable");
    for (int i = 0; i < outputList.length(); i++) {
        QDomNode output = outputList.item(i);
        QString o = output.attributes().namedItem("name").nodeValue();
        v.push_back(o.toStdString());
    }
}

void
vleVpz::viewOutputNames(std::vector<std::string>& v) const
{
    v.clear();
    QDomNode outputs = viewsFromDoc();
    QDomNodeList outputList = outputs.toElement().elementsByTagName("output");
    for (int i = 0; i < outputList.length(); i++) {
        QDomNode output = outputList.item(i);
        QString o = output.attributes().namedItem("name").nodeValue();
        v.push_back(o.toStdString());
    }
}

void
vleVpz::viewNames(std::vector<std::string>& v) const
{
    v.clear();
    QDomNode outputs = viewsFromDoc();
    QDomNodeList outputList = outputs.toElement().elementsByTagName("view");
    for (int i = 0; i < outputList.length(); i++) {
        QDomNode output = outputList.item(i);
        QString o = output.attributes().namedItem("name").nodeValue();
        v.push_back(o.toStdString());
    }
}

QDomNode
vleVpz::viewFromViews(QDomNode node, const QString& viewName) const
{
    if (node.nodeName() != "views") {
        qDebug() << ("Internal error in viewFromViews (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "view") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)) {
            return child;
        }
    }
    qDebug() << ("Internal error in viewFromViews (view not found)");
    return QDomNode();
}

QDomNode
vleVpz::viewFromDoc(const QString& viewName) const
{
    return viewFromViews(viewsFromDoc(), viewName);
}

QStringList
vleVpz::getViewTypeFromDoc(const QString& viewName) const
{
    QDomNode node = viewFromDoc(viewName);
    return vleDomStatic::getViewTypeToView(node);
}

bool
vleVpz::addViewTypeToDoc(const QString& viewName, const QString& viewType)
{
    QDomNode node = viewFromDoc(viewName);
    QStringList viewTypes = vleDomStatic::getViewTypeToView(node);
    if (viewType == "timed") {
        if (viewTypes.contains("timed") and viewTypes.length() == 1) {
            return false;
        } else {
            undoStack->snapshot(node);
            synchronizeUndoStack();
            DomFunctions::setAttributeValue(node, "type", "timed");
            DomFunctions::setAttributeValue(node, "timestep", "1.0");
            return true;
        }
    } else {
        if (not viewTypes.contains("timed") and viewTypes.contains(viewType)) {
            return false;
        } else {
            viewTypes.append(viewType);
            viewTypes.removeAll("timed");
            undoStack->snapshot(viewsFromDoc());
            synchronizeUndoStack();
            node.toElement().removeAttribute("timestep");
            DomFunctions::setAttributeValue(node, "type", viewTypes.join(","));
            return true;
        }
    }
}

bool
vleVpz::rmViewTypeToDoc(const QString& viewName, const QString& viewType)
{
    QDomNode node = viewFromDoc(viewName);
    QStringList viewTypes = vleDomStatic::getViewTypeToView(node);
    if (viewTypes.size() < 2) {
        // cannot remove the only viewType
        return false;
    }
    if (not viewTypes.contains(viewType)) {
        return false;
    }
    viewTypes.removeAll(viewType);
    undoStack->snapshot(viewsFromDoc());
    synchronizeUndoStack();
    DomFunctions::setAttributeValue(node, "type", viewTypes.join(","));
    return true;
}

double
vleVpz::timeStepFromDoc(const QString& viewName) const
{
    QDomNode viewNode = viewFromDoc(viewName);
    if (viewNode.nodeName() != "view") {
        qDebug() << ("Internal error in timeStepFromDoc (wrong main tag)");
        return -1.0;
    }
    QVariant qv(viewNode.attributes().namedItem("timestep").nodeValue());
    return qv.toDouble();
}

bool
vleVpz::setTimeStepToDoc(const QString& viewName, double ts)
{
    QDomNode node = viewFromDoc(viewName);

    if (node.nodeName() != "view") {
        qDebug() << ("Internal error in setTimeStepFromView (wrong main tag)");
        return false;
    }
    if (getViewTypeFromDoc(viewName).contains("timed") and ts > 0.0) {
        undoStack->snapshot(viewsFromDoc());
        synchronizeUndoStack();
        DomFunctions::setAttributeValue(
          node, "timestep", QVariant(ts).toString());
        return true;
    }
    return false;
}

bool
vleVpz::setOutputPluginToDoc(const QString& viewName,
                             const QString& outputPlugin)
{
    int prevCurr = undoStack->curr;
    QDomNode viewsNode = viewsFromDoc();

    undoStack->snapshot(viewsNode);

    QDomNode outputsNode = outputsFromViews(viewsNode);
    QDomNode outputNode = outputFromOutputs(outputsNode, viewName);
    QDomNode pkg = outputNode.attributes().namedItem("package");
    QDomNode plugin = outputNode.attributes().namedItem("plugin");
    QStringList ll = outputPlugin.split("/");
    pkg.setNodeValue(ll.at(0));
    plugin.setNodeValue(ll.at(1));

    if (hasMeta) {
        // TODO, ne devrait pas etre en dur
        QString guiPluginName("");
        if (outputPlugin == "vle.output/storage") {
            guiPluginName = "gvle.output/storage";
        }
        if (outputPlugin == "vle.output/file") {
            guiPluginName = "gvle.output/file";
        }
        // update vpm with snapshot but without signal
        setOutputGUIplugin(viewName, guiPluginName);

        // update in vpz with snapshot but without signal
        bool oldEn = undoStack->enableSnapshot(false);
        provideOutputGUIplugin(viewName);
        undoStack->enableSnapshot(oldEn);
    }
    synchronizeUndoStack();
    tryEmitUndoAvailability(prevCurr, undoStack->curr, undoStack->saved);
    return true;
}

QString
vleVpz::getOutputPlugin(QDomNode node)
{
    if (node.nodeName() != "output") {
        QString mess = "Internal error in getOutputPlugin (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return QString();
    }
    QString plug = node.attributes().namedItem("package").nodeValue();
    plug += "/";
    plug += node.attributes().namedItem("plugin").nodeValue();
    return plug;
}

// enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
//    XMLTYPE, NIL, MATRIX, USER };

vle::value::Value::type
vleVpz::valueType(const QString& condName,
                  const QString& portName,
                  int index) const
{
    //    BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
    //                XMLTYPE, NIL, MATRIX, USER
    QDomNode port = portFromDoc(condName, portName);
    QDomNode valNode = port.childNodes().at(index);
    if (valNode.nodeName() == "boolean") {
        return vle::value::Value::BOOLEAN;
    } else if (valNode.nodeName() == "integer") {
        return vle::value::Value::INTEGER;
    } else if (valNode.nodeName() == "double") {
        return vle::value::Value::DOUBLE;
    } else if (valNode.nodeName() == "string") {
        return vle::value::Value::STRING;
    } else if (valNode.nodeName() == "set") {
        return vle::value::Value::SET;
    } else if (valNode.nodeName() == "map") {
        return vle::value::Value::MAP;
    } else if (valNode.nodeName() == "tuple") {
        return vle::value::Value::TUPLE;
    } else if (valNode.nodeName() == "table") {
        return vle::value::Value::TABLE;
    } else if (valNode.nodeName() == "xml") {
        return vle::value::Value::XMLTYPE;
    } else if (valNode.nodeName() == "null") {
        return vle::value::Value::NIL;
    } else if (valNode.nodeName() == "matrix") {
        return vle::value::Value::MATRIX;
    }
    return vle::value::Value::USER;
}

std::unique_ptr<vle::value::Value>
vleVpz::buildValueFromDoc(const QString& condName,
                          const QString& portName,
                          int valIndex) const
{
    return vleDomStatic::getValueFromPortCond(
      condFromConds(condsFromDoc(), condName), portName, valIndex);
}

unsigned int
vleVpz::nbValuesInPortFromDoc(const QString& condName, const QString& portName)
{
    QDomNode port = portFromDoc(condName, portName);
    return DomFunctions::childNodesWithoutText(port).size();
}

bool
vleVpz::fillWithMultipleValueFromDoc(
  const QString& condName,
  const QString& portName,
  std::vector<std::unique_ptr<value::Value>>& values) const
{
    QDomNode port = portFromDoc(condName, portName);
    return fillWithMultipleValue(port, values);
}

bool
vleVpz::fillWithMultipleValue(
  QDomNode portNode,
  std::vector<std::unique_ptr<value::Value>>& values) const
{
    values.clear();
    QDomNodeList valueList = portNode.childNodes();
    std::unique_ptr<value::Value> val;
    for (int k = 0; k < valueList.length(); k++) {
        val = vleDomStatic::buildValue(valueList.at(k), false);
        if (val) {
            values.push_back(std::move(val));
        }
    }
    return true;
}

void
vleVpz::fillWithClassesFromDoc(std::vector<std::string>& toFill)
{
    toFill.clear();
    QDomNode classes = classesFromDoc();
    QList<QDomNode> classNodes = DomFunctions::childNodesWithoutText(classes);
    for (int i = 0; i < classNodes.size(); i++) {
        toFill.push_back(
          DomFunctions::attributeValue(classNodes[i], "name").toStdString());
    }
}

bool
vleVpz::fillWithValue(const QString& condName,
                      const QString& portName,
                      int index,
                      const vle::value::Value& val)
{
    QDomNode port = portFromDoc(condName, portName);
    undoStack->snapshot(port);
    synchronizeUndoStack();
    QList<QDomNode> chs = DomFunctions::childNodesWithoutText(port);
    QDomNode valNode = chs[index];
    if (val.getType() != valueType(condName, portName, index)) {
        QDomNode parent = valNode.parentNode();
        parent.replaceChild(buildEmptyValueFromDoc(val.getType()), valNode);
        chs = DomFunctions::childNodesWithoutText(port);
        valNode = chs[index];
    }
    fillWithValue(valNode, val);
    emit conditionsUpdated();
    return true;
}

bool
vleVpz::fillWithValue(QDomNode node, const vle::value::Value& val)
{
    return vleDomStatic::fillWithValue(getDomDoc(), node, val);
}

bool
vleVpz::isInteger(QDomNode node)
{
    return node.nodeName() == "integer";
}

int
vleVpz::getInteger(QDomNode node)
{
    if (node.nodeName() != "integer") {
        QString mess = "Internal error in getInteger (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return -1;
    }
    QVariant qv = node.nodeValue();
    return qv.toInt();
}

int
vleVpz::getIntegerFromMap(QDomNode node, QString key)
{
    if (node.nodeName() != "map") {
        QString mess = "Internal error in getIntegerFromMap (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return -1;
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == key)) {
            if (child.childNodes().length() > 0) {
                return getInteger(child.childNodes().item(0));
            }
        }
    }
    qDebug() << ("Internal error in getIntegerFromMap (key not found)");
    return -1;
}

bool
vleVpz::existIntegerKeyInMap(QDomNode node, QString key)
{
    if (node.nodeName() != "map") {
        QString mess =
          "Internal error in existIntegerKeyInMap (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return -1;
    }
    QDomNodeList childs = node.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == key)) {
            QDomNodeList childsKey = child.childNodes();
            return (childsKey.length() > 0 and isInteger(childsKey.item(0)));
        }
    }
    return false;
}

void
vleVpz::addIntegerKeyInMap(QDomNode* node, const QString& key, int val)
{
    if (node->nodeName() != "map") {
        QString mess = "Internal error in addIntegerKeyInMap (wrong main tag)";
        mess += "\n got : ";
        mess += node->nodeName();
        qDebug() << (mess);
        return;
    }
    QString qv = QVariant(val).toString();

    // update key if present
    QDomNodeList childs = node->childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == key)) {
            QDomElement newInt = child.toDocument().createElement("integer");
            QDomText newIntVal = newInt.toDocument().createTextNode(qv);
            newInt.appendChild(newIntVal);
            child.toElement().appendChild(newInt);
            return;
        }
    }
    // add key if required
    QDomElement newKey = node->toDocument().createElement("key");
    newKey.setTagName("key");
    newKey.setAttribute("name", key);
    QDomElement newInt = newKey.toDocument().createElement("integer");
    QDomText newIntVal = newInt.toDocument().createTextNode(qv);
    newInt.appendChild(newIntVal);
    newKey.appendChild(newInt);
    node->appendChild(newKey);
}

QDomNode
vleVpz::getOutputFromViews(QDomNode node, const QString& outputName)
{
    QDomNode outputs = outputsFromViews(node);
    QDomNode output = outputFromOutputs(outputs, outputName);
    return output;
}

QString
vleVpz::getOutputPluginFromDoc(const QString& outputName)
{
    QDomNode output = getOutputFromViews(viewsFromDoc(), outputName);
    return getOutputPlugin(output);
}

void
vleVpz::fillWithDynamicsList(QList<QString>& toFill) const
{
    toFill.clear();
    QDomNodeList nodeList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynamics = nodeList.item(0).toElement();
    QDomNodeList childs = dynamics.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
            (child.attributes().contains("name"))) {
            toFill.append(child.attributes().namedItem("name").nodeValue());
        }
    }
}

bool
vleVpz::existDynamicIntoDoc(const QString& dyn) const
{
    QDomNodeList dynList = dynListFromDoc();
    return existDynamicIntoDynList(dyn, dynList);
}

bool
vleVpz::existDynamicIntoDynList(const QString& dyn,
                                const QDomNodeList& dynList) const
{
    for (int i = 0; i < dynList.length(); i++) {
        QDomElement child = dynList.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == dyn)) {
            return true;
        }
    }
    return false;
}

QString
vleVpz::getDynamicPackage(const QString& dyn) const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynamics = nodeList.item(0).toElement();
    QDomNodeList childs = dynamics.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
            (child.attributes().contains("package")) and
            (child.attributes().namedItem("name").nodeValue() == dyn)) {
            return child.attributes().namedItem("package").nodeValue();
        }
    }
    return "";
}

QString
vleVpz::getDynamicLibrary(const QString& dyn) const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynamics = nodeList.item(0).toElement();
    QDomNodeList childs = dynamics.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
            (child.attributes().contains("library")) and
            (child.attributes().namedItem("name").nodeValue() == dyn)) {
            return child.attributes().namedItem("library").nodeValue();
        }
    }
    return "";
}

void
vleVpz::importFromClipboard(QDomNode node_dest)
{

    QString vpzFile = QDir::tempPath() + "/clipboard/exp/clipboard.vpz";
    QString vpmFile =
      QDir::tempPath() + "/clipboard/metadata/exp/clipboard.vpm";
    vleVpz src(vpzFile, vpmFile, mGvlePlugins);

    // get clipboard
    QStringList queries;
    bool track = false;
    {
        QClipboard* clipboard = QApplication::clipboard();
        QString clip_content = clipboard->text();
        QStringList clip_content_list = clip_content.split(";;");
        for (int i = 0; i < clip_content_list.length(); i++) {
            if (i < clip_content_list.length() - 1) {
                queries.append(clip_content_list.at(i).split(":=").at(1));
            } else {
                QString track_str = clip_content_list.at(i);
                track_str.remove("track=");
                if (track_str == "yes") {
                    track = true;
                } else {
                    track = false;
                }
            }
        }
    }
    QString src_mod_query = "";
    QSet<QString> elems = modifyImportSourceForImport(
      node_dest, queries, track, src, src_mod_query);
    if (elems.empty()) {
        return;
    }

    bool snapshot_all = false;

    // localize snapshot
    for (QSet<QString>::iterator i = elems.begin(); i != elems.end(); i++) {
        QStringList elem = i->split("::");
        if ((elem.at(0) == "model") and track) {
            snapshot_all = true;
        }
    }

    if (snapshot_all) {
        undoStack->snapshot(vleProjectFromDoc());
    } else {
        undoStack->snapshot(node_dest);
    }
    synchronizeUndoStack();

    bool emit_cond = false;
    bool emit_dyn = false;
    bool emit_obs = false;
    bool emit_mod = false;

    // import conditions, condition port, dynamics and observables
    for (QSet<QString>::iterator i = elems.begin(); i != elems.end(); i++) {
        QStringList elem = i->split("::");
        if (elem.at(0) == "cond") {
            condsFromDoc().appendChild(
              src.condFromConds(src.condsFromDoc(), elem.at(1)));
            emit_cond = true;
        } else if (elem.at(0) == "dyn") {
            dynamicsFromDoc().appendChild(src.dynamicFromDoc(elem.at(1)));
            emit_dyn = true;
        } else if (elem.at(0) == "obs") {
            obsFromDoc().appendChild(
              src.obsFromObss(src.obsFromDoc(), elem.at(1)));
            emit_obs = true;
        } else if (elem.at(0) == "cond_port") {
            QString src_cond = elem.at(1).split("//").at(0);
            QString dest_cond = elem.at(1).split("//").at(1);
            QString src_cond_port = elem.at(1).split("//").at(2);
            QDomNode dest_cond_node = condFromConds(condsFromDoc(), dest_cond);
            QDomNode src_cond_node =
              src.condFromConds(src.condsFromDoc(), src_cond);
            QDomNode src_port_node =
              src.portFromCond(src_cond_node, src_cond_port);

            dest_cond_node.appendChild(src_port_node);
            emit_cond = true;
        }
    }

    if (src_mod_query != "") {
        // import models
        QDomNode dest_submodels =
          DomFunctions::obtainChild(node_dest, "submodels", &mDoc);
        QDomNode src_mod = src.vdo()->getNodeFromXQuery(src_mod_query);
        QSet<QString> mods_to_import;
        for (QSet<QString>::iterator i = elems.begin(); i != elems.end();
             i++) {
            QStringList elem = i->split("::");
            if (elem.at(0) == "model") {
                QDomNode model_node =
                  vleDomStatic::subModel(src_mod, elem.at(1));
                dest_submodels.appendChild(model_node);
                mods_to_import.insert(elem.at(1));
                emit_mod = true;
            }
        }
        // import connections
        QDomNode dest_connections =
          DomFunctions::obtainChild(node_dest, "connections", &mDoc);
        QDomNode src_connections =
          DomFunctions::obtainChild(src_mod, "connections");
        QList<QDomNode> cons =
          DomFunctions::childNodesWithoutText(src_connections, "connection");
        for (int i = 0; i < cons.size(); i++) {
            QDomNode con = cons.at(i);
            if (DomFunctions::attributeValue(con, "type") == "internal") {
                if (mods_to_import.contains(
                      vleDomStatic::connectionModOrig(con)) and
                    mods_to_import.contains(
                      vleDomStatic::connectionModDest(con))) {
                    dest_connections.appendChild(con);
                }
            }
        }
    }
    if (emit_cond) {
        emit conditionsUpdated();
    }
    if (emit_dyn) {
        emit dynamicsUpdated();
    }
    if (emit_obs) {
        emit observablesUpdated();
    }
    if (emit_mod) {
        emit modelsUpdated();
    }
}
void
vleVpz::exportToClipboard(QDomNode node_source)
{
    // save vpz and vpm files
    saveTemp("clipboard", "clipboard");
    // fill clipboard
    QClipboard* clipboard = QApplication::clipboard();
    QString query = vdo()->getXQuery(node_source);
    QString clip_content = "query:=" + query + ";;" + "track=yes";
    clipboard->setText(clip_content);
}

void
vleVpz::exportToClipboard(QList<QDomNode> nodes_source, bool track)
{
    // save vpz and vpm files
    saveTemp("clipboard", "clipboard");
    // fill clipboard
    QString clip_content;
    QClipboard* clipboard = QApplication::clipboard();
    for (int i = 0; i < nodes_source.length(); i++) {
        clip_content +=
          "query:=" + vdo()->getXQuery(nodes_source.at(i)) + ";;";
    }
    clip_content += "track=";
    if (track) {
        clip_content += "yes";
    } else {
        clip_content += "no";
    }
    clipboard->setText(clip_content);
}

bool
vleVpz::isAltered()
{
    return false;
}

void
vleVpz::removeDynamic(const QString& dynamic)
{
    removeDyn(dynamic);
}

void
vleVpz::save()
{
    saveVpz(mFilename);
    undoStack->registerSaveState();
    if (hasMeta) {
        saveVpm(mFileNameVpm);
        undoStackVpm->registerSaveState();
    }
}

void
vleVpz::saveVpz(QString filenameVpz)
{
    QFile file(filenameVpz);
    if (not file.exists()) {
        if (not file.open(QIODevice::WriteOnly)) {
            qDebug() << "VPZ File (" << filenameVpz
                     << ") can't be opened for write !";
            return;
        }
        file.close();
    }

    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = mDoc.toByteArray();
    file.write(xml);
    file.close();
}
void
vleVpz::saveVpm(QString filenameVpm)
{
    QFile file(filenameVpm);
    QFileInfo fileInfo(file);
    if (not fileInfo.dir().exists()) {
        if (not QDir().mkpath(fileInfo.dir().path())) {
            qDebug() << "Cannot create dir " << fileInfo.dir().path();
            return;
        }
    }
    if (not file.exists()) {
        if (not file.open(QIODevice::WriteOnly)) {
            qDebug() << "VPM File (" << filenameVpm
                     << ") can't be opened for write !";
            return;
        }
        file.close();
    }
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = mDocVpm->toByteArray();
    file.write(xml);
    file.close();
}

void
vleVpz::saveTemp(QString pkg, QString prefix)
{
    QDir().mkdir(QDir::tempPath() + "/" + pkg);
    QDir().mkdir(QDir::tempPath() + "/" + pkg + "/exp");
    QDir().mkdir(QDir::tempPath() + "/" + pkg + "/metadata");
    QDir().mkdir(QDir::tempPath() + "/" + pkg + "/metadata/exp");
    QString vpzFile = QDir::tempPath() + "/" + pkg + "/exp/" + prefix + ".vpz";
    QString vpmFile =
      QDir::tempPath() + "/" + pkg + "/metadata/exp/" + prefix + ".vpm";
    saveVpz(vpzFile);
    saveVpm(vpmFile);
}

void
vleVpz::undo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = hasMeta;
    if (hasMeta) {
        if (undoStack->curr != undoStackVpm->curr) {
            qDebug() << " Internal error vleVpz::undo() " << undoStack->curr
                     << "!=" << undoStackVpm->curr;
        }
        undoStackVpm->undo();
    }
    undoStack->undo();
}
void
vleVpz::redo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = hasMeta;
    if (hasMeta) {
        if (undoStack->curr != undoStackVpm->curr) {
            qDebug() << " Internal error vleVpz::redo() " << undoStack->curr
                     << "!=" << undoStackVpm->curr;
        }
        undoStackVpm->redo();
    }
    undoStack->redo();
}

PluginExpCond*
vleVpz::provideCondGUIplugin(const QString& condName)
{
    QString guiPluginName = getCondGUIplugin(condName);
    if (guiPluginName == "") {
        return 0;
    } else {
        PluginExpCond* plugin =
          mGvlePlugins->newInstanceCondPlugin(guiPluginName);
        bool enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
        bool oldBlock = undoStackVpm->blockSignals(true);
        bool oldBlock2 = vleVpz::undoStack->blockSignals(true);
        if (plugin) {
            plugin->init(this, condName);
        }
        vleVpz::undoStack->blockSignals(oldBlock2);
        undoStackVpm->blockSignals(oldBlock);
        vleVpz::undoStack->enableSnapshot(enableSnapshot);
        return plugin;
    }
}

QString
vleVpz::getCondGUIplugin(const QString& condName) const
{
    if (not hasMeta) {
        return "";
    }
    QDomElement docElem = mDocVpm->documentElement();
    QDomNode condsPlugins = mDocVpm->elementsByTagName("condPlugins").item(0);
    QDomNodeList plugins =
      condsPlugins.toElement().elementsByTagName("condPlugin");
    for (int i = 0; i < plugins.length(); i++) {
        QDomNode plug = plugins.item(i);
        if (plug.attributes().contains("cond") and
            (plug.attributes().namedItem("cond").nodeValue() == condName)) {
            return plug.attributes().namedItem("plugin").nodeValue();
        }
    }
    return "";
}

bool
vleVpz::removeCondGUIplugin(const QString& condName)
{
    if (not existCondFromDoc(condName)) {
        return false;
    }
    // update vpm
    setCondGUIplugin(condName, "");
    synchronizeUndoStack();

    emit conditionsUpdated();
    return true;
}

PluginOutput*
vleVpz::provideOutputGUIplugin(const QString& viewName)
{
    QString guiPluginName = getOutputGUIplugin(viewName);
    if (guiPluginName == "") {
        return 0;
    } else {
        PluginOutput* plugin =
          mGvlePlugins->provideOutputPlugin(guiPluginName);
        if (plugin) {
            plugin->init(this, viewName);
        }
        return plugin;
    }
}

QString
vleVpz::getOutputGUIplugin(const QString& viewName) const
{
    if (mDocVpm) {
        QDomElement docElem = mDocVpm->documentElement();
        QDomNode condsPlugins =
          mDocVpm->elementsByTagName("outputGUIplugins").item(0);
        QDomNodeList plugins =
          condsPlugins.toElement().elementsByTagName("outputGUIplugin");
        for (int i = 0; i < plugins.length(); i++) {
            QDomNode plug = plugins.item(i);
            if (plug.attributes().contains("view") and
                (plug.attributes().namedItem("view").nodeValue() ==
                 viewName)) {
                return plug.attributes().namedItem("GUIplugin").nodeValue();
            }
        }
    }
    return "";
}

void
vleVpz::setCurrentSource(QString source)
{
    if (hasMeta) {
        undoStackVpm->current_source = source;
    }
    undoStack->current_source = source;
}

QString
vleVpz::getCurrentSource()
{
    return undoStack->current_source;
}

QByteArray
vleVpz::xGetXml()
{
    QDomDocument doc;
    xSaveDom(&doc);
    QByteArray xml = doc.toByteArray();
    return xml;
}

void
vleVpz::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

/*****************************************************************************
 * Slots
 *****************************************************************************/

void
vleVpz::onUndoRedoStackVpm(QDomNode oldVpm, QDomNode newVpm)
{
    oldValVpm = oldVpm;
    newValVpm = newVpm;
    waitUndoRedoVpm = false;
    if (not waitUndoRedoVpz) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}

void
vleVpz::onUndoRedoStackVpz(QDomNode oldVpz, QDomNode newVpz)
{
    oldValVpz = oldVpz;
    newValVpz = newVpz;
    waitUndoRedoVpz = false;
    if (not waitUndoRedoVpm) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}

/*****************************************************************************
 * Private functions
 *****************************************************************************/

void
vleVpz::xReadDom()
{
    QFile file(mFilename);
    QXmlInputSource source(&file);
    QXmlSimpleReader reader;
    mDoc.setContent(&source, &reader);
}

void
vleVpz::xReadDomMetadata()
{
    if (mDocVpm) {
        QFile file(mFileNameVpm);
        mDocVpm->setContent(&file);
    }
}

void
vleVpz::xSaveDom(QDomDocument* doc)
{
    QDomProcessingInstruction pi;
    pi = doc->createProcessingInstruction(
      "xml", "version=\"1.0\" encoding=\"UTF-8\" ");
    doc->appendChild(pi);

    QDomElement vpzRoot = doc->createElement("vle_project");
    // Save VPZ file revision
    vpzRoot.setAttribute("version", getVersion());

    vpzRoot.setAttribute("author", getAuthor());
    vpzRoot.setAttribute("date", getDate());
    vpzRoot.setAttribute("version", getVersion());

    vpzRoot.appendChild(structuresFromDoc().cloneNode());
    vpzRoot.appendChild(dynamicsFromDoc().cloneNode());
    vpzRoot.appendChild(experimentFromDoc().cloneNode());
    vpzRoot.appendChild(classesFromDoc().cloneNode());
    doc->appendChild(vpzRoot);
}

void
vleVpz::xCreateDomMetadata()
{
    if (not mDocVpm) {
        mDocVpm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocVpm->createProcessingInstruction(
          "xml", "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocVpm->appendChild(pi);

        QDomElement vpmRoot = mDocVpm->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "me");
        QDomElement xCondPlug = mDocVpm->createElement("condPlugins");
        vpmRoot.appendChild(xCondPlug);
        QDomElement xOutPlug = mDocVpm->createElement("outputGUIplugins");
        vpmRoot.appendChild(xOutPlug);
        mDocVpm->appendChild(vpmRoot);
    }
}

void
vleVpz::setCondGUIplugin(const QString& condName, const QString& name)
{
    if (not hasMeta) {
        return;
    }
    QDomElement docElem = mDocVpm->documentElement();
    QDomNode condsPlugins = mDocVpm->elementsByTagName("condPlugins").item(0);
    undoStackVpm->snapshot(condsPlugins);
    QDomNodeList plugins =
      condsPlugins.toElement().elementsByTagName("condPlugin");
    for (int i = 0; i < plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j = 0; j < plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "cond") and
                (plug.attributes().item(j).nodeValue() == condName)) {
                plug.toElement().setAttribute("plugin", name);
                return;
            }
        }
    }
    QDomElement el = mDocVpm->createElement("condPlugin");
    el.setAttribute("cond", condName);
    el.setAttribute("plugin", name);
    condsPlugins.appendChild(el);
}

void
vleVpz::renameCondGUIplugin(const QString& oldCond, const QString& newCond)
{
    if (not hasMeta) {
        return;
    }

    QDomElement docElem = mDocVpm->documentElement();

    QDomNode condsPlugins = mDocVpm->elementsByTagName("condPlugins").item(0);
    undoStackVpm->snapshot(condsPlugins);
    QDomNodeList plugins =
      condsPlugins.toElement().elementsByTagName("condPlugin");
    for (int i = 0; i < plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j = 0; j < plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "cond") and
                (plug.attributes().item(j).nodeValue() == oldCond)) {
                plug.toElement().setAttribute("cond", newCond);
                return;
            }
        }
    }
}

void
vleVpz::setOutputGUIplugin(const QString& viewName,
                           const QString& pluginName,
                           bool snapshot)
{
    if (not hasMeta) {
        return;
    }

    QDomElement docElem = mDocVpm->documentElement();
    QDomNode outGUIplugs =
      mDocVpm->elementsByTagName("outputGUIplugins").item(0);
    if (outGUIplugs.isNull()) {
        QDomNode metaData =
          mDocVpm->elementsByTagName("vle_project_metadata").item(0);
        if (snapshot) {
            undoStackVpm->snapshot(metaData);
        }
        metaData.appendChild(mDocVpm->createElement("outputGUIplugins"));
        outGUIplugs = mDocVpm->elementsByTagName("outputGUIplugins").item(0);
    } else {
        if (snapshot) {
            undoStackVpm->snapshot(outGUIplugs);
        }
    }

    QDomNodeList plugins =
      outGUIplugs.toElement().elementsByTagName("outputGUIplugin");
    for (int i = 0; i < plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        if (DomFunctions::attributeValue(plug, "view") == viewName) {
            if (pluginName == "") {
                outGUIplugs.removeChild(plug);
                return;
            } else {
                DomFunctions::setAttributeValue(plug, "GUIplugin", pluginName);
            }
        }
    }
    if (pluginName != "") {
        QDomElement el = mDocVpm->createElement("outputGUIplugin");
        el.setAttribute("view", viewName);
        el.setAttribute("GUIplugin", pluginName);
        outGUIplugs.appendChild(el);
    }
}

void
vleVpz::synchronizeUndoStack()
{
    if (not hasMeta) {
        return;
    }
    if (undoStackVpm->snapshotEnabled) {
        while (undoStack->curr > undoStackVpm->curr) {
            undoStackVpm->snapshot(QDomNode());
        }
    }
    if (undoStack->snapshotEnabled) {
        while (undoStackVpm->curr > undoStack->curr) {
            undoStack->snapshot(QDomNode());
        }
    }
}

void
vleVpz::tryEmitUndoAvailability(unsigned int prevCurr,
                                unsigned int curr,
                                unsigned int saved)
{
    int undoAvailability =
      DomDiffStack::computeUndoAvailability(prevCurr, curr, saved);
    if (undoAvailability == 1) {
        emit undoAvailable(true);
    }
    if (undoAvailability == -1) {
        emit undoAvailable(false);
    }
}

QSet<QString>
vleVpz::modifyImportSourceForImport(QDomNode dest_node,
                                    const QStringList& import_queries,
                                    bool track,
                                    vleVpz& src,
                                    QString& src_mod_query)
{
    src_mod_query = "";
    QSet<QString> elems;
    QStringList log_messages;
    bool importing_cond = false;
    bool importing_cond_ports = false;
    bool importing_models = false;
    QSet<QString> condsToExclude;
    QSet<QString> condPortsToExclude;
    QSet<QString> dynToExclude;
    QSet<QString> obsToExclude;
    QSet<QString> modelsToExclude;

    for (int i = 0; i < import_queries.size(); i++) {
        QString query = import_queries.at(i);
        QDomNode src_node = src.vdo()->getNodeFromXQuery(query);
        QString src_node_name = src_node.nodeName();
        QString dest_node_name = dest_node.nodeName();
        if (src_node_name == "condition") {
            if (dest_node_name != "conditions") {
                elems.clear();
                break;
            }
            QString src_cond = DomFunctions::attributeValue(src_node, "name");
            if (not importing_cond) {
                condsToExclude.unite(
                  vleDomStatic::conditions(src.condsFromDoc()));
                importing_cond = true;
            }
            QString dest_cond_new = src_cond;
            if (not DomFunctions::childWhithNameAttr(
                      condsFromDoc(), "condition", src_cond)
                      .isNull()) {
                dest_cond_new = DomFunctions::childNameProvider(
                  condsFromDoc(), "condition", src_cond, condsToExclude);
                src.renameConditionToDoc(src_cond, dest_cond_new);
                condsToExclude.insert(dest_cond_new);
                log_messages.append(
                  QString("condition '%1' is imported as '%2'")
                    .arg(src_cond)
                    .arg(dest_cond_new));
            }
            elems.insert(QString("cond::%1").arg(dest_cond_new));
        } else if ((src_node_name == "port") and
                   (src_node.parentNode().nodeName() == "condition")) {
            QString src_cond_port =
              DomFunctions::attributeValue(src_node, "name");
            QString src_cond =
              DomFunctions::attributeValue(src_node.parentNode(), "name");
            if (dest_node_name != "condition") {
                elems.clear();
                break;
            }
            QString dest_cond =
              DomFunctions::attributeValue(dest_node, "name");
            if (getCondGUIplugin(dest_cond) != "") {
                elems.clear();
                break;
            }
            if (not importing_cond_ports) {
                condPortsToExclude.unite(
                  DomFunctions::childNames(src_node.parentNode(), "port"));
                importing_cond_ports = true;
            }
            QString dest_cond_port_new = src_cond_port;
            if (not DomFunctions::childWhithNameAttr(
                      dest_node, "port", src_cond_port)
                      .isNull()) {
                dest_cond_port_new = DomFunctions::childNameProvider(
                  dest_node, "port", src_cond_port, condPortsToExclude);
                src.renameCondPortToDoc(
                  src_cond, src_cond_port, dest_cond_port_new);
                condPortsToExclude.insert(dest_cond_port_new);
                log_messages.append(
                  QString("condition port '%1' from condition"
                          " '%2' is imported as '%3'")
                    .arg(src_cond_port)
                    .arg(dest_cond)
                    .arg(dest_cond_port_new));
            }
            elems.insert(QString("cond_port::%1//%2//%3")
                           .arg(src_cond)
                           .arg(dest_cond)
                           .arg(dest_cond_port_new));
        } else if (src_node_name == "model") {
            if ((dest_node_name != "model") or
                (DomFunctions::attributeValue(dest_node, "type") !=
                 "coupled")) {
                elems.clear();
                break;
            }
            QDomNode src_mod_parent = src_node.parentNode().parentNode();
            // check that copy comes from a coupled model
            if (src_mod_parent.nodeName() != "model") {
                elems.clear();
                break;
            }
            // cheack that all imported models are contained nto the same model
            if (src_mod_query == "") {
                src_mod_query = src.vdo()->getXQuery(src_mod_parent);
            } else {
                if (src.vdo()->getXQuery(src_mod_parent) != src_mod_query) {
                    elems.clear();
                    break;
                }
            }

            // modify source model name if present into dest
            QString src_mod_name =
              DomFunctions::attributeValue(src_node, "name");
            if (not importing_models) {
                modelsToExclude.unite(
                  DomFunctions::childNames(src_node.parentNode(), "model"));
                condsToExclude.unite(
                  vleDomStatic::conditions(src.condsFromDoc()));
                obsToExclude.unite(
                  vleDomStatic::observables(src.obsFromDoc()));
                dynToExclude.unite(
                  vleDomStatic::dynamics(src.dynamicsFromDoc()));
                importing_models = true;
            }
            QString dest_mod_new = src_mod_name;
            if (not DomFunctions::childWhithNameAttr(
                      DomFunctions::obtainChild(dest_node, "submodels"),
                      "model",
                      src_mod_name)
                      .isNull()) {
                dest_mod_new = DomFunctions::childNameProvider(
                  DomFunctions::obtainChild(dest_node, "submodels"),
                  "model",
                  src_mod_name,
                  modelsToExclude);
                src.renameModel(src_node, dest_mod_new);
                modelsToExclude.insert(dest_mod_new);
                log_messages.append(QString("model '%1' is imported as '%2'")
                                      .arg(src_mod_name)
                                      .arg(dest_mod_new));
            }
            elems.insert(QString("model::%1").arg(dest_mod_new));
            if (track) {
                // modify source model conditions attached to model to import
                // by renaming cond if present in dest model
                QSet<QString> src_conds_att =
                  vleDomStatic::attachedCondsToAtomic(src_node);
                for (int j = 0; j < src_conds_att.size(); j++) {
                    QString src_cond = src_conds_att.values().at(j);
                    QString dest_cond_new = src_cond;
                    if (not DomFunctions::childWhithNameAttr(
                              condsFromDoc(), "condition", src_cond)
                              .isNull()) {
                        dest_cond_new =
                          DomFunctions::childNameProvider(condsFromDoc(),
                                                          "condition",
                                                          src_cond,
                                                          condsToExclude);
                        src.renameConditionToDoc(src_cond, dest_cond_new);
                        condsToExclude.insert(dest_cond_new);
                        log_messages.append(
                          QString("condition '%1' is imported as '%2'")
                            .arg(src_cond)
                            .arg(dest_cond_new));
                    }
                    elems.insert(QString("cond::%1").arg(dest_cond_new));
                }
                // modify source model observables attached to model to import
                // by renaming obs if present in dest model
                QString src_obs_att =
                  vleDomStatic::attachedObsToAtomic(src_node);
                if (src_obs_att != "") {
                    QString dest_obs_new = src_obs_att;
                    if (not DomFunctions::childWhithNameAttr(
                              obsFromDoc(), "observable", src_obs_att)
                              .isNull()) {
                        dest_obs_new =
                          DomFunctions::childNameProvider(obsFromDoc(),
                                                          "observable",
                                                          src_obs_att,
                                                          obsToExclude);
                        src.renameObservableToDoc(src_obs_att, dest_obs_new);
                        obsToExclude.insert(dest_obs_new);
                        log_messages.append(
                          QString("observable '%1' is imported as '%2'")
                            .arg(src_obs_att)
                            .arg(dest_obs_new));
                    }
                    elems.insert(QString("obs::%1").arg(dest_obs_new));
                }
                // modify source model dyn attached to model to import
                // by renaming dyn if present in dest model
                QString src_dyn_att =
                  vleDomStatic::attachedDynToAtomic(src_node);
                if (src_dyn_att != "") {
                    QString dest_dyn_new = src_dyn_att;
                    if (not DomFunctions::childWhithNameAttr(
                              dynamicsFromDoc(), "dynamic", src_dyn_att)
                              .isNull()) {
                        dest_dyn_new =
                          DomFunctions::childNameProvider(dynamicsFromDoc(),
                                                          "dynamic",
                                                          src_dyn_att,
                                                          dynToExclude);
                        src.renameDynamicToDoc(src_dyn_att, dest_dyn_new);
                        dynToExclude.insert(dest_dyn_new);
                        log_messages.append(
                          QString("dynamic '%1' is imported as '%2'")
                            .arg(src_dyn_att)
                            .arg(dest_dyn_new));
                    }
                    elems.insert(QString("dyn::%1").arg(dest_dyn_new));
                }
            } else {
                // if not track remove all model dependencies into source
                DomFunctions::setAttributeValue(src_node, "dynamics", "");
                DomFunctions::setAttributeValue(src_node, "conditions", "");
                DomFunctions::setAttributeValue(src_node, "observables", "");
            }
        }
    }
    if (elems.size() > 0 and log_messages.size() > 0) {
        logger()->log(" During import:");
        for (int i = 0; i < log_messages.size(); i++) {
            logger()->log(QString("--%1").arg(log_messages.at(i)));
        }
    }
    return elems;
}

void
vleVpz::clearConfModel(QDomNode model)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    synchronizeUndoStack();
    bool isUpConditions = false;

    bool oldSnapshotStatus = undoStack->enableSnapshot(false);
    QDomElement modele = model.toElement();
    removeDyn(DomFunctions::attributeValue(modele, "dynamics"));
    rmObservableFromDoc(DomFunctions::attributeValue(modele, "observables"));

    QSet<QString> attachedConds = vleDomStatic::attachedCondsToAtomic(modele);
    QSet<QString>::const_iterator itb = attachedConds.begin();
    QSet<QString>::const_iterator ite = attachedConds.end();
    for (; itb != ite; itb++) {
        rmConditionToDoc(*itb);
        isUpConditions = true;
    }
    {
        QDomNode in = DomFunctions::obtainChild(model, "in", &mDoc);
        QDomNodeList ports = in.toElement().elementsByTagName("port");
        while (ports.length() > 0) {
            rmModelPort(ports.at(0));
        }
    }

    {
        QDomNode out = DomFunctions::obtainChild(model, "out", &mDoc);
        QDomNodeList ports = out.toElement().elementsByTagName("port");
        while (ports.length() > 0) {
            rmModelPort(ports.at(0));
        }
    }
    undoStack->enableSnapshot(oldSnapshotStatus);

    emit dynamicsUpdated();
    emit observablesUpdated();
    if (isUpConditions) {
        emit conditionsUpdated();
    }
    emit modelsUpdated();
}

void
vleVpz::unConfigureModel(QDomNode model)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    synchronizeUndoStack();
    bool oldSnapshotStatus = undoStack->enableSnapshot(false);
    QDomElement modele = model.toElement();

    modele.setAttribute("dynamics", "");
    modele.setAttribute("observables", "");
    modele.setAttribute("conditions", "");

    {
        QDomNode in = DomFunctions::obtainChild(model, "in", &mDoc);
        QDomNodeList ports = in.toElement().elementsByTagName("port");
        while (ports.length() > 0) {
            rmModelPort(ports.at(0));
        }
    }

    {
        QDomNode out = DomFunctions::obtainChild(model, "out", &mDoc);
        QDomNodeList ports = out.toElement().elementsByTagName("port");
        while (ports.length() > 0) {
            rmModelPort(ports.at(0));
        }
    }
    undoStack->enableSnapshot(oldSnapshotStatus);

    emit modelsUpdated();
}

void
vleVpz::configureModel(QDomNode model,
                       QDomNode dynamic,
                       QDomNode observable,
                       QDomNode condition,
                       QDomNode in,
                       QDomNode out)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    synchronizeUndoStack();
    bool oldSnapshotStatus = undoStack->enableSnapshot(false);
    QDomElement modele = model.toElement();

    QString dyn = dynamic.attributes().namedItem("name").nodeValue();

    QDomNodeList dynList = dynListFromDoc();
    if (not existDynamicIntoDynList(dyn, dynList)) {
        dynamicsFromDoc().appendChild(dynamic);
    }
    modele.setAttribute("dynamics", dyn);

    QString obs = observable.attributes().namedItem("name").nodeValue();

    if (not existObsFromDoc(obs)) {
        obsFromDoc().appendChild(observable);
    }
    modele.setAttribute("observables", obs);

    QString cond = condition.attributes().namedItem("name").nodeValue();

    if (not existCondFromDoc(cond)) {
        condsFromDoc().appendChild(condition);
    }
    modele.setAttribute("conditions", cond);

    modele.replaceChild(in, DomFunctions::obtainChild(modele, "in", &mDoc));
    modele.replaceChild(out, DomFunctions::obtainChild(modele, "out", &mDoc));
    undoStack->enableSnapshot(oldSnapshotStatus);

    emit dynamicsUpdated();
    emit observablesUpdated();
    emit conditionsUpdated();
    emit modelsUpdated();
}
}
} // namespaces
