/**
 * @file vle/extension/DSDevs.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/extension/DSDevs.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Exception.hpp>
#include <iostream>


namespace vle { namespace extension {

DSDevs::DSDevs(const graph::AtomicModel& model,
               const devs::InitEventList& events) :
    devs::Executive(model, events),
    m_state(IDLE),
    m_response(false),
    m_coupledModel(0)
{
    if (not model.getParent()) {
        throw utils::ModellingError(
            "DSDevs ext.: not in a coupeld model");
    }

    m_coupledModel = model.getParent();
}

devs::Time DSDevs::init(const devs::Time& /* time */)
{
    m_state = IDLE;
    return devs::Time::infinity;
}

void DSDevs::output(const devs::Time& /* time */,
                    devs::ExternalEventList& output) const
{
    devs::ExternalEvent* ev = 0;

    if (m_state != DSDevs::IDLE) {
        assert(m_nameList.size() == m_response.size());

        std::list < std::string >::const_iterator it;
        std::list < bool >::const_iterator jt;

        for (it = m_nameList.begin(), jt = m_response.begin(); it !=
             m_nameList.end(); ++it, ++jt) {
            ev = new devs::ExternalEvent("ok");
            ev << devs::attribute("name", *it);
            ev << devs::attribute("ok", *jt);
            output.addEvent(ev);
        }

        if (not m_newName.empty()) {
            value::Set* eeset = value::Set::create();
            for (std::list < std::string >::const_iterator it =
                 m_newName.begin(); it != m_newName.end(); ++it) {
                eeset->add(value::String::create(*it));
            }
            ev = new devs::ExternalEvent("name");
            ev << devs::attribute("name", eeset);
            output.addEvent(ev);
        }
    }
}

devs::Time DSDevs::timeAdvance() const
{
    return (m_state != IDLE) ? devs::Time(0.0) : devs::Time::infinity;
}

devs::Event::EventType DSDevs::confluentTransitions(
    const devs::Time& /* time */,
    const devs::ExternalEventList& /* extEventlist */) const
{
    return devs::Event::EXTERNAL;
}

void DSDevs::externalTransition(const devs::ExternalEventList& event,
                                   const devs::Time& /* time */)
{
    devs::ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
        m_nameList.push_back((*it)->getSourceModelName());
        const std::string& portname((*it)->getPortName());

        m_response.push_back(
            processSwitch(portname,
                          (*it)->getAttributes().getMap(portname)));
        ++it;
    }
}

void DSDevs::internalTransition(const devs::Time& /* event */)
{
    m_nameList.clear();
    m_response.clear();
    m_newName.clear();

    if (m_state != IDLE) {
        m_state = IDLE;
    }
}

