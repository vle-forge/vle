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

#ifndef gvle_VLEVPZ_H
#define gvle_VLEVPZ_H

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
#include "vleDomDiffStack.h"
#include "logger.h"


namespace vle {
namespace gvle {

class vlePackage;
class vleDomDiffStack;

/**
 * @brief Class that implements vleDomObject especially for vleVpz
 */
class vleDomVpz : public vleDomObject
{
public:
    vleDomVpz(QDomDocument* doc);
    ~vleDomVpz();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};


/**
 * @brief Main class
 */
class vleVpz : public QObject
{
    Q_OBJECT
public:
    vleVpz(const QString& filename);
    vleVpz(QXmlInputSource& filename);

    /******************************************************
     * Access to specific nodes in the vpz from Doc
     ******************************************************/
    const QDomDocument& getDomDoc() const;
    QDomDocument& getDomDoc();
    /**
     * @brief get <classes> tag from Vpz doc
     */
    QDomNode classesFromDoc();
    /**
     * @brief get <class> tag from Vpz doc
     * @param className, attribute 'name' of <class>
     */
    QDomNode classFromDoc(const QString& className);
    /**
     * @brief get first <model> tag into a <class> tag
     * @param className, attribute 'name' of <class>
     */
    QDomNode classModelFromDoc(const QString& className);
    /**
     * @brief tells if a class exists
     * @param className: name of the class
     */
    bool existClassFromDoc(const QString& className);
    /**
     * @brief add a class to a doc
     * @param atomic: if true add a class with an atomic model
     * @return the name otf the class
     */
    QString addClassToDoc(bool atomic);
    /**
     * @brief Renames the class of the doc
     * @param oldClass: name of the class
     * @param newClass: new name to set to the class
     */
    void renameClassToDoc(const QString& oldClass, const QString& newClass);
    /**
     * @brief copy a class
     * @param className: the name of the class to copy
     * @return the name of the copy
     */
    QString copyClassToDoc(const QString& className);
    /**
     * @brief removes a class
     * @param className: the name of the class to remove
     */
    void removeClassToDoc(const QString& className);
    /**
     * @brief get <vle_project> tag from Vpz doc
     */
    QDomNode vleProjectFromDoc() const;
    /**
     * @brief get <experiment> tag from Vpz doc
     */
    QDomNode experimentFromDoc() const;
    /**
     * @brief get <observables> tag from Vpz doc
     */
    QDomNode obsFromDoc() const;
    /**
     * @brief get <views> tag from Vpz doc
     */
    QDomNode viewsFromDoc() const;
    /**
     * @brief get <conditions> tag from Vpz doc
     */
    QDomNode condsFromDoc() const;
    /**
     * @brief get node with tag <structures> tag from Vpz doc,
     */
    QDomNode structuresFromDoc() const;
    /**
     * @brief get node with tag <dynamics> tag from Vpz doc,
     */
    QDomNode dynamicsFromDoc() const;
    /**
     * @brief get list of node with tag <dynamic> tag from Vpz doc,
     */
    QDomNodeList dynListFromDoc() const;
    /**
     * @brief get <dynamic> tag from Vpz doc
     */
    QDomNode dynamicFromDoc(const QString& dyn) const;
    /**
     * @brief get list of node with tag <port> tag from Vpz doc,
     * for condition of name 'condName'
     */
    QDomNodeList portsListFromDoc(const QString& condName) const;
    /**
     * @brief get <port> tag from Vpz Doc into condition condName
     * which attribute 'name' is portName
     */
    QDomNode portFromDoc(const QString& condName, const QString& portName) const;
    /**
     * @brief get the root model
     */
    QDomNode modelFromDoc();

