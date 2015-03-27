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

#include "vlevpz.h"
#include <QClipboard>
#include <QFlags>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QtDebug>

vleVpz::vleVpz()
{
    mRootModel = 0;
    mClassesRaw = 0;
    mViewsRaw   = 0;
}

vleVpz::vleVpz(const QString &filename) :
    mFilename(filename), mFile(mFilename), mDoc("vle_project")
{
    mRootModel = 0;
    mClassesRaw = 0;
    mViewsRaw   = 0;

//    mFilename = filename;

    xReadDom();

    if (!mRootModel){
        mRootModel = new vleVpzModel();
    }
}

const QDomDocument& vleVpz::getDomDoc() const
{
    return mDoc;
}

QDomDocument& vleVpz::getDomDoc()
{
    emit sigChanged(mFilename);
    return mDoc;
}

QString vleVpz::getFilename()
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
    docElem.setAttribute("name", name);
}


QString vleVpz::getExpDuration() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("duration"))
    {
        QDomNode xDuration = attrMap.namedItem("duration");
        return xDuration.nodeValue();
    }
    return QString("");
}

void vleVpz::setExpDuration(const QString duration)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    docElem.setAttribute("duration", duration);
}

QString vleVpz::getExpBegin() const
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    QDomNamedNodeMap attrMap = docElem.attributes();

    if (attrMap.contains("begin"))
    {
        QDomNode xBegin = attrMap.namedItem("begin");
        return xBegin.nodeValue();
    }
    return QString("");
}

void vleVpz::setExpBegin(const QString begin)
{
    QDomNodeList nodeList = getDomDoc().elementsByTagName("experiment");
    QDomElement docElem = nodeList.item(0).toElement();
    docElem.setAttribute("begin", begin);
}

void vleVpz::setBasePath(const QString path)
{
    mPath = path;
}


vlePackage *vleVpz::getPackage()
{
    return mPackage;
}

void vleVpz::setPackage(vlePackage *package)
{
    mPackage = package;
}

/**
 * @brief vleVpz::focusChange (slot)
 *        Called when a model emit a focus signal
 *
 */
void vleVpz::focusChange(vleVpzModel *model)
{
    // re-emit the signal for the upper layers (filevpzview)
    emit sigFocus(model);
}

/**
 * @brief vleVpz::enterModel (slot)
 *        Called when a model emit an "enter" signal (ex: double-click)
 *
 */
void vleVpz::enterModel(vleVpzModel *model)
{
    // re-emit the signal for the upper layers (filevpzview)
    emit sigEnterModel(model);
}

/**
 * @brief vleVpz::isAltered
 *        Test if the VPZ has been modified since the last save
 *
 */
bool vleVpz::isAltered()
{
    if (mRootModel)
    {
        return mRootModel->isAltered();
    }
    return false;
}

/**
 * @brief vleVpz::getDynamic
 *        Search and return a loaded Dynamic, identified by his name
 *
 */
vleVpzDynamic *vleVpz::getDynamic(QString name)
{
    QListIterator<vleVpzDynamic*> items(mDynamics);
    while( items.hasNext() )
    {
        vleVpzDynamic *dyn = items.next();
        if (dyn->getName() == name)
            return dyn;
    }

    return 0;
}

/**
 * @brief vleVpz::addDynamic
 *        Add a new Dynamic
 *
 */
void vleVpz::addDynamic(vleVpzDynamic *dynamic)
{
    if (dynamic)
        mDynamics.prepend(dynamic);
}

/**
 * @brief vleVpz::removeDynamic
 *        Remove an existing Dynamic
 *
 */
void vleVpz::removeDynamic(vleVpzDynamic *dynamic)
{
    // Remove the links of models with the dyncmic
    removeModelDynamic(mRootModel, dynamic, true);

    // Remove from the VPZ
    mDynamics.removeAll(dynamic);

    // Then delete it
    delete dynamic;
}

/**
 * @brief vleVpz::removeModelDynamic
 *        Remove  link(s) of model(s) with a dynamic before deleting it
 *
 */
int vleVpz::removeModelDynamic(vleVpzModel *model, vleVpzDynamic *dynamic, bool recurse)
{
    int count = 0;

    // If the specified model use the dynamic
    if (model->getDynamic() == dynamic)
    {
        // Remove it
        model->removeDynamic();
        count ++;
    }

    // If the recurse flag is set, call this method for all submodels
    if (recurse)
    {
        for (int i = 0; i < model->countSubmodels(); i++)
            count += removeModelDynamic(model->getSubmodelAt(i), dynamic, recurse);
    }

    return count;
}

/**
 * @brief vleVpz::addCondition
 *        Create a new Condition (experiment) and register it into current VPZ
 *
 */
vpzExpCond *vleVpz::addCondition(QString name)
{
    vpzExpCond *newCondition = new vpzExpCond(this);

    if (name != "")
        newCondition->setName(name);

    mConditions.append(newCondition);

    return newCondition;
}

/**
 * @brief vleVpz::removeCondition
 *        Remove (and delete) a loaded Condition
 *
 */
bool vleVpz::removeCondition(vpzExpCond *cond)
{
    if ( ! mConditions.contains(cond))
        return false;
    mConditions.removeOne(cond);
    delete cond;

    return true;
}

vpzExpCondValue *vleVpz::addConditionValue(vpzExpCondPort *port, vpzExpCondValue::ValueType type)
{
    vpzExpCondValue *v;

    // Port must be a valid vpzExpCondPort
    if (port == 0)
        return 0;

    v = port->createValue(type);
    return v;
}

bool vleVpz::delConditionValue(vpzExpCondPort *port, vpzExpCondValue *value)
{
    // Port must be a valid vpzExpCondPort
    if (port == 0)
        return false;

    return port->removeValue(value);
}

vpzExpCond * vleVpz::getCondition(QString name)
{
    //
    QListIterator<vpzExpCond*> condItems(mConditions);
    while( condItems.hasNext() )
    {
        vpzExpCond *condItem = condItems.next();
        if (condItem->getName() == name)
            return condItem;
    }
    return 0;
}

vpzExpCond * vleVpz::getFirstCondition()
{
    mConditionIteratorIndex = 0;
    if (mConditions.count())
        return mConditions.at(mConditionIteratorIndex);
    else
        return 0;
}
vpzExpCond *vleVpz::getNextCondition()
{
    if (mConditions.count() > (mConditionIteratorIndex + 1))
    {
        mConditionIteratorIndex++;
        return mConditions.at(mConditionIteratorIndex);
    }
    else
        return 0;
}

/**
 * @brief vleVpz::addModeler (slot)
 *        Called when a model must be linked with a modeler
 *
 */
void vleVpz::addModeler(QString name)
{
    // Search the sender model
    QObject *senderObject = QObject::sender();
    vleVpzModel *model = qobject_cast<vleVpzModel *>(senderObject);
    if (model == 0)
        return;

    sourceCpp *classFile = model->getModelerClass();
    if (classFile == 0)
        return;

    PluginModeler *modeler = classFile->getModeler();
    if (modeler == 0)
    {
        QMessageBox msgBox;

        msgBox.setText(tr("The modeler plugin could not be loaded (%1) !").arg(classFile->getModelerName()));
        msgBox.exec();
        return;
    }

    QString condName = name + model->getName();
    vpzExpCond *exp = addCondition(condName);

    modeler->initExpCond(exp, classFile);
    model->addCondition(exp);

    // Inform upper layers that a new condition has been created
    emit sigConditionsChanged();

    addModelerDynamic(model, name);
    // Inform upper layers that dynamic has been changed
    emit sigDynamicsChanged();

    emit sigOpenModeler(model);
}

/**
 * @brief vleVpz::addModelerDynamic
 *        Part of the add-modeler process - Update the model Dynamic
 *
 */
