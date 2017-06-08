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

#ifndef GVLE_VLE_DOM_H
#define GVLE_VLE_DOM_H

#include <QDebug>

#include "dom_tools.hpp"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QString>
#include <vle/value/Value.hpp>

namespace vle {
namespace gvle {

/**
 * @brief Class that implements DomObject especially for vleVpz
 */
class vleDomVpz : public DomObject
{
public:
    vleDomVpz(QDomDocument* doc);
    ~vleDomVpz();
    QString getXQuery(QDomNode node) override;
    QDomNode getNodeFromXQuery(const QString& query,
                               QDomNode d = QDomNode()) override;
};

/**
 * @brief Class that implements DomObject especially for vleVpz metadata
 */
class vleDomVpm : public DomObject
{
public:
    vleDomVpm(QDomDocument* doc);
    ~vleDomVpm();
    QString getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d = QDomNode());
};

/**
 * @brief Class that provides static functions to handle vpz files.
 * snapshots can be performed if a DomDiffStach is provided by the user.
 */
class vleDomStatic
{
public:
    vleDomStatic();
    virtual ~vleDomStatic();

    /******************************************************
     * Static functions
     ******************************************************/

    /**
     * @brief build an empty node corresponding to the value type from Vpz doc
     */
    static QDomElement buildEmptyValueFromDoc(QDomDocument& domDoc,
                                              vle::value::Value::type vleType);

    /**
     * @brief build a vle value from either tag
     * <integer>, <string>, <map> etc..
     * @param node correponding to a vle value
     * @param buildText, build vle::value::String for QDomText
     * @note: result is a new allocated vle value.
     *
     */
    static std::unique_ptr<value::Value> buildValue(const QDomNode& valNode,
                                                    bool buildText);

    /**
     * @brief Fill a Node from a value
     * @note: the main tag should corresponds to the value type ie:
     * <integer>, <string>, <map> etc..
     * @note: the map is first cleared
     *
     */
    static bool fillWithValue(QDomDocument& domDoc,
                              QDomNode node,
                              const vle::value::Value& val);

    /**
     * @brief List dynamics names
     * @param atom, is expected to be of the form
     *    <dynamics>
     *    ...
     *    </dynamics>
     * @return list of dynamics
     */
    static QSet<QString> dynamics(QDomNode atom);
    /**
     * @brief get the dynamic attached to an atomic model
     * @param atom, is expected to be of the form
     *    <model type="atomic" dynamics="" .../>
     * @return the dyn attached to atom
     */
    static QString attachedDynToAtomic(QDomNode atom);

    /**
     * @brief Add condition
     * @param atom, is expected to be of the form
     *    <conditions>
     *    ...
     *    </conditions>
     * @param condName, the name of the condition to add
     * @param domDoc, used to create the node
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return cond node if it is created or null node otherwise
     */
    static QDomNode addCond(QDomNode atom,
                            const QString& condName,
                            QDomDocument* domDoc,
                            DomDiffStack* snapObj = 0);
    static QSet<QString> conditions(QDomNode atom);

    /**
     * @brief get the list of attached cond
     * @param atom, is expexted to be of the form
     *    <model name="somemodel" type="atomic" ...>
     *    ...
     *    </model>
     * @return the list of conditions attached to the atomic model
     */
    static QSet<QString> attachedCondsToAtomic(const QDomNode& atom);
    /**
     * @brief Attach a list of conditions to an atomic model
     * @param atom, is expexted to be of the form
     *    <model name="myname" type="atomic" ...>
     *    ...
     *    </model>
     * @param the list of conditions to attach to the atomic model
     */
    static void attachCondsToAtomic(QDomNode& atom,
                                    const QSet<QString>& conds);

