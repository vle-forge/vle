/**
 * @file vle/extension/DSDevs.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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
#include <vle/utils/Trace.hpp>
#include <iostream>


namespace vle { namespace extension {

DSDevs::DSDevs(const devs::ExecutiveInit& model,
               const devs::InitEventList& events)
    : devs::Executive(model, events), m_state(IDLE), m_response(false)
{
    assert(model.model().getParent());
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
    return devs::Event::INTERNAL;
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
        coupledmodel().writeXML(out);
    } else if (event.onPort("hierarchy")) {
        coupledmodel().writeXML(out);
    } else if (event.onPort("complete")) {
        coupledmodel().writeXML(out);
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
        throw utils::InternalError(fmt(_(
                    "DSDevs ext.: unknow action '%1%'")) % action);
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
    return addModelT(pre, cls, cnt);
}

bool DSDevs::processRemoveModel(const value::Map& val)
{
    const std::string& nam(val.getString("modelName"));
    m_state = REMOVE_MODEL;
    return removeModelT(nam);
}

bool DSDevs::processChangeModel(const value::Map& val)
{
    const std::string& nam(val.getString("modelName"));
    const std::string& oldc(val.getString("className"));
    const std::string& newc(val.getString("newClassName"));
    m_state = CHANGE_MODEL;
    return changeModelT(nam, oldc, newc);
}

bool DSDevs::processBuildModel(const value::Map& val)
{
    const std::string& pre(val.getString("prefixModelName"));
    const std::string& cls(val.getString("className"));
    const std::string& cod(val.getString("xmlCode"));
    const std::string& xmld(val.getString("xmlDynamics"));
    const std::string& xmli(val.getString("xmlInits"));
    m_state = BUILD_MODEL;
    return buildModelT(pre, cls, cod, xmld, xmli);
}

bool DSDevs::processAddInputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = ADD_INPUTPORT;
    return addInputPortT(mdl, prt);
}

bool DSDevs::processRemoveInputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = REMOVE_INPUTPORT;
    return removeInputPortT(mdl, prt);
}

bool DSDevs::processAddOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = ADD_OUTPUTPORT;
    return addOutputPortT(mdl, prt);
}

bool DSDevs::processRemoveOutputPort(const value::Map& val)
{
    const std::string& mdl(val.getString("modelName"));
    const std::string& prt(val.getString("portName"));
    m_state = REMOVE_OUTPUTPORT;
    return removeOutputPortT(mdl, prt);
}

bool DSDevs::processAddConnection(const value::Map& val)
{
    const std::string& srcmn(val.getString("srcModelName"));
    const std::string& srcpn(val.getString("srcPortName"));
    const std::string& dstmn(val.getString("dstModelName"));
    const std::string& dstpn(val.getString("dstPortName"));
    m_state = ADD_CONNECTION;
    return addConnectionT(srcmn, srcpn, dstmn, dstpn);
}

bool DSDevs::processRemoveConnection(const value::Map& val)
{
    const std::string& srcmn(val.getString("srcModelName"));
    const std::string& srcpn(val.getString("srcPortName"));
    const std::string& dstmn(val.getString("dstModelName"));
    const std::string& dstpn(val.getString("dstPortName"));
    m_state = REMOVE_CONNECTION;
    return removeConnectionT(srcmn, srcpn, dstmn, dstpn);
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
    return changeConnectionT(srcmn, srcpn, oldm, oldp, newm, newp);
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

bool DSDevs::addConnectionT(const std::string& srcModelName,
                            const std::string& srcPortName,
                            const std::string& dstModelName,
                            const std::string& dstPortName)
{
    try {
        Executive::addConnection(srcModelName, srcPortName, dstModelName,
                                 dstPortName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs add connection error: %1%")) % e.what());
        return false;
    }
}

bool DSDevs::changeConnectionT(const std::string& srcModelName,
                               const std::string& srcPortName,
                               const std::string& oldDstModelName,
                               const std::string& oldDstPortName,
                               const std::string& newDstModelName,
                               const std::string& newDstPortName)
{
    try {
        Executive::removeConnection(srcModelName, srcPortName, oldDstModelName,
                                    oldDstPortName);
        Executive::addConnection(srcModelName, srcPortName, newDstModelName,
                                 newDstPortName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs change connection error: %1%")) %
                       e.what());
        return false;
    }
}

bool DSDevs::removeConnectionT(const std::string& srcModelName,
                               const std::string& srcPortName,
                               const std::string& dstModelName,
                               const std::string& dstPortName)
{
    try {
        Executive::removeConnection(srcModelName, srcPortName, dstModelName,
                                    dstPortName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs remove connection: %1%")) % e.what());
        return false;
    }
}

bool DSDevs::addModelT(const std::string& /* prefixModelName */,
                       const std::string& /* className */,
                       const value::Set* /* connection */)
{
    throw utils::NotYetImplemented(_(
            "DSDevs ext.: addModel from class not allowed"));
}

bool DSDevs::removeModelT(const std::string& modelname)
{
    try {
        Executive::delModel(modelname);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DSDevs::changeModelT(const std::string& /*modelName*/,
                          const std::string& /*className*/,
                          const std::string& /*newClassName*/)
{
    throw utils::NotYetImplemented(_(
            "DSDevs ext.: changeModel not allowed"));
}

bool DSDevs::buildModelT(const std::string& /*prefixModelName*/,
                         const std::string& /*className*/,
                         const std::string& /*xmlCode*/,
                         const std::string& /*xmlDynamics*/,
                         const std::string& /*xmlInits*/)
{
    throw utils::NotYetImplemented(_(
            "DSDevs ext.: buildModel Not yet available"));
}

bool DSDevs::addInputPortT(const std::string& modelName,
                           const std::string& portName)
{
    try {
        Executive::addInputPort(modelName, portName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs add input port error: %1%")) % e.what());
        return false;
    }
}

bool DSDevs::addOutputPortT(const std::string& modelName,
                            const std::string& portName)
{
    try {
        Executive::addOutputPort(modelName, portName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs add output port error: %1%")) % e.what());
        return false;
    }
}

bool DSDevs::removeInputPortT(const std::string& modelName,
                              const std::string& portName)
{
    try {
        Executive::removeInputPort(modelName, portName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs remove input port error: %1%")) %
                       e.what());
        return false;
    }
}

bool DSDevs::removeOutputPortT(const std::string& modelName,
                               const std::string& portName)
{
    try {
        Executive::removeOutputPort(modelName, portName);
        return true;
    } catch (const std::exception& e) {
        TraceExtension(fmt(_("DSDevs remove output port error: %1%")) %
                       e.what());
        return false;
    }
}

const graph::ModelList& DSDevs::getModelList() const
{
    return coupledmodel().getModelList();
}

}} // namespace vle extension