value::Value* DSDevs::observation(const devs::ObservationEvent& event) const
{
    std::ostringstream out;

    if (event.onPort("coupled")) {
        m_coupledModel->writeXML(out);
    } else if (event.onPort("hierarchy")) {
        m_coupledModel->writeXML(out);
    } else if (event.onPort("complete")) {
        m_coupledModel->writeXML(out);
    }

    return buildString(out.str());
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

value::Map* DSDevs::buildMessageAddConnection(const std::string& srcModelName,
                                             const std::string& srcPortName,
                                             const std::string& dstModelName,
                                             const std::string& dstPortName)
{
    value::Map* lst =value::Map::create();
    lst->addString("srcModelName", srcModelName);
    lst->addString("srcPortName", srcPortName);
    lst->addString("dstModelName", dstModelName);
    lst->addString("dstPortName", dstPortName);
    return lst;
}

value::Map* DSDevs::buildMessageChangeConnection(
    const std::string& srcModelName,
    const std::string& srcPortName,
    const std::string& oldDstModelName,
    const std::string& oldDstPortName,
    const std::string& newDstModelName,
    const std::string& newDstPortName)
{
    value::Map* lst =value::Map::create();
    lst->addString("srcModelName", srcModelName);
    lst->addString("srcPortName", srcPortName);
    lst->addString("oldDstModelName", oldDstModelName);
    lst->addString("oldDstPortName", oldDstPortName);
    lst->addString("newDstModelName", newDstModelName);
    lst->addString("newDstPortName", newDstPortName);
    return lst;
}

value::Map* DSDevs::buildMessageRemoveConnection(
    const std::string& srcModelName,
    const std::string& srcPortName,
    const std::string& dstModelName,
    const std::string& dstPortName)
{
    value::Map* lst =value::Map::create();
    lst->addString("srcModelName", srcModelName);
    lst->addString("srcPortName", srcPortName);
    lst->addString("dstModelName", dstModelName);
    lst->addString("dstPortName", dstPortName);
    return lst;
}

value::Map* DSDevs::buildMessageAddModel(const std::string& prefixModelName,
                                        const std::string& className,
                                        const std::string& xmlDynamics,
                                        const std::string& xmlInits,
                                        value::Set* connection)
{
    value::Map* lst =value::Map::create();
    lst->addString("prefixModelName", prefixModelName);
    lst->addString("className", className);
    lst->addString("xmlDynamics", xmlDynamics);
    lst->addString("xmlInits", xmlInits);
    lst->add("addStringConnection", connection);
    return lst;
}

value::Map* DSDevs::buildMessageRemoveModel(const std::string& modelName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    return lst;
}

value::Map* DSDevs::buildMessageChangeModel(const std::string& modelName,
                                           const std::string& className,
                                           const std::string& newClassName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    lst->addString("className", className);
    lst->addString("newClassName", newClassName);
    return lst;
}

value::Map* DSDevs::buildMessageBuildModel(const std::string& prefixModelName,
                                          const std::string& className,
                                          const std::string& xmlCode,
                                          const std::string& xmlDynamics,
                                          const std::string& xmlInits)
{
    value::Map* lst =value::Map::create();
    lst->addString("prefixModelName", prefixModelName);
    lst->addString("className", className);
    lst->addString("xmlCode", xmlCode);
    lst->addString("xmlDynamics", xmlDynamics);
    lst->addString("xmlInits", xmlInits);
    return lst;
}

value::Map* DSDevs::buildMessageAddInputPort(const std::string& modelName,
                                            const std::string& portName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    lst->addString("portName", portName);
    return lst;
}

value::Map* DSDevs::buildMessageAddOutputPort(const std::string& modelName,
                                             const std::string& portName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    lst->addString("portName", portName);
    return lst;
}

value::Map* DSDevs::buildMessageRemoveInputPort(const std::string& modelName,
                                               const std::string& portName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    lst->addString("portName", portName);
    return lst;
}

value::Map* DSDevs::buildMessageRemoveOutputPort(const std::string& modelName,
                                                const std::string& portName)
{
    value::Map* lst =value::Map::create();
    lst->addString("modelName", modelName);
    lst->addString("portName", portName);
    return lst;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

value::Set* DSDevs::addToBagAddConnection(const std::string& srcModelName,
                                         const std::string& srcPortName,
                                         const std::string& dstModelName,
                                         const std::string& dstPortName,
                                         value::Set* currentbag)
{
    value::Map* mp = buildMessageAddConnection(srcModelName, srcPortName,
                                              dstModelName, dstPortName);
    mp->addString("action", "addStringConnection");

    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagChangeConnection(const std::string& srcModelName,
                                            const std::string& srcPortName,
                                            const std::string& oldDstModelName,
                                            const std::string& oldDstPortName,
                                            const std::string& newDstModelName,
                                            const std::string& newDstPortName,
                                            value::Set* currentbag)
{
    value::Map* mp = buildMessageChangeConnection(
        srcModelName, srcPortName, oldDstModelName, oldDstPortName,
        newDstModelName, newDstPortName);
    mp->addString("action", "changeConnection");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}


value::Set* DSDevs::addToBagRemoveConnection(const std::string& srcModelName,
                                            const std::string& srcPortName,
                                            const std::string& dstModelName,
                                            const std::string& dstPortName,
                                            value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveConnection(srcModelName, srcPortName,
                                                 dstModelName, dstPortName);
    mp->addString("action", "removeConnection");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagAddModel(const std::string& prefixModelName,
                                    const std::string& className,
                                    const std::string& xmlDynamics,
                                    const std::string& xmlInits,
                                    value::Set* connection,
                                    value::Set* currentbag)
{
    value::Map* mp = buildMessageAddModel(prefixModelName, className,
                                          xmlDynamics, xmlInits, connection);
    mp->addString("action", "addStringModel");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}


value::Set* DSDevs::addToBagRemoveModel(const std::string& modelName,
                                       value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveModel(modelName);
    mp->addString("action", "removeModel");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}


value::Set* DSDevs::addToBagChangeModel(const std::string& modelName,
                                       const std::string& className,
                                       const std::string& newClassName,
                                       value::Set* currentbag)
{
    value::Map* mp = buildMessageChangeModel(modelName, className, newClassName);
    mp->addString("action", "changeModel");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagBuildModel(const std::string& prefixModelName,
                                      const std::string& className,
                                      const std::string& xmlCode,
                                      const std::string& xmlDynamics,
                                      const std::string& xmlInits,
                                      value::Set* currentbag)
{
    value::Map* mp = buildMessageBuildModel(prefixModelName, className, xmlCode,
                                            xmlDynamics, xmlInits);
    mp->addString("action", "buildModel");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagAddInputPort(const std::string& modelName,
                                        const std::string& portName,
                                        value::Set* currentbag)
{
    value::Map* mp = buildMessageAddInputPort(modelName, portName);
    mp->addString("action", "addStringInputPort");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagAddOutputPort(const std::string& modelName,
                                         const std::string& portName,
                                         value::Set* currentbag)
{
    value::Map* mp = buildMessageAddOutputPort(modelName, portName);
    mp->addString("action", "addStringOutputPort");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagRemoveInputPort(const std::string& modelName,
                                           const std::string& portName,
                                           value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveInputPort(modelName, portName);
    mp->addString("action", "removeInputPort");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

value::Set* DSDevs::addToBagRemoveOutputPort(const std::string& modelName,
                                            const std::string& portName,
                                            value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveOutputPort(modelName, portName);
    mp->addString("action", "removeOutputPort");
    if (currentbag == 0) {
        currentbag = value::Set::create();
    }
    currentbag->add(mp);
    return currentbag;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DSDevs::processSwitch(const std::string& action, const value::Map& val)
{
    if (action == "addModel") {
        return processAddModel(val);
    } else if (action == "removeModel") {
        return processRemoveModel(val);
    } else if (action == "changeModel") {
        return processChangeModel(val);
    } else if (action == "buildmodel") {
        return processBuildModel(val);
    } else if (action == "addInputPort") {
        return processAddInputPort(val);
    } else if (action == "delInputPort") {
        return processRemoveInputPort(val);
    } else if (action == "addOutputPort") {
        return processAddOutputPort(val);
    } else if (action == "delOutputPort") {
        return processRemoveOutputPort(val);
    } else if (action == "addConnection") {
        return processAddConnection(val);
    } else if (action == "delConnection") {
        return processRemoveConnection(val);
    } else if (action == "changeConnection") {
        return processChangeConnection(val);
    } else if (action == "bag") {
        return processBag(val);
    } else {
        throw utils::InternalError(boost::format(
                "DSDevs ext.: unknow action '%1%'") % action);
    }
}

bool DSDevs::processAddModel(const value::Map& val)
{
    const std::string& pre(val.getString("prefixModelName"));
    const std::string& cls(val.getString("className"));
    std::string xmld, xmli;
    const value::Set* cnt = 0;

    if (val.existValue("xmlDynamics"))
        xmld.assign(val.getString("xmlDynamics"));

    if (val.existValue("xmlInits"))
        xmli.assign(val.getString("xmlInits"));

    if (val.existValue("addConnection"))
        cnt = &val.getSet("addConnection");

    m_state = ADD_MODEL;
    return addModel(pre, cls, cnt);
}

bool DSDevs::processRemoveModel(const value::Map& val)
{
    const std::string& nam(val.getString("modelName"));
    m_state = REMOVE_MODEL;
    return removeModel(nam);
}

bool DSDevs::processChangeModel(const value::Map& val)
{
    const std::string& nam(val.getString("modelName"));
    const std::string& oldc(val.getString("className"));
    const std::string& newc(val.getString("newClassName"));
    m_state = CHANGE_MODEL;
    return changeModel(nam, oldc, newc);
}

bool DSDevs::processBuildModel(const value::Map& val)
{
    const std::string& pre(val.getString("prefixModelName"));
    const std::string& cls(val.getString("className"));
    const std::string& cod(val.getString("xmlCode"));
    const std::string& xmld(val.getString("xmlDynamics"));
    const std::string& xmli(val.getString("xmlInits"));
    m_state = BUILD_MODEL;
    return buildModel(pre, cls, cod, xmld, xmli);
}

bool DSDevs::processAddInputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = ADD_INPUTPORT;
    return addInputPort(mdl, prt);
}

bool DSDevs::processRemoveInputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = REMOVE_INPUTPORT;
    return removeInputPort(mdl, prt);
}

bool DSDevs::processAddOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = ADD_OUTPUTPORT;
    return addOutputPort(mdl, prt);
}

bool DSDevs::processRemoveOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = REMOVE_OUTPUTPORT;
    return removeOutputPort(mdl, prt);
}

bool DSDevs::processAddConnection(const value::Map& val)
{
    const std::string& srcmn(val.getString("srcModelName"));
    const std::string& srcpn(val.getString("srcPortName"));
    const std::string& dstmn(val.getString("dstModelName"));
    const std::string& dstpn(val.getString("dstPortName"));
    m_state = ADD_CONNECTION;
    return addConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processRemoveConnection(const value::Map& val)
{
    const std::string& srcmn(val.getString("srcModelName"));
    const std::string& srcpn(val.getString("srcPortName"));
    const std::string& dstmn(val.getString("dstModelName"));
    const std::string& dstpn(val.getString("dstPortName"));
    m_state = REMOVE_CONNECTION;
    return removeConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processChangeConnection(const value::Map& val)
{
    const std::string& srcmn(val.getString("srcModelName"));
    const std::string& srcpn(val.getString("srcPortName"));
    const std::string& oldm(val.getString("oldDstModelName"));
    const std::string& oldp(val.getString("oldDstPortName"));
    const std::string& newm(val.getString("newDstModelName"));
    const std::string& newp(val.getString("newDstPortName"));
    m_state = CHANGE_CONNECTION;
    return changeConnection(srcmn, srcpn, oldm, oldp, newm, newp);
}

bool DSDevs::processBag(const value::Map& val)
{
    const value::Value* valuebag(&val.get("bag"));
    const value::Set* bag = value::toSetValue(valuebag);
    bool result = true;

    const value::VectorValue& vv(bag->value());
    for (value::VectorValue::const_iterator it = vv.begin();
         it != vv.end(); ++it) {
        value::Map* msg = value::toMapValue(*it);
        value::Value& msgaction = msg->get("action");
        value::String& straction = value::toStringValue(msgaction);

        const std::string& action = straction.value();
        result = processSwitch(action, *msg);

        if (not result)
            break;
    }
    m_state = BAG;
    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DSDevs::addConnection(const std::string& srcModelName,
                           const std::string& srcPortName,
                           const std::string& dstModelName,
                           const std::string& dstPortName)
{
    const std::string& modelName = m_coupledModel->getName();
    graph::Model* srcModel = (modelName == srcModelName)?
        m_coupledModel : m_coupledModel->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        m_coupledModel:m_coupledModel->findModel(dstModelName);

    if (srcModel and dstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->addInputConnection(srcPortName,
                                               dstModel, dstPortName);
        } else {
            if (modelName == dstModelName) {
                m_coupledModel->addOutputConnection(srcModel, srcPortName,
                                                    dstPortName);
            } else {
                m_coupledModel->addInternalConnection(srcModel, srcPortName,
                                                      dstModel, dstPortName);
            }
        }
        return true;
    }
    return false;
}

bool DSDevs::changeConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& oldDstModelName,
                              const std::string& oldDstPortName,
                              const std::string& newDstModelName,
                              const std::string& newDstPortName)
{
    const std::string& modelName = m_coupledModel->getName();
    graph::Model* srcModel = (modelName == srcModelName)?
        m_coupledModel : m_coupledModel->findModel(srcModelName);
    graph::Model* oldDstModel = (modelName == oldDstModelName)?
        m_coupledModel : m_coupledModel->findModel(oldDstModelName);
    graph::Model* newDstModel = (modelName == newDstModelName)?
        m_coupledModel : m_coupledModel->findModel(newDstModelName);

    if (newDstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->delInputConnection(srcPortName,
                                               oldDstModel, oldDstPortName);
            m_coupledModel->addInputConnection(srcPortName,
                                               newDstModel, newDstPortName);
        } else {
            if (modelName == oldDstModelName) {
                m_coupledModel->delOutputConnection(
                    srcModel, srcPortName, oldDstPortName);
                m_coupledModel->addOutputConnection(
                    srcModel, srcPortName, newDstPortName);
            } else {
                m_coupledModel->delInternalConnection(
                    srcModel, srcPortName, oldDstModel, oldDstPortName);
                m_coupledModel->addInternalConnection(
                    srcModel, srcPortName, newDstModel, newDstPortName);
            }
        }
        return true;
    }
    else
        return false;
}

bool DSDevs::removeConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& dstModelName,
                              const std::string& dstPortName)
{
    const std::string& modelName = m_coupledModel->getName();
    graph::Model* srcModel = (modelName == srcModelName)?
        m_coupledModel : m_coupledModel->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        m_coupledModel : m_coupledModel->findModel(dstModelName);

    if (srcModel and dstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->delInputConnection(srcPortName, dstModel,
                                               dstPortName);
        } else {
            if (modelName == dstModelName) {
                m_coupledModel->delOutputConnection(srcModel, srcPortName,
                                                    dstPortName);
            } else {
                m_coupledModel->delInternalConnection(srcModel, srcPortName,
                                                      dstModel, dstPortName);
            }
        }
        return true;
    }
    return false;
}

bool DSDevs::addModel(const std::string& /* prefixModelName */,
                      const std::string& /* className */,
                      const value::Set* /* connection */)
{
    //std::string newname(m_coupledModel->buildNewName(prefixModelName));
    //m_newName.push_back(newname);

    //try {
        //vpz::Dynamic dyn("FIXME");
        //vpz::Condition cond("FIXME");
        //vpz::Observable obs("FIXME");
        //coordinator().createModelFromClass(m_coupledModel, className);
    //} catch (const std::exception& e) {
        //std::cerr << boost::format(
            //"Warning: Unable to dynamic add model, with prefixname '%1%' "
            //"class name '%2%'. Error reported is: '%3%\n'") %
            //prefixModelName % className % e.what();
        //return false;
    //}

    //if (connection) {
        //for (value::Set::VectorValueIt it =
             //connection->getValue().begin(); it !=
             //connection->getValue().end(); ++it) {

            //if ((*it)->isMap()) {
                //value::Map* mp = value::to_map(*it);
                //std::string srcm, srcp, dstm, dstp;

                //if (mp->existValue("srcModelName")) {
                    //srcm.assign(mp->getStringValue("srcModelName"));
                    //if (srcm.empty())
                        //srcm.assign(newname);
                //} else
                    //srcm.assign(newname);

                //if (mp->existValue("dstModelName")) {
                    //dstm.assign(mp->getStringValue("dstModelName"));
                    //if (dstm.empty())
                        //dstm.assign(newname);
                //} else
                    //dstm.assign(newname);

                //srcp = mp->getStringValue("srcPortName");
                //dstp = mp->getStringValue("dstPortName");
                //addConnection(srcm, srcp, dstm, dstp);
                 //}
             //}
         //}
         //return true;

    throw utils::NotYetImplemented(
        "DSDevs ext.: addModel from class not allowed");
}

bool DSDevs::removeModel(const std::string& /* modelName */)
{
    // FIX ME
    //    getModel().getSimulator()->delModel(m_coupledModel, modelName);
    return true;
}

bool DSDevs::changeModel(const std::string& modelName,
                         const std::string& className,
                         const std::string& newClassName)
{
    std::cerr << "ChangeModel Not yet implemented " << modelName
        << " " << className << " " << newClassName << "\n";

    return false;
}

bool DSDevs::buildModel(const std::string& prefixModelName,
                        const std::string& className,
                        const std::string& xmlCode,
                        const std::string& xmlDynamics,
                        const std::string& xmlInits)
{
    // FIXME
    // 1 - Decoupe xmlCode en code source.
    // 2 - selection  le compilateur.
    // 3 - Compile le source.
    // 4 - Installe le plugin.
    // addModel(prefixModelName, className, xmlDynamics, xmlInits);
    std::cerr << "BuildModel Not yet implemented " << prefixModelName
        << " " << className << " " << " " << xmlCode << " "
        << xmlDynamics << " " << xmlInits << "\n";

    return false;
}

bool DSDevs::addInputPort(const std::string& modelName,
                          const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->findModel(modelName);
    mdl->addInputPort(portName);
    return true;
}

bool DSDevs::addOutputPort(const std::string& modelName,
                           const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->findModel(modelName);
    mdl->addOutputPort(portName);
    return true;
}

bool DSDevs::removeInputPort(const std::string& modelName,
                             const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->findModel(modelName);
    mdl->delInputPort(portName);
    return true;
}

bool DSDevs::removeOutputPort(const std::string& modelName,
                              const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->findModel(modelName);
    mdl->delOutputPort(portName);
    return true;
}

graph::ModelList& DSDevs::getModelList() const
{
    return  m_coupledModel->getModelList();
}

}} // namespace vle extension