void vleVpz::addModelerDynamic(vleVpzModel *model, QString lib)
{
    // First, check if the model has already a dynamic
    vleVpzDynamic *dyn = model->getDynamic();
    if (dyn)
    {
        // Test if the current dynamic can be used with this modeler
        if ((dyn->getPackage() == mPackage->getName()) &&
            (dyn->getLibrary() == lib))
            // Yes, model has already a valid dynamic, nothing to do
            return;

        QMessageBox msgBox;
        msgBox.setText(tr("The current model Dynamic will be removed. Continue ?"));
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        int ret = msgBox.exec();
        // If the user say "No", current dynamic stay unchanged
        if (ret == QMessageBox::No)
            return;

        model->removeDynamic();
    }

    // Second, search into the existing dynamics if one can be used
    vleVpzDynamic *newDyn = 0;
    for (int i = 0; i < mDynamics.count(); i++)
    {
        vleVpzDynamic *existingDyn = mDynamics.at(i);
        if (existingDyn->getPackage() != mPackage->getName())
            continue;
        if (existingDyn->getLibrary() != lib)
            continue;

        // A dynamic with good Library and Package values found ! :)
        newDyn = existingDyn;
        break;
    }

    // If no dynamic available, create a new one
    if (newDyn == 0)
    {
        QString dynName = "dyn_" + model->getName();
        QString pkgName = mPackage->getName();

        newDyn = new vleVpzDynamic(dynName, lib, pkgName);
        addDynamic(newDyn);
    }

    model->setDynamic(newDyn->getName());
}

/**
 * @brief vleVpz::openModeler (slot)
 *        Connected to vleVpzModels and called by them to open modeler
 *
 */
void vleVpz::openModeler()
{
    // Search the origin of the signal
    QObject *senderObject = QObject::sender();
    vleVpzModel *model = qobject_cast<vleVpzModel *>(senderObject);
    if (model == 0)
        return;

    // Forward the event as signal (mainly to fileVpzView)
    emit sigOpenModeler(model);
}

/**
 * @brief vleVpz::save
 *        Entry point of the save process - Write current VPZ to a file
 *
 */
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

/**
 * @brief vleVpz::xSaveDom
 *        Create VPZ root entries and call specific methods to save parts
 *
 */
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

    QDomElement xStruct = doc->createElement("structures");
    if ( xSaveStructures(doc, &xStruct) )
        vpzRoot.appendChild(xStruct);

    QDomElement xDyn = doc->createElement("dynamics");
    if ( xSaveDynamics(doc, &xDyn) )
        vpzRoot.appendChild(xDyn);

    if (mClassesRaw)
        vpzRoot.appendChild(*mClassesRaw);

    QDomElement xExp = doc->createElement("experiment");
    if ( xSaveExperiments(doc, &xExp) )
        vpzRoot.appendChild(xExp);

    doc->appendChild(vpzRoot);
}

/**
 * @brief vleVpz::xSaveStructures
 *        Add strucures elements to a DomDocument (mainly insert models)
 *
 */
bool vleVpz::xSaveStructures(QDomDocument *doc, QDomNode *baseNode)
{
    if (mRootModel == 0)
        return false;

    QDomElement xModel = doc->createElement("model");
    if (xSaveModel(doc, &xModel, mRootModel))
        baseNode->appendChild(xModel);

    return true;
}

/**
 * @brief vleVpz::xSaveModel
 *        Insert the content of a specific model (or submodel) to "structures"
 *
 */
bool vleVpz::xSaveModel(QDomDocument *doc, QDomElement *baseNode, vleVpzModel *model)
{
    // First, add general attributes
    baseNode->setAttribute("name", model->getName());
    if (model->getX())
    {
        baseNode->setAttribute("width",  model->getWidth());
        baseNode->setAttribute("height", model->getHeight());
        baseNode->setAttribute("x",      model->getX());
        baseNode->setAttribute("y",      model->getY());
    }

    // Insert Dynamic attribute if used
    if (model->getDynamic())
        baseNode->setAttribute("dynamics", model->getDynamic()->getName());

    // Insert Condition attribute if used
    if (model->getConditionStringList() != "")
        baseNode->setAttribute("conditions", model->getConditionStringList());

    // Insert Observables attribute if used
    if (model->getObservables() != "")
        baseNode->setAttribute("observables", model->getObservables());

    // If the model has Input port(s) ... save them
    if (model->getInPorts()->length())
    {
        QDomElement xIn = doc->createElement("in");

        QListIterator<vleVpzPort*> items(*model->getInPorts());
        while( items.hasNext() )
        {
            vleVpzPort *port = items.next();

            QDomElement xPort = doc->createElement("port");
            xPort.setAttribute("name", port->getName());
            xIn.appendChild(xPort);
        }
        baseNode->appendChild(xIn);
    }

    // If the model has Output port(s) ... save them
    if (model->getOutPorts()->length())
    {
        QDomElement xOut = doc->createElement("out");

        QListIterator<vleVpzPort*> items(*model->getOutPorts());
        while( items.hasNext() )
        {
            vleVpzPort *port = items.next();

            QDomElement xPort = doc->createElement("port");
            xPort.setAttribute("name", port->getName());
            xOut.appendChild(xPort);
        }
        baseNode->appendChild(xOut);
    }

    // If the model has sub-models .. recursivly save them
    if (model->countSubmodels())
    {
        baseNode->setAttribute("type", "coupled");

        QDomElement xSubs = doc->createElement("submodels");

        QListIterator<vleVpzModel*> items(*model->getSubmodels());
        while( items.hasNext() )
        {
            vleVpzModel *submodel = items.next();

            QDomElement xSubmodel = doc->createElement("model");
            if (xSaveModel(doc, &xSubmodel, submodel))
                xSubs.appendChild(xSubmodel);
        }

        baseNode->appendChild(xSubs);
    }
    else
        baseNode->setAttribute("type", "atomic");

    // If the model has connections ... save them
    if (model->getConnections()->length())
    {
        QDomElement xConns = doc->createElement("connections");

        QListIterator<vleVpzConn*> items(*model->getConnections());
        while( items.hasNext() )
        {
            vleVpzConn *conn = items.next();

            QDomElement xConn = doc->createElement("connection");
            xConn.setAttribute("type", conn->typeName());

            QDomElement xConnSrc = doc->createElement("origin");
            xConnSrc.setAttribute("model", conn->getSourceModelName());
            xConnSrc.setAttribute("port", conn->getSourcePortName());
            xConn.appendChild(xConnSrc);

            QDomElement xConnDst = doc->createElement("destination");
            xConnDst.setAttribute("model", conn->getDestinationModelName());
            xConnDst.setAttribute("port", conn->getDestinationPortName());
            xConn.appendChild(xConnDst);

            xConns.appendChild(xConn);
        }
        baseNode->appendChild(xConns);
    }

    return true;
}

/**
 * @brief vleVpz::xSaveDynamics
 *        Save the loaded Dynamics into the DomDocument
 *
 */
bool vleVpz::xSaveDynamics(QDomDocument *doc, QDomNode *baseNode)
{
    if (mDynamics.length() == 0)
        return false;

    QListIterator<vleVpzDynamic*> items(mDynamics);
    while( items.hasNext() )
    {
        vleVpzDynamic *dyn = items.next();

        QDomElement xDyn = doc->createElement("dynamic");
        xDyn.setAttribute("package", dyn->getPackage());
        xDyn.setAttribute("library", dyn->getLibrary());
        xDyn.setAttribute("name",    dyn->getName());
        baseNode->appendChild(xDyn);
    }

    return true;
}

/**
 * @brief vleVpz::xSaveExperiments
 *        Save the loaded Experiments into the DomDocument
 *
 */