    /**
     * @brief get <connections> node from Vpz Doc into a <model>
     * which model_query, a Xquery that points to the model
     */
    QDomNode modelConnectionsFromDoc(const QString& model_query);
    /**
     * @brief get <connection> node from Vpz Doc into a <connections>
     * @param model_query, a Xquery that points to the model
     * @param destinationFullPath, the full path to the dest model
     * @param sourcePort, the name of the source port
     * @param destinationPort, the name of the dest port
     */
    QDomNode modelConnectionFromDoc(const QString& model_query,
            const QString& destinationFullPath, const QString& sourcePort,
            const QString& destinationPort);


    QDomNode baseModelFromModelQuery(const QString& model_query);


    /******************************************************
     * Access to specific nodes in the vpz from internal nodes
     ******************************************************/
    /**
     * @brief get <observables> tag from <views> tag
     */
    QDomNode obsFromViews(QDomNode node) const;
    /**
     * @brief get <outputs> tag from <views> tag
     */
    QDomNode outputsFromViews(QDomNode node);
    /**
     * @brief get map from tag <output> (configuration of output)
     */
    QDomNode mapFromOutput(QDomNode node);
    /**
     * @brief get atomic model tag <model> from a tag <model>,
     * which name is atom
     */
    QDomNode atomicModelFromModel(const QDomNode& node,
            const QString& atom) const;
    /**
     * @brief get <connections> tag  from a tag <model>,
     * @param node: QDomNode <model> with xpath = //model
     */
    QDomNode connectionsFromModel(const QDomNode& node) const;
    /**
     * @brief get <model> tag from a tag <model> into <submodels>,
     * @param node: QDomNode <model> with xpath = //model
     */
    QList<QDomNode> submodelsFromModel(const QDomNode& node);

    /*******************************************************
     * Utils Functions QDom
     ******************************************************/
    vleDomVpz* vdo();

    QList<QDomNode> childNodesWithoutText(QDomNode node,
            const QString& nodeName = "") const;

    /**
     * @brief remove children that are QDomText to a node
     */
    void removeTextChilds(QDomNode node, bool recursively=true);
    /**
     * @brief set attribute value of a node which name is attrName
     * and value is val
     */
    void setAttributeValue(QDomElement node, const QString& attrName,
               const QString& val);



    QString mergeQueries(const QString& query1, const QString& query2);
    QString subQuery(const QString& query, int begin, int end);

    /*****************************************************
     * TODO A TRIER
     *****************************************************/

    QString        getFilename() const;
    QString        getBasePath();

    QString        getAuthor() const;
    void           setAuthor(const QString author);
    QString        getDate() const;
    void           setDate(const QString date);
    QString        getVersion() const;
    void           setVersion(const QString version);
    QString        getExpName() const;
    void           setExpName(const QString name);
    QString        getExpDuration() const;
    void           setExpDuration(const QString duration);
    QString        getExpBegin() const;
    void           setExpBegin(const QString begin);

    // Observables primitives

