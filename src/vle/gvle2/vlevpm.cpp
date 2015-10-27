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

#include "vlevpm.h"
#include "plugin_cond.h"

#include <QDebug>

namespace vle {
namespace gvle2 {


vleDomVpm::vleDomVpm(QDomDocument* doc): vleDomObject(doc)

{

}
vleDomVpm::~vleDomVpm()
{

}
QString
vleDomVpm::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    //element uniq in children
    if ((name == "condPlugins")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified wy attribute name
    if ((name == "condPlugin")){
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
                +attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomVpm::getNodeFromXQuery(const QString& query,
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
        if (curr != "vle_project_metadata" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return(getNodeFromXQuery(rest,rootNode));
    }

    //handle recursion with uniq node
    if ((curr == "condPlugins")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
    }
    //handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("condPlugin"));
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

vleVpm::vleVpm(const QString& vpzpath, const QString& vpmpath):
        vleVpz(vpzpath), mDoc(0), mFileName(vpmpath), mVdo(0), undoStack(0),
        waitUndoRedoVpz(false), waitUndoRedoVpm(false), oldValVpz(),
        newValVpz(), oldValVpm(), newValVpm(), plugins()
{
    xCreateDom();
    plugins.loadPlugins();
    mVdo = new vleDomVpm(mDoc);
    undoStack = new vleDomDiffStack(mVdo);
    undoStack->init(*mDoc);


    QObject::connect(vleVpz::undoStack,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoVpz(QDomNode, QDomNode)));
    QObject::connect(vleVpz::undoStack,
                SIGNAL(snapshotVdo(QDomNode, bool)),
                this, SLOT(onSnapshotVpz(QDomNode, bool)));
    QObject::connect(undoStack,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoVpm(QDomNode, QDomNode)));
    QObject::connect(undoStack,
                SIGNAL(snapshotVdo(QDomNode, bool)),
                this, SLOT(onSnapshotVpm(QDomNode, bool)));

}

QString
vleVpm::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vleVpm::xCreateDom()
{
    if (not mDoc) {
        mDoc = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDoc->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDoc->appendChild(pi);

        QDomElement vpmRoot = mDoc->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "me");
        QDomElement xCondPlug = mDoc->createElement("condPlugins");
        vpmRoot.appendChild(xCondPlug);
        mDoc->appendChild(vpmRoot);
    }
}

void
vleVpm::xReadDom()
{
    if (mDoc) {
        QFile file(mFileName);
        mDoc->setContent(&file);
    }
}

void
vleVpm::xSaveDom()
{
    if (mDoc) {
        QFile file(mFileName);
        file.open(QIODevice::Truncate | QIODevice::WriteOnly);
        QByteArray xml = mDoc->toByteArray();
        file.write(xml);
        file.close();
    }
}


void
vleVpm::setCondPlugin(const QString& condName, const QString& name)
{
    xCreateDom();

    QDomElement docElem = mDoc->documentElement();

    QDomNode condsPlugins =
            mDoc->elementsByTagName("condPlugins").item(0);
    undoStack->snapshot(condsPlugins);
    QDomNodeList plugins =
            condsPlugins.toElement().elementsByTagName("condPlugin");
    for (unsigned int i =0; i< plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j=0; j< plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "cond") and
                    (plug.attributes().item(j).nodeValue() == condName))  {
                plug.toElement().setAttribute("plugin", name);
                return;
            }
        }
    }
    QDomElement el = mDoc->createElement("condPlugin");
    el.setAttribute("cond", condName);
    el.setAttribute("plugin", name);
    condsPlugins.appendChild(el);


}

void
vleVpm::addConditionFromPluginToDoc(const QString& condName,
        const QString& pluginName)
{
    QString libName = plugins.getCondPluginPath(pluginName);
    QPluginLoader loader(libName);
    PluginExpCond* plugin = qobject_cast<PluginExpCond*>(loader.instance());

    bool oldBlock;
    bool enableSnapshot;
    oldBlock = vleVpz::undoStack->blockSignals(true);
    vleVpz::undoStack->snapshot(condsFromDoc());
    vleVpz::undoStack->blockSignals(oldBlock);

    enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
    addConditionToDoc(condName);
    plugin->setExpCond(this, condName);
    delete plugin;
    vleVpz::undoStack->enableSnapshot(enableSnapshot);
    oldBlock = undoStack->blockSignals(true);
    setCondPlugin(condName, pluginName);
    undoStack->blockSignals(oldBlock);


}

