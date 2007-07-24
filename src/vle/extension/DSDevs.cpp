/**
 * @file extension/DSDevs.cpp
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace extension {



DSDevs::DSDevs(const graph::AtomicModel& model) :
    devs::Executive(model),
    m_state(IDLE),
    m_response(false),
    m_coupledModel(0)
{
    Assert(utils::InternalError, model.getParent(),
           "The DSDEVS is not in an coupled model.");

    m_coupledModel = model.getParent();
}

devs::Time DSDevs::init()
{
    m_state = IDLE;
    return devs::Time::infinity;
}

void DSDevs::getOutputFunction(const devs::Time& /* time */,
                               devs::ExternalEventList& output) 
{
    devs::ExternalEvent* ev = 0;

    if (m_state != DSDevs::IDLE) {
        AssertI(m_nameList.size() == m_response.size());

        while (not m_nameList.empty()) {
            ev = new devs::ExternalEvent("ok");
            ev << devs::attribute("name", m_nameList.front());
            ev << devs::attribute("ok", m_response.front());
            output.addEvent(ev);

            m_nameList.pop_front();
            m_response.pop_front();
        }

        if (not m_newName.empty()) {
            value::Set eeset = value::SetFactory::create();
            for (std::list < std::string >::const_iterator it =
                 m_newName.begin(); it != m_newName.end(); ++it) {
                eeset->addValue(value::StringFactory::create(*it));
            }
            ev = new devs::ExternalEvent("name");
            ev << devs::attribute("name", eeset);
            output.addEvent(ev);
            m_newName.clear();
        }
    }
}

devs::Time DSDevs::getTimeAdvance()
{
    return (m_state != IDLE) ? 0 : devs::Time::infinity;
}

devs::Event::EventType DSDevs::processConflict(
    const devs::InternalEvent& /* internal */,
    const devs::ExternalEventList& /* extEventlist */) const
{
    return devs::Event::EXTERNAL;
}

void DSDevs::processExternalEvents(const devs::ExternalEventList& event,
                                   const devs::Time& /* time */)
{
    devs::ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
        m_nameList.push_back((*it)->getSourceModelName());
        const std::string& portname((*it)->getPortName());

        m_response.push_back(
            processSwitch(portname,
                          (*it)->getAttributes()->getMapValue(portname)));
        ++it;
    }
}

void DSDevs::processInternalEvent(const devs::InternalEvent& /* event */)
{
    if (m_state != IDLE) {
        m_state = IDLE;
    }
}

value::Value DSDevs::processStateEvent(const devs::StateEvent& event) const
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

value::Map DSDevs::buildMessageAddConnection(const std::string& srcModelName,
                                             const std::string& srcPortName,
                                             const std::string& dstModelName,
                                             const std::string& dstPortName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("srcModelName", value::StringFactory::create(srcModelName));
    lst->addValue("srcPortName", value::StringFactory::create(srcPortName));
    lst->addValue("dstModelName", value::StringFactory::create(dstModelName));
    lst->addValue("dstPortName", value::StringFactory::create(dstPortName));
    return lst;
}

value::Map DSDevs::buildMessageChangeConnection(
    const std::string& srcModelName,
    const std::string& srcPortName,
    const std::string& oldDstModelName,
    const std::string& oldDstPortName,
    const std::string& newDstModelName,
    const std::string& newDstPortName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("srcModelName", value::StringFactory::create(srcModelName));
    lst->addValue("srcPortName", value::StringFactory::create(srcPortName));
    lst->addValue("oldDstModelName",
                  value::StringFactory::create(oldDstModelName));
    lst->addValue("oldDstPortName",
                  value::StringFactory::create(oldDstPortName));
    lst->addValue("newDstModelName",
                  value::StringFactory::create(newDstModelName));
    lst->addValue("newDstPortName",
                  value::StringFactory::create(newDstPortName));
    return lst;
}

value::Map DSDevs::buildMessageRemoveConnection(
    const std::string& srcModelName,
    const std::string& srcPortName,
    const std::string& dstModelName,
    const std::string& dstPortName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("srcModelName",
                  value::StringFactory::create(srcModelName));
    lst->addValue("srcPortName", value::StringFactory::create(srcPortName));
    lst->addValue("dstModelName",
                  value::StringFactory::create(dstModelName));
    lst->addValue("dstPortName", value::StringFactory::create(dstPortName));
    return lst;
}