    /**
     * @brief tells if there is a <port> wich attribute 'name' equals
     * portName in a <observable> node
     */
    bool existPortFromObs(const QDomNode& node, const QString& portName) const;
    /**
     * @brief tells if a observable exists
     */
    bool existObsFromDoc(const QString& obsName) const;
    /**
     * @brief tells if the port portName exists in observable obsName
     */
    bool existObsPortFromDoc(const QString& condName,
            const QString& portName) const;
    /**
     * @brief tells if a observable port already has a view
     */
    bool existViewFromObsPortDoc(const QString& obsName, const QString& obsPort,
            const QString& viewName) const;
    /**
     * @brief get <observables> tag from Vpz doc
     */
    QDomNode observablesFromDoc() const;
    /**
     * @brief get list of node with tag <port> tag from Vpz doc,
     * for observable of name 'obsName'
     */
    QDomNodeList obsPortsListFromDoc(const QString& obsName) const;
    /**
     * @brief get list of node with tag <observable> tag from tag <observables>
     */
    QDomNodeList obssListFromObss(const QDomNode& node) const;
    /**
     * @brief get list of node with tag <port> tag from tag <observable>
     */
    QDomNodeList portsListFromObs(const QDomNode& node) const;
    /**
     * @brief get <observable> tag from  tag <observables>
     * which attribute 'name' is obsName
     */
    QDomNode obsFromObss(const QDomNode& node, const QString& obsName) const;
    /**
     * @brief get list of <attachedview> from an observable port
     */
    QDomNodeList viewsListFromObsPort(const QDomNode& node) const;
    /**
     * @brief get <port> tag from  tag <observable>
     * which attribute 'name' is portName
     */
    QDomNode portFromObs(const QDomNode& node, const QString& portName) const;
    /**
     * @brief set the 'name' attribute of tag <Observable> to a new value
     * by the way also update models that do use this renamed obs.
     */
    void renameObservableToDoc(const QString& oldName, const QString& newName);
    /**
     * @brief rename an observable port
     * (set attribute 'name' of <port> child of an <observable> with
     * attribute 'name' equals obsName)
     * @param obsName, the observable name
     * @param oldPortName, the old name of the port
     * @param newPortName, the new name of the port
     */
    void renameObsPortToDoc(const QString& obsName,
            const QString& oldPortName, const QString& newPortName);
    /**
     * @brief set the 'name' attribute of tag <Observable> to a new value
     * for the models configuration that needs that
     */
    void renameObservableFromModel(QDomNode &node,
            const QString &oldName, const QString &newName);

    /**
     * @brief rename a model xpath = //model
     * @param node, tag <model> to rename
     * @param newName, the new name to assign to node
     * @note all connections are renamed also.
     */
    void renameModel(QDomNode node, const QString& newName);

    /**
     * @brief remove a model
     * @param node: QDomNode <model> with xpath = //model
     * @note: update connections by getting ancestor node
     */
    void rmModel(QDomNode node);
    /**
     * @brief remove a model connection
     * @param node: QDomNode <connection>
     *     with xpath = //model/connections/connection
     */
    void rmModelConnection(QDomNode node, bool undo=true);

    /**
     * @brief return true if modName is a submodel of node
     * @param node, which node name is <model>
     * @param modName, the submodel name to check
     */
    bool existSubModel(QDomNode node, const QString& modName);

    /**
     * @brief add a submodel to a model
     * @param node: QDomNode <model> with xpath = //model
     * @param type: "coupled" or "atomic"
     * @param pos: position x, y
     * @note: add submodels tag is required
     */
    void addModel(QDomNode node, const QString& type, QPointF pos);

    /**
     * @brief copy a list of models to a coupled model (rename if necessary)
     * @param modsToCopy: list of QDomNode <model> to copy
     * it is required that all models have the same ancestor (part of the same
     * coupled model)
     * @param modDest: destination model with xpath=//model[@type=coupled]
     * @param xtranslation:
     * @param ytranslation:
     */
    void copyModelsToModel(QList<QDomNode> modsToCopy, QDomNode modDest,
            qreal xtranslation, qreal ytranslation);
    /**
     * @brief set the 'name' attribute of tag <port> to a new value
     * @param node: QDomNode <port> with xpath = //model//in//port
     * @param newName: new name
     * @note: update connections by getting ancestor nodes
     */
    void renameModelPort(QDomNode node, const QString& newName);
    /**
     * @brief remove a port to a model, ie. tag <port>
     * @param node: QDomNode with xpath = //model//in//port
     * @note: remove all conections associated to
     */
    void rmModelPort(QDomNode node);
    /**
     * @brief add a port to a model; ie tag <model>
     * @param node: QDomNode <model> with xpath = //model
     * @param type: "in" or "out"
     */
    void addModelPort(QDomNode node, const QString& type);
    /**
     * @brief add an input port to a model; ie tag <model>
     * @param node: QDomNode <model> with xpath = //model
     */
    void addModelInputPort(QDomNode node);
    /**
     * @brief add an output port to a model; ie tag <model>
     * @param node: QDomNode <model> with xpath = //model
     */
    void addModelOutputPort(QDomNode node);
    /**
     * @brief add a connection between two port
     * @param node1: QDomNode <port> with xpath = //model//port
     * @param node2: QDomNode <port> with xpath = //model//port
     */
    void addModelConnection(QDomNode node1, QDomNode node2);
    /**
     * @brief rm a view from a observable port
     */
    void rmViewToObsPortDoc(const QString& obsName, const QString& obsPort,
            const QString& viewName);

