/*
 * @file vle/vpz/SaxStackVpz.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#include <boost/cstdint.hpp>
#include <boost/cast.hpp>

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

vpz::Vpz* SaxStackVpz::pushVpz(const xmlChar** att)
{
    checkEmptyStack();

    for (int i = 0; att[i] != NULL; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"date") == 0) {
            m_vpz.project().setDate(xmlCharToString(att[i + 1]));
        } else if (xmlStrcmp(att[i], (const xmlChar*)"author") == 0) {
            m_vpz.project().setAuthor(xmlCharToString(att[i + 1]));
        } else if (xmlStrcmp(att[i], (const xmlChar*)"version") == 0) {
            m_vpz.project().setVersion(xmlCharToString(att[i + 1]));
        } else if (xmlStrcmp(att[i], (const xmlChar*)"instance") == 0) {
            m_vpz.project().setInstance(xmlCharToInt(att[i + 1]));
        } else if (xmlStrcmp(att[i], (const xmlChar*)"replica") == 0) {
            m_vpz.project().setReplica(xmlCharToInt(att[i + 1]));
        }
    }

    push(&m_vpz);
    return &m_vpz;
}

void SaxStackVpz::pushStructure()
{
    checkNotEmptyStack();
    checkParentIsVpz();

    push(new vpz::Structures());
}

void SaxStackVpz::pushModel(const xmlChar** att)
{
    checkNotEmptyStack();
    checkParentOfModel();

    graph::CoupledModel* cplparent = 0;

    if (parent()->isSubmodels()) {
        vpz::Base* sub = pop();
        vpz::Model* tmp = static_cast < vpz::Model* >(parent());
        cplparent = static_cast < graph::CoupledModel* >(tmp->model());
        push(sub);
    }

    graph::Model* gmdl = 0;
    const xmlChar* conditions = 0, *dynamics = 0, *observables = 0;
    const xmlChar* x = 0, *y = 0, *width = 0, *height = 0;
    const xmlChar* type = 0, *name = 0;

    for (int i = 0; att[i] != NULL; i+=2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"type") == 0) {
            type = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"conditions") == 0) {
            conditions = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"dynamics") == 0) {
            dynamics = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"observables") == 0) {
            observables = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"x") == 0) {
            x = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"y") == 0) {
            y = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"width") == 0) {
            width = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"height") == 0) {
            height = att[i + 1];
        }
    }

    if (name == 0) {
        throw utils::SaxParserError(_("Attribute name not defined"));
    }

    if (type == 0) {
        throw utils::SaxParserError(fmt(
                _("Attribute type not defined for model '%1%'")) % name);
    }

    if (xmlStrcmp(type, (const xmlChar*)"atomic") == 0) {
        try {
            gmdl = new graph::AtomicModel((const char*)name, cplparent);
        } catch(const utils::DevsGraphError& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Error build atomic model '%1%' with error: %2%")) % name %
                e.what()));
        }

        Class* clsparent(getLastClass());
        if (clsparent == 0) {
            vpz().project().model().atomicModels().add(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(
                    conditions ? xmlCharToString(conditions) : "",
                    dynamics ? xmlCharToString(dynamics) : "",
                    observables ? xmlCharToString(observables) : ""));
        } else {
            clsparent->atomicModels().add(
                reinterpret_cast < graph::Model* >(gmdl),
                AtomicModel(
                    conditions ? xmlCharToString(conditions) : "",
                    dynamics ? xmlCharToString(dynamics) : "",
                    observables ? xmlCharToString(observables) : ""));
        }
    } else if (xmlStrcmp(type, (const xmlChar*)"coupled") == 0) {
        try {
            gmdl = new graph::CoupledModel((const char*)name, cplparent);
        } catch(const utils::DevsGraphError& e) {
            throw(utils::SaxParserError(fmt(_(
                    "Error build coupled model '%1%' with error: %2%")) % name %
                e.what()));
        }
    } else {
        throw(utils::SaxParserError(fmt(
                    _("Unknow model type %1%")) % (const char*)type));
    }

    vpz::Model* mdl = new vpz::Model();
    mdl->setModel(gmdl);
    buildModelGraphics(gmdl,
                       x ? xmlCharToString(x) : "",
                       y ? xmlCharToString(y) : "",
                       width ? xmlCharToString(width) : "",
                       height ? xmlCharToString(height) : "");

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

void SaxStackVpz::pushPort(const xmlChar** att)
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
        std::string name;

        for (int i = 0; att[i] != NULL; i += 2) {
            if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
                name = xmlCharToString(att[i + 1]);
            }
        }

        if (name.empty()) {
            throw utils::SaxParserError(_("Port without name"));
        }

        if (type->isIn()) {
            gmdl->addInputPort(name);
        } else if (type->isOut()) {
            gmdl->addOutputPort(name);
        }
        push(type);
    }
}

void SaxStackVpz::pushPortType(const char* att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isModel());

    vpz::Base* prt = 0;

    if (strcmp(att, "in") == 0) {
        prt = new vpz::In();
    } else if (strcmp(att, "out") == 0) {
        prt = new vpz::Out();
    } else if (strcmp(att, "state") == 0) {
        prt = new vpz::State();
    } else if (strcmp(att, "init") == 0) {
        prt = new vpz::Init();
    } else {
        throw(utils::SaxParserError(fmt(_(
                    "Unknow port type %1%.")) % att));
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

void SaxStackVpz::pushConnection(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isConnections());

    const xmlChar* type = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"type") == 0) {
            type = att[i + 1];
        }
    }

    if (type == 0) {
        throw utils::SaxParserError(
            _("Connection tag does not have an attribue 'type'"));
    }

    vpz::Base* cnt = 0;
    if (xmlStrcmp(type, (const xmlChar*)"internal") == 0) {
        cnt = new vpz::InternalConnection();
    } else if (xmlStrcmp(type, (const xmlChar*)"input") == 0) {
        cnt = new vpz::InputConnection();
    } else if (xmlStrcmp(type, (const xmlChar*)"output") == 0) {
        cnt = new vpz::OutputConnection();
    } else {
        throw utils::SaxParserError(fmt(
                _( "Unknow connection type %1%")) % type);
    }
    push(cnt);
}

void SaxStackVpz::pushOrigin(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isInternalConnection() or
            parent()->isInputConnection() or
            parent()->isOutputConnection());

    const xmlChar* model = 0;
    const xmlChar* port = 0;

    for (int i = 0; att[i] != NULL; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"model") == 0) {
            model = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"port") == 0) {
            port = att[i + 1];
        }
    }

    if (not model or not port) {
        throw utils::SaxParserError(
            _("Origin tag does not have attributes 'model' or 'port'"));
    }

    vpz::Base* orig = new vpz::Origin((const char*)model, (const char*)port);
    push(orig);
}

void SaxStackVpz::pushDestination(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isOrigin());

    const xmlChar* model = 0;
    const xmlChar* port = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"model") == 0) {
            model = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"port") == 0) {
            port = att[i + 1];
        }
    }

    if (not model or not port) {
        throw utils::SaxParserError(
            _("Destination tag does not have attributes 'model' or 'port'"));
    }

    vpz::Base* dest = new vpz::Destination((const char*)model, (const char*)port);
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

void SaxStackVpz::pushDynamic(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isDynamics());

    const xmlChar* name = 0;
    const xmlChar* package = 0;
    const xmlChar* library = 0;
    const xmlChar* model = 0;
    const xmlChar* language = 0;
    const xmlChar* type = 0;
    const xmlChar* location = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"package") == 0) {
            package = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"library") == 0) {
            library = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"model") == 0) {
            model = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"language") == 0) {
            language = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"type") == 0) {
            type = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"location") == 0) {
            location = att[i + 1];
        }
    }

    if (not name or not library) {
        throw utils::SaxParserError(
            _("Dynamic tag does not have 'name' or 'library' attribute"));
    }

    vpz::Dynamic dyn(xmlCharToString(name));
    dyn.setLibrary(xmlCharToString(library));

    if (model) {
        dyn.setModel(xmlCharToString(model));
    } else {
        dyn.setModel("");
    }

    if (package) {
        dyn.setPackage(xmlCharToString(package));
    } else {
        dyn.setPackage("");
    }

    if (language) {
        dyn.setLanguage(xmlCharToString(language));
    } else {
        dyn.setLanguage("");
    }

    if (type) {
        if (xmlStrcmp(type, (xmlChar*)"local") == 0) {
            dyn.setLocalDynamics();
        } else {
            if (location == 0) {
                throw utils::SaxParserError(
                    _("Dynamic tag define distant dynamic without location"));
            }

            std::string loc(xmlCharToString(location));
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

void SaxStackVpz::pushExperiment(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    vpz::Experiment& exp(m_vpz.project().experiment());
    push(&exp);

    const xmlChar* name = 0;
    const xmlChar* duration = 0;
    const xmlChar* seed = 0;
    const xmlChar* begin = 0;
    const xmlChar* combination = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"duration") == 0) {
            duration = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"seed") == 0) {
            seed = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"begin") == 0) {
            begin = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"combination") == 0) {
            combination = att[i + 1];
        }
    }

    if (not name or not duration or not seed) {
        throw utils::SaxParserError(
            _("Experiment tag does not have 'name', 'duration' or 'seed'"
              " attributes"));
    }

    exp.setName(xmlCharToString(name));
    exp.setDuration(xmlCharToDouble(duration));

    {
        unsigned long int t = xmlCharToUnsignedInt(seed);
        try {
            boost::uint32_t res = boost::numeric_cast < boost::uint32_t >(t);
            exp.setSeed(res);
        } catch (const std::exception& e) {
            throw utils::SaxParserError(fmt(
                _("Experiment tag can not uses the seed '%1%'")) % seed);
        }
    }

    if (begin) {
        exp.setBegin(xmlCharToDouble(begin));
    } else {
        exp.setBegin(0.0);
    }

    if (combination) {
        exp.setCombination(xmlCharToString(combination));
    }
}

void SaxStackVpz::pushReplicas(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isExperiment());

    vpz::Replicas& rep(m_vpz.project().experiment().replicas());

    const xmlChar* seed = 0;
    const xmlChar* number = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"seed") == 0) {
            seed = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"number") == 0) {
            number = att[i + 1];
        }
    }

    if (not seed or not number) {
        throw utils::SaxParserError(
            _("Replicas tag does not have 'seed' or 'number' attributes"));
    }

    try {
        if (seed) {
            unsigned long int t = xmlCharToUnsignedInt(seed);
            try {
                boost::uint32_t res = boost::numeric_cast < boost::uint32_t >(t);
                rep.setSeed(res);
            } catch (const std::exception& e) {
                throw utils::SaxParserError(fmt(
                        _("cannot convert '%1%' into a correct integer")) % seed);
            }
        }

        if (number) {
            rep.setNumber(xmlCharToInt(number));
        }
    } catch (const std::exception& e) {
        throw utils::SaxParserError(fmt(
                _("Replicas tag: error converting attribute: %1%")) % e.what());
    }
}

void SaxStackVpz::pushConditions()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isExperiment());

    push(&m_vpz.project().experiment().conditions());
}

void SaxStackVpz::pushCondition(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isConditions());

    vpz::Conditions& cnds(m_vpz.project().experiment().conditions());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Condition tag does not have attribute 'name'"));
    }

    vpz::Condition newcondition(xmlCharToString(name));
    vpz::Condition& cnd(cnds.add(newcondition));
    push(&cnd);
}

void SaxStackVpz::pushConditionPort(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isCondition());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Condition port tag does not have attribute 'name'"));
    }

    vpz::Condition* cnd(static_cast < vpz::Condition* >(parent()));
    cnd->add(xmlCharToString(name));
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

void SaxStackVpz::pushOutput(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isOutputs());

    const xmlChar* name = 0;
    const xmlChar* format = 0;
    const xmlChar* plugin = 0;
    const xmlChar* location = 0;
    const xmlChar* package = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"format") == 0) {
            format = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"plugin") == 0) {
            plugin = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"location") == 0) {
            location = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"package") == 0) {
            package = att[i + 1];
        }
    }

    Outputs& outs(m_vpz.project().experiment().views().outputs());

    if (xmlStrcmp(format, (const xmlChar*)"local") == 0) {
        Output& result = outs.addLocalStream(xmlCharToString(name), location ?
                                             xmlCharToString(location) : "",
                                             xmlCharToString(plugin));
        if (package) {
            result.setPackage(xmlCharToString(package));
        }
        push(&result);
    } else if (xmlStrcmp(format, (const xmlChar*)"distant") == 0) {
        Output& result = outs.addDistantStream(xmlCharToString(name),
                                               location ?
                                               xmlCharToString(location) : "",
                                               xmlCharToString(plugin));
        if (package) {
            result.setPackage(xmlCharToString(package));
        }
        push(&result);
    } else {
        throw utils::SaxParserError(fmt(
                _("Output tag does not define a '%1%' format")) % name);
    }
}

void SaxStackVpz::pushView(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isViews());

    const xmlChar* name = 0;
    const xmlChar* type = 0;
    const xmlChar* output = 0;
    const xmlChar* timestep = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"type") == 0) {
            type = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"output") == 0) {
            output = att[i + 1];
        } else if (xmlStrcmp(att[i], (const xmlChar*)"timestep") == 0) {
            timestep = att[i + 1];
        }
    }

    Views& views(m_vpz.project().experiment().views());

    if (xmlStrcmp(type, (const xmlChar*)"timed") == 0) {
        if (not timestep) {
            throw utils::SaxParserError(
                _("View tag does not have a timestep attribute"));
        }
        views.addTimedView(xmlCharToString(name),
                           xmlCharToDouble(timestep),
                           xmlCharToString(output));
    } else if (xmlStrcmp(type, (const xmlChar*)"event") == 0) {
        views.addEventView(xmlCharToString(name), xmlCharToString(output));
    } else if (xmlStrcmp(type, (const xmlChar*)"finish") == 0) {
        views.addFinishView(xmlCharToString(name), xmlCharToString(output));
    } else {
        throw utils::SaxParserError(fmt(
                _("View tag does not accept type '%1%'")) % type);
    }
}

void SaxStackVpz::pushAttachedView(const xmlChar** att)
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

void SaxStackVpz::pushObservable(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservables());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Observable tag does not have attribute 'name'"));
    }

    Views& views(m_vpz.project().experiment().views());
    Observable& obs(views.addObservable(xmlCharToString(name)));
    push(&obs);
}

void SaxStackVpz::pushObservablePort(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservable());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Observable port tag does not have attribute 'name'"));
    }

    vpz::Observable* out(static_cast < vpz::Observable* >(parent()));
    vpz::ObservablePort& ports(out->add(xmlCharToString(name)));
    push(&ports);
}

void SaxStackVpz::pushObservablePortOnView(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isObservablePort());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Observable port view tag does not have attribute 'name'"));
    }

    vpz::ObservablePort* port(static_cast < vpz::ObservablePort* >(parent()));
    port->add(xmlCharToString(name));
}

void SaxStackVpz::pushClasses()
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isVpz());

    push(&m_vpz.project().classes());
}

void SaxStackVpz::pushClass(const xmlChar** att)
{
    Assert < utils::SaxParserError >(not m_stack.empty());
    Assert < utils::SaxParserError >(parent()->isClasses());

    const xmlChar* name = 0;

    for (int i = 0; att[i] != 0; i += 2) {
        if (xmlStrcmp(att[i], (const xmlChar*)"name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
            _("Class tag does not have attribute 'name'"));
    }

    Class& cls = m_vpz.project().classes().add(xmlCharToString(name));
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

void SaxStackVpz::checkParentIsVpz() const
{
    if (not parent()->isVpz()) {
        throw utils::SaxParserError(
            _("Parent is not Vpz element"));
    }
}

void SaxStackVpz::checkParentIsClass() const
{
    if (not parent()->isClass()) {
        throw utils::SaxParserError(
            _("Parent is not Class element"));
    }
}

void SaxStackVpz::checkParentIsSubmodels() const
{
    if (not parent()->isSubmodels()) {
        throw utils::SaxParserError(
            _("Parent is not Submodels element"));
    }
}

void SaxStackVpz::checkParentIsStructures() const
{
    if (not parent()->isStructures()) {
        throw utils::SaxParserError(
            _("Parent is not Structures element"));
    }
}

void SaxStackVpz::checkParentOfModel() const
{
    if (not parent()->isClass() and not parent()->isSubmodels() and not
        parent()->isStructures()) {
        throw utils::SaxParserError(
            _("Parent of model is not class, submodel or structure"));
    }
}

}} // namespace vle vpz