bool vleVpz::xSaveExperiments(QDomDocument *doc, QDomElement *baseNode)
{
    baseNode->setAttribute("begin", getExpBegin());
    baseNode->setAttribute("duration", getExpDuration());

    if (mExpCombination != "")
        baseNode->setAttribute("combination", mExpCombination);

    baseNode->setAttribute("name", getExpName());

    if ( (mConditions.length() == 0) && (mViewsRaw == 0))
        return false;

    if (mConditions.length())
    {
        // Create the "conditions" element into experiment
        QDomElement xExpCnd = doc->createElement("conditions");

        //
        QListIterator<vpzExpCond*> condItems(mConditions);
        while( condItems.hasNext() )
        {
            vpzExpCond *cnd = condItems.next();

            QDomElement xCnd = doc->createElement("condition");
            xCnd.setAttribute("name", cnd->getName());
            if (cnd->hasPlugin())
                xCnd.setAttribute("plugin", cnd->getPlugin());
            if ( xSaveExpCondition(doc, &xCnd, cnd) )
                xExpCnd.appendChild(xCnd);
        }

        baseNode->appendChild(xExpCnd);
    }

    baseNode->appendChild(*mViewsRaw);

    return true;
}

bool vleVpz::xSaveExpCondition(QDomDocument *doc, QDomNode *baseNode, vpzExpCond *cond)
{
    QList <vpzExpCondPort *> *portList = cond->getPorts();
    for (int i = 0; i < portList->length(); i++)
    {
        vpzExpCondPort *port = portList->at(i);

        QDomElement xPort = doc->createElement("port");
        port->xSave(doc, &xPort);
        baseNode->appendChild(xPort);
    }
    return true;
}

void vleVpz::xReadDom()
{
    mDoc.setContent(&mFile);
    QDomElement docElem = mDoc.documentElement();

    // First try to load Dynamics and Conditions
    for(QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if (n.nodeName() == "dynamics")
            xReadDomDynamics(n);
        if (n.nodeName() == "experiment")
            xReadDomExperiments(n);
    }

    for(QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if (n.nodeName() == "structures")
            xReadDomStructures(n);
        if (n.nodeName() == "classes")
            mClassesRaw = new QDomNode(n);
    }
}

void vleVpz::xReadDomStructures(const QDomNode &baseNode)
{
    QDomNodeList list = baseNode.childNodes();
    if (list.isEmpty()) {
        vleVpzModel *model = new vleVpzModel(this);
        model->setName("Top Model");
        mRootModel = model;
    } else {
        for (unsigned int i=0; i < list.length(); i++)
        {
            QDomNode item = list.item(i);
            if (item.nodeName() != "model")
                continue;

            vleVpzModel *model = new vleVpzModel(this);
            model->xLoadNode(item);
            mRootModel = model;
            break;
        }
    }
}

void vleVpz::xReadDomDynamics(const QDomNode &baseNode)
{
    QString dynName;
    QString dynLib;
    QString dynPackage;

    QDomNodeList list = baseNode.childNodes();

    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        if (item.nodeName() != "dynamic")
            continue;

        QDomNamedNodeMap attrMap = item.attributes();
        if (attrMap.contains("name"))
        {
            QDomNode xName = attrMap.namedItem("name");
            dynName = xName.nodeValue();
        }
        if (attrMap.contains("library"))
        {
            QDomNode xLib = attrMap.namedItem("library");
            dynLib = xLib.nodeValue();
        }
        if (attrMap.contains("package"))
        {
            QDomNode xPkg = attrMap.namedItem("package");
            dynPackage = xPkg.nodeValue();
        }

        vleVpzDynamic *dyn = new vleVpzDynamic();
        dyn->setName(dynName);
        dyn->setLibrary(dynLib);
        dyn->setPackage(dynPackage);
        mDynamics.append(dyn);
    }
}

/**
 * @brief vleVpz::xReadDomExperiments
 *        Read and parse the "Experiments" section of the VPZ file
 *
 */
void vleVpz::xReadDomExperiments(const QDomNode &baseNode)
{
    QDomNamedNodeMap attrMap = baseNode.attributes();

    if (attrMap.contains("combination"))
        mExpCombination = attrMap.namedItem("combination").nodeValue();

    QDomNodeList list = baseNode.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);

        if (item.nodeName() == "conditions")
            xReadDomExpConditions(item);
        if (item.nodeName() == "views")
            mViewsRaw = new QDomNode(item);
    }
}

/**
 * @brief vleVpz::xReadDomExpConditions
 *        Read and parse the "Conditions" subsection of "Experiments"
 *
 */
void vleVpz::xReadDomExpConditions(const QDomNode &baseNode)
{
    QDomNodeList list = baseNode.childNodes();

    // Parse each "condition" inside the "conditions" section
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        // All other nodes are ignored
        if (item.nodeName() != "condition")
            continue;

        QString condName;

        QDomNamedNodeMap attrMap = item.attributes();
        if (attrMap.contains("name"))
        {
            QDomNode xName = attrMap.namedItem("name");
            condName = xName.nodeValue();
        }

        bool usePlugin = false;
        QString pluginName;
        if (attrMap.contains("plugin"))
        {
            QDomNode xPlugin = attrMap.namedItem("plugin");
            pluginName = xPlugin.nodeValue();
            usePlugin = true;
        }

        vpzExpCond *cond = addCondition();
        cond->setName(condName);
        if (usePlugin)
            cond->setPlugin(pluginName);

        QDomNodeList condList = item.childNodes();
        for (unsigned int j=0; j < condList.length(); j++)
        {
            QDomNode         portItem = condList.item(j);
            QDomNamedNodeMap portAttrMap = portItem.attributes();

            if (portItem.nodeName() != "port")
                continue;

            vpzExpCondPort *port = new vpzExpCondPort();

            QString portName;
            if (portAttrMap.contains("name"))
            {
                QDomNode xName = portAttrMap.namedItem("name");
                portName = xName.nodeValue();
            }
            port->setName(portName);

            port->xLoadNode(portItem);

            cond->addPort(port);
        }
    }
}

/* ------------------------------ Models ------------------------------ */

/**
 * @brief vleVpzModel::vleVpzModel
 *        Constructor that load model from a XML node
 *
 */
vleVpzModel::vleVpzModel(vleVpz *parent)
                      : QWidget(0)
{
    mVpz = 0;

    if (parent)
    {
        mVpz = parent;
        QObject::connect(this, SIGNAL(sigFocus(vleVpzModel*)),
                         mVpz, SLOT(focusChange(vleVpzModel*)));
        QObject::connect(this, SIGNAL(sigDblClick(vleVpzModel*)),
                         mVpz, SLOT(enterModel(vleVpzModel*)));

        QObject::connect(this, SIGNAL(sigAddModeler(QString)),
                         mVpz, SLOT  (addModeler(QString)) );
        QObject::connect(this, SIGNAL(sigOpenModeler()),
                         mVpz, SLOT  (openModeler()) );
    }

    mConditions.clear();

    mIsAltered = false;
    mDynamic   = 0;

    mTooltip = 0;

    mWidgetX = 10;
    mWidgetY = 10;
    mWidgetHeight = 50;
    mWidgetWidth  = 50;
    mWidInPortY   = 0;
    mWidOutPortY  = 8;

    mSettingLine     = 3;
    mSettingCorner   = 10;
    mSettingFontSize = 6;

    mPainterFont.setPointSize(mSettingFontSize);

    mIsMaximized= false;
    mIsFocus    = false;
    mIsSelected = false;
    mSelect     = false;

    mPortInSel  = 0;
    mPortOutSel = 0;

    mModelerClass = 0;

    setToolTip("Model");

    // Configure a custom context menu (called on right-click on model)
    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT  (contextMenu(const QPoint&)));

    setStyleSheet("background-color: transparent; color: rgb(0, 0, 255);");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    mTitle.setParent(this);
}

/**
 * @brief vleVpzModel::~vleVpzModel
 *        Default destructor
 *
 */