    /**
     * @brief remove a <observable> tag from a Vpz Doc
     * with attribute 'name'  obsName
     */
    void rmObservableFromDoc(const QString& obsName);
    /**
     * @brief remove a <observable> tag from a QDomNode
     * and sub node
     * with attribute 'name' obsName
     * with attribute 'node' QDomNode model
     */
    void rmObservableFromModel(QDomNode &node, const QString &obsName);
    /**
     * @brief remove <port> tag from a observable to a Vpz doc
     * which attribute 'name' is portName
     */
    void rmObsPortToDoc(const QString& condName, const QString& portName);
    /**
     * @brief remove <port> tag from <observable>
     * which attribute 'name' is portName
     */
    void rmObsPortFromCond(QDomNode node, const QString& portName);
    /**
     * @brief unset a observable from a atomic model only if the model
     * is observed by
     * @param atom, atomic model node
     */
    void unsetObsFromAtomicModel(QDomNode &node, const QString& obsName);
    /**
     * @brief unset a observable from a atomic model
     * @param model_query, a Xquery that points to model
     * @param obsName, obs name
     */
    void unsetObsFromAtomicModel(const QString& model_query);
    /**
     * @brief get a new port name for obs obsName not already in tag
     * <observable> from the Vpz doc
     */
    QString newObsPortNameToDoc(const QString& obsName) const;
    /**
     * @brief get a new observable name not already in tag <observables>
     * from the Vpz doc
     */
    QString newObsNameToDoc() const;
    /**
     * @brief add a <observable> tag to a Vpz Doc
     * whith attribute 'name' obsName
     */
    void addObservableToDoc(const QString& obsName);
    /**
     * @brief add a <port> tag to a Vpz Doc
     * with attribute 'name'  portName for condition condName
     */
    void addObsPortToDoc(const QString& obsName, const QString& portName);

    /**
     * @brief add an <attachedview> to an observable port
     */
    void attachViewToObsPortDoc(const QString& obsName, const QString& portName,
                                const QString& viewName);

    /**
     * @brief set width of a model
     * @param node, model node
     * @param width
     */
    void setWidthToModel(QDomNode node, double width);

    void setHeightToModel(QDomNode node, double height);

    void setPositionToModel(QDomNode node, double x, double y);

    void setPositionToModel(QList<QDomNode>& nodes, QList<double> xs,
            QList<double> ys);

    /**
     * @brief set an observable to an anatomic model
     * @param node, atomic model
     * @param obsName, observable name
     *
     */
    void setObsToAtomicModel(QDomNode &node, const QString& obsName);
    /**
     * @brief set an observable to an anatomic model
     * @param model_query, a Xquery that points to a model
     * @param obsName, observable name
     *
     */
    void setObsToAtomicModel(const QString& model_query, const QString& obsName);

    /**
     * @brief build an empty node corresponding to the value type from Vpz doc
     */
    QDomElement buildEmptyValueFromDoc(vle::value::Value::type vleType);

    /**
    * @brief tells if a view exists
    */
    bool existViewFromDoc(const QString& viewName) const;

    bool existDynamicFromDoc(const QString& dynName) const;

     /**
     * @brief tells if a condition exists
     */
    bool existCondFromDoc(const QString& condName) const;

