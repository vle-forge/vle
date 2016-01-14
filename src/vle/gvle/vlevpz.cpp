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
#include "vlevpz.h"
#include "vlepackage.h"

#include <QDebug>

namespace vle {
namespace gvle {


vleDomVpz::vleDomVpz(QDomDocument* doc): vleDomObject(doc)

{

}
vleDomVpz::~vleDomVpz()
{

}
QString
vleDomVpz::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    //element uniq in children
    if ((name == "structures") or
        (name == "submodels") or
        (name == "in") or
        (name == "out") or
        (name == "connections") or
        (name == "classes") or
        (name == "dynamics") or
        (name == "conditions") or
        (name == "experiment") or
        (name == "views") or
        (name == "observables")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified wy attribute name
    if ((name == "port") or
        (name == "model") or
        (name == "class") or
        (name == "dynamic") or
        (name == "condition") or
        (name == "observable")){
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
                +attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project") {
        return "./vle_project";
    }
    return "";
}

QDomNode
vleDomVpz::getNodeFromXQuery(const QString& query,
        QDomNode d)
{
    //handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j=0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j=2;
    } else {
        curr = queryList.at(0);
        j=1;
    }
    for (int i=j; i<queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size()-1) {
            rest = rest + "/";
        }
    }
    //handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return(getNodeFromXQuery(rest,rootNode));
    }

    //handle recursion with uniq node
    if ((curr == "structures") or
        (curr == "submodels") or
        (curr == "in") or
        (curr == "out") or
        (curr == "connections") or
        (curr == "classes") or
        (curr == "dynamics") or
        (curr == "conditions") or
        (curr == "experiment") or
        (curr == "views") or
        (curr == "observables")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
    }
    //handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("model"));
    nodeByNames.push_back(QString("port"));
    nodeByNames.push_back(QString("class"));
    nodeByNames.push_back(QString("dynamic"));
    nodeByNames.push_back(QString("condition"));
    nodeByNames.push_back(QString("observable"));
    QString selNodeByName ="";
    for (unsigned int i=0; i< nodeByNames.size(); i++) {
        if (curr.startsWith(nodeByNames[i])) {
            selNodeByName = nodeByNames[i];
        }
    }
    if (not selNodeByName.isEmpty()) {
        QStringList currSplit = curr.split("\"");
        QDomNode selMod = childWhithNameAttr(d, selNodeByName, currSplit.at(1));
        return getNodeFromXQuery(rest,selMod);
    }
    return QDomNode();
}

/************************************************************************/

vleVpz::vleVpz(const QString& filename) :
    mFilename(filename), mPath(), mFile(mFilename), mDoc("vle_project"),
    mPackage(0), mLogger(0), undoStack(0)

{
    xReadDom();

    mVdo = new vleDomVpz(&mDoc);
    undoStack = new vleDomDiffStack(mVdo);
    undoStack->init(getDomDoc());
}

vleVpz::vleVpz(QXmlInputSource& source):
  mFilename(), mPath(), mFile(), mDoc("vle_project"),
  mPackage(0), mLogger(0), undoStack(0)
{
    QXmlSimpleReader reader;
    mDoc.setContent(&source, &reader);
    mVdo = new vleDomVpz(&mDoc);
    undoStack = new vleDomDiffStack(mVdo);
    undoStack->init(getDomDoc());
}

const QDomDocument&
vleVpz::getDomDoc() const
{
    return mDoc;
}

QDomDocument&
vleVpz::getDomDoc()
{
    emit sigChanged(mFilename);
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
    for (unsigned int i=0; i<list.length(); i++) {
        if (mVdo->attributeValue(list.at(i), "name") == className) {
            return list.at(i);
        }
    }
    qDebug() << ("Internal error in classFromDoc (class not found): ") << className;
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
    for (unsigned int i=0; i<list.length(); i++) {
        if (mVdo->attributeValue(list.at(i), "name") == className) {
            return true;
        }
    }
    return false;
}

QString
vleVpz::addClassToDoc(bool atomic)
{
    QDomNode classesElem = mVdo->obtainChild(vleProjectFromDoc(), "classes");

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
            found =true;
        }
    }
    QDomNode newClass = getDomDoc().createElement("class");
    setAttributeValue(newClass.toElement(), "name", name);
    QDomNode newModel = getDomDoc().createElement("model");
    setAttributeValue(newModel.toElement(), "name", "NewModel");
    if (atomic) {
        setAttributeValue(newModel.toElement(), "type", "atomic");
        setAttributeValue(newModel.toElement(), "conditions", "");
        setAttributeValue(newModel.toElement(), "dynamics", "");
        setAttributeValue(newModel.toElement(), "observables", "");
    } else {
        setAttributeValue(newModel.toElement(), "type", "coupled");
    }
    setAttributeValue(newModel.toElement(), "x", "0");
    setAttributeValue(newModel.toElement(), "y", "0");
    setAttributeValue(newModel.toElement(), "width", "50");
    setAttributeValue(newModel.toElement(), "height", "50");
    newClass.appendChild(newModel);
    classesElem.appendChild(newClass);
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
    setAttributeValue(cl.toElement(),"name", newClass);
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
    while(not found) {
        name = name_prefix;
        name += "_";
        name += QVariant(k).toString();
        if (existClassFromDoc(name)) {
            k++;
        } else {
            found = true;
        }
    }
    setAttributeValue(copy.toElement(), "name", name);
    classesElem.appendChild(copy);
    return name;
}

void
vleVpz::removeClassToDoc(const QString& className)
{
    QDomNode classesElem = classesFromDoc();
    QDomNode classNode = classFromDoc(className);
    classesElem.removeChild(classNode);
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
    for (unsigned int i=0; i < dynList.length(); i++) {
       if (mVdo->attributeValue(dynList.at(i), "name") == dyn) {
           return dynList.at(i);
       }
    }
    qDebug() << ("Internal error in dynamicFromDoc (dyn not found): ") << dyn;
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
    return mVdo->obtainChild(structures, "model");
}

QDomNode
vleVpz::modelConnectionsFromDoc(const QString& mod_query)
{
    QDomNode mod = mVdo->getNodeFromXQuery(mod_query);
    QDomNodeList childList = mod.childNodes();
    for (unsigned int i=0; i<childList.length(); i++) {
        QDomNode item = childList.item(i);
        if (item.nodeName() == "connections") {
            return item;
        }
    }
    qDebug() << "Internal Error in modelConnectionsFromDoc\n";
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

QDomNode
vleVpz::modelConnectionFromDoc(const QString& sourceFullPath,
        const QString& destinationFullPath, const QString& sourcePort,
        const QString& destinationPort)
{
    QString coupledModel("");
    QStringList sourcePathSplit = sourceFullPath.split(".");
    QStringList destinationPathSplit = destinationFullPath.split(".");
    if (sourcePathSplit.length() == destinationPathSplit.length()-1) {
        //source is a coupled
        for (int i =0; i< sourcePathSplit.length(); i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += sourcePathSplit[i];
        }
    } else if (sourcePathSplit.length()-1 == destinationPathSplit.length()) {
        //destination is the coupled
        for (int i =0; i< destinationPathSplit.length(); i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += destinationPathSplit[i];
        }
    } else if (sourcePathSplit.length() == destinationPathSplit.length()) {
        //the two are in common coupled model
        for (int i =0; i< destinationPathSplit.length()-1; i++) {
            if (i > 0) {
                coupledModel += ".";
            }
            coupledModel += destinationPathSplit[i];
        }
    } else {
        qDebug() << "Internal error in modelConnectionFromDoc 1";
        return QDomNode();
    }
    QString sourceModel = sourcePathSplit[sourcePathSplit.length()-1];
    QString destModel = destinationPathSplit[destinationPathSplit.length()-1];
    QDomNode connexions = modelConnectionsFromDoc(coupledModel);
    QDomNodeList conList =
            connexions.toElement().elementsByTagName("connection");
    for (unsigned int j=0; j<conList.length(); j++) {
        QDomNode con = conList.at(j);
        if ((mVdo->attributeValue(con.toElement().elementsByTagName("origin")
                .item(0), "model") == sourceModel) and
            (mVdo->attributeValue(con.toElement().elementsByTagName("origin")
                .item(0), "port") == sourcePort) and
            (mVdo->attributeValue(con.toElement().elementsByTagName("destination")
                .item(0), "model") == destModel) and
            (mVdo->attributeValue(con.toElement().elementsByTagName("destination")
                .item(0), "port") == destinationPort)) {
            return con;
        }
    }
    qDebug() << "Internal error in modelConnectionFromDoc 2";
    return QDomNode();
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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "observables") {
            return child;
        }
    }
    qDebug() << ("Internal error in obsFromView (observables not found)");
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
    for (unsigned int i=0; i < childs.length(); i++) {
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
        QString mess = "Internal error in mapFromOutput (unexpected node type)";
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
        qDebug() << ("Internal error in atomicModelFromModel (wrong main tag)");
        return res;
    }
    QDomNode currSubModelTag = node.firstChildElement("submodels");
    if (currSubModelTag.nodeName() != "submodels") {
        if ((mVdo->attributeValue(node, "type") == "atomic")
                and (mVdo->attributeValue(node, "name") == atom)) {
            return node;
        }
        return res;
    }
    QDomNodeList currSubModels = currSubModelTag.childNodes();
    bool found = false;
    for (unsigned int i =0; i < currSubModels.length(); i++) {
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
        qDebug() << ("Internal error in connectionsFromModel (wrong main tag)");
        return QDomNode();
    }
    QDomNodeList children = node.toElement().childNodes();
    for (unsigned int i =0; i < children.length(); i++) {
        QDomNode conns = children.at(i);
        if (conns.nodeName() == "connections") {
            return conns;
        }
    }
    return QDomNode();
}

QList<QDomNode>
vleVpz::submodelsFromModel(const QDomNode& node)
{
    QDomNode sub = mVdo->obtainChild(node, "submodels", false);
    if (sub.isNull()) {
        return QList<QDomNode>();
    } else {
        return childNodesWithoutText(sub, "model");
    }
}

vleDomVpz*
vleVpz::vdo()
{
    return mVdo;
}