vleVpzModel::~vleVpzModel()
{
    // Clean all the inter-model connections
    QListIterator<vleVpzConn*> connItems(mConnections);
    while( connItems.hasNext() )
        delete connItems.next();
    // Clean all the allocated sub-models
    QListIterator<vleVpzModel*> items(mSubmodels);
    while( items.hasNext() )
        delete items.next();
    // Clean Input ports objects
    QListIterator<vleVpzPort*> inItems(mInPorts);
    while( inItems.hasNext() )
        delete inItems.next();
    // Clean output ports objects
    QListIterator<vleVpzPort*> outItems(mOutPorts);
    while( outItems.hasNext() )
        delete outItems.next();
}

/**
 * @brief vleVpzModel::copyFromModel
 *        Configure local model with values take from another model
 *
 */
void vleVpzModel::copyFromModel(vleVpzModel *model)
{
    // Copy input ports
    QListIterator<vleVpzPort*> items(*model->getInPorts());
    while( items.hasNext() )
    {
        vleVpzPort *port = items.next();

        vleVpzPort *localPort = new vleVpzPort(this);
        localPort->setName(port->getName());
        localPort->setType(vleVpzPort::TypeInput);
        localPort->move(2, nextInPosY());
        mInPorts.append(localPort);
    }

    // Copy output ports
    QListIterator<vleVpzPort*> portsOut(*model->getOutPorts());
    while( portsOut.hasNext() )
    {
        vleVpzPort *port = portsOut.next();

        vleVpzPort *localPort = new vleVpzPort(this);
        localPort->setName(port->getName());
        localPort->setType(vleVpzPort::TypeOutput);
        localPort->move(2, nextOutPosY());
        mOutPorts.append(localPort);
    }

    // Update the widget size, according to new ports lists
    fixWidgetSize(true);
}

bool vleVpzModel::isAltered()
{
    return mIsAltered;
}

QString vleVpzModel::getName()
{
    return mName;
}

vleVpz *vleVpzModel::getVpz()
{
    return mVpz;
}

void vleVpzModel::addSubmodel(vleVpzModel *model)
{
    mSubmodels.append(model);
}

void vleVpzModel::delSubmodel(vleVpzModel *model)
{
    int idx;

    if (model == 0)
        return;

    // Search the specified model into sub-models list
    idx = mSubmodels.indexOf(model);
    if (idx < 0)
        return;

    mSubmodels.removeAt(idx);

    for (int i = 0; i < mConnections.length(); i++)
    {
        vleVpzConn *conn = mConnections.at(i);
        if ( (conn->getSource()      == model) ||
             (conn->getDestination() == model) )
        {
            mConnections.removeAt(i);
            delete conn;
            i--;
        }
    }
}

int vleVpzModel::countSubmodels()
{
    return mSubmodels.length();
}

/**
 * @brief vleVpzModel::getDynamic
 *        Accessor for the Dynamic attribute of the model
 *
 */
vleVpzDynamic *vleVpzModel::getDynamic()
{
    return mDynamic;
}

/**
 * @brief vleVpzModel::setDynamic
 *        Set the current Dynamic of the model (selected by his name)
 *
 */
void vleVpzModel::setDynamic(QString dynamicName)
{
    // Search the Dynamic into VPZ using specified name
    vleVpzDynamic *dynamic = mVpz->getDynamic(dynamicName);

    // If found, update the model
    if (dynamic)
        mDynamic = dynamic;
    else
        qDebug() << "Model " << mName << " try to use an unknown dynamic : " << dynamicName;
}

/**
 * @brief vleVpzModel::removeDynamic
 *        Remove the current Dynamic (if any)
 *
 */
void vleVpzModel::removeDynamic()
{
    mDynamic = 0;
}


void vleVpzModel::addCondition(vpzExpCond *cond)
{
    if (cond == 0)
        return;
    mConditions.append(cond);
    cond->registerModel(this);
}

void vleVpzModel::removeCondition(vpzExpCond *cond)
{
    if (! mConditions.contains(cond))
        return;
    mConditions.removeOne(cond);
    cond->unRegisterModel(this);
}

QString vleVpzModel::getConditionStringList()
{
    QString result;
    for (int i = 0; i < mConditions.length(); i++)
    {
        if (i)
            result.append(",");
        result.append(mConditions.at(i)->getName());
    }
    return result;
}

/**
 * @brief vleVpzModel::xLoadNode
 *        Load the model from the
 *
 */
void vleVpzModel::xLoadNode(const QDomNode &node)
{
    QDomNamedNodeMap attrMap = node.attributes();
    if (attrMap.contains("name")) // Attribute Required !
    {
        QDomNode name = attrMap.namedItem("name");
        mName = name.nodeValue();
        mTitle.setText(mName);
    }
    if (attrMap.contains("x")) // Optional
    {
        QDomNode name = attrMap.namedItem("x");
        mWidgetX = name.nodeValue().toInt();
    }
    if (attrMap.contains("y")) // Optional
    {
        QDomNode name = attrMap.namedItem("y");
        mWidgetY = name.nodeValue().toInt();
    }
    if (attrMap.contains("height")) // Optional
    {
        QDomNode name = attrMap.namedItem("height");
        mWidgetHeight = name.nodeValue().toInt();
    }
    if (attrMap.contains("width")) // Optional
    {
        QDomNode name = attrMap.namedItem("width");
        mWidgetWidth = name.nodeValue().toInt();
    }
    if (attrMap.contains("dynamics")) // Optional
    {
        QDomNode xDyn = attrMap.namedItem("dynamics");
        QString dynName = xDyn.nodeValue();
        if (mVpz)
            setDynamic(dynName);
    }
    if (attrMap.contains("conditions")) // Optional)
    {
        QDomNode xCondList = attrMap.namedItem("conditions");
        QString condFlatList = xCondList.nodeValue();
        QStringList condList = condFlatList.split(",");
        if (mVpz)
        {
            QStringListIterator condListIt(condList);
            while (condListIt.hasNext())
            {
                QString condName = condListIt.next();
                vpzExpCond * cond = mVpz->getCondition(condName);
                addCondition(cond);
            }
        }
    }
    if (attrMap.contains("observables")) // Optional
    {
        QDomNode xObs = attrMap.namedItem("observables");
        // This save a RAW copy of observables
        mObservables = xObs.nodeValue();
    }
#ifdef X_DEBUG
    qDebug() << "VpzModel Name=" << mName << "H="<<mWidgetHeight<<" W="<<mWidgetWidth<<" x="<<mWidgetX<<" y="<<mWidgetY<< " this="<<this;
#endif

    QDomNodeList list = node.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        if (item.nodeName() == "connections")
            xReadConnections(item);
        else if (item.nodeName() == "in")
            xReadInputs(item);
        else if (item.nodeName() == "out")
            xReadOutputs(item);
        else if (item.nodeName() == "submodels")
            xreadSubmodels(item);
    }

    fixWidgetSize(true);
}

/**
 * @brief vleVpzModel::xreadSubmodels
 *        Load a submodel of the current object from an XML node
 *
 */
void vleVpzModel::xreadSubmodels(const QDomNode &baseNode)
{
    QDomNodeList list = baseNode.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        if (item.nodeName() != "model")
            continue;

        vleVpzModel *model = new vleVpzModel(mVpz);
        model->xLoadNode(item);
        addSubmodel(model);
    }
}

/**
 * @brief vleVpzModel::xreadConnections
 *        Load the list of sub-models connections
 *
 */
