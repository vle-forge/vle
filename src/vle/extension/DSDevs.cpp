/**
 * @file DSDevs.cpp
 * @author The VLE Development Team.
 * @brief A base to executive class allowing modeller to changer graph during
 * simulation. Modeller can, add, changer, del connection or model.
 */

/*
 * Copyright (c) 2004, 2005 - The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  value::Set*e the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/extension/DSDevs.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/Port.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace extension {

DSDevs::DSDevs(devs::sAtomicModel* model) :
    devs::Dynamics(model),
    m_state(IDLE),
    m_response(false),
    m_coupledModel(0)
{
    Assert(utils::InternalError, model->getStructure()->getParent(),
           "The DSDEVS is not in an coupled model.");

    m_coupledModel = model->getStructure()->getParent();
}

bool DSDevs::parseXML(xmlpp::Element*)
{
    return true;
}

devs::Time DSDevs::init()
{
    m_state = IDLE;
    return devs::Time::infinity;
}

devs::ExternalEventList* DSDevs::getOutputFunction(const devs::Time& currentTime)
{
    devs::ExternalEventList* lst = new devs::ExternalEventList;
    devs::ExternalEvent* ev;

    if (m_state != DSDevs::IDLE) {
        AssertI(m_nameList.size() == m_response.size());

        while (not m_nameList.empty()) {
            ev = new devs::ExternalEvent("ok", currentTime, getModel());
            ev << devs::attribute("name", m_nameList.front());
            ev << devs::attribute("ok", m_response.front());
            lst->addEvent(ev);

            m_nameList.pop_front();
            m_response.pop_front();
        }

        if (not m_newName.empty()) {
            value::Set* eeset = new value::Set;
            for (std::list < std::string >::const_iterator it =
                 m_newName.begin(); it != m_newName.end(); ++it) {
                eeset->addValue(new value::String(*it));
            }
            ev = new devs::ExternalEvent("name", currentTime, getModel());
            ev << devs::attribute("name", eeset);
            lst->addEvent(ev);
            m_newName.clear();
        }
    }
    return lst;
}

devs::Time DSDevs::getTimeAdvance()
{
    return (m_state != IDLE) ? 0 : devs::Time::infinity;
}

void DSDevs::processExternalEvent(devs::ExternalEvent* event)
{
    m_nameList.push_back(event->getSourceModelName());
    const std::string& portname(event->getPortName());

    m_response.push_back(
        processSwitch(portname, *event->getAttributes().getMapValue(portname)));
}

void DSDevs::processInitEvent(devs::InitEvent*)
{
}

void DSDevs::processInternalEvent(devs::InternalEvent*)
{
    if (m_state != IDLE)
        m_state = IDLE;
}

value::Value* DSDevs::processStateEvent(devs::StateEvent* evt) const
{
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("STATE");
    
    if (evt->getPortName() == "coupled") {
        m_coupledModel->writeXML(root);
    } else if (evt->getPortName() == "hierarchy") {
        m_coupledModel->writeXML(root);
    } else if (evt->getPortName() == "complete") {
        m_coupledModel->writeXML(root);
    }

    return buildString(doc.write_to_string_formatted());
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

value::Map* DSDevs::buildMessageAddConnection(const std::string& srcModelName,
                                const std::string& srcPortName,
                                const std::string& dstModelName,
                                const std::string& dstPortName)
{
    value::Map* lst = new value::Map;
    lst->addValue("srcModelName", new value::String(srcModelName));
    lst->addValue("srcPortName", new value::String(srcPortName));
    lst->addValue("dstModelName", new value::String(dstModelName));
    lst->addValue("dstPortName", new value::String(dstPortName));
    return lst;
}

value::Map* DSDevs::buildMessageChangeConnection(const std::string& srcModelName,
                                    const std::string& srcPortName,
                                    const std::string& oldDstModelName,
                                    const std::string& oldDstPortName,
                                    const std::string& newDstModelName,
                                    const std::string& newDstPortName)
{
    value::Map* lst = new value::Map;
    lst->addValue("srcModelName", new value::String(srcModelName));
    lst->addValue("srcPortName", new value::String(srcPortName));
    lst->addValue("oldDstModelName", new value::String(oldDstModelName));
    lst->addValue("oldDstPortName", new value::String(oldDstPortName));
    lst->addValue("newDstModelName", new value::String(newDstModelName));
    lst->addValue("newDstPortName", new value::String(newDstPortName));
    return lst;
}

value::Map* DSDevs::buildMessageRemoveConnection(const std::string& srcModelName,
                                   const std::string& srcPortName,
                                   const std::string& dstModelName,
                                   const std::string& dstPortName)
{
    value::Map* lst = new value::Map;
    lst->addValue("srcModelName", new value::String(srcModelName));
    lst->addValue("srcPortName", new value::String(srcPortName));
    lst->addValue("dstModelName", new value::String(dstModelName));
    lst->addValue("dstPortName", new value::String(dstPortName));
    return lst;
}

value::Map* DSDevs::buildMessageAddModel(const std::string& prefixModelName,
                           const std::string& className,
                           const std::string& xmlDynamics,
                           const std::string& xmlInits,
                           value::Set* connection)
{
    value::Map* lst = new value::Map;
    lst->addValue("prefixModelName", new value::String(prefixModelName));
    lst->addValue("className", new value::String(className));
    lst->addValue("xmlDynamics", new value::String(xmlDynamics));
    lst->addValue("xmlInits", new value::String(xmlInits));
    lst->addValue("addConnection", connection);
    return lst;
}

value::Map* DSDevs::buildMessageRemoveModel(const std::string& modelName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName", new value::String(modelName));
    return lst;
}

value::Map* DSDevs::buildMessageChangeModel(const std::string& modelName,
                              const std::string& className,
                              const std::string& newClassName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName", new value::String(modelName));
    lst->addValue("className", new value::String(className));
    lst->addValue("newClassName", new value::String(newClassName));
    return lst;
}

value::Map* DSDevs::buildMessageBuildModel(const std::string& prefixModelName,
                             const std::string& className,
                             const std::string& xmlCode,
                             const std::string& xmlDynamics,
                             const std::string& xmlInits)
{
    value::Map* lst = new value::Map;
    lst->addValue("prefixModelName", new value::String(prefixModelName));
    lst->addValue("className", new value::String(className));
    lst->addValue("xmlCode", new value::String(xmlCode));
    lst->addValue("xmlDynamics", new value::String(xmlDynamics));
    lst->addValue("xmlInits", new value::String(xmlInits));
    return lst;
}

value::Map* DSDevs::buildMessageAddInputPort(const std::string& modelName,
                               const std::string& portName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName",  new value::String(modelName));
    lst->addValue("portName",  new value::String(portName));
    return lst;
}

value::Map* DSDevs::buildMessageAddOutputPort(const std::string& modelName,
                           const std::string& portName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName",  new value::String(modelName));
    lst->addValue("portName",  new value::String(portName));
    return lst;
}

value::Map* DSDevs::buildMessageRemoveInputPort(const std::string& modelName,
                             const std::string& portName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName",  new value::String(modelName));
    lst->addValue("portName",  new value::String(portName));
    return lst;
}

value::Map* DSDevs::buildMessageRemoveOutputPort(const std::string& modelName,
                                   const std::string& portName)
{
    value::Map* lst = new value::Map;
    lst->addValue("modelName",  new value::String(modelName));
    lst->addValue("portName",  new value::String(portName));
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
    mp->addValue("action", new value::String("addConnection"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
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
    mp->addValue("action", new value::String("changeConnection"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}


value::Set* DSDevs::addToBagRemoveConnection(const std::string& srcModelName,
                                 const std::string& srcPortName,
                                 const std::string& dstModelName,
                                 const std::string& dstPortName,
                                 value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveConnection(srcModelName, srcPortName,
                                           dstModelName, dstPortName);
    mp->addValue("action", new value::String("removeConnection"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}

value::Set* DSDevs::addToBagAddModel(const std::string& prefixModelName,
                         const std::string& className,
                         const std::string& xmlDynamics,
                         const std::string& xmlInits,
                         value::Set* connection,
                         value::Set* currentbag)
{
    value::Map* mp = buildMessageAddModel(prefixModelName, className, xmlDynamics,
                                   xmlInits, connection);
    mp->addValue("action", new value::String("addModel"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}


value::Set* DSDevs::addToBagRemoveModel(const std::string& modelName,
                            value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveModel(modelName);
    mp->addValue("action", new value::String("removeModel"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}


value::Set* DSDevs::addToBagChangeModel(const std::string& modelName,
                            const std::string& className,
                            const std::string& newClassName,
                            value::Set* currentbag)
{
    value::Map* mp = buildMessageChangeModel(modelName, className, newClassName);
    mp->addValue("action", new value::String("changeModel"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
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
    mp->addValue("action", new value::String("buildModel"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}

value::Set* DSDevs::addToBagAddInputPort(const std::string& modelName,
                             const std::string& portName,
                             value::Set* currentbag)
{
    value::Map* mp = buildMessageAddInputPort(modelName, portName);
    mp->addValue("action", new value::String("addInputPort"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}

value::Set* DSDevs::addToBagAddOutputPort(const std::string& modelName,
                              const std::string& portName,
                              value::Set* currentbag)
{
    value::Map* mp = buildMessageAddOutputPort(modelName, portName);
    mp->addValue("action", new value::String("addOutputPort"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}

value::Set* DSDevs::addToBagRemoveInputPort(const std::string& modelName,
                                const std::string& portName,
                                value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveInputPort(modelName, portName);
    mp->addValue("action", new value::String("removeInputPort"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
}

value::Set* DSDevs::addToBagRemoveOutputPort(const std::string& modelName,
                                 const std::string& portName,
                                 value::Set* currentbag)
{
    value::Map* mp = buildMessageRemoveOutputPort(modelName, portName);
    mp->addValue("action", new value::String("removeOutputPort"));
    value::Set* r = (currentbag == 0) ? new value::Set : currentbag;
    r->addValue(mp);
    return r;
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
        Throw(utils::InternalError, boost::format("Unknow action '%1%'\n") %
              action);
    }
}

bool DSDevs::processAddModel(const value::Map& val)
{
    const std::string& pre(val.getStringValue("prefixModelName"));
    const std::string& cls(val.getStringValue("className"));
    std::string xmld, xmli;
    value::Set* cnt = 0;

    if (val.existValue("xmlDynamics"))
        xmld.assign(val.getStringValue("xmlDynamics"));

    if (val.existValue("xmlInits"))
        xmli.assign(val.getStringValue("xmlInits"));

    if (val.existValue("addConnection"))
        cnt = val.getSetValue("addConnection");

    m_state = ADD_MODEL;
    return addModel(pre, cls, xmld, xmli, cnt);
}

bool DSDevs::processRemoveModel(const value::Map& val)
{
    const std::string& nam(val.getStringValue("modelName"));
    m_state = REMOVE_MODEL;
    return removeModel(nam);
}

bool DSDevs::processChangeModel(const value::Map& val)
{
    const std::string& nam(val.getStringValue("modelName"));
    const std::string& oldc(val.getStringValue("className"));
    const std::string& newc(val.getStringValue("newClassName"));
    m_state = CHANGE_MODEL;
    return changeModel(nam, oldc, newc);
}

bool DSDevs::processBuildModel(const value::Map& val)
{
    const std::string& pre(val.getStringValue("prefixModelName"));
    const std::string& cls(val.getStringValue("className"));
    const std::string& cod(val.getStringValue("xmlCode"));
    const std::string& xmld(val.getStringValue("xmlDynamics"));
    const std::string& xmli(val.getStringValue("xmlInits"));
    m_state = BUILD_MODEL;
    return buildModel(pre, cls, cod, xmld, xmli);
}

bool DSDevs::processAddInputPort(const value::Map& val)
{
    const std::string& mdl(val.getStringValue("modelName"));
    const std::string& prt(val.getStringValue("portName"));
    m_state = ADD_INPUTPORT;
    return addInputPort(mdl, prt);
}

bool DSDevs::processRemoveInputPort(const value::Map& val)
{
    const std::string& mdl(val.getStringValue("modelName"));
    const std::string& prt(val.getStringValue("portName"));
    m_state = REMOVE_INPUTPORT;
    return removeInputPort(mdl, prt);
}

bool DSDevs::processAddOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getStringValue("modelName"));
    const std::string& prt(val.getStringValue("portName"));
    m_state = ADD_OUTPUTPORT;
    return addOutputPort(mdl, prt);
}

bool DSDevs::processRemoveOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getStringValue("modelName"));
    const std::string& prt(val.getStringValue("portName"));
    m_state = REMOVE_OUTPUTPORT;
    return removeOutputPort(mdl, prt);
}

bool DSDevs::processAddConnection(const value::Map& val)
{
    const std::string& srcmn(val.getStringValue("srcModelName"));
    const std::string& srcpn(val.getStringValue("srcPortName"));
    const std::string& dstmn(val.getStringValue("dstModelName"));
    const std::string& dstpn(val.getStringValue("dstPortName"));
    m_state = ADD_CONNECTION;
    return addConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processRemoveConnection(const value::Map& val)
{
    const std::string& srcmn(val.getStringValue("srcModelName"));
    const std::string& srcpn(val.getStringValue("srcPortName"));
    const std::string& dstmn(val.getStringValue("dstModelName"));
    const std::string& dstpn(val.getStringValue("dstPortName"));
    m_state = REMOVE_CONNECTION;
    return removeConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processChangeConnection(const value::Map& val)
{
    const std::string& srcmn(val.getStringValue("srcModelName"));
    const std::string& srcpn(val.getStringValue("srcPortName"));
    const std::string& oldm(val.getStringValue("oldDstModelName"));
    const std::string& oldp(val.getStringValue("oldDstPortName"));
    const std::string& newm(val.getStringValue("newDstModelName"));
    const std::string& newp(val.getStringValue("newDstPortName"));
    m_state = CHANGE_CONNECTION;
    return changeConnection(srcmn, srcpn, oldm, oldp, newm, newp);
}

bool DSDevs::processBag(const value::Map& val)
{
    value::Value* valuebag(val.getValue("bag"));

    Assert(utils::InternalError, valuebag->getType() == value::Value::SET,
           "No bag found into message.");

    value::Set* bag = (value::Set*)valuebag;
    bool result = true;

    value::Set::VectorValue& vv(bag->getValue());
    for (value::Set::VectorValue::iterator it = vv.begin(); it != vv.end(); ++it) {
        value::Map* msg(static_cast < value::Map* >((*it)));

        Assert(utils::InternalError, bag, "Not a map into bag message.");

        value::Value* msgaction = msg->getValue("action");
        value::String* straction = dynamic_cast < value::String* >(msgaction);

        Assert(utils::InternalError, straction, "No correct action message.");

        const std::string& action = straction->stringValue();
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
        m_coupledModel : m_coupledModel->getModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        m_coupledModel:m_coupledModel->getModel(dstModelName);

    if (srcModel and dstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->addInputConnection(srcModel, srcPortName,
                                               dstModel, dstPortName);
        } else {
            if (modelName == dstModelName) {
                m_coupledModel->addOutputConnection(srcModel, srcPortName,
                                                    dstModel, dstPortName);
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
        m_coupledModel : m_coupledModel->getModel(srcModelName);
    graph::Model* oldDstModel = (modelName == oldDstModelName)?
        m_coupledModel : m_coupledModel->getModel(oldDstModelName);
    graph::Model* newDstModel = (modelName == newDstModelName)?
        m_coupledModel : m_coupledModel->getModel(newDstModelName);

    if (newDstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->delInputConnection(srcModel, srcPortName,
                                               oldDstModel, oldDstPortName);
            m_coupledModel->addInputConnection(srcModel, srcPortName,
                                               newDstModel, newDstPortName);
        } else {
            if (modelName == oldDstModelName) {
                m_coupledModel->delOutputConnection(
                        srcModel, srcPortName, oldDstModel, oldDstPortName);
                m_coupledModel->addOutputConnection(
                        srcModel, srcPortName, newDstModel, newDstPortName);
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
        m_coupledModel : m_coupledModel->getModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        m_coupledModel : m_coupledModel->getModel(dstModelName);

    if (srcModel and dstModel) {
        if (modelName == srcModelName) {
            m_coupledModel->delInputConnection(srcModel, srcPortName, dstModel,
                                               dstPortName);
        } else {
            if (modelName == dstModelName) {
                m_coupledModel->delOutputConnection(srcModel, srcPortName,
                                                    dstModel, dstPortName);
            } else {
                m_coupledModel->delInternalConnection(srcModel, srcPortName,
                                                      dstModel, dstPortName);
            }
        }
        return true;
    }
    return false;
}

bool DSDevs::addModel(const std::string& prefixModelName,
                      const std::string& className,
                      const std::string& xmlDynamics,
                      const std::string& xmlInits,
                      value::Set* connection)
{
    std::string newname(m_coupledModel->buildNewName(prefixModelName));
    m_newName.push_back(newname);

    try {
        graph::Model* mdl = getModel()->getSimulator()->createModels(
            m_coupledModel, className, xmlDynamics, xmlInits);
        mdl->setName(newname);
    } catch (const std::exception& e) {
        std::cerr << boost::format(
            "Warning: Unable to dynamic add model, with prefixname '%1%' "
            "class name '%2%'. Error reported is: '%3%\n'") %
            prefixModelName % className % e.what();
        return false;
    }

    if (connection) {
        for (value::Set::VectorValue::iterator it = connection->getValue().begin();
             it != connection->getValue().end(); ++it) {
	    
            if ((*it)->getType() == value::Value::MAP) {
                value::Map* mp = (value::Map*)(*it);
                std::string srcm, srcp, dstm, dstp;
		
                if (mp->existValue("srcModelName")) {
                    srcm.assign(mp->getStringValue("srcModelName"));
                    if (srcm.empty())
                        srcm.assign(newname);
                } else
                    srcm.assign(newname);
                    
                if (mp->existValue("dstModelName")) {
                    dstm.assign(mp->getStringValue("dstModelName"));
                    if (dstm.empty())
                        dstm.assign(newname);
                } else
                    dstm.assign(newname);
            
                srcp = mp->getStringValue("srcPortName");
                dstp = mp->getStringValue("dstPortName");
                addConnection(srcm, srcp, dstm, dstp);
            }
        }
    }
    return true;
}

bool DSDevs::removeModel(const std::string& modelName)
{
    getModel()->getSimulator()->delModel(m_coupledModel, modelName);
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
    graph::Model* mdl = m_coupledModel->find(modelName);

    if (mdl) {
        if (mdl->getInPort(portName) == 0) {
            mdl->addInputPort(portName);
            return true;
        }
    }
    return false;
}

bool DSDevs::addOutputPort(const std::string& modelName,
                           const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    if (mdl) {
        if (mdl->getOutPort(portName) == 0) {
            mdl->addOutputPort(portName);
            return true;
        }
    }
    return false;
}

bool DSDevs::removeInputPort(const std::string& modelName,
                             const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    if (mdl) {
        if (mdl->getInPort(portName)) {
            mdl->delInputPort(portName);
            return true;
        }
    }
    return false;
}

bool DSDevs::removeOutputPort(const std::string& modelName,
                              const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    if (mdl and mdl->isAtomic()) {
        if (mdl->getOutPort(portName)) {
            mdl->delOutputPort(portName);
            return true;
        }
    }
    return false;
}

std::vector < graph::Model* >& DSDevs::getModelList() const
{
    return  m_coupledModel->getModelList();
}

}} // namespace vle extension
