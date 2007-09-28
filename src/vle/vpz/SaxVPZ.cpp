/** 
 * @file SaxVPZ.cpp
 * @brief A class to parse VPZ XML with Sax parser.
 * @author The vle Development Team
 * @date jeu, 14 déc 2006 10:34:50 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/vpz/SaxVPZ.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Structures.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Port.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/EndTrame.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/XML.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>


namespace vle { namespace vpz {


std::ostream& operator<<(std::ostream& out, const VpzStackSax& stack)
{
    VpzStackSax nv(stack);

    out << "VpzStackSax:\n";
    while (not nv.m_stack.empty()) {
        out << "type " << nv.m_stack.top()->getType() << "\n";
        nv.pop();
    }

    out << "Xml:\n" << nv.m_vpz << "\n";
    return out;
}


vpz::Vpz* VpzStackSax::push_vpz(const AttributeList& att)
{
    AssertS(utils::SaxParserError, m_stack.empty());

    if (exist_attribute(att, "date")) {
        m_vpz.project().setDate(get_attribute < std::string >(att, "date"));
    }

    m_vpz.project().setAuthor(get_attribute < std::string >(att, "author")),
    m_vpz.project().setVersion(get_attribute < float >(att, "version")),
    
    m_stack.push(&m_vpz);

    return &m_vpz;
}

void VpzStackSax::push_structure()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    vpz::Structures* structure = new vpz::Structures();
    m_stack.push(structure);
}

void VpzStackSax::push_model(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isStructures() or
            m_stack.top()->isSubmodels());

    graph::CoupledModel* parent = 0;

    if (m_stack.top()->isSubmodels()) {
        vpz::Base* sub = pop();
        vpz::Model* tmp = static_cast < vpz::Model* >(m_stack.top());
        parent = static_cast < graph::CoupledModel* >(tmp->model());
        m_stack.push(sub);
    }

    graph::Model* gmdl = 0;

    std::string type(get_attribute < std::string >(att, "type"));
    std::string name(get_attribute < std::string >(att, "name"));
    if (type == "atomic") {
        std::string cnd, dyn, obs;
        if (exist_attribute(att, "conditions"))
            cnd = get_attribute < std::string >(att, "conditions");

        if (exist_attribute(att, "dynamics"))
            dyn = get_attribute < std::string >(att, "dynamics");

        if (exist_attribute(att, "observables"))
            obs = get_attribute < std::string >(att, "observables");
        
        gmdl = new graph::AtomicModel(name, parent);
        vpz().project().model().atomicModels().insert(std::make_pair(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(cnd, dyn, obs, "")));
    } else if (type == "coupled") {
        gmdl = new graph::CoupledModel(name, parent);
    } else if (type == "novle") {
        std::string trs(get_attribute < std::string >(att, "translator"));

        gmdl = new graph::NoVLEModel(name, parent);
        vpz().project().model().atomicModels().insert(std::make_pair(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel("", "", "", trs)));
    } else {
        Throw(utils::InternalError, (boost::format(
                        "Unknow model type %1%") % type));
    }
    
    vpz::Model* mdl = new vpz::Model();
    mdl->set_model(gmdl);

    if (m_stack.top()->isStructures()) {
        vpz().project().model().set_model(gmdl);
    }

    m_stack.push(mdl);
}

void VpzStackSax::push_port(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());

    if (m_stack.top()->isCondition()) {
        push_condition_port(att);
    } else if (m_stack.top()->isObservable()) {
        push_observable_port(att);
    } else {
        AssertS(utils::SaxParserError, m_stack.top()->isIn() or
                m_stack.top()->isOut() or m_stack.top()->isState() or
                m_stack.top()->isInit());

        vpz::Base* type = pop();

        AssertS(utils::SaxParserError, m_stack.top()->isModel());

        vpz::Model* mdl = static_cast < vpz::Model* >(m_stack.top());
        graph::Model* gmdl = mdl->model();

        std::string name(get_attribute < std::string >(att, "name"));

        if (type->isIn()) {
            gmdl->addInputPort(name);
        } else if (type->isOut()) {
            gmdl->addOutputPort(name);
        } else if (type->isState()) {
            gmdl->addStatePort(name);
        } else if (type->isInit()) {
            gmdl->addInitPort(name);
        }
        m_stack.push(type);
    }
}

void VpzStackSax::push_porttype(const Glib::ustring& name)
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
        Throw(utils::InternalError, (boost::format("Unknow port type %1%.") %
                                     name));
    }
    m_stack.push(prt);
}

void VpzStackSax::push_submodels()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Submodels* sub = new vpz::Submodels();
    m_stack.push(sub);
}

void VpzStackSax::push_connections()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModel());

    vpz::Connections* cnts = new vpz::Connections();
    m_stack.push(cnts);
}

void VpzStackSax::push_connection(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isConnections());

    std::string type(get_attribute < std::string >(att, "type"));
    vpz::Base* cnt = 0;
    
    if (type == "internal") {
        cnt = new vpz::InternalConnection();
    } else if (type == "input") {
        cnt = new vpz::InputConnection();
    } else if (type == "output") {
        cnt = new vpz::OutputConnection();
    } else {
        Throw(utils::InternalError, (boost::format("Unknow connection type %1%")
                                     % type));
    }
    m_stack.push(cnt);
}

void VpzStackSax::push_origin(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isInternalConnection() or
            m_stack.top()->isInputConnection() or
            m_stack.top()->isOutputConnection());

    std::string mdl(get_attribute < std::string >(att, "model"));
    std::string port(get_attribute < std::string >(att, "port"));

    vpz::Base* orig = new vpz::Origin(mdl, port);
    m_stack.push(orig);
}

void VpzStackSax::push_destination(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isOrigin());

    std::string mdl(get_attribute < std::string >(att, "model"));
    std::string port(get_attribute < std::string >(att, "port"));

    vpz::Base* dest = new vpz::Destination(mdl, port);
    m_stack.push(dest);
}

void VpzStackSax::build_connection()
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

void VpzStackSax::push_dynamics()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    m_stack.push(&m_vpz.project().dynamics());
}

void VpzStackSax::push_dynamic(const AttributeList& att)
{ 
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isDynamics());

    vpz::Dynamic dyn(get_attribute < std::string >(att, "name"));
    dyn.setLibrary(get_attribute < std::string >(att, "library"));

    if (exist_attribute(att, "model"))
        dyn.setModel(get_attribute < std::string >(att, "model"));
    else
        dyn.setModel("");

    if (exist_attribute(att, "language"))
        dyn.setLanguage(get_attribute < std::string >(att, "language"));
    else
        dyn.setLanguage("");

    if (exist_attribute(att, "type")) {
        std::string type(get_attribute < std::string >(att, "type"));
        if (type == "local") {
            dyn.setLocalDynamics();
        } else {
            std::string loc(get_attribute < std::string >(att, "location"));
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

void VpzStackSax::push_experiment(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    vpz::Experiment& exp(m_vpz.project().experiment());
    m_stack.push(&exp);

    exp.setName(get_attribute < std::string >(att, "name"));
    exp.setDuration(get_attribute < double >(att, "duration"));
    exp.setSeed(get_attribute < guint32 >(att, "seed"));

    if (exist_attribute(att, "combination")) {
        exp.setCombination(get_attribute < std::string >(att, "combination"));
    }
}

void VpzStackSax::push_replicas(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    vpz::Replicas& rep(m_vpz.project().experiment().replicas());
    rep.setSeed(get_attribute < guint32 >(att, "seed"));
    rep.setNumber(get_attribute < size_t >(att, "number"));
}

void VpzStackSax::push_conditions()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    m_stack.push(&m_vpz.project().experiment().conditions());
}

void VpzStackSax::push_condition(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isConditions());

    vpz::Conditions& cnds(m_vpz.project().experiment().conditions());
    std::string name(get_attribute < std::string >(att, "name"));

    vpz::Condition newcondition(name);
    vpz::Condition& cnd(cnds.add(newcondition));
    m_stack.push(&cnd);
}

void VpzStackSax::push_condition_port(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isCondition());

    std::string name(get_attribute < std::string >(att, "name"));

    vpz::Condition* cnd(static_cast < vpz::Condition* >(m_stack.top()));
    cnd->add(name);
}

value::Set& VpzStackSax::pop_condition_port()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isCondition());

    vpz::Condition* cnd(static_cast < vpz::Condition* >(m_stack.top()));
    value::Set& vals(cnd->last_added_port());

    return vals;
}

void VpzStackSax::push_views()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isExperiment());

    m_stack.push(&m_vpz.project().experiment().views());
}

void VpzStackSax::push_outputs()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    m_stack.push(&m_vpz.project().experiment().views().outputs());
}

void VpzStackSax::push_output(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isOutputs());

    std::string name(get_attribute < std::string >(att, "name"));
    std::string format(get_attribute < std::string >(att, "format"));
    std::string plugin(get_attribute < std::string >(att, "plugin"));
    
    std::string location;
    if (exist_attribute(att, "location")) 
        location = get_attribute < std::string >(att, "location");

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

void VpzStackSax::push_view(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    std::string name(get_attribute< std::string >(att, "name")); 
    std::string type(get_attribute< std::string >(att, "type")); 
    std::string out(get_attribute< std::string >(att, "output")); 

    Views& views(m_vpz.project().experiment().views());

    if (type == "timed") {
        Assert(utils::SaxParserError, exist_attribute(att, "timestep"),
               (boost::format("View %1% have no timestep attribute.") %
                name));

        double ts(get_attribute < double >(att, "timestep"));
        views.addTimedView(name, ts, out);
    } else if (type == "event") {
        views.addEventView(name, out);
    } else {
        Throw(utils::SaxParserError, (boost::format(
                    "Unknow type '%1%' for the view %1%") % type % name));
    }
}

void VpzStackSax::push_attachedview(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservablePort());
    
    push_observable_port_on_view(att);
}

void VpzStackSax::push_observables()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isViews());

    Observables& obs(m_vpz.project().experiment().views().observables());
    m_stack.push(&obs);
}

void VpzStackSax::push_observable(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservables());

    Views& views(m_vpz.project().experiment().views());

    std::string name(get_attribute < std::string >(att, "name"));
    Observable& obs(views.addObservable(name));
    m_stack.push(&obs);
}

void VpzStackSax::push_observable_port(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservable());

    std::string name(get_attribute < std::string >(att, "name"));

    vpz::Observable* out(static_cast < vpz::Observable* >(m_stack.top()));
    vpz::ObservablePort& ports(out->add(name));
    m_stack.push(&ports);
}

void VpzStackSax::push_observable_port_on_view(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isObservablePort());
    
    std::string name(get_attribute < std::string >(att, "name"));

    vpz::ObservablePort* port(static_cast < vpz::ObservablePort* >(
            m_stack.top()));
    port->add(name);
}

void VpzStackSax::push_translators()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isVpz());

    vpz::NoVLEs& novles(m_vpz.project().novles());
    m_stack.push(&novles);
}

void VpzStackSax::push_translator(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isNoVLES());

    std::string name(get_attribute < std::string >(att, "name"));
    std::string lib(get_attribute < std::string >(att, "library"));

    vpz::NoVLE novle(name);
    novle.setNoVLE(lib, "");
    vpz::NoVLE& newvle(m_vpz.project().novles().add(novle));
    m_stack.push(&newvle);
}

void VpzStackSax::pop_translator(const std::string& cdata)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isNoVLE());

    vpz::NoVLE* novle(static_cast < vpz::NoVLE* >(m_stack.top()));
    novle->setData(cdata);
}

void VpzStackSax::push_vletrame()
{
    AssertS(utils::SaxParserError, m_stack.empty());
    m_stack.push(new VLETrame);
}

void VpzStackSax::push_trame(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, dynamic_cast < VLETrame* >(m_stack.top()));

    std::string type(get_attribute < std::string >(att, "type"));
    if (type == "value") {
        m_stack.push(new ValueTrame(
                get_attribute < std::string >(att, "date")));
    } else if (type == "new") {
        m_stack.push(new NewObservableTrame(
                get_attribute < std::string >(att, "date"),
                get_attribute < std::string >(att, "name"),
                get_attribute < std::string >(att, "parent"),
                get_attribute < std::string >(att, "port"),
                get_attribute < std::string >(att, "view")));
    } else if (type == "del") {
        m_stack.push(new DelObservableTrame(
                get_attribute < std::string >(att, "date"),
                get_attribute < std::string >(att, "name"),
                get_attribute < std::string >(att, "parent"),
                get_attribute < std::string >(att, "port"),
                get_attribute < std::string >(att, "view")));
    } else if (type == "parameter") {
        m_stack.push(new ParameterTrame(
                get_attribute < std::string >(att, "date"),
                get_attribute < std::string >(att, "plugin"),
                get_attribute < std::string >(att, "location")));
    } else if (type == "value") {
        m_stack.push(new ValueTrame(
                get_attribute < std::string >(att, "date")));
    } else if (type == "end") {
        m_stack.push(new EndTrame(
                get_attribute < std::string >(att, "date")));
    } else {
        Throw(utils::SaxParserError, boost::format(
                "Unknow trame named %1%") % type);
    }
}

void VpzStackSax::pop_trame()
{
    AssertS(utils::SaxParserError, m_stack.top()->isTrame() or
            m_stack.top()->isModelTrame());

    m_trame.push_back(reinterpret_cast < Trame* >(m_stack.top()));
}

void VpzStackSax::pop_modeltrame(const value::Value& value)
{
    AssertS(utils::SaxParserError, m_stack.top()->isModelTrame());
    ValueTrame* tr = reinterpret_cast < ValueTrame* >(m_stack.top());
    tr->add(value);
}

void VpzStackSax::pop_vletrame()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, dynamic_cast < VLETrame* >(m_stack.top()));
    delete m_stack.top();
    m_stack.pop();
}

void VpzStackSax::push_modeltrame(const AttributeList& att)
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    AssertS(utils::SaxParserError, m_stack.top()->isModelTrame());

    reinterpret_cast < ValueTrame* >(m_stack.top())->add(
        get_attribute < std::string >(att, "name"),
        get_attribute < std::string >(att, "parent"),
        get_attribute < std::string >(att, "port"),
        get_attribute < std::string >(att, "view"));
}

vpz::Base* VpzStackSax::pop()
{
    vpz::Base* top = m_stack.top();
    m_stack.pop();
    return top;
}

const vpz::Base* VpzStackSax::top() const
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    return m_stack.top();
}

vpz::Base* VpzStackSax::top()
{
    AssertS(utils::SaxParserError, not m_stack.empty());
    return m_stack.top();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

VLESaxParser::VLESaxParser(Vpz& vpz) :
    m_vpzstack(vpz),
    m_vpz(vpz),
    m_isValue(false),
    m_isVPZ(false),
    m_isTrame(false),
    m_isEndTrame(false)
{
}

VLESaxParser::~VLESaxParser()
{
}

void VLESaxParser::clear_parser_state()
{
    m_valuestack.clear();
    m_lastCharacters.clear();
    m_isValue = false;
    m_isVPZ = false;
    m_isTrame = false;
    m_isEndTrame = false;
}

void VLESaxParser::on_start_document()
{
    clear_parser_state();
}

void VLESaxParser::on_end_document()
{
    if (m_isVPZ == false) {
        if (not m_vpzstack.trame().empty()) {
            m_isTrame = true;
        } else {
            if (not m_valuestack.get_results().empty()) {
                m_isValue = true;
            }
        }
    }
}

void VLESaxParser::on_start_element(
    const Glib::ustring& name,
    const AttributeList& att)
{
    clearLastCharactersStored();
    if (name == "boolean") {
        m_valuestack.push_boolean();
    } else if (name == "integer") {
        m_valuestack.push_integer();
    } else if (name == "double") {
        m_valuestack.push_double();
    } else if (name == "string") {
        m_valuestack.push_string();
    } else if (name == "set") {
        m_valuestack.push_set();
    } else if (name == "map") {
        m_valuestack.push_map();
    } else if (name == "key") {
        m_valuestack.push_map_key(get_attribute < Glib::ustring >(att, "name"));
    } else if (name == "tuple") {
        m_valuestack.push_tuple();
    } else if (name == "table") {
        m_valuestack.push_table(
            get_attribute < value::TableFactory::index >(att, "width"),
            get_attribute < value::TableFactory::index >(att, "height"));
    } else if (name == "xml") {
        m_valuestack.push_xml();
    } else if (name == "vle_project") {
        AssertS(utils::SaxParserError, not m_isValue and not m_isTrame);
        m_isVPZ = true;
        m_vpzstack.push_vpz(att);
    } else if (name == "structures") {
        m_vpzstack.push_structure();
    } else if (name == "model") {
        m_vpzstack.push_model(att);
    } else if (name == "in" or name == "out" or name == "state" or
               name == "init") {
        m_vpzstack.push_porttype(name);
    } else if (name == "port") {
        m_vpzstack.push_port(att);
    } else if (name == "submodels") {
        m_vpzstack.push_submodels();
    } else if (name == "connections") {
        m_vpzstack.push_connections();
    } else if (name == "connection") {
        m_vpzstack.push_connection(att);
    } else if (name == "origin") {
        m_vpzstack.push_origin(att);
    } else if (name == "destination") {
        m_vpzstack.push_destination(att);
    } else if (name == "dynamics") {
        m_vpzstack.push_dynamics();
    } else if (name == "dynamic") {
        m_vpzstack.push_dynamic(att);
    } else if (name == "experiment") {
        m_vpzstack.push_experiment(att);
    } else if (name == "replicas") {
        m_vpzstack.push_replicas(att);
    } else if (name == "conditions") {
        m_vpzstack.push_conditions();
    } else if (name == "condition") {
        m_vpzstack.push_condition(att);
    } else if (name == "views") {
        m_vpzstack.push_views();
    } else if (name == "outputs") {
        m_vpzstack.push_outputs();
    } else if (name == "output") {
        m_vpzstack.push_output(att);
    } else if (name == "view") {
        m_vpzstack.push_view(att);
    } else if (name == "observables") {
        m_vpzstack.push_observables();
    } else if (name == "observable") {
        m_vpzstack.push_observable(att);
    } else if (name == "attachedview") {
        m_vpzstack.push_attachedview(att);
    } else if (name == "translators") {
        m_vpzstack.push_translators();
    } else if (name == "translator") {
        m_vpzstack.push_translator(att);
    } else if (name == "vle_trame") {
        m_isEndTrame = false;
        m_vpzstack.push_vletrame();
    } else if (name == "trame") {
        m_vpzstack.push_trame(att);
    } else if (name == "modeltrame") {
        m_vpzstack.push_modeltrame(att);
    } else {
        Throw(utils::SaxParserError,
              (boost::format("Unknow element %1%") % name));
    }
}

void VLESaxParser::on_end_element(const Glib::ustring& name)
{
    if (name == "boolean") {
        m_valuestack.push_on_vector_value(
            value::BooleanFactory::create(
                utils::to_boolean(lastCharactersStored())));
    } else if (name == "integer") {
        m_valuestack.push_on_vector_value(
            value::IntegerFactory::create(
                utils::to_int(lastCharactersStored())));
    } else if (name == "double") {
        m_valuestack.push_on_vector_value(
            value::DoubleFactory::create(
                utils::to_double(lastCharactersStored())));
    } else if (name == "string") {
        m_valuestack.push_on_vector_value(
            value::StringFactory::create(
                utils::to_string(lastCharactersStored())));
    } else if (name == "key") {
        // FIXME delete test
    } else if (name == "set" or name == "map") {
        m_valuestack.pop_value();
    } else if (name == "tuple") {
        value::Tuple tuple = value::toTupleValue(m_valuestack.top_value());
        tuple->fill(lastCharactersStored());
        m_valuestack.pop_value();
    } else if (name == "table") {
        value::Table table = value::toTableValue(m_valuestack.top_value());
        table->fill(lastCharactersStored());
        m_valuestack.pop_value();
    } else if (name == "xml") {
        m_valuestack.push_on_vector_value(
            value::XMLFactory::create(m_cdata));
        m_cdata.clear();
    } else if (name == "translator") {
        m_vpzstack.pop_translator(m_cdata);
        m_cdata.clear();
        m_vpzstack.pop();
    } else if (name == "port" and m_vpzstack.top()->isCondition()) {
        value::Set& vals(m_vpzstack.pop_condition_port());
        std::vector < value::Value >& lst(get_values());
        for (std::vector < value::Value >::iterator it =
             lst.begin(); it != lst.end(); ++it) {
            vals->addValue(*it);
        }
        m_valuestack.clear();
    } else if (name == "port" and m_vpzstack.top()->isObservablePort()) {
        m_vpzstack.pop();
    } else if (name == "in" or name == "out" or name == "state" or
               name == "init" or name == "structures" or name == "model" or
               name == "submodels" or name == "vle_project" or
               name == "connections" or name == "conditions" or
               name == "condition" or name == "outputs" or name == "dynamics" or
               name == "views" or name == "observables" or
               name == "observable" or name == "experiment" or
               name == "translators" or name == "vle_project") {
        m_vpzstack.pop();
    } else if (name == "destination") {
        m_vpzstack.build_connection();
    } else if (name == "output") {
        if (not m_cdata.empty()) {
            Output* out;
            out = dynamic_cast < Output* >(m_vpzstack.top());
            if (out) {
                out->setData(m_cdata);
                m_cdata.clear();
            }
        }
        m_vpzstack.pop(); 
    } else if (name == "trame") {
        m_vpzstack.pop_trame();
        if (not m_cdata.empty()) {
            ParameterTrame* pt;
            pt  = dynamic_cast < ParameterTrame* >(m_vpzstack.top());
            if (pt) {
                pt->setData(m_cdata);
                m_cdata.clear();
            }
        }
        m_vpzstack.pop();
    } else if (name == "modeltrame") {
        m_vpzstack.pop_modeltrame(get_value(0));
    } else if (name == "vle_trame") {
        m_vpzstack.pop_vletrame();
        m_isEndTrame = true;
    }
}

void VLESaxParser::on_characters(const Glib::ustring& characters)
{
    addToCharacters(characters);
}

void VLESaxParser::on_comment(const Glib::ustring& /* text */)
{
}