    /**
     * @brief tells if the port portName exists in condition condName
     */
    bool existPortFromDoc(const QString& condName,
            const QString& portName) const;
    /**
     * @brief Renames a view and the output associated to it
     * (attribute 'output' of <view> and attribute 'name' of <output>
     * @brief oldName, the old name of the view (and output)
     * @brief newName, the new name of the view (and output)
     */
    virtual void renameViewToDoc(const QString& oldName, const QString& newName);
    /**
     * @brief set the 'name' attribute of tag <condition> to a new value
     */
    virtual void renameConditionToDoc(const QString& oldName,
            const QString& newName);
    void renameDynamicToDoc(const QString& oldName, const QString& newName);
    /**
     * @brief set the 'name' attribute of tag <port> to a new value
     */
    void renameCondPortToDoc(const QString& condName, const QString& oldName,
            const QString& newName);


    QString newDynamicNameToDoc() const;
    /**
     * @brief get a new view name not already in tag <views>
     * from the Vpz doc
     */
    QString newViewNameToDoc() const;
    /**
     * @brief get a new condition name not already in tag <conditions>
     * from the Vpz doc
     */
    QString newCondNameToDoc() const;
    /**
     * @brief get a new port name for condition condName not already in tag
     * <condition> from the Vpz doc
     */
    QString newCondPortNameToDoc(const QString& condName) const;
    /**
     * @brief Add a view (and output with the same name) to a Vpz Doc
     * (tag <view> with attr 'name' set to viewName and default config based
     * on vle.output/file output plugin)
     * @param viewName, name of the view (and output associated to it) to add
     */
    virtual void addViewToDoc(const QString& viewName);
    /**
     * @brief rm a <view> tag to a Vpz Doc
     * whith attribute 'name'  viewName
     */
    virtual void rmViewToDoc(const QString& viewName);
    /**
     * @brief add a <condition> tag to a Vpz Doc
     * whith attribute 'name'  condName
     */
    QDomNode addConditionToDoc(const QString& condName);

    /**
     * @brief add a <dynamic> tag to a Vpz Doc
     * whith dyn, attribute 'name'  set to dyn
     */
    QDomNode addDynamicToDoc(const QString& dyn);


    /**
     * @brief add a <dynamic> tag to a Vpz Doc
     * @param dyn, attribute 'name'  set to dyn
     * @param pkgName, attribute 'package'  set to pkgName
     * @param libName, attribute 'library'  set to libName
     */
    QDomNode addDynamicToDoc(const QString& dyn, const QString& pkgName,
            const QString& libName);

    void configDynamicToDoc(const QString& dyn, const QString& pkgName,
            const QString& libName);


    /**
     * @brief Copy a dynamic
     * @param dyn, the name of the dynamic to copy
     * @param newDyn, the new name for the copy
     */
    QDomNode copyDynamicToDoc(const QString& dyn, const QString& newDyn);

    /**
     * @brief add a <port> tag to a Vpz Doc
     * with attribute 'name'  portName for condition condName
     */
    QDomNode addCondPortToDoc(const QString& condName, const QString& portName);

    /**
     * @brief remove a <condition> tag to a Vpz Doc
     * with attribute 'name'  condName
     */
    virtual void rmConditionToDoc(const QString& condName);
    /**
     * @brief remove <port> tag from a condition to a Vpz doc
     * which attribute 'name' is portName
     */
    void rmCondPortToDoc(const QString& condName, const QString& portName);
    /**
     * @brief add a value to a tag <port> tag from a condition into a Vpz doc
     */
    void addValuePortCondToDoc(const QString& condName, const QString& portName,
            const vle::value::Value& val);
    /**
     * @brief rm a value from a tag <port> tag from a condition into a Vpz doc
     * 'index' is the index of value in port
     */
    void rmValuePortCondToDoc(const QString& condName, const QString& portName,
            int index);
    /**
     * @brief get list of node with tag <condition> tag from tag <conditions>
     */
    QDomNodeList condsListFromConds(const QDomNode& node) const;

    /**
     * @brief get list of node with tag <port> tag from tag <condition>
     */
    QDomNodeList portsListFromCond(const QDomNode& node) const;