void vleVpzModel::xReadConnections(const QDomNode &baseNode)
{
    unsigned int i,j;
    vleVpzConn *newConn;
    QDomNodeList connNodes = baseNode.childNodes();
    for (i=0; i < connNodes.length(); i++)
    {
        QDomNode conn = connNodes.item(i);
        if (conn.nodeName() != "connection")
            continue;

        // Instanciate a new Connection object for this item
        newConn = new vleVpzConn(this);

        QDomNamedNodeMap connAttrMap = conn.attributes();
        if (connAttrMap.contains("type"))
        {
            QString type = connAttrMap.namedItem("type").nodeValue();
            if (type == "internal")
                newConn->setType(vleVpzConn::Internal);
            else if (type == "input")
                newConn->setType(vleVpzConn::In);
            else if (type == "output")
                newConn->setType(vleVpzConn::Out);
            else
                qDebug() << "xReadConnections() Unknown type " << type;
        }

        QDomNodeList connParams = conn.childNodes();
        for (j = 0; j < connParams.length(); j++)
        {
            QDomNode item = connParams.item(j);
            QDomNamedNodeMap attrMap = item.attributes();
            if ( ! attrMap.contains("model"))
                continue;
            if ( ! attrMap.contains("port"))
                continue;
            QString name = attrMap.namedItem("model").nodeValue();
            QString portName = attrMap.namedItem("port").nodeValue();

            // Search the specified model into loaded submodels
            vleVpzModel *submodel;
            submodel = getSubmodelByName(&name);
            // Save it to src or dst according to vpz tag name
            if (item.nodeName() == "destination")
            {
                vleVpzPort *port = 0;

                if (submodel && (newConn->type() == vleVpzConn::Internal))
                    port = submodel->getInPortByName(&portName);
                else if (submodel && (newConn->type() == vleVpzConn::In))
                    port = submodel->getInPortByName(&portName);
                else if ((submodel == 0) && (newConn->type() == vleVpzConn::Out))
                    port = getOutPortByName(&portName);
                else
                    qDebug() << "xReadConnections(" << mName << ") bad dest " << name << ":" << portName;

                if (port)
                    newConn->setDestination(port);
#ifdef X_DEBUG
                qDebug() << "xReadConnections(" << mName << ") D type=" << newConn->type() << name << ":" << portName;
#endif
            }
            else if (item.nodeName() == "origin")
            {
                vleVpzPort *port = 0;

                if (submodel && (newConn->type() == vleVpzConn::Internal))
                    port = submodel->getOutPortByName(&portName);
                else if (submodel && (newConn->type() == vleVpzConn::Out))
                    port = submodel->getOutPortByName(&portName);
                else if ((submodel == 0) && (newConn->type() == vleVpzConn::In))
                    port = getInPortByName(&portName);
                else
                    qDebug() << "xReadConnections(" << mName << ") type="<<newConn->type() << "bad orig " << name << ":" << portName;

                if (port)
                    newConn->setSource(port);
#ifdef X_DEBUG
                qDebug() << "xReadConnections(" << mName << ") O type="<<newConn->type() << name << ":" << portName;
#endif
            }
        }
        if (newConn->isValid())
            mConnections.append(newConn);
        else
        {
            qDebug() << "xReadConnections(" << mName << ") got invalid connection from vpz";
            delete newConn;
        }
    }
#ifdef X_DEBUG
    qDebug() << "xReadConnections(" << mName << ") " << mConnections.length() << " connections readed";
#endif
}

void vleVpzModel::xReadInputs(const QDomNode &baseNode)
{
    QDomNodeList list = baseNode.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        // Only "port" tags are readed
        if (item.nodeName() != "port")
            continue;
        // Read tag attributes
        QDomNamedNodeMap attrMap = item.attributes();
        if ( ! attrMap.contains("name"))
            continue;
        // Get the value of "name" attribute
        QDomNode xName = attrMap.namedItem("name");
        QString name = xName.nodeValue();

        // Allocate a new port
        vleVpzPort *port = new vleVpzPort();
        port->setParent(this);
        port->setName(name);
        port->setType(vleVpzPort::TypeInput);
        port->move(2, nextInPosY());
        mInPorts.append(port);
    }
}

void vleVpzModel::xReadOutputs(const QDomNode &baseNode)
{
    QDomNodeList list = baseNode.childNodes();
    for (unsigned int i=0; i < list.length(); i++)
    {
        QDomNode item = list.item(i);
        // Only "port" tags are readed
        if (item.nodeName() != "port")
            continue;
        // Read tag attributes
        QDomNamedNodeMap attrMap = item.attributes();
        if ( ! attrMap.contains("name"))
            continue;
        // Get the value of "name" attribute
        QDomNode xName = attrMap.namedItem("name");
        QString name = xName.nodeValue();

        // Allocate a new port
        vleVpzPort *port = new vleVpzPort();
        port->setParent(this);
        port->setName(name);
        port->setType(vleVpzPort::TypeOutput);
        port->move(50, nextOutPosY()); // 50 is an arbitrary value
        mOutPorts.append(port);
    }
}

bool vleVpzModel::xgetXml(QDomDocument *doc, QDomElement *base)
{
    QDomElement xModel = doc->createElement("model");
    xModel.setAttribute("name", mName);
    if (mSubmodels.length())
        xModel.setAttribute("type", "coupled");
    else
        xModel.setAttribute("type", "atomic");
    // Save the X-Y coordinates
    xModel.setAttribute("x", mWidgetX);
    xModel.setAttribute("y", mWidgetY);

    base->appendChild(xModel);

    return true;
}

void vleVpzModel::fixWidgetSize(bool doResize)
{
    // Test widget size and update if if needed
    int szTitleText   = 0;
    int szInPortText  = 0;
    int szOutPortText = 0;
    int szInPortHeight  = 0;
    int szOutPortHeight = 0;
    int itemHeight;

    // Compute the model name length (in pixels)
    QFontMetrics fmTitle = QFontMetrics( mTitle.font() );
    szTitleText = fmTitle.width( mName );

    QFontMetrics fmPorts = QFontMetrics(mPainterFont);

    // Compute the text length (in pixels) of the longest input port name
    QListIterator<vleVpzPort*> inItems(mInPorts);
    while( inItems.hasNext() )
    {
        vleVpzPort *item = inItems.next();
        int size = fmPorts.width( item->getName() );
        if (size > szInPortText)
            szInPortText = size;
        // Compute port height
        itemHeight = item->y() + item->height();
        if (itemHeight > szInPortHeight)
            szInPortHeight = itemHeight;
    }
    // Compute the text length (in pixels) of the longest output port name
    QListIterator<vleVpzPort*> outItems(mOutPorts);
    while( outItems.hasNext() )
    {
        vleVpzPort *item = outItems.next();
        int size = fmPorts.width( item->getName() );
        if (size > szOutPortText)
            szOutPortText = size;
        // Compute port height
        itemHeight = item->y() + item->height();
        if (itemHeight > szOutPortHeight)
            szOutPortHeight = itemHeight;
    }
    // Update widget width if in or out port name is too large
    if (mWidgetWidth < (szTitleText + 12) )
        mWidgetWidth = szTitleText + 12;
    if (mWidgetWidth < (szInPortText + 12 + 20) )
        mWidgetWidth = szInPortText + 34;
    if (mWidgetWidth < (szOutPortText + 12 + 20) )
        mWidgetWidth = szOutPortText + 34;

    // Update widget height if in or out port is outside widget
    if ((szInPortHeight + 15)  > mWidgetHeight)
        mWidgetHeight = szInPortHeight + 15;
    if ((szOutPortHeight + 15) > mWidgetHeight)
        mWidgetHeight = szOutPortHeight + 15;

    if (doResize)
    {
        setGeometry(mWidgetX-2, mWidgetY-2, mWidgetWidth+2, mWidgetHeight+16);
        mTitle.setGeometry(mSettingCorner, mWidgetHeight+3, mWidgetWidth, 12);
    }
}

/**
 * @brief vleVpzModel::getSubmodelByName
 *        Get a pointer on a specific submodel, finded by his name
 *
 */