value::Map DSDevs::buildMessageAddModel(const std::string& prefixModelName,
                                        const std::string& className,
                                        const std::string& xmlDynamics,
                                        const std::string& xmlInits,
                                        value::Set connection)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("prefixModelName", value::StringFactory::create(prefixModelName));
    lst->addValue("className", value::StringFactory::create(className));
    lst->addValue("xmlDynamics", value::StringFactory::create(xmlDynamics));
    lst->addValue("xmlInits", value::StringFactory::create(xmlInits));
    lst->addValue("addConnection", connection);
    return lst;
}

value::Map DSDevs::buildMessageRemoveModel(const std::string& modelName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName", value::StringFactory::create(modelName));
    return lst;
}

value::Map DSDevs::buildMessageChangeModel(const std::string& modelName,
                                           const std::string& className,
                                           const std::string& newClassName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName", value::StringFactory::create(modelName));
    lst->addValue("className", value::StringFactory::create(className));
    lst->addValue("newClassName", value::StringFactory::create(newClassName));
    return lst;
}

value::Map DSDevs::buildMessageBuildModel(const std::string& prefixModelName,
                                          const std::string& className,
                                          const std::string& xmlCode,
                                          const std::string& xmlDynamics,
                                          const std::string& xmlInits)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("prefixModelName", value::StringFactory::create(prefixModelName));
    lst->addValue("className", value::StringFactory::create(className));
    lst->addValue("xmlCode", value::StringFactory::create(xmlCode));
    lst->addValue("xmlDynamics", value::StringFactory::create(xmlDynamics));
    lst->addValue("xmlInits", value::StringFactory::create(xmlInits));
    return lst;
}

value::Map DSDevs::buildMessageAddInputPort(const std::string& modelName,
                                            const std::string& portName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName",  value::StringFactory::create(modelName));
    lst->addValue("portName",  value::StringFactory::create(portName));
    return lst;
}

value::Map DSDevs::buildMessageAddOutputPort(const std::string& modelName,
                                             const std::string& portName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName",  value::StringFactory::create(modelName));
    lst->addValue("portName",  value::StringFactory::create(portName));
    return lst;
}

value::Map DSDevs::buildMessageRemoveInputPort(const std::string& modelName,
                                               const std::string& portName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName",  value::StringFactory::create(modelName));
    lst->addValue("portName",  value::StringFactory::create(portName));
    return lst;
}