    /**
     * @brief get <condition> tag from  tag <conditions>
     * which attribute 'name' is condName
     */
    QDomNode condFromConds(const QDomNode& node, const QString& condName) const;

    /**
     * @brief get <port> tag from  tag <condition>
     * which attribute 'name' is portName
     */
    QDomNode portFromCond(const QDomNode& node, const QString& portName) const;

    /**
     * @brief tells if there is a <port> wich attribute 'name' equals
     * portName in a <condition> node
     */
    bool existPortFromCond(const QDomNode& node, const QString& portName) const;

    /**
     * @brief remove <condition> tag from <conditions>
     * which attribute 'name' is condName
     */
    void rmCondFromConds(QDomNode node, const QString& condName);

    /**
     * @brief remove <port> tag from <condition>
     * which attribute 'name' is portName
     */
    void rmPortFromCond(QDomNode node, const QString& portName);
    /**
     * @brief add a <view> tag to <views>
     * whith attribute 'name'  viewName
     * NOTE: add also an output and configure it with vle.output/file.
     */
    QDomNode addView(QDomNode node, const QString& viewName);
    /**
     * @brief add a <condition> tag to <conditions>
     * whith attribute 'name'  condName
     */
    QDomNode addCondition(QDomNode node, const QString& condName);

    /**
     * @brief get the string list of condition "," separated
     * @param model_query, a Xquery that points to a model
     */
    QString modelCondsFromDoc(const QString& model_query);

    /**
     * @brief Tells if a condition is attached to an atomic model
     * @param model_query, a Xquery that points to a model
     * @param condName, the condition name
     * @return true if condName is attached to model_query
     */
    bool isAttachedCond(const QString& model_query,
            const QString& condName);

    /**
     * @brief get the dynamics of an atomic model
     * @param model_query, a Xquery that points to a model
     */
    QString modelDynFromDoc(const QString& model_query);

    /**
     * @brief get the observable
     * @param model_query, a Xquery that points to a model
     *
     */
    QString modelObsFromDoc(const QString& model_query);

    /**
     * @brief sets a dynamic to an an atomic model
     * @param model_query, a Xquery that points to a model
     *
     */
    void setDynToAtomicModel(const QString& model_query, const QString& dyn,
            bool undo=true);

    /**
     * @brief Removes a dynamic
     * @param dyn, the dynamic to remove
     */
    void removeDyn(const QString& dyn);

    /**
     * @brief attach a condition to an an atomic model
     * @param model_query, a Xquery that points to a model
     *
     */
    void attachCondToAtomicModel(const QString& model_query,
            const QString& condName);
    /**
     * @brief detach a condition to an anatomic model
     * @param model_query, a Xquery that points to a model
     * @param condName, condition name
     *
     */
    void detachCondToAtomicModel(const QString& model_query,
            const QString& condName);

    /**
     * @brief get <output> tag from <outputs> tag
     * which name is outputName
     */
    QDomNode outputFromOutputs(QDomNode node, const QString& outputName);


    /**
     * @brief build a map from an output configuration
     * (map under <output>)
     * @param outputName, the name of the output (=viewName)
     * @return the configuration map if present or 0
     */
    vle::value::Map* buildOutputConfigMap(const QString& outputName);
    /**
     * @brief fills the configuration of an output
     * (map under <output>)
     * @param outputName, the name of the output (=viewName)
     * @param mapConfig, the map for configuration
     */
    void fillOutputConfigMap(const QString& outputName,
            const vle::value::Map& mapConfig);

