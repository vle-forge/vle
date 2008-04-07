/**
 * @file src/vle/vpz/SaxStackVpz.cpp
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




#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/vpz/Structures.hpp>
#include <vle/vpz/Port.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/EndTrame.hpp>
#include <vle/vpz/RefreshTrame.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace vpz {

SaxStackVpz::~SaxStackVpz()
{
    Assert(utils::SaxParserError, m_stack.empty(), "SaxStackVpz not empty");
}

std::ostream& operator<<(std::ostream& out, const SaxStackVpz& stack)
{
    SaxStackVpz nv(stack);

    out << "SaxStackVpz:\n";
    while (not nv.m_stack.empty()) {
        out << "type " << nv.m_stack.top()->getType() << "\n";
        nv.pop();
    }

    out << "Xml:\n" << nv.m_vpz << "\n";
    return out;
}

void SaxStackVpz::clear()
{
    while (not m_stack.empty()) {
        if (m_stack.top()->isStructures() or
            m_stack.top()->isModel() or
            m_stack.top()->isIn() or
            m_stack.top()->isOut() or
            m_stack.top()->isState() or
            m_stack.top()->isInit() or
            m_stack.top()->isSubmodels() or
            m_stack.top()->isConnections() or
            m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection() or
            m_stack.top()->isOrigin() or
            m_stack.top()->isDestination() or
            m_stack.top()->isTrame() or
            m_stack.top()->isModelTrame()) {
            delete m_stack.top();
        }
        m_stack.pop();
    }
}

vpz::Vpz* SaxStackVpz::pushVpz(const AttributeList& att)
{
    AssertS(utils::SaxParserError, m_stack.empty());

    if (existAttribute(att, "date")) {
        m_vpz.project().setDate(getAttribute < std::string >(att, "date"));
    }

    m_vpz.project().setAuthor(getAttribute < std::string >(att, "author"));
    m_vpz.project().setVersion(getAttribute < float >(att, "version"));

    if (existAttribute(att, "instance")) {
        m_vpz.project().setInstance(getAttribute < int >(att, "instance"));
    }

    if (existAttribute(att, "replica")) {
        m_vpz.project().setReplica(getAttribute < int >(att, "replica"));
    }
    
    m_stack.push(&m_vpz);

    return &m_vpz;
}

void SaxStackVpz::pushStructure()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    vpz::Structures* structure = new vpz::Structures();
    m_stack.push(structure);
}

void SaxStackVpz::pushModel(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isStructures() or
            m_stack.top()->isSubmodels() or m_stack.top()->isClass());

    graph::CoupledModel* parent = 0;

    if (m_stack.top()->isSubmodels()) {
        vpz::Base* sub = pop();
        vpz::Model* tmp = static_cast < vpz::Model* >(m_stack.top());
        parent = static_cast < graph::CoupledModel* >(tmp->model());
        m_stack.push(sub);
    }

    graph::Model* gmdl = 0;

    std::string type(getAttribute < std::string >(att, "type"));
    std::string name(getAttribute < std::string >(att, "name"));
    if (type == "atomic") {
        std::string cnd, dyn, obs;
        if (existAttribute(att, "conditions"))
            cnd = getAttribute < std::string >(att, "conditions");

        if (existAttribute(att, "dynamics"))
            dyn = getAttribute < std::string >(att, "dynamics");

        if (existAttribute(att, "observables"))
            obs = getAttribute < std::string >(att, "observables");

        try {
            gmdl = new graph::AtomicModel(name, parent);
        } catch(const utils::DevsGraphError& e) {
            Throw(utils::SaxParserError, boost::format(
                    "Error build atomic model '%1%' with error: %2%") % name %
                e.what());
        }
        vpz().project().model().atomicModels().insert(std::make_pair(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(cnd, dyn, obs)));
    } else if (type == "coupled") {
        try {
            gmdl = new graph::CoupledModel(name, parent);
        } catch(const utils::DevsGraphError& e) {
            Throw(utils::SaxParserError, boost::format(
                    "Error build coupled model '%1%' with error: %2%") % name %
                e.what());
        }
    } else {
        Throw(utils::SaxParserError, (boost::format(
                        "Unknow model type %1%") % type));
    }
 
    vpz::Model* mdl = new vpz::Model();
    mdl->set_model(gmdl);

    if (m_stack.top()->isStructures()) {
        vpz().project().model().set_model(gmdl);
    } else if (m_stack.top()->isClass()) {
        reinterpret_cast < Class* >(m_stack.top())->setModel(gmdl);
    }

    m_stack.push(mdl);
}

void SaxStackVpz::pushPort(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());

    if (m_stack.top()->isCondition()) {
        pushConditionPort(att);
    } else if (m_stack.top()->isObservable()) {
        pushObservablePort(att);
    } else {
        AssertS(utils::SaxParserError, m_stack.top()->isIn() or
                m_stack.top()->isOut());

        vpz::Base* type = pop();

        AssertS(utils::SaxParserError, m_stack.top()->isModel());

        vpz::Model* mdl = static_cast < vpz::Model* >(m_stack.top());
        graph::Model* gmdl = mdl->model();

        std::string name(getAttribute < std::string >(att, "name"));

        if (type->isIn()) {
            gmdl->addInputPort(name);
        } else if (type->isOut()) {
            gmdl->addOutputPort(name);
        }
        m_stack.push(type);
    }
}

void SaxStackVpz::pushPortType(const Glib::ustring& name)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Base* prt = 0;
    if (name == "in") {
        prt = new vpz::In();
    } else if (name == "out") {
        prt = new vpz::Out();
    } else if (name == "state") {
        prt = new vpz::State();
    } else if (name == "init") {
        prt = new vpz::Init();
    } else {
        Throw(utils::SaxParserError, (boost::format("Unknow port type %1%.") %
                                     name));
    }
    m_stack.push(prt);
}

void SaxStackVpz::pushSubModels()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Submodels* sub = new vpz::Submodels();
    m_stack.push(sub);
}

void SaxStackVpz::pushConnections()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Connections* cnts = new vpz::Connections();
    m_stack.push(cnts);
}

void SaxStackVpz::pushConnection(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isConnections());

    std::string type(getAttribute < std::string >(att, "type"));
    vpz::Base* cnt = 0;
    
    if (type == "internal") {
        cnt = new vpz::InternalConnection();
    } else if (type == "input") {
        cnt = new vpz::InputConnection();
    } else if (type == "output") {
        cnt = new vpz::OutputConnection();
    } else {
        Throw(utils::SaxParserError, (boost::format("Unknow connection type %1%")
                                     % type));
    }
    m_stack.push(cnt);
}

void SaxStackVpz::pushOrigin(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection());

    std::string mdl(getAttribute < std::string >(att, "model"));
    std::string port(getAttribute < std::string >(att, "port"));

    vpz::Base* orig = new vpz::Origin(mdl, port);
    m_stack.push(orig);
}

void SaxStackVpz::pushDestination(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isOrigin());

    std::string mdl(getAttribute < std::string >(att, "model"));
    std::string port(getAttribute < std::string >(att, "port"));

    vpz::Base* dest = new vpz::Destination(mdl, port);
    m_stack.push(dest);
}

void SaxStackVpz::buildConnection()
{
    AssertS(utils::SaxParserError, not m_stack.empty());

    AssertS(utils::SaxParserError, m_stack.top()->isDestination());
    vpz::Destination* dest = static_cast < vpz::Destination* >(pop());

    AssertS(utils::SaxParserError, m_stack.top()->isOrigin());
    vpz::Origin* orig = static_cast < vpz::Origin* >(pop());
    
    AssertS(utils::SaxParserError, m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection());
    vpz::Base* cnt = pop();

    AssertS(utils::SaxParserError, m_stack.top()->isConnections());
    vpz::Base* cntx = pop();
    
    AssertS(utils::SaxParserError, m_stack.top()->isModel());
    vpz::Model* model = static_cast < vpz::Model* >(m_stack.top());

    graph::CoupledModel* cpl = static_cast < graph::CoupledModel*
        >(model->model());

    if (cnt->isInternalConnection()) {
        cpl->addInternalConnection(orig->model, orig->port, dest->model,
                                   dest->port);
    } else if (cnt->isInputConnection()) {
        cpl->addInputConnection(orig->port, dest->model, dest->port);
    } else if (cnt->isOutputConnection()) {
        cpl->addOutputConnection(orig->model, orig->port, dest->port);
    }

    m_stack.push(cntx);
    delete dest;
    delete orig;
    delete cnt;
}

void SaxStackVpz::pushDynamics()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    m_stack.push(&m_vpz.project().dynamics());
}

void SaxStackVpz::pushDynamic(const AttributeList& att)
{ 
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isDynamics());

    vpz::Dynamic dyn(getAttribute < std::string >(att, "name"));
    dyn.setLibrary(getAttribute < std::string >(att, "library"));

    if (existAttribute(att, "model"))
        dyn.setModel(getAttribute < std::string >(att, "model"));
    else
        dyn.setModel("");

    if (existAttribute(att, "language"))
        dyn.setLanguage(getAttribute < std::string >(att, "language"));
    else
        dyn.setLanguage("");

    if (existAttribute(att, "type")) {
        std::string type(getAttribute < std::string >(att, "type"));
        if (type == "local") {
            dyn.setLocalDynamics();
        } else {
            std::string loc(getAttribute < std::string >(att, "location"));
            std::string ip;
            int port;
            utils::net::explodeStringNet(loc, ip, port);
            dyn.setDistantDynamics(ip, port);
        }
    } else {
        dyn.setLocalDynamics();
    }

    vpz::Dynamics& dyns(m_vpz.project().dynamics());
    dyns.add(dyn);
}

void SaxStackVpz::pushExperiment(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    vpz::Experiment& exp(m_vpz.project().experiment());
    m_stack.push(&exp);

    exp.setName(getAttribute < std::string >(att, "name"));
    exp.setDuration(getAttribute < double >(att, "duration"));
    exp.setSeed(getAttribute < guint32 >(att, "seed"));

    if (existAttribute(att, "combination")) {
        exp.setCombination(getAttribute < std::string >(att, "combination"));
    }
}

void SaxStackVpz::pushReplicas(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    vpz::Replicas& rep(m_vpz.project().experiment().replicas());

    bool haveseed = false;
    bool havenumber = false;

    for (AttributeList::const_iterator it = att.begin();
         it != att.end(); ++it) {
        if ((*it).name == "seed") {
            try {
                rep.setSeed(boost::lexical_cast < guint32 >((*it).value));
                haveseed = true;
            } catch(const boost::bad_lexical_cast& ) {
                Throw(utils::ArgError, boost::format(
                        "Cannot convert replicas's tag seed: '%1%'") %
                    (*it).value);
            }
        } else if ((*it).name == "number") {
            try {
                rep.setNumber(boost::lexical_cast < size_t >((*it).value));
                havenumber = true;
            } catch(const boost::bad_lexical_cast& ) {
                Throw(utils::ArgError, boost::format(
                        "Cannot convert replicas's tag number: '%1%'") %
                    (*it).value);
            }
        }
    }

    Assert(utils::ArgError, haveseed,
           "The replicas's tag does not define a seed");

    Assert(utils::ArgError, havenumber,
           "The replica's tag does not define a number");
}

void SaxStackVpz::pushConditions()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    m_stack.push(&m_vpz.project().experiment().conditions());
}

void SaxStackVpz::pushCondition(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isConditions());

    vpz::Conditions& cnds(m_vpz.project().experiment().conditions());
    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Condition newcondition(name);
    vpz::Condition& cnd(cnds.add(newcondition));
    m_stack.push(&cnd);
}

void SaxStackVpz::pushConditionPort(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isCondition());

    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Condition* cnd(static_cast < vpz::Condition* >(m_stack.top()));
    cnd->add(name);
}

value::Set& SaxStackVpz::popConditionPort()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isCondition());

    vpz::Condition* cnd(static_cast < vpz::Condition* >(m_stack.top()));
    value::Set& vals(cnd->lastAddedPort());

    return vals;
}

void SaxStackVpz::pushViews()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    m_stack.push(&m_vpz.project().experiment().views());
}

void SaxStackVpz::pushOutputs()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    m_stack.push(&m_vpz.project().experiment().views().outputs());
}

void SaxStackVpz::pushOutput(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isOutputs());

    std::string name(getAttribute < std::string >(att, "name"));
    std::string format(getAttribute < std::string >(att, "format"));
    std::string plugin(getAttribute < std::string >(att, "plugin"));
    
    std::string location;
    if (existAttribute(att, "location")) 
        location = getAttribute < std::string >(att, "location");

    Outputs& outs(m_vpz.project().experiment().views().outputs());
    if (format == "local") {
        m_stack.push(&outs.addLocalStream(name, location, plugin));
    } else if (format == "distant") {
        m_stack.push(&outs.addDistantStream(name, location, plugin));
    } else {
        Throw(utils::SaxParserError, (boost::format(
                    "Unknow format '%1%' for the output %2%") % format % name));
    }
}

void SaxStackVpz::pushView(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    std::string name(getAttribute< std::string >(att, "name")); 
    std::string type(getAttribute< std::string >(att, "type")); 
    std::string out(getAttribute< std::string >(att, "output")); 

    Views& views(m_vpz.project().experiment().views());

    if (type == "timed") {
        Assert(utils::SaxParserError, existAttribute(att, "timestep"),
               (boost::format("View %1% have no timestep attribute.") %
                name));

        double ts(getAttribute < double >(att, "timestep"));
        views.addTimedView(name, ts, out);
    } else if (type == "event") {
        views.addEventView(name, out);
    } else if (type == "finish") {
        views.addFinishView(name, out);
    } else {
        Throw(utils::SaxParserError, (boost::format(
                    "Unknow type '%1%' for the view %1%") % type % name));
    }
}

void SaxStackVpz::pushAttachedView(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservablePort());
    
    pushObservablePortOnView(att);
}

void SaxStackVpz::pushObservables()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    Observables& obs(m_vpz.project().experiment().views().observables());
    m_stack.push(&obs);
}

void SaxStackVpz::pushObservable(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservables());

    Views& views(m_vpz.project().experiment().views());

    std::string name(getAttribute < std::string >(att, "name"));
    Observable& obs(views.addObservable(name));
    m_stack.push(&obs);
}

void SaxStackVpz::pushObservablePort(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservable());

    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Observable* out(static_cast < vpz::Observable* >(m_stack.top()));
    vpz::ObservablePort& ports(out->add(name));
    m_stack.push(&ports);
}

void SaxStackVpz::pushObservablePortOnView(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservablePort());
    
    std::string name(getAttribute < std::string >(att, "name"));

    vpz::ObservablePort* port(static_cast < vpz::ObservablePort* >(
            m_stack.top()));
    port->add(name);
}

void SaxStackVpz::pushVleTrame()
{
    AssertS(utils::SaxParserError, m_stack.empty());
    m_stack.push(new VLETrame);
}

void SaxStackVpz::pushTrame(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, dynamic_cast < VLETrame* >(m_stack.top()));

    std::string type, date, name, parent, port, view, plugin, location;
    for (AttributeList::const_iterator it = att.begin();
         it != att.end(); ++it) {
        if (it->name == "type") {
            type = it->value; 
        } if (it->name == "date") {
            date = it->value;
        } else if (it->name == "name") {
            name = it->value;
        } else if (it->name == "parent") {
            parent = it->value;
        } else if (it->name == "port") {
            port = it->value;
        } else if (it->name == "view") {
            view = it->value;
        } else if (it->name == "plugin") {
            plugin = it->value;
        } else if (it->name == "location") {
            location = it->value;
        }
    }

    if (type == "value") {
        m_stack.push(new ValueTrame(date));
    } else if (type == "new") {
        m_stack.push(new NewObservableTrame(date, name, parent, port, view));
    } else if (type == "del") {
        m_stack.push(new DelObservableTrame(date, name, parent, port, view));
    } else if (type == "parameter") {
        m_stack.push(new ParameterTrame(date, plugin, location));
    } else if (type == "value") {
        m_stack.push(new ValueTrame(date));
    } else if (type == "end") {
        m_stack.push(new EndTrame(date));
    } else if (type == "refresh") {
        m_stack.push(new RefreshTrame());
    } else {
        Throw(utils::SaxParserError, boost::format(
                "Unknow trame named %1%") % type);
    }
}

void SaxStackVpz::popTrame()
{
    AssertS(utils::SaxParserError, m_stack.top()->isTrame() or
            m_stack.top()->isModelTrame());

    m_trame.push_back(reinterpret_cast < Trame* >(m_stack.top()));
}

void SaxStackVpz::popModelTrame(const value::Value& value)
{
    AssertS(utils::SaxParserError, m_stack.top()->isModelTrame());
    ValueTrame* tr = reinterpret_cast < ValueTrame* >(m_stack.top());
    tr->add(value);
}

void SaxStackVpz::popVleTrame()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, dynamic_cast < VLETrame* >(m_stack.top()));
    delete m_stack.top();
    m_stack.pop();
}

void SaxStackVpz::pushClasses()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    m_stack.push(&m_vpz.project().classes());
}

void SaxStackVpz::pushClass(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isClasses());

    std::string name(getAttribute < std::string >(att, "name"));

    Class& cls = m_vpz.project().classes().add(name);
    m_stack.push(&cls);
}

void SaxStackVpz::popClasses()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isClasses());

    m_stack.pop();
}

void SaxStackVpz::popClass()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isClass());

    m_stack.pop();
}

void SaxStackVpz::pushModelTrame(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModelTrame());

    std::string name, parent, port, view;
    for (AttributeList::const_iterator it = att.begin();
         it != att.end(); ++it) {
        if (it->name == "name") {
            name = it->value;
        } else if (it->name == "parent") {
            parent = it->value;
        } else if (it->name == "port") {
            port = it->value;
        } else if (it->name == "view") {
            view = it->value;
        }
    }

    reinterpret_cast < ValueTrame* >(m_stack.top())->add(name, parent, port, view);
}

vpz::Base* SaxStackVpz::pop()
{
    vpz::Base* top = m_stack.top();
    m_stack.pop();
    return top;
}

const vpz::Base* SaxStackVpz::top() const
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    return m_stack.top();
}

vpz::Base* SaxStackVpz::top()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    return m_stack.top();
}

}} // namespace vle vpz
