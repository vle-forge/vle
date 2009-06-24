/**
 * @file vle/vpz/SaxStackVpz.cpp
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
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Debug.hpp>

namespace vle { namespace vpz {

std::ostream& operator<<(std::ostream& out, const SaxStackVpz& stack)
{
    SaxStackVpz nv(stack);

    out << "SaxStackVpz:\n";
    while (not nv.m_stack.empty()) {
        out << "type " << nv.parent()->getType() << "\n";
        nv.pop();
    }

    out << "Xml:\n" << nv.m_vpz << "\n";
    return out;
}

void SaxStackVpz::clear()
{
    while (not m_stack.empty()) {
        if (parent()->isStructures() or
            parent()->isModel() or
            parent()->isIn() or
            parent()->isOut() or
            parent()->isState() or
            parent()->isInit() or
            parent()->isSubmodels() or
            parent()->isConnections() or
            parent()->isInternalConnection() or
            parent()->isInputConnection() or
            parent()->isOutputConnection() or
            parent()->isOrigin() or
            parent()->isDestination()) {
            delete parent();
        }
        pop();
    }
}

vpz::Vpz* SaxStackVpz::pushVpz(const AttributeList& att)
{
    Assert < utils::SaxParserError >(m_stack.empty());

    if (existAttribute(att, "date")) {
        m_vpz.project().setDate(getAttribute < std::string >(att, "date"));
    }

    m_vpz.project().setAuthor(getAttribute < std::string >(att, "author"));
    m_vpz.project().setVersion(getAttribute < std::string >(att, "version"));

    if (existAttribute(att, "instance")) {
        m_vpz.project().setInstance(getAttribute < int >(att, "instance"));
    }

    if (existAttribute(att, "replica")) {
        m_vpz.project().setReplica(getAttribute < int >(att, "replica"));
    }

    push(&m_vpz);

    return &m_vpz;
}

void SaxStackVpz::pushStructure()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    push(new vpz::Structures());
}

void SaxStackVpz::pushModel(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isStructures() or
            parent()->isSubmodels() or parent()->isClass());

    graph::CoupledModel* cplparent = 0;

    if (parent()->isSubmodels()) {
        vpz::Base* sub = pop();
        vpz::Model* tmp = static_cast < vpz::Model* >(parent());
        cplparent = static_cast < graph::CoupledModel* >(tmp->model());
        push(sub);
    }

    graph::Model* gmdl = 0;
    std::string type, name, conditions, dynamics, observables;
    std::string x, y, width, height;

    for (AttributeList::const_iterator it = att.begin(); it != att.end();
         ++it) {
        if ((it->name) == "type") {
            type = it->value;
        } else if ((it->name) == "name") {
            name = it->value;
        } else if ((it->name) == "conditions") {
            conditions = it->value;
        } else if ((it->name) == "dynamics") {
            dynamics = it->value;
        } else if ((it->name) == "observables") {
            observables = it->value;
        } else if ((it->name) == "x") {
            x = it->value;
        } else if ((it->name) == "y") {
            y = it->value;
        } else if ((it->name) == "width") {
            width = it->value;
        } else if ((it->name) == "height") {
            height = it->value;
        }
    }

    Assert < utils::SaxParserError >(
        not name.empty(), _("Attribute name not defined"));
    Assert < utils::SaxParserError >(
        not type.empty(), fmt(_("Attribute type not defined for model '%1%'"))
        % name);

    if (type == "atomic") {
        try {
            gmdl = new graph::AtomicModel(name, cplparent);
        } catch(const utils::DevsGraphError& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Error build atomic model '%1%' with error: %2%")) % name %
                e.what()));
        }

        Class* clsparent(getLastClass());
        if (clsparent == 0) {
            vpz().project().model().atomicModels().add(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(conditions, dynamics, observables));
        } else {
            clsparent->atomicModels().add(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(conditions, dynamics, observables));
        }
    } else if (type == "coupled") {
        try {
            gmdl = new graph::CoupledModel(name, cplparent);
        } catch(const utils::DevsGraphError& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Error build coupled model '%1%' with error: %2%")) % name %
                e.what()));
        }
    } else {
        throw(utils::SaxParserError(fmt(_(
                        "Unknow model type %1%")) % type));
    }

    vpz::Model* mdl = new vpz::Model();
    mdl->setModel(gmdl);
    buildModelGraphics(gmdl, x, y, width, height);

    if (parent()->isStructures()) {
        vpz().project().model().setModel(gmdl);
    } else if (parent()->isClass()) {
        reinterpret_cast < Class* >(parent())->setModel(gmdl);
    }

    push(mdl);
}

void SaxStackVpz::buildModelGraphics(graph::Model* mdl,
                                     const std::string& x,
                                     const std::string& y,
                                     const std::string& width,
                                     const std::string& height)
{
    if (not x.empty() and not y.empty()) {
        try {
            mdl->setX(boost::lexical_cast < int >(x));
            mdl->setY(boost::lexical_cast < int >(y));
        } catch (boost::bad_lexical_cast& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Cannot convert x or y position for model %1%")) %
                mdl->getName()));
        }
    }

    if (not width.empty() and not height.empty()) {
        try {
            mdl->setWidth(boost::lexical_cast < int >(width));
            mdl->setHeight(boost::lexical_cast < int >(height));
        } catch (boost::bad_lexical_cast& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Cannot convert width or height for model %1%")) %
                mdl->getName()));
        }
    }
}

void SaxStackVpz::pushPort(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());

    if (parent()->isCondition()) {
        pushConditionPort(att);
    } else if (parent()->isObservable()) {
        pushObservablePort(att);
    } else {
        Assert < utils::SaxParserError >(parent()->isIn() or
                parent()->isOut());

        vpz::Base* type = pop();

        Assert < utils::SaxParserError >(parent()->isModel());

        vpz::Model* mdl = static_cast < vpz::Model* >(parent());
        graph::Model* gmdl = mdl->model();

        std::string name(getAttribute < std::string >(att, "name"));

        if (type->isIn()) {
            gmdl->addInputPort(name);
        } else if (type->isOut()) {
            gmdl->addOutputPort(name);
        }
        push(type);
    }
}

void SaxStackVpz::pushPortType(const Glib::ustring& name)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isModel());

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
        throw(utils::SaxParserError(fmt(_(
                    "Unknow port type %1%.")) % name));
    }
    push(prt);
}

void SaxStackVpz::pushSubModels()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isModel());

    vpz::Submodels* sub = new vpz::Submodels();
    push(sub);
}

void SaxStackVpz::pushConnections()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isModel());

    vpz::Connections* cnts = new vpz::Connections();
    push(cnts);
}

void SaxStackVpz::pushConnection(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isConnections());

    std::string type(getAttribute < std::string >(att, "type"));
    vpz::Base* cnt = 0;

    if (type == "internal") {
        cnt = new vpz::InternalConnection();
    } else if (type == "input") {
        cnt = new vpz::InputConnection();
    } else if (type == "output") {
        cnt = new vpz::OutputConnection();
    } else {
        throw(utils::SaxParserError(fmt(_(
                    "Unknow connection type %1%")) % type));
    }
    push(cnt);
}

void SaxStackVpz::pushOrigin(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isInternalConnection() or
            parent()->isInputConnection() or
            parent()->isOutputConnection());

    std::string mdl(getAttribute < std::string >(att, "model"));
    std::string port(getAttribute < std::string >(att, "port"));

    vpz::Base* orig = new vpz::Origin(mdl, port);
    push(orig);
}

void SaxStackVpz::pushDestination(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isOrigin());

    std::string mdl(getAttribute < std::string >(att, "model"));
    std::string port(getAttribute < std::string >(att, "port"));

    vpz::Base* dest = new vpz::Destination(mdl, port);
    push(dest);
}

void SaxStackVpz::buildConnection()
{
    Assert < utils::SaxParserError >(not m_stack.empty());

    Assert < utils::SaxParserError >(parent()->isDestination());
    vpz::Destination* dest = static_cast < vpz::Destination* >(pop());

    Assert < utils::SaxParserError >(parent()->isOrigin());
    vpz::Origin* orig = static_cast < vpz::Origin* >(pop());

    Assert < utils::SaxParserError >(parent()->isInternalConnection() or
            parent()->isInputConnection() or
            parent()->isOutputConnection());
    vpz::Base* cnt = pop();

    Assert < utils::SaxParserError >(parent()->isConnections());
    vpz::Base* cntx = pop();

    Assert < utils::SaxParserError >(parent()->isModel());
    vpz::Model* model = static_cast < vpz::Model* >(parent());

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

    push(cntx);
    delete dest;
    delete orig;
    delete cnt;
}

void SaxStackVpz::pushDynamics()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    push(&m_vpz.project().dynamics());
}

void SaxStackVpz::pushDynamic(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isDynamics());

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

    vpz::Dynamics* dyns(static_cast < Dynamics* >(parent()));
    dyns->add(dyn);
}

void SaxStackVpz::pushExperiment(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    vpz::Experiment& exp(m_vpz.project().experiment());
    push(&exp);

    exp.setName(getAttribute < std::string >(att, "name"));
    exp.setDuration(getAttribute < double >(att, "duration"));
    exp.setSeed(getAttribute < guint32 >(att, "seed"));

    if (existAttribute(att, "begin")) {
        exp.setBegin(getAttribute < double >(att, "begin"));
    } else {
        exp.setBegin(0.0);
    }

    if (existAttribute(att, "combination")) {
        exp.setCombination(getAttribute < std::string >(att, "combination"));
    }
}

void SaxStackVpz::pushReplicas(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isExperiment());

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
                throw(utils::ArgError(fmt(_(
                        "Cannot convert replicas's tag seed: '%1%'")) %
                    (*it).value));
            }
        } else if ((*it).name == "number") {
            try {
                rep.setNumber(boost::lexical_cast < size_t >((*it).value));
                havenumber = true;
            } catch(const boost::bad_lexical_cast& ) {
                throw(utils::ArgError(fmt(_(
                        "Cannot convert replicas's tag number: '%1%'")) %
                    (*it).value));
            }
        }
    }

    Assert < utils::ArgError >(haveseed,
                               _("The replicas's tag does not define a seed"));

    Assert < utils::ArgError >(havenumber,
                               _("The replica's tag does not define a number"));
}

void SaxStackVpz::pushConditions()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isExperiment());

    push(&m_vpz.project().experiment().conditions());
}

void SaxStackVpz::pushCondition(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isConditions());

    vpz::Conditions& cnds(m_vpz.project().experiment().conditions());
    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Condition newcondition(name);
    vpz::Condition& cnd(cnds.add(newcondition));
    push(&cnd);
}

void SaxStackVpz::pushConditionPort(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isCondition());

    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Condition* cnd(static_cast < vpz::Condition* >(parent()));
    cnd->add(name);
}

value::Set& SaxStackVpz::popConditionPort()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isCondition());

    vpz::Condition* cnd(static_cast < vpz::Condition* >(parent()));
    value::Set& vals(cnd->lastAddedPort());

    return vals;
}

void SaxStackVpz::pushViews()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isExperiment());

    push(&m_vpz.project().experiment().views());
}

void SaxStackVpz::pushOutputs()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isViews());

    push(&m_vpz.project().experiment().views().outputs());
}

void SaxStackVpz::popOutput()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isOutput());

    pop();
}

void SaxStackVpz::pushOutput(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isOutputs());

    std::string name(getAttribute < std::string >(att, "name"));
    std::string format(getAttribute < std::string >(att, "format"));
    std::string plugin(getAttribute < std::string >(att, "plugin"));

    std::string location;
    if (existAttribute(att, "location"))
        location = getAttribute < std::string >(att, "location");

    Outputs& outs(m_vpz.project().experiment().views().outputs());
    if (format == "local") {
        push(&outs.addLocalStream(name, location, plugin));
    } else if (format == "distant") {
        push(&outs.addDistantStream(name, location, plugin));
    } else {
        throw(utils::SaxParserError(fmt(_(
                    "Unknow format '%1%' for the output %2%")) % format %
                name));
    }
}

void SaxStackVpz::pushView(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isViews());

    std::string name(getAttribute< std::string >(att, "name"));
    std::string type(getAttribute< std::string >(att, "type"));
    std::string out(getAttribute< std::string >(att, "output"));

    Views& views(m_vpz.project().experiment().views());

    if (type == "timed") {
        Assert < utils::SaxParserError >(existAttribute(att, "timestep"),
               (fmt(_("View %1% have no timestep attribute.")) %
                name));

        double ts(getAttribute < double >(att, "timestep"));
        views.addTimedView(name, ts, out);
    } else if (type == "event") {
        views.addEventView(name, out);
    } else if (type == "finish") {
        views.addFinishView(name, out);
    } else {
        throw(utils::SaxParserError(fmt(_(
                    "Unknow type '%1%' for the view %1%")) % type % name));
    }
}

void SaxStackVpz::pushAttachedView(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservablePort());

    pushObservablePortOnView(att);
}

void SaxStackVpz::pushObservables()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isViews());

    Observables& obs(m_vpz.project().experiment().views().observables());
    push(&obs);
}

void SaxStackVpz::pushObservable(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservables());

    Views& views(m_vpz.project().experiment().views());

    std::string name(getAttribute < std::string >(att, "name"));
    Observable& obs(views.addObservable(name));
    push(&obs);
}

void SaxStackVpz::pushObservablePort(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservable());

    std::string name(getAttribute < std::string >(att, "name"));

    vpz::Observable* out(static_cast < vpz::Observable* >(parent()));
    vpz::ObservablePort& ports(out->add(name));
    push(&ports);
}

void SaxStackVpz::pushObservablePortOnView(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservablePort());

    std::string name(getAttribute < std::string >(att, "name"));

    vpz::ObservablePort* port(static_cast < vpz::ObservablePort* >(
            parent()));
    port->add(name);
}

void SaxStackVpz::pushClasses()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    push(&m_vpz.project().classes());
}

void SaxStackVpz::pushClass(const AttributeList& att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isClasses());

    std::string name(getAttribute < std::string >(att, "name"));

    Class& cls = m_vpz.project().classes().add(name);
    push(&cls);
}

void SaxStackVpz::popClasses()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isClasses());

    pop();
}

void SaxStackVpz::popClass()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isClass());

    pop();
}

vpz::Base* SaxStackVpz::pop()
{
    vpz::Base* top = parent();
    m_stack.pop_front();
    return top;
}

const vpz::Base* SaxStackVpz::top() const
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    return parent();
}

vpz::Base* SaxStackVpz::top()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    return parent();
}

Class* SaxStackVpz::getLastClass() const
{
    std::list < vpz::Base* >::const_iterator it;

    it = std::find_if(m_stack.begin(), m_stack.end(), Base::IsClass());
    return (it != m_stack.end()) ? reinterpret_cast < Class* >(*it) : 0;
}

}} // namespace vle vpz