value::Map DSDevs::buildMessageRemoveOutputPort(const std::string& modelName,
                                                const std::string& portName)
{
    value::Map lst =value::MapFactory::create();
    lst->addValue("modelName",  value::StringFactory::create(modelName));
    lst->addValue("portName",  value::StringFactory::create(portName));
    return lst;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

value::Set DSDevs::addToBagAddConnection(const std::string& srcModelName,
                                         const std::string& srcPortName,
                                         const std::string& dstModelName,
                                         const std::string& dstPortName,
                                         value::Set currentbag)
{
    value::Map mp = buildMessageAddConnection(srcModelName, srcPortName,
                                              dstModelName, dstPortName);
    mp->addValue("action", value::StringFactory::create("addConnection"));

    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagChangeConnection(const std::string& srcModelName,
                                            const std::string& srcPortName,
                                            const std::string& oldDstModelName,
                                            const std::string& oldDstPortName,
                                            const std::string& newDstModelName,
                                            const std::string& newDstPortName,
                                            value::Set currentbag)
{
    value::Map mp = buildMessageChangeConnection(
        srcModelName, srcPortName, oldDstModelName, oldDstPortName,
        newDstModelName, newDstPortName);
    mp->addValue("action", value::StringFactory::create("changeConnection"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


value::Set DSDevs::addToBagRemoveConnection(const std::string& srcModelName,
                                            const std::string& srcPortName,
                                            const std::string& dstModelName,
                                            const std::string& dstPortName,
                                            value::Set currentbag)
{
    value::Map mp = buildMessageRemoveConnection(srcModelName, srcPortName,
                                                 dstModelName, dstPortName);
    mp->addValue("action", value::StringFactory::create("removeConnection"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagAddModel(const std::string& prefixModelName,
                                    const std::string& className,
                                    const std::string& xmlDynamics,
                                    const std::string& xmlInits,
                                    value::Set connection,
                                    value::Set currentbag)
{
    value::Map mp = buildMessageAddModel(prefixModelName, className, xmlDynamics,
                                         xmlInits, connection);
    mp->addValue("action", value::StringFactory::create("addModel"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


value::Set DSDevs::addToBagRemoveModel(const std::string& modelName,
                                       value::Set currentbag)
{
    value::Map mp = buildMessageRemoveModel(modelName);
    mp->addValue("action", value::StringFactory::create("removeModel"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


value::Set DSDevs::addToBagChangeModel(const std::string& modelName,
                                       const std::string& className,
                                       const std::string& newClassName,
                                       value::Set currentbag)
{
    value::Map mp = buildMessageChangeModel(modelName, className, newClassName);
    mp->addValue("action", value::StringFactory::create("changeModel"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagBuildModel(const std::string& prefixModelName,
                                      const std::string& className,
                                      const std::string& xmlCode,
                                      const std::string& xmlDynamics,
                                      const std::string& xmlInits,
                                      value::Set currentbag)
{
    value::Map mp = buildMessageBuildModel(prefixModelName, className, xmlCode,
                                           xmlDynamics, xmlInits);
    mp->addValue("action", value::StringFactory::create("buildModel"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagAddInputPort(const std::string& modelName,
                                        const std::string& portName,
                                        value::Set currentbag)
{
    value::Map mp = buildMessageAddInputPort(modelName, portName);
    mp->addValue("action", value::StringFactory::create("addInputPort"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagAddOutputPort(const std::string& modelName,
                                         const std::string& portName,
                                         value::Set currentbag)
{
    value::Map mp = buildMessageAddOutputPort(modelName, portName);
    mp->addValue("action", value::StringFactory::create("addOutputPort"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagRemoveInputPort(const std::string& modelName,
                                           const std::string& portName,
                                           value::Set currentbag)
{
    value::Map mp = buildMessageRemoveInputPort(modelName, portName);
    mp->addValue("action", value::StringFactory::create("removeInputPort"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

value::Set DSDevs::addToBagRemoveOutputPort(const std::string& modelName,
                                            const std::string& portName,
                                            value::Set currentbag)
{
    value::Map mp = buildMessageRemoveOutputPort(modelName, portName);
    mp->addValue("action", value::StringFactory::create("removeOutputPort"));
    if (currentbag.get() == 0) {
        currentbag = value::SetFactory::create();
    }
    currentbag->addValue(mp);
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
        Throw(utils::InternalError, boost::format("Unknow action '%1%'\n") %
              action);
    }
}

bool DSDevs::processAddModel(const value::Map& val)
{
    const std::string& pre(val->getStringValue("prefixModelName"));
    const std::string& cls(val->getStringValue("className"));
    std::string xmld, xmli;
    value::Set cnt;

    if (val->existValue("xmlDynamics"))
        xmld.assign(val->getStringValue("xmlDynamics"));

    if (val->existValue("xmlInits"))
        xmli.assign(val->getStringValue("xmlInits"));

    if (val->existValue("addConnection"))
        cnt = val->getSetValue("addConnection");

    m_state = ADD_MODEL;
    return addModel(pre, cls, cnt);
}

bool DSDevs::processRemoveModel(const value::Map& val)
{
    const std::string& nam(val->getStringValue("modelName"));
    m_state = REMOVE_MODEL;
    return removeModel(nam);
}

bool DSDevs::processChangeModel(const value::Map& val)
{
    const std::string& nam(val->getStringValue("modelName"));
    const std::string& oldc(val->getStringValue("className"));
    const std::string& newc(val->getStringValue("newClassName"));
    m_state = CHANGE_MODEL;
    return changeModel(nam, oldc, newc);
}

bool DSDevs::processBuildModel(const value::Map& val)
{
    const std::string& pre(val->getStringValue("prefixModelName"));
    const std::string& cls(val->getStringValue("className"));
    const std::string& cod(val->getStringValue("xmlCode"));
    const std::string& xmld(val->getStringValue("xmlDynamics"));
    const std::string& xmli(val->getStringValue("xmlInits"));
    m_state = BUILD_MODEL;
    return buildModel(pre, cls, cod, xmld, xmli);
}

bool DSDevs::processAddInputPort(const value::Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = ADD_INPUTPORT;
    return addInputPort(mdl, prt);
}

bool DSDevs::processRemoveInputPort(const value::Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = REMOVE_INPUTPORT;
    return removeInputPort(mdl, prt);
}

bool DSDevs::processAddOutputPort(const value::Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = ADD_OUTPUTPORT;
    return addOutputPort(mdl, prt);
}

bool DSDevs::processRemoveOutputPort(const value::Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = REMOVE_OUTPUTPORT;
    return removeOutputPort(mdl, prt);
}

bool DSDevs::processAddConnection(const value::Map& val)
{
    const std::string& srcmn(val->getStringValue("srcModelName"));
    const std::string& srcpn(val->getStringValue("srcPortName"));
    const std::string& dstmn(val->getStringValue("dstModelName"));
    const std::string& dstpn(val->getStringValue("dstPortName"));
    m_state = ADD_CONNECTION;
    return addConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processRemoveConnection(const value::Map& val)
{
    const std::string& srcmn(val->getStringValue("srcModelName"));
    const std::string& srcpn(val->getStringValue("srcPortName"));
    const std::string& dstmn(val->getStringValue("dstModelName"));
    const std::string& dstpn(val->getStringValue("dstPortName"));
    m_state = REMOVE_CONNECTION;
    return removeConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processChangeConnection(const value::Map& val)
{
    const std::string& srcmn(val->getStringValue("srcModelName"));
    const std::string& srcpn(val->getStringValue("srcPortName"));
    const std::string& oldm(val->getStringValue("oldDstModelName"));
    const std::string& oldp(val->getStringValue("oldDstPortName"));
    const std::string& newm(val->getStringValue("newDstModelName"));
    const std::string& newp(val->getStringValue("newDstPortName"));
    m_state = CHANGE_CONNECTION;
    return changeConnection(srcmn, srcpn, oldm, oldp, newm, newp);
}

bool DSDevs::processBag(const value::Map& val)
{
    value::Value valuebag(val->getValue("bag"));
    value::Set bag = value::to_set(valuebag);
    bool result = true;

    value::SetFactory::VectorValue& vv(bag->getValue());
    for (value::SetFactory::VectorValueIt it = vv.begin();
         it != vv.end(); ++it) {
        value::Map msg = to_map(*it);
        value::Value msgaction = msg->getValue("action");
        value::String straction = value::to_string(msgaction);

        Assert(utils::InternalError, straction, "No correct action message.");

        const std::string& action = straction->stringValue();
        result = processSwitch(action, msg);

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
        m_coupledModel : m_coupledModel->getModel(srcModelName);
    graph::Model* oldDstModel = (modelName == oldDstModelName)?
        m_coupledModel : m_coupledModel->getModel(oldDstModelName);
    graph::Model* newDstModel = (modelName == newDstModelName)?
        m_coupledModel : m_coupledModel->getModel(newDstModelName);

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
        m_coupledModel : m_coupledModel->getModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        m_coupledModel : m_coupledModel->getModel(dstModelName);

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
                      value::Set /* connection */)
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
        //for (value::SetFactory::VectorValueIt it =
             //connection->getValue().begin(); it !=
             //connection->getValue().end(); ++it) {

            //if ((*it)->isMap()) {
                //value::Map mp = value::to_map(*it);
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

    Throw(utils::NotYetImplemented, "addModel from class not allowed");
         //return true;
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
    graph::Model* mdl = m_coupledModel->find(modelName);
    mdl->addInputPort(portName);
    return true;
}

bool DSDevs::addOutputPort(const std::string& modelName,
                           const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    mdl->addOutputPort(portName);
    return true;
}

bool DSDevs::removeInputPort(const std::string& modelName,
                             const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    mdl->delInputPort(portName);
    return true;
}

bool DSDevs::removeOutputPort(const std::string& modelName,
                              const std::string& portName)
{
    graph::Model* mdl = m_coupledModel->find(modelName);
    mdl->delOutputPort(portName);
    return true;
}

graph::ModelList& DSDevs::getModelList() const
{
    return  m_coupledModel->getModelList();
}

}} // namespace vle extension