QList<QDomNode>
vleVpz::childNodesWithoutText(QDomNode node, const QString& nodeName) const
{
    QDomNodeList childs = node.childNodes();
    QList<QDomNode> childsWithoutText;
    for (unsigned int i=0; i<childs.length();i++) {
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

void
vleVpz::removeTextChilds(QDomNode node, bool recursively)
{
    QDomNodeList chs;
    QList<QDomNode> torm;
    bool stop = false;
    bool first = true;
    while(not stop) {
        torm.clear();
        chs = node.childNodes();
        for (unsigned int i=0; i<chs.length(); i++) {
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
            for (int i=0; i<torm.size(); i++) {
                QDomNode ch = torm[i];
                node.removeChild(ch);
            }
        }
    }
}

void
vleVpz::setAttributeValue(QDomElement node, const QString& attrName,
        const QString& val)
{
    if (node.attributes().contains(attrName)) {
         node.attributes().namedItem(attrName).setNodeValue(val);
    } else {
        node.toElement().setAttribute(attrName, val);
    }
}



QString
vleVpz::mergeQueries(const QString& query1, const QString& query2)
{

    QStringList qList1 = query1.split("/");
    QStringList qList2 = query2.split("/");
    if ((qList2.size() >= 2) and
            (qList1.at(qList1.size()-1) == qList2.at(1))) {
        QString res = "";
        QStringList resList = qList1;
        qList2.removeFirst();
        qList2.removeFirst();
        resList.append(qList2);
        int nbElem = resList.size();
        for(int i=0; i<nbElem; i++) {
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
    QString res ="";
    for (int i=begin; i<end2; i++) {
        if (i == begin) {
            res = qList.at(i);
        } else {
            res = res+"/"+ qList.at(i);
        }
    }
    return res;
}


/*****************************************************
 * TODO A TRIER
 *****************************************************/

QString vleVpz::getFilename() const
{
    return mFilename;
}
QString vleVpz::getBasePath()
{
    return mPath;
}

QString vleVpz::getAuthor() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("author"))
    {
        QDomNode xAuthor = attrMap.namedItem("author");
        return xAuthor.nodeValue();
    }
    return QString("");
}

void vleVpz::setAuthor(const QString author)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("author", author);
}

QString vleVpz::getDate() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("date"))
    {
        QDomNode xDate = attrMap.namedItem("date");
        return xDate.nodeValue();
    }
    return QString("");
}

void vleVpz::setDate(const QString date)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("date", date);
}

QString vleVpz::getVersion() const
{
    QDomElement docElem = getDomDoc().documentElement();

    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("version"))
    {
        QDomNode xVersion = attrMap.namedItem("version");
        return xVersion.nodeValue();
    }
    return QString("");
}

void vleVpz::setVersion(const QString version)
{
    QDomElement docElem = getDomDoc().documentElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("version", version);
}

QString vleVpz::getExpName() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("name"))
    {
        QDomNode xName = attrMap.namedItem("name");
        return xName.nodeValue();
    }
    return QString("");
}

void vleVpz::setExpName(const QString name)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("name", name);
}

QString vleVpz::getExpDuration() const
{
    QDomNode xDuration = portFromDoc("simulation_engine",
                                     "duration");
    std::vector<vle::value::Value*> valuesToFill;
    fillWithMultipleValue(xDuration, valuesToFill);
    vle::value::Value* durationValue = valuesToFill[0];
    return QString::number(durationValue->toDouble().value());
}

void vleVpz::setExpDuration(const QString duration)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("duration", duration);
    bool ok;
    double durationNum = duration.toDouble(&ok);
    if (ok) {
        vle::value::Value* durationValue =
            new vle::value::Double(durationNum);
        fillWithValue("simulation_engine", "duration", 0, *durationValue);
        delete durationValue;

        emit experimentUpdated();
    }
}

QString
vleVpz::getExpBegin() const
{
    QDomNode xBegin = portFromDoc("simulation_engine",
                                     "begin");
    std::vector<vle::value::Value*> valuesToFill;
    fillWithMultipleValue(xBegin, valuesToFill);
    vle::value::Value* beginValue = valuesToFill[0];
    return QString::number(beginValue->toDouble().value());
}


void
vleVpz::setExpBegin(const QString begin)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    undoStack->snapshot(docElem);
    docElem.setAttribute("begin", begin);
    bool ok;
    double beginNum = begin.toDouble(&ok);
    if (ok) {
        vle::value::Value* beginValue =
            new vle::value::Double(beginNum);
        fillWithValue("simulation_engine", "begin", 0, *beginValue);
        delete beginValue;

        emit experimentUpdated();
    }
}

void
vleVpz::renameObservableToDoc(const QString& oldName,
        const QString& newName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    QDomNode toRename = obsFromObss(obss, oldName);
    QDomElement docElem = toRename.toElement();
    docElem.setAttribute("name", newName);

    QDomNode structures =
        getDomDoc().documentElement().elementsByTagName("structures").at(0);
    QDomNode currModel =
        structures.toElement().elementsByTagName("model").at(0);
    renameObservableFromModel(currModel, oldName, newName);

    emit observablesUpdated();
}

void
vleVpz::renameObsPortToDoc(const QString& obsName,
        const QString& oldPortName, const QString& newPortName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode portToRename = mVdo->childWhithNameAttr(obs, "port", oldPortName);
    setAttributeValue(portToRename.toElement(),"name", newPortName);

    emit observablesUpdated();
}





QDomElement
vleVpz::buildEmptyValueFromDoc(vle::value::Value::type vleType)
{
    QDomElement elem;
    switch (vleType) {
    case vle::value::Value::BOOLEAN: {
        elem = getDomDoc().createElement("boolean");
        break;
    } case vle::value::Value::INTEGER: {
        elem = getDomDoc().createElement("integer");
        break;
    } case vle::value::Value::DOUBLE: {
        elem = getDomDoc().createElement("double");
        break;
    } case vle::value::Value::STRING: {
        elem = getDomDoc().createElement("string");
        break;
    } case vle::value::Value::SET: {
        elem = getDomDoc().createElement("set");
        break;
    } case vle::value::Value::MAP: {
        elem = getDomDoc().createElement("map");
        break;
    } case vle::value::Value::TUPLE: {
        elem = getDomDoc().createElement("tuple");
        break;
    } case vle::value::Value::TABLE: {
        elem = getDomDoc().createElement("table");
        break;
    } case vle::value::Value::MATRIX: {
        elem = getDomDoc().createElement("matrix");
        break;
    } default: {
        qDebug() << "Internal error in buildEmptyValueFromDoc ";
        break;
    }}
    return elem;
}