void VLESaxParser::on_warning(const Glib::ustring& text)
{
    TraceAlways(boost::format("XML warning: %1%") % text);
}

void VLESaxParser::on_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
         (boost::format("XML Error: %1%") % text));
}

void VLESaxParser::on_fatal_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
         (boost::format("XML Fatal error: %1%") % text));
}

void VLESaxParser::on_cdata_block(const Glib::ustring& text)
{
    m_cdata.assign(text);
}

void VLESaxParser::on_validity_error(const Glib::ustring& text)
{
    Throw(utils::SaxParserError,
          (boost::format("XML validity error: %1%") % text));
}

void VLESaxParser::on_validity_warning(const Glib::ustring& text)
{
    TraceAlways(boost::format("XML validity warning: %1%") % text);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const std::vector < value::Value >& VLESaxParser::get_values() const
{
    return m_valuestack.get_results();
}

std::vector < value::Value >& VLESaxParser::get_values()
{
    return m_valuestack.get_results();
}

const value::Value& VLESaxParser::get_value(const size_t pos) const
{
    return m_valuestack.get_result(pos);
}

const Glib::ustring& VLESaxParser::lastCharactersStored() const
{
    return m_lastCharacters;
}

void VLESaxParser::clearLastCharactersStored()
{
    m_lastCharacters.clear();
}

void VLESaxParser::addToCharacters(const Glib::ustring& characters)
{
    m_lastCharacters.append(characters);
}

bool exist_attribute(const AttributeList& lst,
                     const Glib::ustring& name)
{
    AttributeList::const_iterator it;
    it = std::find_if(lst.begin(), lst.end(),
                      xmlpp::SaxParser::AttributeHasName(name));
    return it != lst.end();
}

}} // namespace vle vpz