    /**
     * @brief get the list of attribute values of 'name' of different <output>
     * in <experiment>/<views>/<outputs>
     * @param the values to fill
     */
    void viewOutputNames(std::vector<std::string>& v) const;
    /**
     * @brief get the list of attribute values of 'name' of different <observable>
     * in <experiment>/<views>/<observables>
     * @param the values to fill
     */
    void viewObservableNames(std::vector<std::string>& v) const;
    /**
     * @brief get the list of attribute values of 'name' of different <view>
     * in <experiment>/<views>
     * @param the values to fill
     */
    void viewNames(std::vector<std::string>& v) const;
    /**
     * @brief get <view> tag from <views> tag
     * which name is viewName
     */
    QDomNode viewFromViews(QDomNode node, const QString& viewName) const;
    /**
     * @brief Get the node of a view
     * @brief viewName, the name of the view to get
     */
    QDomNode viewFromDoc(const QString& viewName) const;
    /**
     * @brief get type (timed, finish or event) from a view
     *  (attribute 'type' of tag <view>)
     * @brief viewName, the name of the view
     */
     QString viewTypeFromDoc(const QString& viewName) const;
     /**
      * @brief set view type (timed, event, finish) to a view
      * (attribute 'type' to a tag <view>)
      * @param viewName, the name of the view
      * @param viewType, either timed, finish or event
      */
     void setViewTypeToDoc(const QString& viewName, const QString& viewType);
     /**
      * @brief get time step of a view
      *  (attribute 'timestep' of tag <view>)
      * @brief viewName, the name of the view
      */
     double timeStepFromDoc(const QString& viewName) const;
     /**
      * @brief set time step to a view
      *  (attribute 'timestep' of tag <view>)
      * @param viewName, the name of the view
      * @param ts, the new timestep
      */
     void setTimeStepToDoc(const QString& viewName, double ts);
     /**
      * @brief set output plugin to a view
      * @param outputPlugin, has to be of the form : package/plugin
      */
     virtual bool setOutputPluginToDoc(const QString& viewName,
             const QString& outputPlugin);
     /**
     * @brief get output plugin from a tag <output>
     * plugin name has the form :
     *   package/plugin
     */
    QString getOutputPlugin(QDomNode node);
    /**
     * @brief gives the type of a value of
     */
    vle::value::Value::type valueType(const QString& condName,
            const QString& portName, int index) const;
    /**
     * @brief build a vle value from either tag
     * <integer>, <string>, <map> etc..
     * @param node correponding to a vle value
     * @param buildText, build vle::value::String for QDomText
     * @note: result is a new allocated vle value.
     */
    vle::value::Value* buildValue(const QDomNode& valNode,
            bool buildText) const;
    /**
     * @brief build a vle value at a given index from a port
     * @param condName, the condition name: tag <condition>
     * @param portName, the port name: tag <port>
     * @param valIndex, index of the value to build from
     * the set of values attached to the port
     */
    vle::value::Value* buildValueFromDoc(const QString& condName,
            const QString& portName, int valIndex) const;
    /**
     * @brief build a vle value at a given index from a port
     * @param portNode, the node containing the port values: tag <port>
     * @param valIndex, index of the value to build from
     * the set of values attached to the port
     */
    vle::value::Value* buildValue(const QDomNode& portNode, int valIndex) const;

    unsigned int nbValuesInPortFromDoc(const QString& condName,
            const QString& portName);


    /**
     * @brief Fill a vector of vle values with the multipl values contained by
     * a condition port
     * @param condName, the name of the cond: tag <condtion>
     * @param portName, the name of the port of condName: tag <port>
     * @param values, the vector of values to fill
     * @note: values is first cleared
     */
    bool fillWithMultipleValueFromDoc(const QString& condName,
         const QString& portName, std::vector<vle::value::Value*>& values) const;
    /**
     * @brief Fill a vector of vle values with the multipl values contained by
     * a condition port
     * @param port, containing the multiple values: tag <port>
     * @param values, the vector of values to fill
     * @note: values is first cleared
     */
    bool fillWithMultipleValue(QDomNode port,
            std::vector<vle::value::Value*>& values) const;
    /**
     * @brief Fill a value at index of <port> portName of <condtion> condName
     * @note: the map is first cleared
     */
    bool fillWithValue(const QString& condName, const QString& portName,
            int index, const vle::value::Value& val);