bool
vleVpz::existObsFromDoc(const QString& obsName) const
{
    QDomNode obss = obsFromViews(viewsFromDoc());
    if (obss.isNull()) {
        return false;
    }
    QDomNodeList obsList = obss.childNodes();
    for (unsigned int i = 0; i < obsList.length(); i++) {
        QDomNode obs = obsList.at(i);
        if (mVdo->attributeValue(obs, "name") == obsName) {
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
    for (unsigned int i = 0; i < viewList.length(); i++) {
        QDomNode view = viewList.at(i);
        if (mVdo->attributeValue(view, "name") == viewName) {
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
    for (unsigned int i = 0; i < dynList.length(); i++) {
        QDomNode view = dynList.at(i);
        if (mVdo->attributeValue(view, "name") == dynName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existCondFromDoc(const QString& condName) const
{
    QDomNode conds = condsFromDoc();
    QDomNodeList condList = conds.childNodes();
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.at(i);
        if (mVdo->attributeValue(cond, "name") == condName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existObsPortFromDoc(const QString& obsName,
                            const QString& portName) const
{
    QDomNodeList ports = portsListFromObs(
        obsFromObss(obsFromDoc(), obsName));
    for (unsigned int i =0; i < ports.length(); i++) {
        QDomNode port = ports.at(i);
        if (mVdo->attributeValue(port, "name") == portName) {
            return true;
        }
    }
    return false;
}

bool
vleVpz::existPortFromDoc(const QString& condName,
                         const QString& portName) const
{
    QDomNodeList ports = portsListFromCond(
            condFromConds(condsFromDoc(), condName));
    for (unsigned int i =0; i < ports.length(); i++) {
        QDomNode port = ports.at(i);
        if (mVdo->attributeValue(port, "name") == portName) {
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
    QList<QDomNode> viewList = childNodesWithoutText(views, "view");
    for (int i =0; i< viewList.size(); i++) {
        QDomNode v = viewList[i];
        if (mVdo->attributeValue(v,"name") == oldName) {
            setAttributeValue(v.toElement(), "name", newName);
        }
        if (mVdo->attributeValue(v,"output") == oldName) {
            setAttributeValue(v.toElement(), "output", newName);
        }
    }
    //rename output (view and output names are set to be equal)
    QDomNode outputs = mVdo->obtainChild(views, "outputs", true);
    QList<QDomNode> outList = childNodesWithoutText(outputs, "output");
    for (int i =0; i< outList.size(); i++) {
        QDomNode o = outList[i];
        if (mVdo->attributeValue(o,"name") == oldName) {
            setAttributeValue(o.toElement(), "name", newName);
        }
    }
    //rename view names into observables (ie. attached views to ports)
    QDomNode obss = mVdo->obtainChild(views, "observables", true);
    QList<QDomNode> obsList = childNodesWithoutText(obss, "observable");
    for (int i =0; i< obsList.size(); i++) {
        QDomNode o = obsList[i];
        QList<QDomNode> portList = childNodesWithoutText(o, "port");
        for (int j =0; j< portList.size(); j++) {
            QDomNode p = portList[j];
            QList<QDomNode> attViewL = childNodesWithoutText(p, "attachedview");
            for (int k =0; k< attViewL.size(); k++) {
                QDomNode av = attViewL[k];
                if (mVdo->attributeValue(av,"name") == oldName) {
                    setAttributeValue(av.toElement(), "name", newName);
                }
            }
        }
    }
    emit viewsUpdated();
}

void
vleVpz::renameConditionToDoc(const QString& oldName, const QString& newName)
{
    QDomNodeList condList = condsListFromConds(condsFromDoc());
    for (unsigned int i =0; i < condList.length(); i++) {
        QDomNode cond = condList.at(i);
        if (mVdo->attributeValue(cond, "name") == oldName) {
            undoStack->snapshot(condsFromDoc());
            setAttributeValue(cond.toElement(), "name", newName);
            emit conditionsUpdated();
            return;
        }
    }
    qDebug() << "Internal error in renameConditionToDoc (not found)";
}

void
vleVpz::renameDynamicToDoc(const QString& oldName, const QString& newName)
{
    QDomNode dyn = mVdo->childWhithNameAttr(dynamicsFromDoc(),"dynamic",oldName);
    if (dyn.isNull()) {
        qDebug() << "Internal error in renameDynamicToDoc (not found)";
        return;
    }
    undoStack->snapshot(vleProjectFromDoc());
    setAttributeValue(dyn.toElement(), "name", newName);

    QDomNodeList mods = getDomDoc().elementsByTagName("model");
    for (int i=0; i<mods.size(); i++) {
        QDomNode mod = mods.at(i);
        if ((mVdo->attributeValue(mod,"type") == "atomic") and
                (mVdo->attributeValue(mod,"dynamics") == oldName)) {
            setAttributeValue(mod.toElement(),"dynamics",newName);
        }
    }
    emit dynamicsUpdated();
}

void
vleVpz::renameCondPortToDoc(const QString& condName, const QString& oldName,
        const QString& newName)
{
    QDomNode cond = condFromConds(condsFromDoc(), condName);
    undoStack->snapshot(cond);
    QDomNodeList portList = portsListFromCond(cond);
    for (unsigned int i =0; i < portList.length(); i++) {
        QDomNode port = portList.at(i);
        if (mVdo->attributeValue(port, "name") == oldName) {
            setAttributeValue(port.toElement(), "name", newName);
            emit conditionsUpdated();
            return;
        }
    }
    qDebug() << "Internal error in renameCondPortToDoc (not found)";
}

QString
vleVpz::newDynamicNameToDoc() const
{
    QString condName = "NewDynamic";
    unsigned int idCond = 0;
    bool condNameFound = false;
    while (not condNameFound) {
        if (existDynamicFromDoc(condName)) {
            idCond ++;
            condName = "NewDynamic";
            condName += "_";
            condName += QVariant(idCond).toString();
        } else {
            condNameFound = true;
        }
    }
    return condName;
}

QString
vleVpz::newViewNameToDoc() const
{
    QString condView = "NewView";
    unsigned int idView = 0;
    bool viewNameFound = false;
    while (not viewNameFound) {
        if (existViewFromDoc(condView)) {
            idView ++;
            condView = "NewView";
            condView += "_";
            condView += QVariant(idView).toString();
        } else {
            viewNameFound = true;
        }
    }
    return condView;
}

QString
vleVpz::newCondNameToDoc() const
{
    QString condName = "NewCondition";
    unsigned int idCond = 0;
    bool condNameFound = false;
    while (not condNameFound) {
        if (existCondFromDoc(condName)) {
            idCond ++;
            condName = "NewCondition";
            condName += "_";
            condName += QVariant(idCond).toString();
        } else {
            condNameFound = true;
        }
    }
    return condName;
}

QString
vleVpz::newObsPortNameToDoc(const QString& obsName) const
{
    QString portName = "NewPort";
    unsigned int idPort = 0;
    bool portNameFound = false;
    while (not portNameFound) {
        if (existObsPortFromDoc(obsName, portName)) {
            idPort ++;
            portName = "NewPort";
            portName += "_";
            portName += QVariant(idPort).toString();
        } else {
            portNameFound = true;
        }
    }
    return portName;
}

QString
vleVpz::newObsNameToDoc() const
{
    QString obsName = "NewObservable";
    unsigned int idObs = 0;
    bool obsNameFound = false;
    while (not obsNameFound) {
        if (existObsFromDoc(obsName)) {
            idObs ++;
            obsName = "NewObservable";
            obsName += "_";
            obsName += QVariant(idObs).toString();
        } else {
            obsNameFound = true;
        }
    }
    return obsName;
}

QString
vleVpz::newCondPortNameToDoc(const QString& condName) const
{
    QString portName = "NewPort";
    unsigned int idPort = 0;
    bool portNameFound = false;
    while (not portNameFound) {
        if (existPortFromDoc(condName, portName)) {
            idPort ++;
            portName = "NewPort";
            portName += "_";
            portName += QVariant(idPort).toString();
        } else {
            portNameFound = true;
        }
    }
    return portName;
}

QDomNodeList
vleVpz::obsPortsListFromDoc(const QString& obsName) const
{
    return portsListFromObs(obsFromObss(obsFromViews(viewsFromDoc()),
                                        obsName));
}

void
vleVpz::addObservableToDoc(const QString& obsName)
{
    QDomNode observables = obsFromDoc();
    undoStack->snapshot(observables);
    QDomNodeList obss = obssListFromObss(observables);
    for (unsigned int i=0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == obsName)){
            qDebug() << ("Internal error in addObservable (already here)");
        }
    }
    QDomElement elem = getDomDoc().createElement("observable");
    elem.setAttribute("name", obsName);
    observables.appendChild(elem);

    emit observablesUpdated();
}

void
vleVpz::addViewToDoc(const QString& viewName)
{
    QDomNode views = viewsFromDoc();
    undoStack->snapshot(views);
    if (views.isNull()) {
        QDomNode exp = experimentFromDoc();
        QDomElement viewsTag = getDomDoc().createElement("views");
        exp.appendChild(viewsTag);
    }
    QDomNode view = addView(viewsFromDoc(),viewName);
    emit viewsUpdated();
}

void
vleVpz::rmViewToDoc(const QString& viewName)
{
    QDomNode views = viewsFromDoc();
    undoStack->snapshot(views);
    if (views.isNull()) {
        return;
    }
    QList<QDomNode> viewList = childNodesWithoutText(views, "view");
    for (int i=0; i<viewList.size(); i++) {
        QDomNode v = viewList[i];
        if (mVdo->attributeValue(v,"name") == viewName) {
            views.removeChild(v);
        }
    }
    QDomNode outputs = mVdo->obtainChild(views, "outputs", false);
    if (outputs.isNull()) {
        return;
    }
    QList<QDomNode> outList = childNodesWithoutText(outputs, "output");
    for (int i=0; i<outList.size(); i++) {
        QDomNode o = outList[i];
        if (mVdo->attributeValue(o,"name") == viewName) {
            outputs.removeChild(o);
        }
    }
    QDomNode observables = mVdo->obtainChild(views, "observables", false);
    if (observables.isNull()) {
        return;
    }
    QList<QDomNode> obsList = childNodesWithoutText(observables, "observable");
    for (int i=0; i<obsList.size(); i++) {
        QDomNode o = obsList[i];
        QList<QDomNode> portList = childNodesWithoutText(o, "port");
        for (int j=0; j<portList.size(); j++) {
            QDomNode p = portList[j];
            QList<QDomNode> atList = childNodesWithoutText(p, "attachedview");
            for (int k=0; k<atList.size(); k++) {
                QDomNode a = atList[k];
                if (mVdo->attributeValue(a,"name") == viewName) {
                    p.removeChild(a);
                }
            }
        }
    }
    emit viewsUpdated();
}



QDomNode
vleVpz::addConditionToDoc(const QString& condName)
{
    QDomNode conds = condsFromDoc();
    undoStack->snapshot(conds);
    if (conds.isNull()) {
        QDomNode exp = experimentFromDoc();
        QDomElement conditionsTag = getDomDoc().createElement("conditions");
        exp.appendChild(conditionsTag);
    }
    QDomNode cond = addCondition(condsFromDoc(),condName);
    emit conditionsUpdated();
    return cond;
}


QDomNode
vleVpz::addDynamicToDoc(const QString& dyn)
{
    QDomNodeList dynList = dynListFromDoc();
    if (existDynamicIntoDynList(dyn, dynList)) {
        qDebug() << ("Internal error in addDynamicToDoc (already here)");
        return QDomNode();
    }
    QDomElement elem = getDomDoc().createElement("dynamic");
    elem.setAttribute("name", dyn);
    dynamicsFromDoc().appendChild(elem);
    return elem;
}

QDomNode
vleVpz::addDynamicToDoc(const QString& dyn, const QString& pkgName,
        const QString& libName)
{
    undoStack->snapshot(dynamicsFromDoc());
    QDomNode newNode = addDynamicToDoc(dyn);
    setAttributeValue(newNode.toElement(), "package", pkgName);
    setAttributeValue(newNode.toElement(), "library", libName);
    return newNode;
}

void
vleVpz::configDynamicToDoc(const QString& dyn, const QString& pkgName,
        const QString& libName)
{
    QDomNode dynNode = dynamicFromDoc(dyn);
    if (dynNode.isNull()) {
        qDebug() << "Internal error in configDynamicToDoc (not found)";
        return ;
    }
    undoStack->snapshot(dynNode);
    setAttributeValue(dynNode.toElement(), "package", pkgName);
    setAttributeValue(dynNode.toElement(), "library", libName);
}

QDomNode
vleVpz::copyDynamicToDoc(const QString& dyn, const QString& newDyn)
{
    QDomNode dynNode = dynamicFromDoc(dyn);
    QString pkg = mVdo->attributeValue(dynNode, "package");
    QString lib = mVdo->attributeValue(dynNode, "library");
    QDomNode newNode = addDynamicToDoc(newDyn);
    setAttributeValue(newNode.toElement(),"package", pkg);
    setAttributeValue(newNode.toElement(),"library", lib);
    return newNode;
}

void
vleVpz::addObsPortToDoc(const QString& obsName, const QString& portName)
{
    QDomNode observables = obsFromDoc();
    undoStack->snapshot(observables);
    QDomNode obs = obsFromObss(observables, obsName);
    QDomNodeList obss = portsListFromObs(obs);
    for (unsigned int i=0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)){
            qDebug() << ("Internal error in addPort (already here)");
            return;
        }
    }
    QDomElement elem = getDomDoc().createElement("port");
    elem.setAttribute("name", portName);
    obs.appendChild(elem);

    emit observablesUpdated();
}

QDomNode
vleVpz::addCondPortToDoc(const QString& condName, const QString& portName)
{
    QDomNode node = condFromConds(condsFromDoc(), condName);
    undoStack->snapshot(node);
    QDomNodeList conds = portsListFromCond(node);
    for (unsigned int i=0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)){
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
    QDomNode conds = condsFromDoc();
    undoStack->snapshot(conds);
    rmCondFromConds(conds, condName);
}

bool
vleVpz::existViewFromObsPortDoc(const QString& obsName, const QString& portName,
                                const QString& viewName) const
{
    QDomNode obss = obsFromDoc();
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode toBeChecked = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(toBeChecked);
    for (unsigned int i=0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)){
            return true;
        }
    }
    return false;
}

void
vleVpz::rmViewToObsPortDoc(const QString& obsName, const QString& portName,
                           const QString& viewName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode port = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(port);
    for (unsigned int i=0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)){
            port.removeChild(child);
        }
    }

    emit observablesUpdated();
}

void
vleVpz::attachViewToObsPortDoc(const QString& obsName, const QString& portName,
                               const QString& viewName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
    QDomNode obs = obsFromObss(obss, obsName);

    QDomNode toBeAttachedTo = portFromObs(obs, portName);
    QDomNodeList views = viewsListFromObsPort(toBeAttachedTo);
    for (unsigned int i=0; i < views.length(); i++) {
        QDomNode child = views.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == viewName)){
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
    QDomNode obs = obsFromObss(obss, obsName);
    QDomNode toRm = portFromObs(obs, portName);
    obs.removeChild(toRm);

    emit observablesUpdated();
}

void
vleVpz::rmCondPortToDoc(const QString& condName, const QString& portName)
{
    rmPortFromCond(condFromConds(condsFromDoc(), condName), portName);
}

void
vleVpz::addValuePortCondToDoc(const QString& condName, const QString& portName,
        const vle::value::Value& val)
{
    QDomNode port = portFromDoc(condName, portName);
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

void
vleVpz::rmValuePortCondToDoc(const QString& condName, const QString& portName,
        int index)
{
    if (index >= 0) {
        QDomNode port = portFromDoc(condName, portName);
        undoStack->snapshot(port);
        if (port.childNodes().length() > (unsigned int) index ) {
            QDomNode n = port.childNodes().at(index);
            port.removeChild(n);
        }
    }
}

void
vleVpz::rmObservableFromDoc(const QString &obsName)
{
    QDomNode obss = obsFromDoc();
    undoStack->snapshot(obss);
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
vleVpz::rmObservableFromModel(QDomNode &node, const QString &obsName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in rmObservableFromModel (wrong main tag)");
        return;
    }
    QDomNode currModel =
        node.toElement().elementsByTagName("model").at(0);
    QString currName = mVdo->attributeValue(currModel, "name");

    QDomNodeList list = currModel.childNodes();
    for (unsigned int j = 0; j < list.length(); j++) {
        QDomNode item = list.item(j);
        if (item.nodeName() == "submodels") {
            QDomNodeList list = item.childNodes();
            for (unsigned int k = 0; k < list.length(); k++) {
                QDomNode item = list.item(k);
                if (item.nodeName() == "model") {
                    rmObservableFromModel(item, obsName);
                }
            }
        } else {
            unsetObsFromAtomicModel(currModel, obsName);
        }
    }

    emit observablesUpdated();
}

void
vleVpz::renameObservableFromModel(QDomNode &node, const QString &oldName, const QString &newName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in rmObservableFromModel (wrong main tag)");
        return;
    }
    QDomNode currModel =
        node.toElement().elementsByTagName("model").at(0);
    QString currName = mVdo->attributeValue(currModel, "name");

    QDomNodeList list = currModel.childNodes();
    for (unsigned int j = 0; j < list.length(); j++) {
        QDomNode item = list.item(j);
        if (item.nodeName() == "submodels") {
            QDomNodeList list = item.childNodes();
            for (unsigned int k = 0; k < list.length(); k++) {
                QDomNode item = list.item(k);
                if (item.nodeName() == "model") {
                    renameObservableFromModel(item, oldName, newName);
                }
            }
        } else {
            if (mVdo->attributeValue(currModel, "observables") == oldName) {
                setObsToAtomicModel(currModel, newName);
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
        qDebug() << ("Internal error in viewsListFromObsPort (wrong main tag)");
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
    for (unsigned int i=0; i < obss.length(); i++){
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == obsName)){
            return child;
        }
    }
    qDebug() << ("Internal error in obsFromObss (not found)") ;

    return QDomNode();
}

QDomNode
vleVpz::condFromConds(const QDomNode& node, const QString& condName) const
{
    QDomNodeList conds = condsListFromConds(node);
    for (unsigned int i=0; i < conds.length(); i++){
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == condName)){
            return child;
        }
    }
    qDebug() << ("Internal error in condFromConds (not found)");
    return QDomNode();
}

QDomNode
vleVpz::portFromObs(const QDomNode& node, const QString& portName) const
{
    QDomNodeList obss = portsListFromObs(node);
    for (unsigned int i=0; i < obss.length(); i++) {
        QDomNode child = obss.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)){
            return child;
        }
    }
    qDebug() << "Internal error in portFromObs (not found)" << portName;
    return QDomNode();
}