void
vleVpm::renameConditionToDoc(const QString& oldName, const QString& newName)
{

    QString condPlugin = getCondPlugin(oldName);
    if (condPlugin == "") {
        vleVpz::renameConditionToDoc(oldName, newName);
    } else {
        //snapshtot vpz conditions (without signal)
        bool oldBlock;
        oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::undoStack->snapshot(condsFromDoc());
        vleVpz::undoStack->blockSignals(oldBlock);
        //rename condition into vpz without snapshot
        bool enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
        vleVpz::renameConditionToDoc(oldName, newName);
        vleVpz::undoStack->enableSnapshot(enableSnapshot);
        //change condition name into vpm (without signal but with snapshot)
        oldBlock = undoStack->blockSignals(true);
        setCondPlugin(oldName, "");
        setCondPlugin(newName, condPlugin);
        undoStack->blockSignals(oldBlock);
    }
}

void
vleVpm::rmConditionToDoc(const QString& condName)
{
    QString condPlugin = getCondPlugin(condName);
    if (condPlugin == "") {
        vleVpz::rmConditionToDoc(condName);
    } else {
        //snapshtot vpz conditions (without signal)
        bool oldBlock;
        oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::undoStack->snapshot(condsFromDoc());
        vleVpz::undoStack->blockSignals(oldBlock);
        //remove condition into vpz without snapshot
        bool enableSnapshot = vleVpz::undoStack->enableSnapshot(false);
        vleVpz::rmConditionToDoc(condName);
        vleVpz::undoStack->enableSnapshot(enableSnapshot);
        //change condition name into vpm (without signal but with snapshot)
        oldBlock = undoStack->blockSignals(true);
        setCondPlugin(condName, "");
        undoStack->blockSignals(oldBlock);
    }
}

QString
vleVpm::getCondPlugin(const QString& condName)
{

    QStringList res;
    if (mDoc) {
        QDomElement docElem = mDoc->documentElement();
        QDomNode condsPlugins =
                mDoc->elementsByTagName("condPlugins").item(0);
        QDomNodeList plugins =
                condsPlugins.toElement().elementsByTagName("condPlugin");
        for (unsigned int i =0; i< plugins.length(); i++) {
            QDomNode plug = plugins.item(i);
            if (plug.attributes().contains("cond") and
                (plug.attributes().namedItem("cond").nodeValue() == condName)) {
                return plug.attributes().namedItem("plugin").nodeValue();
            }
        }
    }
    return "";
}

void
vleVpm::setCurrentTab(QString tabName)
{
    undoStack->current_source = tabName;
    vleVpz::undoStack->current_source = tabName;
}

void
vleVpm::undo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = true;
    undoStack->undo();
    vleVpz::undoStack->undo();

}
void
vleVpm::redo()
{
    waitUndoRedoVpz = true;
    waitUndoRedoVpm = true;
    undoStack->redo();
    vleVpz::undoStack->redo();
}

void
vleVpm::onSnapshotVpz(QDomNode /*snapVpz*/, bool isMerged)
{
    if (not isMerged) {
        bool oldBlock = undoStack->blockSignals(true);
        undoStack->snapshot(QDomNode());
        undoStack->blockSignals(oldBlock);
    }

}

void
vleVpm::onSnapshotVpm(QDomNode /*snapVpm*/, bool isMerged)
{
    if (not isMerged) {
        bool oldBlock = vleVpz::undoStack->blockSignals(true);
        vleVpz::undoStack->snapshot(QDomNode());
        vleVpz::undoStack->blockSignals(oldBlock);
    }

}
void
vleVpm::onUndoRedoVpm(QDomNode oldVpm, QDomNode newVpm)
{
    oldValVpm = oldVpm;
    newValVpm = newVpm;
    waitUndoRedoVpm = false;
    if (not waitUndoRedoVpz ) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}


void
vleVpm::onUndoRedoVpz(QDomNode oldVpz, QDomNode newVpz)
{
    oldValVpz = oldVpz;
    newValVpz = newVpz;
    waitUndoRedoVpz = false;
    if (not waitUndoRedoVpm ) {
        emit undoRedo(oldValVpz, newValVpz, oldValVpm, newValVpm);
    }
}


}}//namespaces