    /**
     * @brief fill a vector of strings with classes names
     * @param the vector to fill
     */
    void fillWithClassesFromDoc(std::vector<std::string>& toFill);

    /**
     * @brief Fill a Node from a value
     * @note: the main tag should corresponds to the value type ie:
     * <integer>, <string>, <map> etc..
     * @note: the map is first cleared
     */
    bool fillWithValue(QDomNode node, const vle::value::Value& val);

    /**
     * @brief tells if a node is an integer from undefined tag
     */
    bool isInteger(QDomNode node);
    /**
     * @brief get an integer vle value from tag <integer>
     */
    int getInteger(QDomNode node);
    /**
     * @brief get the integer vle value from tag <map> at a specified key
     */
    int getIntegerFromMap(QDomNode node, QString key);
    /**
     * @brief tells the key name has an integer from tag <map>
     */
    bool existIntegerKeyInMap(QDomNode node, QString key);
    /**
     * @brief add an integer key in a <map>
     */
    void addIntegerKeyInMap(QDomNode* node, const QString& key,
            int val);
    /**
     * @brief Remove all childs from a QDomNode (keep attributes)
     */
    void removeAllChilds(QDomNode node);
    /**
     * @brief get <output>  from a tag <views>
     * @note : combine outputsFromViews, outputFromOutput
     */
    QDomNode getOutputFromViews(QDomNode node,
            const QString& outputName);
    /**
     * @brief Get vle output plugin attached to an output (=view)
     * @param outputName (=viewName), the name of the output or view
     * @return a vle output plugin of the form : package/plugin
     */
    QString getOutputPluginFromDoc(const QString& outputName);

    /**
     * @brief Fill a QList with names of the dynamics
     * @param toFill, the QList to fill with names of dynamics
     */
    void fillWithDynamicsList(QList <QString>& toFill) const;

    /**
     * @brief Tells if a dynamic exists
     * @param dyn, the dynamic to search for
     * @return true if the dynamic dyn exists, false otherwise
     */
    bool existDynamicIntoDoc(const QString& dyn) const;

    /**
     * @brief Tells if a dynamic exists
     * @param dyn, the dynamic to search for
     * @return true if the dynamic dyn exists, false otherwise
     */
    bool existDynamicIntoDynList(const QString& dyn,
            const QDomNodeList& dynList) const;

    /**
     * @brief Get the name of the package containing a dynamic
     * @param dyn, the dynamic to look for
     * @return the name of the package containing dyn
     *
     */
    QString getDynamicPackage(const QString& dyn) const;

    /**
     * @brief Get the name of the library containing a dynamic
     * @param dyn, the dynamic to look for
     * @return the name of the library containing dyn
     */
    QString getDynamicLibrary(const QString& dyn) const;

    void           setBasePath(const QString path);
    vlePackage*    getPackage();
    void           setPackage(vlePackage* package);
    bool           isAltered();
    virtual void   save();
    void           removeDynamic(const QString& dynamic);

signals:
    void sigChanged(QString filename);
    void observablesUpdated();
    void viewsUpdated();
    void conditionsUpdated();
    void modelsUpdated();
    void dynamicsUpdated();
    void experimentUpdated();

    protected:
    bool startElement(const QString &namespaceURI,
            const QString &localName,
            const QString &qName,
            const QXmlAttributes &attributes);
public:
    QByteArray xGetXml();
    void setLogger(Logger *logger)
    {
        mLogger = logger;
    }
    Logger* logger()
    {
        return mLogger;
    }

private:
    void xReadDom();
    void xSaveDom(QDomDocument *doc);
private:
    QString      mFilename;
    QString      mPath;
    QFile        mFile;
    QDomDocument mDoc;
    vlePackage*  mPackage;
    Logger*      mLogger;
protected:
    vleDomVpz*       mVdo;
    vleDomDiffStack* undoStack;
};

}}//namepsaces

#endif