QDomNode
vleVpz::portFromCond(const QDomNode& node, const QString& portName) const
{
    QDomNodeList conds = portsListFromCond(node);
    for (unsigned int i=0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == portName)){
            return child;
        }
    }
    qDebug() << "Internal error in portFromCond (not found)" << portName;
    return QDomNode();
}

bool
vleVpz::existPortFromObs(const QDomNode& node, const QString& portName) const
{
    QDomNodeList portList = portsListFromObs(node);
    for (unsigned int i=0; i< portList.length(); i++) {
       QDomNode port = portList.at(i);
       if (mVdo->attributeValue(port, "name") == portName) {
           return true;
       }
    }
    return false;
}

bool
vleVpz::existPortFromCond(const QDomNode& node, const QString& portName) const
{
    QDomNodeList portList = portsListFromCond(node);
    for (unsigned int i=0; i< portList.length(); i++) {
       QDomNode port = portList.at(i);
       if (mVdo->attributeValue(port, "name") == portName) {
           return true;
       }
    }
    return false;
}


void
vleVpz::renameModel(QDomNode node, const QString& newName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in renameModel (wrong main tag)");
        return;
    }

    QString oldName = mVdo->attributeValue(node, "name");
    //rename connections at the coupled level
    if (node.parentNode().parentNode().nodeName() == "model") {
        undoStack->snapshot(node.parentNode().parentNode());//snapshot of the coupled
        QDomNode parent = node.parentNode().parentNode();
        QList<QDomNode> consTag =
                childNodesWithoutText(parent, "connections");
        if (consTag.size() == 1) {
            QList<QDomNode> cons =
                    childNodesWithoutText(consTag.at(0), "connection");
            for (int i=0; i<cons.size();i++) {
                QDomNode con = cons.at(i);
                QDomNode orig = con.toElement()
                        .elementsByTagName("origin").at(0);
                QDomNode dest = con.toElement()
                        .elementsByTagName("destination").at(0);
                if (mVdo->attributeValue(con, "type") == "input") {
                    if (mVdo->attributeValue(dest, "model") == oldName) {
                        setAttributeValue(dest.toElement(), "model", newName);
                    }
                } else if (mVdo->attributeValue(con, "type") == "output") {
                    if (mVdo->attributeValue(orig, "model") == oldName) {
                        setAttributeValue(orig.toElement(), "model", newName);
                    }
                } else if (mVdo->attributeValue(con, "type") == "internal") {
                    if (mVdo->attributeValue(dest, "model") == oldName) {
                        setAttributeValue(dest.toElement(), "model", newName);
                    }
                    if (mVdo->attributeValue(orig, "model") == oldName) {
                        setAttributeValue(orig.toElement(), "model", newName);
                    }
                }
            }
        }
    } else {
        undoStack->snapshot(node.parentNode());//snapshot of the container of
                                               // the current model only
    }
    //rename connections at the submodels level
    {
        QList<QDomNode> consTag =
                childNodesWithoutText(node, "connections");
        if (consTag.size() == 1) {
            QList<QDomNode> cons =
                    childNodesWithoutText(consTag.at(0), "connection");
            for (int i=0; i<cons.size();i++) {
                QDomNode con = cons.at(i);
                QDomNode orig = con.toElement()
                                            .elementsByTagName("origin").at(0);
                QDomNode dest = con.toElement()
                                            .elementsByTagName("destination").at(0);
                if (mVdo->attributeValue(con, "type") == "input") {
                    if (mVdo->attributeValue(dest, "model") == oldName) {
                        setAttributeValue(dest.toElement(), "model", newName);
                    }
                } else if (mVdo->attributeValue(con, "type") == "output") {
                    if (mVdo->attributeValue(orig, "model") == oldName) {
                        setAttributeValue(orig.toElement(), "model", newName);
                    }
                } else if (mVdo->attributeValue(con, "type") == "internal") {
                    if (mVdo->attributeValue(dest, "model") == oldName) {
                        setAttributeValue(dest.toElement(), "model", newName);
                    }
                    if (mVdo->attributeValue(orig, "model") == oldName) {
                        setAttributeValue(orig.toElement(), "model", newName);
                    }
                }
            }
        }
    }
    setAttributeValue(node.toElement(), "name", newName);
    emit modelsUpdated();
}

void
vleVpz::rmModel(QDomNode node)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in rmModel (wrong main tag)");
        return;
    }
    QString modelName = mVdo->attributeValue(node, "name");
    QDomNode parent = node.parentNode();//either structures, class or submodels
    if (parent.nodeName() == "submodels") {
        undoStack->snapshot(parent.parentNode());
    } else {
        undoStack->snapshot(parent);
    }

    QDomNode conTag = connectionsFromModel(parent.parentNode());
    QList<QDomNode> toRemove;
    if (not conTag.isNull()) {
        //remove connections at the coupled level
        QDomNodeList conList = conTag.toElement()
                .elementsByTagName("connection");
        for (unsigned int i=0; i< conList.length(); i++) {
            QDomNode con = conList.at(i);
            QDomNode orig = con.toElement().elementsByTagName("origin").at(0);
            QDomNode dest = con.toElement()
                                .elementsByTagName("destination").at(0);
             if ((mVdo->attributeValue(con, "type") == "input")  and
                (mVdo->attributeValue(dest, "model") == modelName)) {
                toRemove.append(con);
            } else if ((mVdo->attributeValue(con, "type") == "output")  and
                (mVdo->attributeValue(orig, "model") == modelName)) {
                toRemove.append(con);
            } else if ((mVdo->attributeValue(con, "type") == "internal")  and
                    ((mVdo->attributeValue(orig, "model") == modelName) or
                     (mVdo->attributeValue(dest, "model") == modelName))) {
                toRemove.append(con);
            }
        }
    }
    for (int i=0; i< toRemove.length(); i++) {
        toRemove.at(i).parentNode().removeChild(toRemove.at(i));
    }
    node.parentNode().removeChild(node);
    removeTextChilds(parent);
    removeTextChilds(parent.parentNode());
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
    }
    parent.removeChild(node);
    removeTextChilds(parent);
}

