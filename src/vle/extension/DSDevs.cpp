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
#include <vle/graph/Port.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Exception.hpp>

using namespace vle::devs;
using namespace vle::extension;
using namespace vle::graph;
using namespace vle::value;

DSDevs::DSDevs(const vle::graph::AtomicModel& model) :
  devs::Dynamics(model),
  m_state(IDLE),
  m_response(false),
  m_coupledModel(0)
{
  Assert(utils::InternalError, model.getParent(),
	 "The DSDEVS is not in an coupled model.");
  
  m_coupledModel = model.getParent();
}
    
Time DSDevs::init()
{
  m_state = IDLE;
  return Time::infinity;
}

void DSDevs::getOutputFunction(const Time& /* time */,
			       ExternalEventList& output) 
{
  ExternalEvent* ev;

  if (m_state != DSDevs::IDLE) {
    AssertI(m_nameList.size() == m_response.size());

    while (not m_nameList.empty()) {
      ev = new ExternalEvent("ok");
      ev << attribute("name", m_nameList.front());
      ev << attribute("ok", m_response.front());
      output.addEvent(ev);
      
      m_nameList.pop_front();
      m_response.pop_front();
    }
    
    if (not m_newName.empty()) {
      Set eeset = SetFactory::create();
      for (std::list < std::string >::const_iterator it =
	     m_newName.begin(); it != m_newName.end(); ++it) {
	eeset->addValue(StringFactory::create(*it));
      }
      ev = new ExternalEvent("name");
      ev << attribute("name", eeset);
      output.addEvent(ev);
      m_newName.clear();
    }
  }
}

Time DSDevs::getTimeAdvance()
{
  return (m_state != IDLE) ? 0 : Time::infinity;
}

Event::EventType DSDevs::processConflict(const InternalEvent& /* internal */,
					 const ExternalEventList& /* extEventlist */) const
{
  return Event::EXTERNAL;
}

void DSDevs::processExternalEvents(const ExternalEventList& event,
				   const Time& /* time */)
{
  ExternalEventList::const_iterator it = event.begin();

  while (it != event.end()) {
    m_nameList.push_back((*it)->getSourceModelName());
    const std::string& portname((*it)->getPortName());

    m_response.push_back(processSwitch(portname, (*it)->getAttributes()->getMapValue(portname)));
    ++it;
  }
}

void DSDevs::processInternalEvent(const InternalEvent& /* event */)
{
  if (m_state != IDLE)
    m_state = IDLE;
}