vleVpzModel* vleVpzModel::getSubmodelByName(QString *name)
{
    vleVpzModel *model = 0;

    QListIterator<vleVpzModel*> subItems(mSubmodels);
    while( subItems.hasNext() )
    {
        vleVpzModel *modelItem = subItems.next();
        if (modelItem->getName() == name)
        {
            model = modelItem;
            break;
        }
    }
    return model;
}
vleVpzPort * vleVpzModel::getInPortByName(QString *name)
{
    vleVpzPort *port = 0;

    QListIterator<vleVpzPort*> portItems(mInPorts);
    while( portItems.hasNext() )
    {
        vleVpzPort *item = portItems.next();
        if (item->getName() == name)
        {
            port = item;
            break;
        }
    }
    return port;
}
vleVpzPort * vleVpzModel::getOutPortByName(QString *name)
{
    vleVpzPort *port = 0;

    QListIterator<vleVpzPort*> portItems(mOutPorts);
    while( portItems.hasNext() )
    {
        vleVpzPort *item = portItems.next();
        if (item->getName() == name)
        {
            port = item;
            break;
        }
    }
    return port;
}

/**
 * @brief vleVpzModel::setName
 *        Change the name of the current model
 *
 */
void vleVpzModel::setName(QString name)
{
    mName = name;
    // Update the title Label
    mTitle.setText(mName);
    fixWidgetSize(true);
    // Mark the model as altered
    mIsAltered = true;
}

vpzExpCond *vleVpzModel::getModelerExpCond()
{
    QList<QString> expNameCandidate;
    vlePackage * pkg = mVpz->getPackage();
    int nb = pkg->getModelerClassCount();
    for (int i = 0; i < nb; i++)
    {
        QString expName = pkg->getModelerClass(i);
        expName += mName;
        expNameCandidate.append(expName);
    }

    for (int i = 0; i < mConditions.count(); i++)
    {
        vpzExpCond *cond = mConditions.at(i);
        if (expNameCandidate.contains( cond->getName() ) )
        {
            if (mModelerClass == 0)
            {
                // As candidate list is based on getModelerClass() ... list id are equals "n"
                int n = expNameCandidate.indexOf( cond->getName() );
                QString className = pkg->getModelerClass(n);

                QString classFileName =  mVpz->getPackage()->getSrcFilePath(className + ".cpp");
                mModelerClass = mVpz->getPackage()->openSourceCpp(classFileName);
            }

            return cond;
        }
    }
    return 0;
}

bool vleVpzModel::hasModeler()
{
    if (getModelerExpCond() == 0)
        return false;

    return true;
}

/**
 * @brief vleVpzModel::select
 *        Mark the model as selected (this will change colors, style, ...)
 *
 */
void vleVpzModel::select(bool setFocus)
{
    mIsSelected = true;
    mTitle.setStyleSheet("color: rgb(255, 0, 0);");
    update();
    if (setFocus)
    {
        mIsFocus = true;
        emit sigFocus(this);
    }
}

/**
 * @brief vleVpzModel::deselect
 *        Clear the selected flag (see select() )
 *
 */
void vleVpzModel::deselect()
{
    mIsFocus = false;
    mIsSelected = false;
    mTitle.setStyleSheet("color: rgb(0, 0, 255);");
    update();
}

/**
 * @brief vleVpzModel::isSelected
 *        Read the select flag (true if currently selected)
 *
 */
bool vleVpzModel::isSelected()
{
    return mIsSelected;
}

void vleVpzModel::deselectSub()
{
    QListIterator<vleVpzModel*> items(mSubmodels);
    while( items.hasNext() )
    {
        vleVpzModel *sbm = items.next();
        sbm->deselect();
    }
}

/**
 * @brief vleVpzModel::dispNormal
 *        Set model display with his default size
 *
 */
void vleVpzModel::dispNormal()
{
    // Hide all the sub-models (or other included widgets)
    QObjectList childs = children();
    for (int i = 0; i < childs.count(); i++)
    {
        QWidget *c = (QWidget *)childs.at(i);
        c->hide();
    }

    // Resize widget to his default value
    setMinimumSize(mWidgetWidth, mWidgetHeight);
    setGeometry(mWidgetX-2, mWidgetY-2, mWidgetWidth+2, mWidgetHeight+16);

    // Put the title at the bottom
    mTitle.setGeometry(0, mWidgetHeight+3, mWidgetWidth, 12);

    // Clear the maximized flag
    mIsMaximized = false;
}
/**
 * @brief vleVpzModel::dispMaximize
 *        Set model display maximized (and show submodels into)
 *
 */
void vleVpzModel::dispMaximize()
{
    int newHeight = mWidgetHeight;
    int newWidth  = mWidgetWidth;

    // Get the size and position of each submodel widgets
    QListIterator<vleVpzModel*> subItems(mSubmodels);
    while( subItems.hasNext() )
    {
        vleVpzModel *submodel = subItems.next();
        // Update root model width to show submodel
        int subMaxX = submodel->getX() + submodel->getWidth();
        if (newWidth < (subMaxX + 10))
            newWidth = subMaxX + 10;
        // Update root model height to show submodel
        int subMaxY = submodel->getY() + submodel->getHeight();
        if (newHeight < (subMaxY + 30))
            newHeight = subMaxY + 30;
    }

    // Refresh routing - First time, for internal wires
    QListIterator<vleVpzConn*> connItems(mConnections);
    while( connItems.hasNext() )
    {
        vleVpzConn *conn = connItems.next();
        conn->route();
    }

    // Get the position of each routing wire
    QListIterator<vleVpzConn*> conns(mConnections);
    while( conns.hasNext() )
    {
        vleVpzConn *conn = conns.next();
        if (newWidth < (conn->getMaxX() + 10))
            newWidth = conn->getMaxX() + 10;
    }
    setGeometry(0, 0, newWidth, newHeight);
    setMinimumSize(newWidth, newHeight);

    // Refresh routing - second time, tu update in/out
    connItems.toFront();
    while( connItems.hasNext() )
    {
        vleVpzConn *conn = connItems.next();
        conn->route();
    }


    mTitle.setGeometry(0, newHeight-12, newWidth, 12);
    mIsMaximized = true;
}

/**
 * @brief vleVpzModel::isMaximized
 *        Return true is the model is currently maximized
 *
 */
bool vleVpzModel::isMaximized()
{
    return mIsMaximized;
}

int vleVpzModel::getRealWidth()
{
    QRect curGeo = geometry();
    return curGeo.width();
}

bool vleVpzModel::mouseMultipleSelect()
{
    bool result = false;
    int startX, endX;
    int startY, endY;
    if (mSelectCurrent.x() > mSelectStart.x())
    {
        startX = mSelectStart.x();
        endX   = mSelectCurrent.x();
    } else {
        startX = mSelectCurrent.x();
        endX   = mSelectStart.x();
    }
    if (mSelectCurrent.y() > mSelectStart.y())
    {
        startY = mSelectStart.y();
        endY   = mSelectCurrent.y();
    } else {
        startY = mSelectCurrent.y();
        endY   = mSelectStart.y();
    }
    QListIterator<vleVpzModel*> items(mSubmodels);
    while( items.hasNext() )
    {
        vleVpzModel *sbm = items.next();
        if ((sbm->getX() > startX) &&
            (sbm->getX() < endX)   &&
            (sbm->getY() > startY) &&
            (sbm->getY() < endY)   )
        {
            sbm->select();
            result = true;
        }
    }

    return result;
}

/**
 * @brief vleVpzModel::event (extends QWidget)
 *        Catch all events, used to implement custom tooltip
 *
 */