bool
vleVpz::existSubModel(QDomNode node, const QString& modName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in addModel (wrong main tag)");
        return false;
    }
    QList<QDomNode> chs = childNodesWithoutText(node, "submodels");
    if (chs.size() == 1) {
        QList<QDomNode> mods = childNodesWithoutText(chs[0], "model");
        for (int i=0; i<mods.size(); i++) {

            if (mVdo->attributeValue(mods[i], "name") == modName) {
                return true;
            }
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

    QDomNodeList subList = subModels.elementsByTagName("model");
    QString newModelName = "NewModel";
    bool found = false;
    unsigned int idInPort = 0;
    while (not found) {
        found = true;
        for (unsigned int i=0; i< subList.length(); i++) {
            if (mVdo->attributeValue(subList.at(i), "name") == newModelName) {
                found = false;
            }
        }
        if (not found) {
            idInPort++;
            newModelName = "NewModel";
            newModelName += "_";
            newModelName += QVariant(idInPort).toString();
        }
    }
    QDomNode newModel = getDomDoc().createElement("model");
    if (type == "coupled") {
        setAttributeValue(newModel.toElement(),"height","50");
        setAttributeValue(newModel.toElement(),"width","50");
        setAttributeValue(newModel.toElement(),"y",
                QVariant(pos.y()).toString());
        setAttributeValue(newModel.toElement(),"x",
                QVariant(pos.x()).toString());
        setAttributeValue(newModel.toElement(),"type","coupled");
        setAttributeValue(newModel.toElement(),"name",newModelName);
    } else {
        setAttributeValue(newModel.toElement(),"height","50");
        setAttributeValue(newModel.toElement(),"width","50");
        setAttributeValue(newModel.toElement(),"y",
                QVariant(pos.y()).toString());
        setAttributeValue(newModel.toElement(),"x",
                QVariant(pos.x()).toString());
        setAttributeValue(newModel.toElement(),"observables","");
        setAttributeValue(newModel.toElement(),"dynamics","");
        setAttributeValue(newModel.toElement(),"conditions","");
        setAttributeValue(newModel.toElement(),"type","atomic");
        setAttributeValue(newModel.toElement(),"name",newModelName);
    }
    QDomNode inPort = getDomDoc().createElement("in");
    QDomNode outPort = getDomDoc().createElement("out");
    newModel.appendChild(inPort);
    newModel.appendChild(outPort);
    subModels.appendChild(newModel);
    emit modelsUpdated();
}


void
vleVpz::copyModelsToModel(QList<QDomNode> modsToCopy, QDomNode modDest,
        qreal xtranslation, qreal ytranslation)
{
    QDomNode parent;
    //check all parents of modsToCopy are the same
    for (int i =0; i<modsToCopy.size(); i++) {
        if (i == 0) {
            parent = modsToCopy.at(i).parentNode().parentNode();
        } else {
            if (not (modsToCopy.at(i).parentNode().parentNode() ==
                    parent)) {
                qDebug() << ("Internal error in copyModelsToModel");
                return;
            }
        }
    }
    //check destination is a coupled model
    if (modDest.nodeName() != "model" or
            mVdo->attributeValue(modDest, "type") != "coupled") {
        qDebug() << ("Internal error in copyModelsToModel (wrong main tag)");
        return;
    }
    //build copies of models
    QList<QString> storeCopyNames;
    QList<QString> storeOrigNames;
    {
        for (int i=0; i<modsToCopy.size(); i++) {
            QDomNode sub = modsToCopy.at(i).cloneNode(true);
            QString subNamePrefix = mVdo->attributeValue(sub, "name");
            storeOrigNames.append(subNamePrefix);
            QString subName;
            int k = 0;
            bool found = false;
            while (not found) {
                subName = subNamePrefix;
                if (k >0) {
                    subName += "_";
                    subName += QVariant(k).toString();
                }
                if (not existSubModel(modDest, subName)) {
                    found = true;
                    renameModel(sub, subName);
                    setAttributeValue(sub.toElement(), "x",
                            QVariant(mVdo->attributeValue(sub, "x").toDouble()
                                    + xtranslation).toString());
                    setAttributeValue(sub.toElement(), "y",
                            QVariant(mVdo->attributeValue(sub, "y").toDouble()
                                    + ytranslation).toString());
                } else {
                    k ++;
                }
            }
            storeCopyNames.append(subName);
            QDomNode subs = mVdo->obtainChild(modDest, "submodels");
            subs.appendChild(sub);
        }
    }
    //add Internal connections to destination model
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
                childNodesWithoutText(consTagOrig, "connection");
        for (int i =0; i<consOrig.size(); i++) {
            QDomNode con = consOrig[i];
            if (mVdo->attributeValue(con, "type") == "internal") {
                QDomNode origPort = con.toElement()
                        .elementsByTagName("origin").at(0);
                QDomNode destPort = con.toElement()
                        .elementsByTagName("destination").at(0);
                if (storeOrigNames.contains(mVdo->attributeValue(origPort, "model"))
                    and storeOrigNames.contains(
                            mVdo->attributeValue(destPort, "model"))){
                    QDomNode conClone = con.cloneNode(true);
                    origPort = conClone.toElement()
                            .elementsByTagName("origin").at(0);
                    destPort = conClone.toElement()
                            .elementsByTagName("destination").at(0);
                    int idOrig = storeOrigNames.indexOf(
                            mVdo->attributeValue(origPort, "model"), 0);
                    int idDest = storeOrigNames.indexOf(
                            mVdo->attributeValue(destPort, "model"), 0);
                    setAttributeValue(origPort.toElement(), "model",
                            storeCopyNames[idOrig]);
                    setAttributeValue(destPort.toElement(), "model",
                            storeCopyNames[idDest]);
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

    QString oldName = mVdo->attributeValue(node, "name");
    QString portType = node.parentNode().nodeName();
    QDomNode mod = node.parentNode().parentNode();
    if (mod.nodeName() != "model") {
        qDebug() << ("Internal error in renameModelPort (wrong parent)");
        return;
    }
    QString modName = mVdo->attributeValue(mod, "name" );
    if (mod.parentNode().nodeName() == "submodels") {
        //adapt connections at the coupled level
        undoStack->snapshot(mod.parentNode().parentNode());
        QDomNode parConns = connectionsFromModel(mod.parentNode().parentNode());
        if (not parConns.isNull()) {
            QList<QDomNode>  conList = childNodesWithoutText(
                    parConns,"connection");
            for (int i=0; i< conList.size(); i++) {
                QDomNode con = conList[i];
                if (portType == "in" ) {
                    if ((mVdo->attributeValue(con, "type") == "input") or
                            (mVdo->attributeValue(con, "type") == "internal")) {
                        QDomNode dest = con.toElement()
                                                .elementsByTagName("destination").at(0);
                        if ((mVdo->attributeValue(dest, "model") == modName) and
                                (mVdo->attributeValue(dest, "port") == oldName)) {
                            setAttributeValue(dest.toElement(), "port", newName);
                        }
                    }
                } else if (portType == "out") {
                    if ((mVdo->attributeValue(con, "type") == "output") or
                            (mVdo->attributeValue(con, "type") == "internal")) {
                        QDomNode orig = con.toElement()
                                                .elementsByTagName("origin").at(0);
                        if ((mVdo->attributeValue(orig, "model") == modName) and
                                (mVdo->attributeValue(orig, "port") == oldName)) {
                            setAttributeValue(orig.toElement(), "port", newName);
                        }
                    }
                }
            }
        }
    } else {
        undoStack->snapshot(node.parentNode().parentNode());
    }
    if (not connectionsFromModel(mod).isNull()){
        //adapt the connections at internal level (if submodels)
        QList<QDomNode> conList = childNodesWithoutText(
                connectionsFromModel(mod),"connection");
        for (int i=0; i< conList.size(); i++) {
            QDomNode con = conList[i];
            if (portType == "in" ) {
                if (mVdo->attributeValue(con, "type") == "input") {
                    QDomNode orig = con.toElement()
                                       .elementsByTagName("origin").at(0);
                    if ((mVdo->attributeValue(orig, "model") == modName) and
                            (mVdo->attributeValue(orig, "port") == oldName)) {
                        setAttributeValue(orig.toElement(), "port", newName);
                    }
                }
            } else if (portType == "out") {
                if (mVdo->attributeValue(con, "type") == "output") {
                    QDomNode dest = con.toElement()
                                       .elementsByTagName("destination").at(0);
                    if ((mVdo->attributeValue(dest, "model") == modName) and
                            (mVdo->attributeValue(dest, "port") == oldName)) {
                        setAttributeValue(dest.toElement(), "port", newName);
                    }
                }
            }
        }
    }
    setAttributeValue(node.toElement(), "name", newName);
    emit modelsUpdated();
}

void
vleVpz::rmModelPort(QDomNode node)
{
    if (node.nodeName() != "port") {
        qDebug() << ("Internal error in rmModelPort (wrong main tag)");
        return;
    }
    QString oldName = mVdo->attributeValue(node, "name");
    QString portType = node.parentNode().nodeName();
    QDomNode parent = node.parentNode().parentNode();
    if (parent.nodeName() != "model") {
        qDebug() << ("Internal error in rmModelPort (wrong parent)");
        return;
    }
    QString ownerModel = mVdo->attributeValue(parent, "name" );
    if (parent.parentNode().nodeName() == "submodels") {
        undoStack->snapshot(parent.parentNode().parentNode());
        //adapt connections at the coupled level
        QDomNodeList conList = parent.parentNode().parentNode().toElement()
                    .elementsByTagName("connections").at(0)
                    .toElement().elementsByTagName("connection");
        for (unsigned int i=0; i< conList.length(); i++) {
            QDomNode con = conList.at(i);
            if (portType == "in" ) {
                if ((mVdo->attributeValue(con, "type") == "input") or
                    (mVdo->attributeValue(con, "type") == "internal")) {
                    QDomNode dest = con.toElement()
                            .elementsByTagName("destination").at(0);
                    if ((mVdo->attributeValue(dest, "model") == ownerModel) and
                            (mVdo->attributeValue(dest, "port") == oldName)) {
                        rmModelConnection(con, false);
                    }
                }
            } else if (portType == "out") {

                if ((mVdo->attributeValue(con, "type") == "output") or
                    (mVdo->attributeValue(con, "type") == "internal")) {
                    QDomNode orig = con.toElement()
                            .elementsByTagName("origin").at(0);
                    if ((mVdo->attributeValue(orig, "model") == ownerModel) and
                            (mVdo->attributeValue(orig, "port") == oldName)) {
                        rmModelConnection(con, false);
                    }
                }
            }
        }
    } else {
        undoStack->snapshot(parent);
    }
    QDomNodeList connSubModels = parent.toElement()
            .elementsByTagName("connections");
    if (connSubModels.length() == 1){
        //adapt the connections at internal level (if submodels)
        QDomNodeList conList = connSubModels.at(0).toElement()
                .elementsByTagName("connection");
        for (unsigned int i=0; i< conList.length(); i++) {
            QDomNode con = conList.at(i);
            if (portType == "in" ) {
                if (mVdo->attributeValue(con, "type") == "input") {
                    QDomNode orig = con.toElement()
                                       .elementsByTagName("origin").at(0);
                    if ((mVdo->attributeValue(orig, "model") == ownerModel) and
                            (mVdo->attributeValue(orig, "port") == oldName)) {
                        rmModelConnection(con, false);
                    }
                }
            } else if (portType == "out") {
                if (mVdo->attributeValue(con, "type") == "output") {
                    QDomNode dest = con.toElement()
                                       .elementsByTagName("destination").at(0);
                    if ((mVdo->attributeValue(dest, "model") == ownerModel) and
                            (mVdo->attributeValue(dest, "port") == oldName)) {
                        rmModelConnection(con, false);
                    }
                }
            }
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

    QDomNode portsType = mVdo->obtainChild(node, type);
    undoStack->snapshot(portsType);

    QDomNodeList ports = portsType.toElement().elementsByTagName("port");
    bool found = false;
    unsigned int idInPort = 0;
    while (not found) {
        found = true;
        for (unsigned int i=0; i< ports.length(); i++) {
            if (mVdo->attributeValue(ports.at(i), "name") == newPortName) {
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
    QString model1name = mVdo->attributeValue(model1, "name");
    QString model2name = mVdo->attributeValue(model2, "name");
    QDomNode coupled1;
    QDomNode coupled2;
    if (model1.parentNode().nodeName() == "submodels") {
         coupled1 = model1.parentNode().parentNode();
    }
    if (model2.parentNode().nodeName() == "submodels") {
        coupled2 = model2.parentNode().parentNode();
    }
    if ((coupled1 == coupled2) and (node1type == "out")
            and (node2type == "in")) {
        QDomNode cons = connectionsFromModel(coupled1);
        if (cons.isNull()) {
            QDomElement c = getDomDoc().createElement("connections");
            coupled1.appendChild(c);
        }
        cons = connectionsFromModel(coupled1);
        undoStack->snapshot(cons);
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "internal");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model",model1name);
        orig.setAttribute("port",mVdo->attributeValue(node1,"name"));
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model",model2name);
        dest.setAttribute("port",mVdo->attributeValue(node2,"name"));
        con.appendChild(dest);
        cons.appendChild(con);
    } else if ((model1 == coupled2) and (node1type == "in")
            and (node2type == "in")) {
        QDomNode cons = connectionsFromModel(coupled2);
        if (cons.isNull()) {
            QDomElement c = getDomDoc().createElement("connections");
            coupled2.appendChild(c);
        }
        undoStack->snapshot(cons);
        cons = connectionsFromModel(coupled2);
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "input");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model",model1name);
        orig.setAttribute("port",mVdo->attributeValue(node1,"name"));
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model",model2name);
        dest.setAttribute("port",mVdo->attributeValue(node2,"name"));
        con.appendChild(dest);
        cons.appendChild(con);
    } else if ((coupled1 == model2)  and (node1type == "out")
            and (node2type == "out")) {
        QDomNode cons = connectionsFromModel(coupled1);
        if (cons.isNull()) {
            QDomElement c = getDomDoc().createElement("connections");
            coupled1.appendChild(c);
        }
        cons = connectionsFromModel(coupled1);
        undoStack->snapshot(cons);
        QDomElement con = getDomDoc().createElement("connection");
        con.setAttribute("type", "output");
        QDomElement orig = getDomDoc().createElement("origin");
        orig.setAttribute("model",model1name);
        orig.setAttribute("port",mVdo->attributeValue(node1,"name"));
        con.appendChild(orig);
        QDomElement dest = getDomDoc().createElement("destination");
        dest.setAttribute("model",model2name);
        dest.setAttribute("port",mVdo->attributeValue(node2,"name"));
        con.appendChild(dest);
        cons.appendChild(con);
    }
    emit modelsUpdated();


}

void
vleVpz::rmCondFromConds(QDomNode node, const QString& condName)
{
    QDomNode toRm = condFromConds(node, condName);
    node.removeChild(toRm);
    emit conditionsUpdated();
}

void
vleVpz::rmPortFromCond(QDomNode node, const QString& portName)
{
    QDomNode toRm = portFromCond(node, portName);
    node.removeChild(toRm);
}

void
vleVpz::setWidthToModel(QDomNode node, double width)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setWidthToModel");
        return ;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setWidthToModel",
                forUndoMerge);
    }
    setAttributeValue(node.toElement(),"width", QVariant(width).toString());
}

void
vleVpz::setHeightToModel(QDomNode node, double height)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setHeightToModel");
        return ;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setHeightToModel",
                forUndoMerge);
    }
    setAttributeValue(node.toElement(),"height", QVariant(height).toString());
}

void
vleVpz::setPositionToModel(QDomNode node, double x, double y)
{
    if (node.nodeName() != "model") {
        qDebug() << ("wrong main tag in setPositionToModel");
        return ;
    }
    {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        vle::value::Set& qs = forUndoMerge->addSet("queries");
        qs.addString(mVdo->getXQuery(node).toStdString());
        undoStack->snapshot(node, "setPositionToModel",
                forUndoMerge);
    }
    setAttributeValue(node.toElement(), "x", QVariant(x).toString());
    setAttributeValue(node.toElement(), "y", QVariant(y).toString());
}
void
vleVpz::setPositionToModel(QList<QDomNode>& nodes, QList<double> xs,
        QList<double> ys)
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
        for (int i =1; i<size; i++) {
            QDomNode nodei = nodes[i];
            QString nodeLoci = mVdo->getXQuery(nodei);
            nodeLoci = subQuery(nodeLoci, 0, -1);
            if (nodeLocation != nodeLoci) {
                qDebug() << ("Internal error setPositionToModel (location)");
                return;
            }
        }
        {//Undo command building
            vle::value::Map* forUndoMerge = new vle::value::Map();
            vle::value::Set& qs = forUndoMerge->addSet("queries");
            for (int i =0; i<size; i++) {
                QDomNode nodei = nodes[i];
                qs.addString(mVdo->getXQuery(nodei).toStdString());
            }
            undoStack->snapshot(nodes[0].parentNode(),
                    "setPositionToModel",
                    forUndoMerge);
        }

        for (int i =0; i<size; i++) {
            QDomNode nodei = nodes[i];
            setAttributeValue(nodei.toElement(), "x", QVariant(xs[i]).toString());
            setAttributeValue(nodei.toElement(), "y", QVariant(ys[i]).toString());
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
    //add output and configure it with vle.output/storage
    elem.setAttribute("output", viewName);
    elem.setAttribute("type","timed");
    elem.setAttribute("timestep", "1");
    QDomNode outputsNode = mVdo->obtainChild(
                            mVdo->obtainChild(
                             mVdo->obtainChild(
                              mVdo->obtainChild(mDoc, "vle_project", true),
                             "experiment", true),
                            "views", true),
                           "outputs", true);
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
    for (unsigned int i=0; i < conds.length(); i++) {
        QDomNode child = conds.item(i);
        if ((child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == condName)){
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
    return mVdo->attributeValue(atom, "conditions");
}

bool
vleVpz::isAttachedCond(const QString& model_query,
        const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    QString attachedConds = mVdo->attributeValue(atom, "conditions");
    QStringList condSplit = attachedConds.split(",");
    return condSplit.contains(condName);
}

QString
vleVpz::modelDynFromDoc(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    return mVdo->attributeValue(atom, "dynamics");
}

QString
vleVpz::modelObsFromDoc(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    return mVdo->attributeValue(atom, "observables");
}

void
vleVpz::setDynToAtomicModel(const QString& model_query, const QString& dyn,
        bool undo)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    if (undo) {
        undoStack->snapshot(atom);
    }
    setAttributeValue(atom.toElement(), "dynamics", dyn);
}

void
vleVpz::removeDyn(const QString& dyn)
{
    qDebug() << " vleVpz::removeDyn " << mVdo->getXQuery(getDomDoc()) << " " << dyn;
    undoStack->snapshot(vleProjectFromDoc());
    QDomNodeList nodeList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynamics = nodeList.item(0).toElement();
    dynamics.removeChild(dynamicFromDoc(dyn));
    QDomNodeList mods = getDomDoc().elementsByTagName("model");
    for (int i=0; i<mods.size(); i++) {
        QDomNode mod = mods.at(i);
        if ((mVdo->attributeValue(mod,"type") == "atomic") and
            (mVdo->attributeValue(mod,"dynamics") == dyn)) {
            setAttributeValue(mod.toElement(),"dynamics","");
        }
    }
}

void
vleVpz::attachCondToAtomicModel(const QString& model_query, const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    QString attachedConds = mVdo->attributeValue(atom, "conditions");
    QStringList condSplit = attachedConds.split(",");
    if (not condSplit.contains(condName)) {
        condSplit.append(condName);
        QString res ="";
        for (int i = 0; i < condSplit.length(); i++) {
            if (i > 0){
                res += ",";
            }
            res += condSplit.at(i);
        }
        setAttributeValue(atom.toElement(), "conditions", res);
        emit conditionsUpdated();
    }
}

void
vleVpz::detachCondToAtomicModel(const QString& model_query,
        const QString& condName)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    QString attachedConds = mVdo->attributeValue(atom, "conditions");
    QStringList condSplit = attachedConds.split(",");
    if (condSplit.contains(condName)) {
        condSplit.removeOne(condName);
        QString res ="";
        for (int i = 0; i < condSplit.length(); i++) {
            if (i > 0){
                res += ",";
            }
            res += condSplit.at(i);
        }
        setAttributeValue(atom.toElement(), "conditions", res);
        emit conditionsUpdated();
    }
}

void
vleVpz::setObsToAtomicModel(QDomNode &node, const QString& obsName)
{
     setAttributeValue(node.toElement(), "observables", obsName);

     emit observablesUpdated();
}

void
vleVpz::setObsToAtomicModel(const QString& model_query, const QString& obsName)
{
   QDomNode atom = mVdo->getNodeFromXQuery(model_query);
   setAttributeValue(atom.toElement(), "observables", obsName);

   emit observablesUpdated();
}

void
vleVpz::unsetObsFromAtomicModel(const QString& model_query)
{
    QDomNode atom = mVdo->getNodeFromXQuery(model_query);
    setAttributeValue(atom.toElement(), "observables","");

    emit observablesUpdated();
}

void
vleVpz::unsetObsFromAtomicModel(QDomNode& node, const QString& obsName)
{
    if (node.nodeName() != "model") {
        qDebug() << ("Internal error in unsetObsFromAtomicModel (wrong main tag)");
        return;
    }
    if(mVdo->attributeValue(node, "observables") == obsName) {
        setAttributeValue(node.toElement(), "observables","");
    }

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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "output") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue()
                        == outputName)){
            return child;
        }
    }
    qDebug() << ("Internal error in outputFromOutputs (output not found)");
    return QDomNode();
}