    /**
     * @brief tells if an atomic model is in debuging mode
     * @param atom, is expexted to be of the form
     *    <model name="somemodel" type="atomic" ...>
     *    ...
     *    </model>
     * @return true if the atomic model is in debugging mode
     */
    static bool debuggingAtomic(const QDomNode& atom);
    static bool setDebuggingToAtomic(QDomNode atom,
                                     bool val,
                                     DomDiffStack* snapObj = 0);

    /**
     * @brief Tells if a condition exists
     * @param conds, is expected to be of the form
     *    <conditions>
     *      <condition name="someport">
     *        ...
     *      </condition>
     *      ...
     *    </conditions>
     * @param condName, the name of the condition to search for
     * @return true if the condition exists
     */
    static bool existCondFromConds(QDomNode conds, const QString& condName);

    /**
     * @brief Tells if a condition port exists
     * @param atom, is expected to be of the form
     *    <condition name="somecond">
     *      <port name="someport">
     *        ...
     *      </port>
     *      ...
     *    </condition>
     * @param portName, the name of the port to search for
     * @return true if the condition port exists
     */
    static bool existPortFromCond(QDomNode atom, const QString& portName);

    /**
     * @brief Build a value from a condition port
     * @param atom, is expected to be of the form
     *    <condition name="somecond">
     *      <port name="someport">
     *        ...
     *      </port>
     *      ...
     *    </condition>
     * @param portName, the name of the port to search for
     * @return true if the condition port exists
     */
    static std::unique_ptr<value::Value>
    getValueFromPortCond(QDomNode atom, const QString& portName, int index);

    /**
     * @brief Remove the port form a condition
     * @param atom, is expected to be of the form
     *    <condition name="somecond">
     *      <port name="someport">
     *        ...
     *      </port>
     *      ...
     *    </condition>
     * @param portName, the name of the port to remove
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return true if the port has existed
     */
    static bool rmPortFromCond(QDomNode atom,
                               const QString& portName,
                               DomDiffStack* snapObj = 0);
    static bool renamePortFromCond(QDomNode atom,
                                   const QString& oldName,
                                   const QString& newName,
                                   DomDiffStack* snapObj = 0);

    /**
     * @brief Set the port values to a cond, without removing others
     * @param domDoc, a Dom document to create new QDomNode
     * @param atom, is expected to be of the form
     *    <condition name="somecond">
     *      <port name="someport">
     *        ...
     *      </port>
     *      ...
     *    </condition>
     * @param val, a map where keys are port names
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return true if modifications have been performed
     *
     * @note the ports into the map are first cleared, the others
     * remain the same. Only the first value can be filled
     */
    static bool fillConditionWithMap(QDomDocument& domDoc,
                                     QDomNode atom,
                                     const vle::value::Map& val,
                                     DomDiffStack* snapObj = 0);

    /**
     * @brief List the observables
     * @param atom, is expected to be of the form
     *    <observables>
     *      <observable name="someobs"/>
     *      ...
     *    </observables>
     * @return the list of the observables
     */
    static QSet<QString> observables(QDomNode atom);
    /**
     * @brief get the observable attached to an atomic model
     * @param atom, is expected to be of the form
     *    <model type="atomic" observables="" .../>
     * @return the obs attached to atom
     */
    static QString attachedObsToAtomic(QDomNode atom);

    /**
     * @brief Add a port to an observable
     * @param domDoc, a Dom document to create new QDomNode
     * @param atom, is expected to be of the form
     *    <observable name="someobs">
     *      <port name="someport"/>
     *      ...
     *    </observable>
     * @param portName, the name of the port
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return false if already present or error has occured
     */
    static bool addObservablePort(QDomDocument& domDoc,
                                  QDomNode atom,
                                  const QString& portName,
                                  DomDiffStack* snapObj = 0);
    static bool rmObservablePort(QDomNode atom,
                                 const QString& portName,
                                 DomDiffStack* snapObj = 0);
    static bool renameObservablePort(QDomNode atom,
                                     const QString& oldName,
                                     const QString& newName,
                                     DomDiffStack* snapObj = 0);