Value DSDevs::processStateEvent(const StateEvent& /* event */) const
{
  xmlpp::Document doc;
    //FIXME
    //xmlpp::Element* root = doc.create_root_node("STATE");
    //if (evt->getPortName() == "coupled") {
    //m_coupledModel->writeXML(root);
    //} else if (evt->getPortName() == "hierarchy") {
    //m_coupledModel->writeXML(root);
    //} else if (evt->getPortName() == "complete") {
    //m_coupledModel->writeXML(root);
    //}

  return buildString(doc.write_to_string_formatted());
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Map DSDevs::buildMessageAddConnection(const std::string& srcModelName,
				      const std::string& srcPortName,
				      const std::string& dstModelName,
				      const std::string& dstPortName)
{
    Map lst = MapFactory::create();
    lst->addValue("srcModelName", StringFactory::create(srcModelName));
    lst->addValue("srcPortName", StringFactory::create(srcPortName));
    lst->addValue("dstModelName", StringFactory::create(dstModelName));
    lst->addValue("dstPortName", StringFactory::create(dstPortName));
    return lst;
}

Map DSDevs::buildMessageChangeConnection(const std::string& srcModelName,
                                    const std::string& srcPortName,
                                    const std::string& oldDstModelName,
                                    const std::string& oldDstPortName,
                                    const std::string& newDstModelName,
                                    const std::string& newDstPortName)
{
    Map lst = MapFactory::create();
    lst->addValue("srcModelName", StringFactory::create(srcModelName));
    lst->addValue("srcPortName", StringFactory::create(srcPortName));
    lst->addValue("oldDstModelName",
                  StringFactory::create(oldDstModelName));
    lst->addValue("oldDstPortName",
                  StringFactory::create(oldDstPortName));
    lst->addValue("newDstModelName",
                  StringFactory::create(newDstModelName));
    lst->addValue("newDstPortName",
                  StringFactory::create(newDstPortName));
    return lst;
}

Map DSDevs::buildMessageRemoveConnection(
                                    const std::string& srcModelName,
                                    const std::string& srcPortName,
                                    const std::string& dstModelName,
                                    const std::string& dstPortName)
{
    Map lst = MapFactory::create();
    lst->addValue("srcModelName",
                  StringFactory::create(srcModelName));
    lst->addValue("srcPortName", StringFactory::create(srcPortName));
    lst->addValue("dstModelName",
                  StringFactory::create(dstModelName));
    lst->addValue("dstPortName", StringFactory::create(dstPortName));
    return lst;
}

Map DSDevs::buildMessageAddModel(const std::string& prefixModelName,
                           const std::string& className,
                           const std::string& xmlDynamics,
                           const std::string& xmlInits,
                           Set connection)
{
    Map lst = MapFactory::create();
    lst->addValue("prefixModelName", StringFactory::create(prefixModelName));
    lst->addValue("className", StringFactory::create(className));
    lst->addValue("xmlDynamics", StringFactory::create(xmlDynamics));
    lst->addValue("xmlInits", StringFactory::create(xmlInits));
    lst->addValue("addConnection", connection);
    return lst;
}

Map DSDevs::buildMessageRemoveModel(const std::string& modelName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName", StringFactory::create(modelName));
    return lst;
}

Map DSDevs::buildMessageChangeModel(const std::string& modelName,
                              const std::string& className,
                              const std::string& newClassName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName", StringFactory::create(modelName));
    lst->addValue("className", StringFactory::create(className));
    lst->addValue("newClassName", StringFactory::create(newClassName));
    return lst;
}

Map DSDevs::buildMessageBuildModel(const std::string& prefixModelName,
                             const std::string& className,
                             const std::string& xmlCode,
                             const std::string& xmlDynamics,
                             const std::string& xmlInits)
{
    Map lst = MapFactory::create();
    lst->addValue("prefixModelName", StringFactory::create(prefixModelName));
    lst->addValue("className", StringFactory::create(className));
    lst->addValue("xmlCode", StringFactory::create(xmlCode));
    lst->addValue("xmlDynamics", StringFactory::create(xmlDynamics));
    lst->addValue("xmlInits", StringFactory::create(xmlInits));
    return lst;
}

Map DSDevs::buildMessageAddInputPort(const std::string& modelName,
                               const std::string& portName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName",  StringFactory::create(modelName));
    lst->addValue("portName",  StringFactory::create(portName));
    return lst;
}

Map DSDevs::buildMessageAddOutputPort(const std::string& modelName,
                           const std::string& portName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName",  StringFactory::create(modelName));
    lst->addValue("portName",  StringFactory::create(portName));
    return lst;
}

Map DSDevs::buildMessageRemoveInputPort(const std::string& modelName,
                             const std::string& portName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName",  StringFactory::create(modelName));
    lst->addValue("portName",  StringFactory::create(portName));
    return lst;
}

Map DSDevs::buildMessageRemoveOutputPort(const std::string& modelName,
                                   const std::string& portName)
{
    Map lst = MapFactory::create();
    lst->addValue("modelName",  StringFactory::create(modelName));
    lst->addValue("portName",  StringFactory::create(portName));
    return lst;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Set DSDevs::addToBagAddConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& dstModelName,
                              const std::string& dstPortName,
                              Set currentbag)
{
    Map mp = buildMessageAddConnection(srcModelName, srcPortName,
                                               dstModelName, dstPortName);
    mp->addValue("action", StringFactory::create("addConnection"));
    
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagChangeConnection(const std::string& srcModelName,
                                 const std::string& srcPortName,
                                 const std::string& oldDstModelName,
                                 const std::string& oldDstPortName,
                                 const std::string& newDstModelName,
                                 const std::string& newDstPortName,
                                 Set currentbag)
{
    Map mp = buildMessageChangeConnection(
            srcModelName, srcPortName, oldDstModelName, oldDstPortName,
            newDstModelName, newDstPortName);
    mp->addValue("action", StringFactory::create("changeConnection"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


Set DSDevs::addToBagRemoveConnection(const std::string& srcModelName,
                                 const std::string& srcPortName,
                                 const std::string& dstModelName,
                                 const std::string& dstPortName,
                                 Set currentbag)
{
    Map mp = buildMessageRemoveConnection(srcModelName, srcPortName,
                                           dstModelName, dstPortName);
    mp->addValue("action", StringFactory::create("removeConnection"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagAddModel(const std::string& prefixModelName,
                         const std::string& className,
                         const std::string& xmlDynamics,
                         const std::string& xmlInits,
                         Set connection,
                         Set currentbag)
{
    Map mp = buildMessageAddModel(prefixModelName, className, xmlDynamics,
                                   xmlInits, connection);
    mp->addValue("action", StringFactory::create("addModel"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


Set DSDevs::addToBagRemoveModel(const std::string& modelName,
                            Set currentbag)
{
    Map mp = buildMessageRemoveModel(modelName);
    mp->addValue("action", StringFactory::create("removeModel"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}


Set DSDevs::addToBagChangeModel(const std::string& modelName,
                            const std::string& className,
                            const std::string& newClassName,
                            Set currentbag)
{
    Map mp = buildMessageChangeModel(modelName, className, newClassName);
    mp->addValue("action", StringFactory::create("changeModel"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagBuildModel(const std::string& prefixModelName,
                           const std::string& className,
                           const std::string& xmlCode,
                           const std::string& xmlDynamics,
                           const std::string& xmlInits,
                           Set currentbag)
{
    Map mp = buildMessageBuildModel(prefixModelName, className, xmlCode,
                                     xmlDynamics, xmlInits);
    mp->addValue("action", StringFactory::create("buildModel"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagAddInputPort(const std::string& modelName,
                             const std::string& portName,
                             Set currentbag)
{
    Map mp = buildMessageAddInputPort(modelName, portName);
    mp->addValue("action", StringFactory::create("addInputPort"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagAddOutputPort(const std::string& modelName,
                              const std::string& portName,
                              Set currentbag)
{
    Map mp = buildMessageAddOutputPort(modelName, portName);
    mp->addValue("action", StringFactory::create("addOutputPort"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagRemoveInputPort(const std::string& modelName,
                                const std::string& portName,
                                Set currentbag)
{
    Map mp = buildMessageRemoveInputPort(modelName, portName);
    mp->addValue("action", StringFactory::create("removeInputPort"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

Set DSDevs::addToBagRemoveOutputPort(const std::string& modelName,
                                 const std::string& portName,
                                 Set currentbag)
{
    Map mp = buildMessageRemoveOutputPort(modelName, portName);
    mp->addValue("action", StringFactory::create("removeOutputPort"));
    if (currentbag.get() == 0) {
        currentbag = SetFactory::create();
    }
    currentbag->addValue(mp);
    return currentbag;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool DSDevs::processSwitch(const std::string& action, const Map& val)
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

bool DSDevs::processAddModel(const Map& val)
{
    const std::string& pre(val->getStringValue("prefixModelName"));
    const std::string& cls(val->getStringValue("className"));
    std::string xmld, xmli;
    Set cnt;

    if (val->existValue("xmlDynamics"))
        xmld.assign(val->getStringValue("xmlDynamics"));

    if (val->existValue("xmlInits"))
        xmli.assign(val->getStringValue("xmlInits"));

    if (val->existValue("addConnection"))
        cnt = val->getSetValue("addConnection");

    m_state = ADD_MODEL;
    return addModel(pre, cls, xmld, xmli, cnt);
}

bool DSDevs::processRemoveModel(const Map& val)
{
    const std::string& nam(val->getStringValue("modelName"));
    m_state = REMOVE_MODEL;
    return removeModel(nam);
}

bool DSDevs::processChangeModel(const Map& val)
{
    const std::string& nam(val->getStringValue("modelName"));
    const std::string& oldc(val->getStringValue("className"));
    const std::string& newc(val->getStringValue("newClassName"));
    m_state = CHANGE_MODEL;
    return changeModel(nam, oldc, newc);
}

bool DSDevs::processBuildModel(const Map& val)
{
    const std::string& pre(val->getStringValue("prefixModelName"));
    const std::string& cls(val->getStringValue("className"));
    const std::string& cod(val->getStringValue("xmlCode"));
    const std::string& xmld(val->getStringValue("xmlDynamics"));
    const std::string& xmli(val->getStringValue("xmlInits"));
    m_state = BUILD_MODEL;
    return buildModel(pre, cls, cod, xmld, xmli);
}

bool DSDevs::processAddInputPort(const Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = ADD_INPUTPORT;
    return addInputPort(mdl, prt);
}

bool DSDevs::processRemoveInputPort(const Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = REMOVE_INPUTPORT;
    return removeInputPort(mdl, prt);
}

bool DSDevs::processAddOutputPort(const Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = ADD_OUTPUTPORT;
    return addOutputPort(mdl, prt);
}

bool DSDevs::processRemoveOutputPort(const Map& val)
{
    const std::string& mdl(val->getStringValue("modelName"));
    const std::string& prt(val->getStringValue("portName"));
    m_state = REMOVE_OUTPUTPORT;
    return removeOutputPort(mdl, prt);
}

bool DSDevs::processAddConnection(const Map& val)
{
    const std::string& srcmn(val->getStringValue("srcModelName"));
    const std::string& srcpn(val->getStringValue("srcPortName"));
    const std::string& dstmn(val->getStringValue("dstModelName"));
    const std::string& dstpn(val->getStringValue("dstPortName"));
    m_state = ADD_CONNECTION;
    return addConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processRemoveConnection(const Map& val)
{
    const std::string& srcmn(val->getStringValue("srcModelName"));
    const std::string& srcpn(val->getStringValue("srcPortName"));
    const std::string& dstmn(val->getStringValue("dstModelName"));
    const std::string& dstpn(val->getStringValue("dstPortName"));
    m_state = REMOVE_CONNECTION;
    return removeConnection(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processChangeConnection(const Map& val)
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

bool DSDevs::processBag(const Map& val)
{
    Value valuebag(val->getValue("bag"));
    Set bag = to_set(valuebag);
    bool result = true;

    SetFactory::VectorValue& vv(bag->getValue());
    for (SetFactory::VectorValueIt it = vv.begin();
         it != vv.end(); ++it) {
        Map msg = to_map(*it);
        Value msgaction = msg->getValue("action");
        String straction = to_string(msgaction);

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

bool DSDevs::addModel(const std::string& /* prefixModelName */,
                      const std::string& /* className */,
                      const std::string& /* xmlDynamics */,
                      const std::string& /* xmlInits */,
                      Set /* connection */)
{
  // FIX ME
//     std::string newname(m_coupledModel->buildNewName(prefixModelName));
//     m_newName.push_back(newname);

//     try {
//       Model* mdl = getModel()->getSimulator()->createModels(m_coupledModel, className, xmlDynamics, xmlInits);
//         mdl->setName(newname);
//     } catch (const std::exception& e) {
//         std::cerr << boost::format(
//             "Warning: Unable to dynamic add model, with prefixname '%1%' "
//             "class name '%2%'. Error reported is: '%3%\n'") %
//             prefixModelName % className % e.what();
//         return false;
//     }

//     if (connection) {
//         for (SetFactory::VectorValueIt it =
//              connection->getValue().begin(); it !=
//              connection->getValue().end(); ++it) {
	    
//             if ((*it)->isMap()) {
//                 Map mp = to_map(*it);
//                 std::string srcm, srcp, dstm, dstp;
		
//                 if (mp->existValue("srcModelName")) {
//                     srcm.assign(mp->getStringValue("srcModelName"));
//                     if (srcm.empty())
//                         srcm.assign(newname);
//                 } else
//                     srcm.assign(newname);
                    
//                 if (mp->existValue("dstModelName")) {
//                     dstm.assign(mp->getStringValue("dstModelName"));
//                     if (dstm.empty())
//                         dstm.assign(newname);
//                 } else
//                     dstm.assign(newname);
            
//                 srcp = mp->getStringValue("srcPortName");
//                 dstp = mp->getStringValue("dstPortName");
//                 addConnection(srcm, srcp, dstm, dstp);
//             }
//         }
//     }

    return true;
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

std::vector < vle::graph::Model* >& DSDevs::getModelList() const
{
    return  m_coupledModel->getModelList();
}