vle::value::Map*
vleVpz::buildOutputConfigMap(const QString& outputName)
{
    QDomNode outputNode = outputFromOutputs(outputsFromViews(
                    viewsFromDoc()), outputName);
    QList<QDomNode> nodes = childNodesWithoutText(outputNode, "map");
    if (nodes.size() == 1) {
        vle::value::Value* val = buildValue(nodes.at(0), false);
        if (val->isMap()) {
            return dynamic_cast<vle::value::Map*>(val);
        } else {
            qDebug() << ("Internal error in buildOutputConfigMap (not a map)");
            delete val;
        }
    }
    return new vle::value::Map();
}

void
vleVpz::fillOutputConfigMap(const QString& outputName,
        const vle::value::Map& mapConfig)
{
    undoStack->snapshot(viewsFromDoc());
    QDomNode outputNode = outputFromOutputs(outputsFromViews(
                    viewsFromDoc()), outputName);
    QDomNode nodeMap = mVdo->obtainChild(outputNode, "map", true);
    fillWithValue(nodeMap, mapConfig);
}

void
vleVpz::viewObservableNames(std::vector<std::string>& v) const
{
    v.clear();
    QDomNode outputs = viewsFromDoc();
    QDomNodeList outputList = outputs.toElement().elementsByTagName("observable");
    for (unsigned int i =0; i < outputList.length(); i++) {
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
    for (unsigned int i =0; i < outputList.length(); i++) {
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
    for (unsigned int i =0; i < outputList.length(); i++) {
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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "view") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == viewName)){
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

QString
vleVpz::viewTypeFromDoc(const QString& viewName) const
{
    QDomNode node = viewFromDoc(viewName);
    if (node.nodeName() != "view") {
        qDebug() << ("Internal error in viewTypeFromDoc (wrong main tag)");
        return QString();
    }
    return node.attributes().namedItem("type").nodeValue();
}

void
vleVpz::setViewTypeToDoc(const QString& viewName, const QString& viewType)
{
    QDomNode viewsNode = viewsFromDoc();
    undoStack->snapshot(viewsNode);
    QDomNode viewNode = viewFromViews(viewsNode,viewName);
    viewNode.attributes().namedItem("type").setNodeValue(viewType);
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

void
vleVpz::setTimeStepToDoc(const QString& viewName, double ts)
{
    QDomNode node = viewFromDoc(viewName);
    undoStack->snapshot(viewsFromDoc());
    if (node.nodeName() != "view") {
        qDebug() << ("Internal error in setTimeStepFromView (wrong main tag)");
        return;
    }
    if (viewTypeFromDoc(viewName) == "timed" and ts > 0.0) {
        setAttributeValue(node.toElement(), "timestep",
                QVariant(ts).toString());
    }
}

bool
vleVpz::setOutputPluginToDoc(const QString& viewName,
        const QString& outputPlugin)
{
    QDomNode viewsNode = viewsFromDoc();
    undoStack->snapshot(viewsNode);
    QDomNode outputsNode = outputsFromViews(viewsNode);
    QDomNode outputNode = outputFromOutputs(outputsNode, viewName);
    QDomNode pkg = outputNode.attributes().namedItem("package");
    QDomNode plugin = outputNode.attributes().namedItem("plugin");
    QStringList ll = outputPlugin.split("/");
    pkg.setNodeValue(ll.at(0));
    plugin.setNodeValue(ll.at(1));
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

//enum type { BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
//    XMLTYPE, NIL, MATRIX, USER };

vle::value::Value::type
vleVpz::valueType(const QString& condName,
        const QString& portName, int index) const
{
    //    BOOLEAN, INTEGER, DOUBLE, STRING, SET, MAP, TUPLE, TABLE,
    //                XMLTYPE, NIL, MATRIX, USER
    QDomNode port = portFromDoc(condName, portName);
    QDomNode valNode = port.childNodes().at(index);
    if (valNode.nodeName() == "boolean") {
        return vle::value::Value::BOOLEAN;
    } else if(valNode.nodeName() == "integer") {
        return vle::value::Value::INTEGER;
    } else if(valNode.nodeName() == "double") {
        return vle::value::Value::DOUBLE;
    } else if(valNode.nodeName() == "string") {
        return vle::value::Value::STRING;
    } else if(valNode.nodeName() == "set") {
        return vle::value::Value::SET;
    } else if(valNode.nodeName() == "map") {
        return vle::value::Value::MAP;
    } else if(valNode.nodeName() == "tuple") {
        return vle::value::Value::TUPLE;
    } else if(valNode.nodeName() == "table") {
        return vle::value::Value::TABLE;
    } else if(valNode.nodeName() == "xml") {
        return vle::value::Value::XMLTYPE;
    } else if(valNode.nodeName() == "null") {
        return vle::value::Value::NIL;
    } else if(valNode.nodeName() == "matrix") {
        return vle::value::Value::MATRIX;
    }
    return vle::value::Value::USER;
}

vle::value::Value*
vleVpz::buildValue(const QDomNode& node, bool buildText) const
{

    if (node.nodeName() == "boolean") {
        if (node.childNodes().length() != 1){
            qDebug() << "Internal error in buildValue (6)";
            return 0;
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();;
        return new vle::value::Boolean(qv.toBool());
    }
    if (node.nodeName() == "integer") {
        if (node.childNodes().length() != 1){
            qDebug() << "Internal error in buildValue (5) "<< node.childNodes().length() <<" " <<mVdo->toQString(node);
            qDebug() << "Internal error in buildValue son0 "<< mVdo->toQString(node.childNodes().at(0));
            qDebug() << "Internal error in buildValue son1 "<< mVdo->toQString(node.childNodes().at(1));
            return 0;
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();;
        return new vle::value::Integer(qv.toInt());
    }
    if (node.nodeName() == "double") {
        if (node.childNodes().length() != 1){
            qDebug() << "Internal error in buildValue (10)" << mVdo->toQString(node);
            return 0;
        }
        QVariant qv = node.childNodes().item(0).toText().nodeValue();;
        return new vle::value::Double(qv.toDouble());
    }
    if (node.nodeName() == "string") {
        if (node.childNodes().length() == 0){
            return new vle::value::String("");
        }
        if (node.childNodes().length() == 1){
            QString qv = node.childNodes().item(0).toText().nodeValue();
            return new vle::value::String(qv.toStdString());
        }
        qDebug() << "Internal error in buildValue (4.3) "<<node.childNodes().length();
        return 0;

    }
    if (node.nodeName() == "tuple") {
        if (node.childNodes().length() > 1){
            qDebug() << "Internal error in buildValue (4)";
            return 0;
        }
        if (node.childNodes().length() == 0) {
            return new vle::value::Tuple(0, 0.0);
        } else {
            QString qv = node.childNodes().item(0).toText().nodeValue();
            QStringList vals = qv.split(" ");
            vle::value::Tuple* res = new vle::value::Tuple(vals.length());
            for (int i = 0; i<vals.size();i++) {
                (*res)[i] = QVariant(vals.at(i)).toDouble();
            }
            return res;
        }
    }
    if (node.nodeName() == "table") {
        if (node.childNodes().length() != 1){
            qDebug() << "Internal error in buildValue (14)" << mVdo->toQString(node);
            qDebug() << "Internal error in buildValue (14) length" << node.childNodes().length();
            return 0;
        }
        int width = QVariant(mVdo->attributeValue(node, "width")).toInt();
        int height = QVariant(mVdo->attributeValue(node, "height")).toInt();
        QString qv = node.childNodes().item(0).toText().nodeValue();
        vle::value::Table* res = new vle::value::Table(width, height);
        res->fill(qv.toStdString());
        return res;
    }
    if (node.nodeName() == "#text") {
        if (buildText) {
            return new vle::value::String(node.nodeValue().toStdString());
        } else {
            return 0;
        }
    }

    QList<QDomNode> chs = childNodesWithoutText(node);
    if (node.nodeName() == "set") {
        vle::value::Set* res = new vle::value::Set();
        for (int i=0; i < chs.size(); i++) {
            QDomNode child = chs[i];
            res->add(buildValue(child, buildText));
        }
        return res;
    }
    if (node.nodeName() == "map") {
        vle::value::Map* res = new vle::value::Map();
        for (int i=0; i < chs.size(); i++) {
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
            res->add(
                 child.attributes().namedItem("name").nodeValue().toStdString(),
                 buildValue(mapItemValue, buildText));
        }
        return res;
    }
    if (node.nodeName() == "matrix") {
        int columns = QVariant(mVdo->attributeValue(node, "columns")).toInt();
        int rows = QVariant(mVdo->attributeValue(node, "rows")).toInt();
        int columnmax = QVariant(mVdo->attributeValue(node, "columnmax")).toInt();
        int rowmax = QVariant(mVdo->attributeValue(node, "rowmax")).toInt();
        int columnstep = QVariant(mVdo->attributeValue(node, "columnstep")).toInt();
        int rowstep = QVariant(mVdo->attributeValue(node, "rowstep")).toInt();

        vle::value::Matrix* res = new vle::value::Matrix(columns,
                rows, columnmax, rowmax, columnstep, rowstep);
        if (chs.size() != (int) columns*rows) {
            qDebug() << "Internal error in buildValue (matrix)" << mVdo->toQString(node) ;
            qDebug() << "Internal error in buildValue (matrix) childLength=" << chs.size() ;
            qDebug() << "Internal error in buildValue (matrix) columns*row=" << (unsigned int) columns*rows ;

            return 0;
        }
        for (unsigned int i=0; i < (unsigned int)rows; i++) {
            for (unsigned int j=0; j< (unsigned int)columns; j++) {
                QDomNode child = chs[(j + (i*columns))];
                res->set(j,i, buildValue(child, buildText));
            }
        }
        return res;
    }
    qDebug() << "Internal error in buildValue (3): " << node.nodeName() << "\n" << mVdo->toQString(node);
    throw vle::utils::ArgError("");
    return 0;
}


vle::value::Value*
vleVpz::buildValueFromDoc(const QString& condName, const QString& portName,
        int valIndex) const
{
    QDomNode portNode = portFromDoc(condName, portName);
    return buildValue(portNode, valIndex);
}

vle::value::Value*
vleVpz::buildValue(const QDomNode& portNode, int valIndex) const
{
    QDomNodeList valueList = portNode.childNodes();
    int ii = 0;
    vle::value::Value* val;
    for (unsigned int k = 0; k < valueList.length(); k++) {
        val = buildValue(valueList.at(k), false);
        if (val) {
            if (valIndex == ii) {
                return val;
            } else {
                delete val;
                ii += 1 ;
            }
        }
    }
    qDebug() << "Internal error in buildValue_valIndex (8) \n";
    return 0;
}

unsigned int
vleVpz::nbValuesInPortFromDoc(const QString& condName, const QString& portName)
{
    QDomNode port = portFromDoc(condName, portName);
    return childNodesWithoutText(port).size();
}

bool
vleVpz::fillWithMultipleValueFromDoc(const QString& condName,
        const QString& portName, std::vector<vle::value::Value*>& values) const
{
    QDomNode port = portFromDoc(condName, portName);
    return fillWithMultipleValue(port, values);
}

bool
vleVpz::fillWithMultipleValue(QDomNode portNode,
        std::vector<vle::value::Value*>& values) const
{
    std::vector<vle::value::Value*>::iterator itb = values.begin();
    std::vector<vle::value::Value*>::iterator ite = values.end();
    for (; itb != ite; itb++) {
        delete *itb;
    }
    values.clear();


    QDomNodeList valueList = portNode.childNodes();
    vle::value::Value* val;
    for (unsigned int k = 0; k < valueList.length(); k++) {
        val = buildValue(valueList.at(k), false);
        if (val) {
           values.push_back(val);
        }
    }
    return true;
}

void
vleVpz::fillWithClassesFromDoc(std::vector<std::string>& toFill)
{
    toFill.clear();
    QDomNode classes = classesFromDoc();
    QList<QDomNode> classNodes = childNodesWithoutText(classes);
    for (int i=0; i< classNodes.size(); i++) {
        toFill.push_back(mVdo->attributeValue(
                classNodes[i], "name").toStdString());
    }
}

bool
vleVpz::fillWithValue(const QString& condName, const QString& portName,
        int index, const vle::value::Value& val)
{
    QDomNode port = portFromDoc(condName, portName);
    undoStack->snapshot(port);
    QList<QDomNode> chs = childNodesWithoutText(port);
    QDomNode valNode = chs[index];
    if (val.getType() != valueType(condName, portName, index)) {
        QDomNode parent = valNode.parentNode();
        parent.replaceChild(buildEmptyValueFromDoc(val.getType()), valNode);
        chs = childNodesWithoutText(port);
        valNode = chs[index];
    }
    fillWithValue(valNode, val);
    emit conditionsUpdated();
    return true;
}

bool
vleVpz::fillWithValue(QDomNode node, const vle::value::Value& val)
{
    switch (val.getType()) {
    case vle::value::Value::BOOLEAN:
    case vle::value::Value::INTEGER:
    case vle::value::Value::DOUBLE:
    case vle::value::Value::STRING:

    {
        if ( (val.isBoolean() and (node.nodeName() != "boolean")) or
                (val.isInteger() and (node.nodeName() != "integer")) or
                (val.isDouble() and (node.nodeName() != "double")) or
                (val.isString() and (node.nodeName() != "string"))) {
            return false;
        }
        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal = getDomDoc().createTextNode(
                    val.writeToString().c_str());
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())){
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(val.writeToString().c_str());
            return true;
        } else {
            qDebug() << "Internal error in fillWithValue (bool, string, integer)";
            return false;
        }
        break;
    } case vle::value::Value::SET: {
        if (node.nodeName() != "set") {
            return false;
        }
        removeAllChilds(node);
        vle::value::Set::const_iterator itb = val.toSet().begin();
        vle::value::Set::const_iterator ite = val.toSet().end();
        for (; itb != ite; itb++) {
            QDomElement elemVal = buildEmptyValueFromDoc(
                    (*itb)->getType());
            fillWithValue(elemVal, (**itb));
            node.appendChild(elemVal);
        }
        break;
    } case vle::value::Value::MAP: {
        if (node.nodeName() != "map") {
            return false;
        }
        removeAllChilds(node);
        vle::value::Map::const_iterator itb = val.toMap().begin();
        vle::value::Map::const_iterator ite = val.toMap().end();
        for (; itb != ite; itb++) {
            QDomElement elem = getDomDoc().createElement("key");
            elem.setAttribute("name", QString(itb->first.c_str()));
            QDomElement elemVal = buildEmptyValueFromDoc(
                    itb->second->getType());
            fillWithValue(elemVal, *(itb->second));
            elem.appendChild(elemVal);
            node.appendChild(elem);
        }
        break;
    }case vle::value::Value::TUPLE: {
        if (node.nodeName() != "tuple") {
            return false;
        }
        QString valTxt ="";
        const vle::value::Tuple& tuple = val.toTuple();
        for (unsigned int i=0; i < tuple.size(); i++) {
            if (i > 0){
                valTxt += " ";
            }
            valTxt += QVariant(tuple.at(i)).toString();
        }
        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal = getDomDoc().createTextNode(valTxt);
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())){
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(valTxt);
            return true;
        } else {
            qDebug() << "Internal error in fillWithValue (tuple)";
            return false;
        }
        break;
    } case vle::value::Value::TABLE: {
        if (node.nodeName() != "table") {
            return false;
        }
        QString strRepr = val.toTable().writeToString().c_str();
        strRepr.remove("(");
        strRepr.remove(")");
        strRepr.replace(",", " ");

        QDomNodeList childs = node.childNodes();
        if (childs.length() == 0) {
            QDomText elemVal = getDomDoc().createTextNode(strRepr);
            node.appendChild(elemVal);
            return true;
        }
        if ((childs.length() == 1) and (childs.at(0).isText())){
            QDomText elemVal = childs.at(0).toText();
            elemVal.setData(strRepr);
            return true;
        } else {

            qDebug() << "Internal error in fillWithValue (table)" << mVdo->toQString(childs.at(0));
            qDebug() << "Internal error in fillWithValue (table)" << childs.length();
            return false;
        }
        setAttributeValue(node.toElement(), "width",
                QVariant((int) val.toTable().width()).toString());
        setAttributeValue(node.toElement(), "height",
                QVariant((int) val.toTable().height()).toString());
        break;
    }  case vle::value::Value::MATRIX: {
        if (node.nodeName() != "matrix") {
            return false;
        }
        const vle::value::Matrix& mat = val.toMatrix();
        int rows = mat.rows();
        int columns = mat.columns();

        setAttributeValue(node.toElement(), "columns",
                QVariant(columns).toString());
        setAttributeValue(node.toElement(), "rows", QVariant(rows).toString());
        setAttributeValue(node.toElement(), "columnmax",
                QVariant(columns).toString());
        setAttributeValue(node.toElement(), "rowmax",
                QVariant(rows).toString());
        setAttributeValue(node.toElement(), "columnstep",
                QVariant((int) mat.resizeColumn()).toString());
        setAttributeValue(node.toElement(), "rowsstep",
                QVariant((int) mat.resizeColumn()).toString());
        removeAllChilds(node);
        for (unsigned int i = 0; i < (unsigned int)rows; i++) {
            for (unsigned int j = 0; j < (unsigned int)columns; j++) {
                const vle::value::Value* val = mat.get(j, i);//WARNING inversed
                QDomElement elemVal = buildEmptyValueFromDoc(val->getType());
                fillWithValue(elemVal, *val);
                node.appendChild(elemVal);
            }
        }
        break;
    } default: {
        qDebug() << "Internal error in fillWithValue (nnn)";
        return false;
    }}
    return true;
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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == key)){
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
        QString mess = "Internal error in existIntegerKeyInMap (wrong main tag)";
        mess += "\n got : ";
        mess += node.nodeName();
        qDebug() << (mess);
        return -1;
    }
    QDomNodeList childs = node.childNodes();
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == key)){
            QDomNodeList childsKey = child.childNodes();
            return (childsKey.length() > 0 and
                    isInteger(childsKey.item(0)));
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

    //update key if present
    QDomNodeList childs = node->childNodes();
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "key") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == key)){
            QDomElement newInt  = child.toDocument().createElement("integer");
            QDomText newIntVal  = newInt.toDocument().createTextNode(qv);
            newInt.appendChild(newIntVal);
            child.toElement().appendChild(newInt);
            return ;
        }
    }
    //add key if required
    QDomElement newKey = node->toDocument().createElement("key");
    newKey.setTagName("key");
    newKey.setAttribute("name", key);
    QDomElement newInt  = newKey.toDocument().createElement("integer");
    QDomText newIntVal  = newInt.toDocument().createTextNode(qv);
    newInt.appendChild(newIntVal);
    newKey.appendChild(newInt);
    node->appendChild(newKey);
}