    /**
     * @brief Add/remove a port to input list of a model
     * @param domDoc, a Dom document to create new QDomNode
     * @param atom, is expected to be of the form
     *    <in>
     *      <port name="someInputPort"/>
     *      ...
     *    </in>
     * @param portName, the name of the port to add/remove
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return false if an error occurred or nothing is done
     */
    static bool addPortToInNode(QDomDocument& domDoc,
                                QDomNode atom,
                                const QString& portName,
                                DomDiffStack* snapObj = 0);
    static bool rmPortToInNode(QDomNode atom,
                               const QString& portName,
                               DomDiffStack* snapObj = 0);
    static bool renamePortToInNode(QDomNode atom,
                                   const QString& oldName,
                                   const QString& newName,
                                   DomDiffStack* snapObj = 0);

    /**
     * @brief Add a port to output list of a model
     * @param domDoc, a Dom document to create new QDomNode
     * @param atom, is expected to be of the form
     *    <out>
     *      <port name="someInputPort"/>
     *      ...
     *    </out>
     * @param portName, the name of the port
     * @param snapObj, snapshot is performed on snapObj if not null
     * @return false if already present or error has occured
     */
    static bool addPortToOutNode(QDomDocument& domDoc,
                                 QDomNode atom,
                                 const QString& portName,
                                 DomDiffStack* snapObj = 0);
    static bool rmPortToOutNode(QDomNode atom,
                                const QString& portName,
                                DomDiffStack* snapObj = 0);
    static bool renamePortToOutNode(QDomNode atom,
                                    const QString& oldName,
                                    const QString& newName,
                                    DomDiffStack* snapObj = 0);

    /**
     * @brief Return the list of connections from a coupled model
     * @param coupled, the coupled model : <model type="coupled">
     * @param connType, either "input", "internal" or "output"
     * @param origin, true or false
     * @param submodel, name of the submodel
     * @param port, name of the submodel port
     *
     * @return the list of <connection> of type connType such as the origin
     * (or the destination depending on 'origin') is the submodel port given
     * in parameters.
     */
    static QList<QDomNode> getConnectionsFromCoupled(QDomNode coupled,
            QString connType, bool origin, QString submodel, QString port);
    /**
     * @brief Rename a submodel of a coupled model, and update connections
     * @brief domDoc, used to create QDomNode if necessary
     * @brief atom, is expected to be a coupled model, eg:
     *   <model type="coupled" ...>
     *    <in/>
     *    <out/>
     *    <submodels>
     *     <model name=old_model ..>
     *      <in/>
     *      <out>
     *       <port name="A"/>
     *      </out>
     *     <model name="otherModel" ..>
     *      <in>
     *       <port name="B"/>
     *      </in>
     *      <out/>
     *    </submodels>
     *    <connections>
     *      <connection type="internal">
     *        <origin model=old_model port="A"/>
     *        <destination model="otherModel" port=B/>
     *      </connection>
     *    </connections>
     *   </model>
     *  @return true if a modification has been performed
     */
    static bool renameModelIntoCoupled(QDomDocument& domDoc,
                                       QDomNode atom,
                                       QString old_model,
                                       QString new_model,
                                       DomDiffStack* snapObj = 0);
    static bool renameModelIntoStructures(QDomDocument& domDoc,
                                          QDomNode atom,
                                          QString old_model,
                                          QString new_model,
                                          DomDiffStack* snapObj = 0);

    static QStringList subModels(QDomNode atom);
    static QDomNode subModel(QDomNode atom, QString model_name);

    static QString connectionModOrig(QDomNode atom);
    static QString connectionModDest(QDomNode atom);

    /**
     * @brief Get the types of an output
     * @param atom, is expected to be of the form
     *    <view type ="sometype" />
     * @return the list of types :
     * - timed or
     * - a combination of finish, output, external, internal, confluent
     *
     */
    static QStringList getViewTypeToView(const QDomNode& atom);
};
}
} // namepsaces

#endif