bool vleVpzModel::event(QEvent *event)
{
    if (event->type() != QEvent::ToolTip)
        QWidget::event(event);

    if (event->type() == QEvent::MouseButtonPress)
    {
        event->accept();
    }
    else if (event->type() == QEvent::ToolTip)
    {
        if (! mIsMaximized)
        {
            if (! mTooltip)
            {
                mTooltip = new QWidget(this->window());
                uiTooltip = new Ui::fileVpzTooltip();
                uiTooltip->setupUi(mTooltip);
                uiTooltip->labelModelName->setText(mName);
            }
            QPoint mousePos = window()->mapFromGlobal(QCursor::pos());
            mTooltip->move(mousePos.x()+10,mousePos.y()+10);
            mTooltip->show();
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        if (mTooltip)
        {
            mTooltip->hide();
        }
    }
    return true;
}
void vleVpzModel::enterEvent(QEvent *event)
{
    (void)event;
    // Do nothing
}

/**
 * @brief vleVpzModel::hideEvent (extends QWidget)
 *        Called when the widget is hidden
 *
 */
void vleVpzModel::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

/**
 * @brief vleVpzModel::showEvent (extends QWidget)
 *        Called when the widget is displayed
 *
 */
void vleVpzModel::showEvent (QShowEvent * event)
{
    QWidget::showEvent(event);
    mTitle.show();
    //
    QListIterator<vleVpzPort*> inItems(mInPorts);
    while( inItems.hasNext() )
    {
        vleVpzPort *item = inItems.next();
        item->show();
    }
    //
    QListIterator<vleVpzPort*> outItems(mOutPorts);
    while( outItems.hasNext() )
    {
        vleVpzPort *item = outItems.next();
        item->show();
    }
}

void vleVpzModel::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == 67)
        {
            QClipboard *clipboard = QApplication::clipboard();

            // Create the root document
            QDomDocument doc("vle_project");
            QDomElement root = doc.createElement("vle_project");
            doc.appendChild(root);
            // Insert base structure
            QDomElement sbmE = doc.createElement("structures");

            QListIterator<vleVpzModel*> items(mSubmodels);
            while( items.hasNext() )
            {
                vleVpzModel *sbm = items.next();
                if (sbm->isSelected())
                    sbm->xgetXml(&doc, &sbmE);
            }
            root.appendChild(sbmE);

            QString dat = doc.toString();
            clipboard->setText(dat);
        }
    }
}

/**
 * @brief vleVpzModel::mousePressEvent (extends QWidget)
 *        Called at the begining of a mouse click inside the widget
 *
 */
void vleVpzModel::mousePressEvent(QMouseEvent *evt)
{
    // Widget is movable only if not maximized
    if (mIsMaximized)
    {
        // Start mouse selection
        mSelect = true;
        // Save
        mSelectStart   = evt->pos();
        mSelectCurrent = evt->pos();
    }
    else
    {
        // Save the current position
        mOldPos = QPoint(evt->globalPos());
        mStartPos = mOldPos;
    }
}

/**
 * @brief vleVpzModel::mouseReleaseEvent (extends QWidget)
 *        Called when the mouse button is released
 *
 */
void vleVpzModel::mouseReleaseEvent(QMouseEvent *event)
{
    if (mIsMaximized)
    {
        emit sigFocus(0);
        if (mSelect)
        {
            // Deselect all currently selected sub-models
            deselectSub();
            // Select submodels included into mouse region
            mouseMultipleSelect();
            // End of mouse selection
            mSelect = false;

            // Refresh display
            repaint();
        }
    }
    else
    {
        if (event->globalPos() == mStartPos)
        {
            // Clear previous selection (if any)
            vleVpzModel *parent = (vleVpzModel *)parentWidget();
            parent->deselectSub();
            // Set the focus on the selected widget
            select(true);
        }
    }
}

/**
 * @brief vleVpzModel::mouseMoveEvent (extends QWidget)
 *        Called when the mouse is moved
 *
 */
void vleVpzModel::mouseMoveEvent(QMouseEvent *evt)
{
    // Widget is movable only if not maximized
    if (mIsMaximized)
    {
        if (mSelect)
        {
            mSelectCurrent = evt->pos();
            repaint();
        }
        return;
    }

    // Compute move length and direction
    const QPoint delta = evt->globalPos() - mOldPos;

    // Compute new X position
    int newX = x() + delta.x();
    if (newX < 5)
        newX = 5;
    mWidgetX = newX;
    // Compute new Y position
    int newY = y() + delta.y();
    if (newY < 5)
        newY = 5;
    mWidgetY = newY;
    // Move the widget
    move(mWidgetX, mWidgetY);
    // Save the new position as reference
    mOldPos = evt->globalPos();

    // Update parent size if needed
    vleVpzModel *parent = (vleVpzModel *)parentWidget();
    if (parent)
        parent->dispMaximize();

    // Update connections trace if needed
    if (parent)
    {
        bool refresh = false;
        QListIterator<vleVpzConn*> connItems(*parent->getConnections());
        while( connItems.hasNext() )
        {
            vleVpzConn *conn = connItems.next();
            if (conn->isLinkedWith(this))
            {
                conn->route();
                refresh = true;
            }
        }
        if (refresh)
            parent->repaint();
    }
}

/**
 * @brief vleVpzModel::mouseDoubleClickEvent (extends QWidget)
 *        Called when a mouse double-click is sent to the widget
 *
 */
void vleVpzModel::mouseDoubleClickEvent(QMouseEvent * event)
{
    (void)event;

    if ( ! mIsMaximized)
    {
        if (countSubmodels() > 0)
            emit sigDblClick(this);
    }
}

void vleVpzModel::portConnect(vleVpzPort *port)
{
    // Connections can be made only into the maximized model
    if ( ! mIsMaximized)
    {
        vleVpzModel *p = (vleVpzModel *)parent();

        // Forward the event to parent
        p->portConnect(port);
        return;
    }

    if (port->getModel() == this)
    {
        // Note: For Input or Output connection, variable
        //       are inverted !

        // If the port is an Input of model, use it as output
        if (port->getType() == vleVpzPort::TypeInput)
        {
            if (mPortOutSel)
                mPortOutSel->select(false);

            if (mPortOutSel != port)
            {
                mPortOutSel = port;
                mPortOutSel->select(true);
            }
            else
                mPortOutSel = 0;
        }

        // If the port is an Output of model, use it as input
        if (port->getType() == vleVpzPort::TypeOutput)
        {
            if (mPortInSel)
                mPortInSel->select(false);

            if (mPortInSel != port)
            {
                mPortInSel = port;
                mPortInSel->select(true);
            }
            else
                mPortInSel = 0;
        }
    }
    // Else, the port is owned by a submodel
    else
    {
        if (port->getType() == vleVpzPort::TypeInput)
        {
            if (mPortInSel)
                mPortInSel->select(false);

            if (mPortInSel != port)
            {
                mPortInSel = port;
                mPortInSel->select(true);
            }
            else
                mPortInSel = 0;
        }
        if (port->getType() == vleVpzPort::TypeOutput)
        {
            if (mPortOutSel)
                mPortOutSel->select(false);

            if (mPortOutSel != port)
            {
                mPortOutSel = port;
                mPortOutSel->select(true);
            }
            else
                mPortOutSel = 0;
        }
    }

    // If one Input and one Output has been selected ... connect them
    if (mPortInSel && mPortOutSel)
    {
        vleVpzConn *newConn = 0;

        if ( (mPortInSel->getModel()  == this) &&
             (mPortOutSel->getModel() == this) )
        {
            //
        }
        else
        {
            // Instanciate a new Connection object
            newConn = new vleVpzConn(this);

            if (mPortOutSel->getModel() == this)
                newConn->setType(vleVpzConn::In);
            else if (mPortInSel->getModel() == this)
                newConn->setType(vleVpzConn::Out);
            else
                newConn->setType(vleVpzConn::Internal);

            newConn->setSource     (mPortOutSel);
            newConn->setDestination(mPortInSel);

            if (newConn->isValid())
            {
                newConn->route();
                mConnections.append(newConn);
                // Refresh widget to show new connection
                repaint();
            }
            else
                delete newConn;
        }

        // De-select ports after connection established
        mPortInSel->select(false);
        mPortInSel = 0;
        mPortOutSel->select(false);
        mPortOutSel = 0;
    }
}

/**
 * @brief vleVpzModel::portDisconnect
 *        Remove the connection(s) associated to a port
 *
 */