void
vleVpz::removeAllChilds(QDomNode node)
{
    QDomNodeList childs = node.childNodes();
    while(node.hasChildNodes()) {
        node.removeChild(childs.item(0));
    }
}

QDomNode
vleVpz::getOutputFromViews(QDomNode node,
        const QString& outputName)
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
vleVpz::fillWithDynamicsList(QList <QString>& toFill) const
{
    toFill.clear();
    QDomNodeList nodeList = getDomDoc().elementsByTagName("dynamics");
    QDomElement dynamics = nodeList.item(0).toElement();
    QDomNodeList childs = dynamics.childNodes();
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
                (child.attributes().contains("name"))){
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
    for (unsigned int i=0; i < dynList.length(); i++) {
        QDomElement child = dynList.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == dyn)){
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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
                (child.attributes().contains("package")) and
                (child.attributes().namedItem("name").nodeValue() == dyn)){
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
    for (unsigned int i=0; i < childs.length(); i++) {
        QDomElement child = childs.item(i).toElement();
        if ((child.nodeName() == "dynamic") and
                (child.attributes().contains("library")) and
                (child.attributes().namedItem("name").nodeValue() == dyn)){
            return child.attributes().namedItem("library").nodeValue();
        }
    }
    return "";
}


void vleVpz::setBasePath(const QString path)
{
    mPath = path;
}


vlePackage*
vleVpz::getPackage()
{
    return mPackage;
}

void
vleVpz::setPackage(vlePackage* package)
{
    mPackage = package;
}

bool vleVpz::isAltered()
{
    return false;
}

void vleVpz::removeDynamic(const QString& dynamic)
{
    removeDyn(dynamic);
}



void vleVpz::save()
{
    QFile file(mFilename);
    if ( ! file.exists())
    {
        if ( ! file.open(QIODevice::WriteOnly))
        {
            qDebug() << "VPZ File (" << mFilename << ")can't be opened for write !";
            return;
        }
        file.close();
    }

    QDomDocument doc;
    xSaveDom(&doc);

    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = doc.toByteArray();
    file.write(xml);
    file.close();
}

QByteArray vleVpz::xGetXml()
{
    QDomDocument doc;
    xSaveDom(&doc);
    QByteArray xml = doc.toByteArray();
    return xml;
}

void vleVpz::xReadDom()
{
    QXmlInputSource source(&mFile);
    QXmlSimpleReader reader;
    mDoc.setContent(&source, &reader);
}

void vleVpz::xSaveDom(QDomDocument *doc)
{
    QDomProcessingInstruction pi;
    pi = doc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" ");
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

}}//namespaces