void vleVpzModel::portDisconnect(vleVpzPort *port)
{
    // Connections can be removed only into the maximized model
    if ( ! mIsMaximized)
    {
        vleVpzModel *p = (vleVpzModel *)parent();

        // Forward the event to parent
        p->portDisconnect(port);
        return;
    }

    for (int i = 0; i < mConnections.length(); i++)
    {
        vleVpzConn *conn = mConnections.at(i);
        if (port->isConnected(conn))
        {
            mConnections.removeAt(i);
            delete conn;
            i--;
        }
    }

    // Refresh widget to remove connection lines
    repaint();
}

void vleVpzModel::contextMenu(const QPoint & pos)
{
    // Forward call, no port selected
    contextMenu(pos, 0);
}

void vleVpzModel::contextMenu(const QPoint & pos, vleVpzPort *port)
{
    // Get the global mouse position
    QPoint globalPos = mapToGlobal(pos);

    QAction *lastAction;

    QMenu ctxMenu;

    if (isMaximized())
    {
        lastAction = ctxMenu.addAction(tr("Connect"));    lastAction->setData(11);
        if (port == 0)
            lastAction->setEnabled(false);
        lastAction = ctxMenu.addAction(tr("Disconnect")); lastAction->setData(12);
        if ((port == 0) || (port->getConn() == 0))
            lastAction->setEnabled(false);
    }
    else
    {
        if ( ! hasModeler())
        {
            QMenu *modelerMenu = ctxMenu.addMenu(tr("Add Modeler"));

            int nb = mVpz->getPackage()->getModelerClassCount();
            for (int i = 0; i < nb; i++)
            {
                QString className = mVpz->getPackage()->getModelerClass(i);
                QString pluginName = mVpz->getPackage()->getModelerClassPlugin(className);

                QString menuLabel;
                menuLabel = className + " (" + pluginName + ")";

                lastAction = modelerMenu->addAction(menuLabel);
                lastAction->setData(40 + i);
            }
        }
        else
        {
            lastAction = ctxMenu.addAction(tr("Open Modeler"));
            lastAction->setData(4);
        }
        ctxMenu.addSeparator();
        lastAction = ctxMenu.addAction(tr("Add input port"));  lastAction->setData(1);
        lastAction = ctxMenu.addAction(tr("Add output port")); lastAction->setData(2);
        lastAction = ctxMenu.addAction(tr("Remove port"));     lastAction->setData(3);
        if (port == 0)
            lastAction->setEnabled(false);
        ctxMenu.addSeparator();
        lastAction = ctxMenu.addAction(tr("Connect"));    lastAction->setData(11);
        if (port == 0)
            lastAction->setEnabled(false);
        lastAction = ctxMenu.addAction(tr("Disconnect")); lastAction->setData(12);
        if ((port == 0) || (port->getConn() == 0))
            lastAction->setEnabled(false);
        lastAction = ctxMenu.addAction(tr("Rename"));     lastAction->setData(13);
        if (port == 0)
            lastAction->setEnabled(false);
    }

    QAction* selectedItem = ctxMenu.exec(globalPos);

    if (selectedItem)
    {
        int actCode = selectedItem->data().toInt();
        switch (actCode)
        {
            case 1:
            case 2:
            {
                // Allocate a new port
                vleVpzPort *newPort = new vleVpzPort();
                // Configure new port
                newPort->setParent(this);
                newPort->setName(tr("New port"));
                if (actCode == 1)
                {
                    newPort->setType(vleVpzPort::TypeInput);
                    newPort->move(2, nextInPosY());
                    // Insert new port to the model
                    mInPorts.append(newPort);
                }
                if (actCode == 2)
                {
                    newPort->setType(vleVpzPort::TypeOutput);
                    newPort->move(2, nextOutPosY());
                    // Insert new port to the model
                    mOutPorts.append(newPort);
                }
                // ... and display it :)
                fixWidgetSize(true);
                newPort->show();
                repaint();
                break;
            }

            // Menu "Remove port"
            case 3:
            {
                portDisconnect(port);
                vleVpzPort::portType pType = port->getType();
                if ( (pType == vleVpzPort::TypeInput)  && (mInPorts.contains(port)) )
                        mInPorts.removeOne(port);
                if ( (pType == vleVpzPort::TypeOutput) && (mOutPorts.contains(port)) )
                        mOutPorts.removeOne(port);
                delete port;
                repaint();
                break;
            }

            // Menu "Modeler"
            case 4:
            {
                emit sigOpenModeler();
                break;
            }

            case 40 ... 50:
            {
                QString className = mVpz->getPackage()->getModelerClass(actCode - 40);
                QString classFileName =  mVpz->getPackage()->getSrcFilePath(className + ".cpp");
                mModelerClass = mVpz->getPackage()->openSourceCpp(classFileName);

                emit sigAddModeler(className);
                break;
            }

            case 11:
                portConnect(port);
                break;
            case 12:
                portDisconnect(port);
                break;
            case 13:
                port->raise();
                port->edit(true);
                break;
        }
    }
}

sourceCpp *vleVpzModel::getModelerClass()
{
    if (mModelerClass == 0)
        hasModeler();

    return mModelerClass;
}

/**
 * @brief vleVpzModel::paintEvent (extends QWidget)
 *        Called when widget content must be refresh
 *
 */
void vleVpzModel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

#ifndef USE_GRAPHICVIEW
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
#endif

    QRect curSize = geometry();
    int curWidth  = curSize.width();
    int curHeight = curSize.height();

    QColor drawColor;
    if (mIsFocus || mIsSelected)
        drawColor = QColor(255, 0, 0);
    else if (mSubmodels.length())
        drawColor = QColor(0, 0, 255);
    else
        drawColor = QColor(0, 255, 0);

    // Instantiate a pen, used to draw lines
    QPen myPen(drawColor, mSettingLine, Qt::SolidLine);
    // Instantiate a brush
    QBrush myBrush(Qt::white);

    QPainter pRect(this);
    pRect.setPen(myPen);
    pRect.setBrush(myBrush);
    if (mSettingCorner)
        pRect.drawRoundedRect(1,1, curWidth-2, curHeight-16, mSettingCorner,mSettingCorner);
    else
        pRect.drawRect(1,1, curWidth-2, curHeight-16);
    pRect.end();

#ifndef USE_GRAPHICVIEW
    if (mConnections.length() && mIsMaximized)
    {
        QPen myPen(QColor(127,127,127), mSettingLine, Qt::SolidLine);
        QPen hlPen(QColor(0,  127,  0), mSettingLine, Qt::SolidLine);
        QPainter pLines(this);
        pLines.setPen(myPen);
        pLines.setBrush(myBrush);
        QListIterator<vleVpzConn*> connItems(mConnections);
        while( connItems.hasNext() )
        {
            vleVpzConn *conn = connItems.next();
            if (conn->isHighlighted())
            {
                pLines.setPen(hlPen);
                pLines.drawPolyline(conn->mLines);
            }
            else
            {
                pLines.setPen(myPen);
                pLines.drawPolyline(conn->mLines);
            }
        }
        pLines.end();
    }
#endif

    QListIterator<vleVpzPort*> outItems(mOutPorts);
    while( outItems.hasNext() )
    {
        vleVpzPort *item = outItems.next();
        int posX = width() - item->width() - 2;
        if (posX != item->x())
            item->move(posX, item->getPosY());
    }

#ifndef USE_GRAPHICVIEW
    // In case of a mouse selection, draw the rect of current region
    if (mSelect)
    {
        QPainter pRect(this);

        QPen selPen = QPen(QColor(drawColor));
        selPen.setWidth(2);
        pRect.setPen(selPen);

        pRect.setBrush(QBrush(QColor(200, 255, 220), Qt::SolidPattern));

        int selWidth  = mSelectCurrent.x() - mSelectStart.x();
        int selHeight = mSelectCurrent.y() - mSelectStart.y();
        pRect.drawRect(mSelectStart.x(),mSelectStart.y(), selWidth, selHeight);

        pRect.end();
    }
#endif
}
