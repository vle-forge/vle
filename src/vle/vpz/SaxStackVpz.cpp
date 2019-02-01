/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Port.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/vpz/Structures.hpp>
#include <vle/vpz/Vpz.hpp>

#include <algorithm>

#include <cstring>

namespace {

template<typename T>
inline void
checkEmptyStack(const T& t)
{
    if (not t.empty())
        throw vle::utils::SaxParserError(_("Not empty vpz stack"));
}

template<typename T>
inline void
checkNotEmptyStack(const T& t)
{
    if (t.empty())
        throw vle::utils::SaxParserError(_("Empty vpz stack"));
}
}

namespace vle {
namespace vpz {

std::ostream&
operator<<(std::ostream& out, const SaxStackVpz& stack)
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

SaxStackVpz::~SaxStackVpz()
{
    clear();
}

void
SaxStackVpz::clear()
{
    while (not m_stack.empty()) {
        if (parent()->isStructures() or parent()->isModel() or
            parent()->isIn() or parent()->isOut() or parent()->isState() or
            parent()->isInit() or parent()->isSubmodels() or
            parent()->isConnections() or parent()->isInternalConnection() or
            parent()->isInputConnection() or parent()->isOutputConnection() or
            parent()->isOrigin() or parent()->isDestination()) {
            delete parent();
        }
        pop();
    }
}

vpz::Vpz*
SaxStackVpz::pushVpz(const char** att)
{
    ::checkEmptyStack(m_stack);

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "date") == 0) {
            m_vpz.project().setDate(att[i + 1]);
        } else if (strcmp(att[i], "author") == 0) {
            m_vpz.project().setAuthor(att[i + 1]);
        } else if (strcmp(att[i], "version") == 0) {
            m_vpz.project().setVersion(att[i + 1]);
        } else if (strcmp(att[i], "instance") == 0) {
            m_vpz.project().setInstance(charToInt(att[i + 1]));
        }
    }

    push(&m_vpz);
    return &m_vpz;
}

void
SaxStackVpz::pushStructure()
{
    ::checkNotEmptyStack(m_stack);
    checkParentIsVpz();

    push(new vpz::Structures());
}

void
SaxStackVpz::pushModel(const char** att)
{
    ::checkNotEmptyStack(m_stack);
    checkParentOfModel();

    vpz::CoupledModel* cplparent = nullptr;

    if (parent()->isSubmodels()) {
        vpz::Base* sub = pop();
        auto* tmp = static_cast<vpz::Model*>(parent());
        cplparent = tmp->node()->toCoupled();
        push(sub);
    }

    vpz::BaseModel* gmdl = nullptr;
    const char *conditions = nullptr, *dynamics = nullptr;
    const char *observables = nullptr, *width = nullptr, *height = nullptr;
    const char *x = nullptr, *y = nullptr;
    const char *type = nullptr, *name = nullptr;
    const char* debug = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "type") == 0) {
            type = att[i + 1];
        } else if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        } else if (strcmp(att[i], "conditions") == 0) {
            conditions = att[i + 1];
        } else if (strcmp(att[i], "dynamics") == 0) {
            dynamics = att[i + 1];
        } else if (strcmp(att[i], "observables") == 0) {
            observables = att[i + 1];
        } else if (strcmp(att[i], "x") == 0) {
            x = att[i + 1];
        } else if (strcmp(att[i], "y") == 0) {
            y = att[i + 1];
        } else if (strcmp(att[i], "width") == 0) {
            width = att[i + 1];
        } else if (strcmp(att[i], "height") == 0) {
            height = att[i + 1];
        } else if (strcmp(att[i], "debug") == 0) {
            debug = att[i + 1];
        }
    }

    if (name == nullptr) {
        throw utils::SaxParserError(_("Attribute name not defined"));
    }

    if (type == nullptr) {
        throw utils::SaxParserError(
          _("Attribute type not defined for model '%s'"), name);
    }

    if (strcmp(type, "atomic") == 0) {
        try {
            auto ptr = new vpz::AtomicModel(name,
                                            cplparent,
                                            conditions ? conditions : "",
                                            dynamics ? dynamics : "",
                                            observables ? observables : "");

            if (debug and (strcmp(debug, "true") == 0))
                ptr->setDebug();

            gmdl = ptr;
        } catch (const utils::DevsGraphError& e) {
            throw utils::SaxParserError(
              _("Error build atomic model '%s' with error: %s"),
              name,
              e.what());
        }
    } else if (strcmp(type, "coupled") == 0) {
        try {
            gmdl = new vpz::CoupledModel(name, cplparent);
        } catch (const utils::DevsGraphError& e) {
            throw utils::SaxParserError(
              _("Error build coupled model '%s' with error: %s"),
              name,
              e.what());
        }
    } else {
        throw utils::SaxParserError(_("Unknow model type %s"), type);
    }

    buildModelGraphics(
      gmdl, x ? x : "", y ? y : "", width ? width : "", height ? height : "");

    if (parent()->isStructures()) {
        vpz().project().model().setGraph(std::unique_ptr<BaseModel>(gmdl));
    } else if (parent()->isClass()) {
        reinterpret_cast<Class*>(parent())->setGraph(
          std::unique_ptr<BaseModel>(gmdl));
    }

    auto mdl = new vpz::Model();
    mdl->setNode(gmdl);

    push(mdl);
}

void
SaxStackVpz::buildModelGraphics(vpz::BaseModel* mdl,
                                const std::string& x,
                                const std::string& y,
                                const std::string& width,
                                const std::string& height)
{
    if (not x.empty() and not y.empty()) {
        try {
            mdl->setX(std::stoi(x));
            mdl->setY(std::stoi(y));
        } catch (const std::exception& /* e */) {
            throw utils::SaxParserError(
              _("Cannot convert x or y position for model %s"),
              mdl->getName().c_str());
        }
    }

    if (not width.empty() and not height.empty()) {
        try {
            mdl->setWidth(std::stoi(width));
            mdl->setHeight(std::stoi(height));
        } catch (const std::exception& /* e */) {
            throw utils::SaxParserError(
              _("Cannot convert width or height for model %s"),
              mdl->getName().c_str());
        }
    }
}

void
SaxStackVpz::pushPort(const char** att)
{
    if (m_stack.empty()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    if (parent()->isCondition()) {
        pushConditionPort(att);
    } else if (parent()->isObservable()) {
        pushObservablePort(att);
    } else {
        if (not(parent()->isIn() or parent()->isOut())) {
            throw utils::SaxParserError(_("Bad file format"));
        }

        vpz::Base* type = pop();

        if (not parent()->isModel()) {
            throw utils::SaxParserError(_("Bad file format"));
        }

        auto* mdl = static_cast<vpz::Model*>(parent());
        vpz::BaseModel* gmdl = nullptr;

        if (mdl->node())
            gmdl = mdl->node();

        std::string name;

        for (int i = 0; att[i] != nullptr; i += 2) {
            if (strcmp(att[i], "name") == 0) {
                name = att[i + 1];
            }
        }

        if (name.empty()) {
            throw utils::SaxParserError(_("Port without name"));
        }

        if (gmdl) {
            if (type->isIn()) {
                gmdl->addInputPort(name);
            } else if (type->isOut()) {
                gmdl->addOutputPort(name);
            }
        }
        push(type);
    }
}

void
SaxStackVpz::pushPortType(const char* att)
{
    if (m_stack.empty() or not parent()->isModel()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    vpz::Base* prt = nullptr;

    if (std::strcmp(att, "in") == 0) {
        prt = new vpz::In();
    } else if (std::strcmp(att, "out") == 0) {
        prt = new vpz::Out();
    } else if (std::strcmp(att, "state") == 0) {
        prt = new vpz::State();
    } else if (std::strcmp(att, "init") == 0) {
        prt = new vpz::Init();
    } else {
        throw utils::SaxParserError(_("Unknow port type %s."), att);
    }
    push(prt);
}

void
SaxStackVpz::pushSubModels()
{
    if (m_stack.empty() or not parent()->isModel()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto sub = new vpz::Submodels();
    push(sub);
}

void
SaxStackVpz::pushConnections()
{
    if (m_stack.empty() or not parent()->isModel()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto cnts = new vpz::Connections();
    push(cnts);
}

void
SaxStackVpz::pushConnection(const char** att)
{
    if (m_stack.empty() or not parent()->isConnections()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* type = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "type") == 0) {
            type = att[i + 1];
        }
    }

    if (type == nullptr) {
        throw utils::SaxParserError(
          _("Connection tag does not have an attribue 'type'"));
    }

    vpz::Base* cnt = nullptr;
    if (strcmp(type, "internal") == 0) {
        cnt = new vpz::InternalConnection();
    } else if (strcmp(type, "input") == 0) {
        cnt = new vpz::InputConnection();
    } else if (strcmp(type, "output") == 0) {
        cnt = new vpz::OutputConnection();
    } else {
        throw utils::SaxParserError(_("Unknow connection type %s"), type);
    }
    push(cnt);
}

void
SaxStackVpz::pushOrigin(const char** att)
{
    if (m_stack.empty() or (not parent()->isInternalConnection() and
                            not parent()->isInputConnection() and
                            not parent()->isOutputConnection())) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* model = nullptr;
    const char* port = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "model") == 0) {
            model = att[i + 1];
        } else if (strcmp(att[i], "port") == 0) {
            port = att[i + 1];
        }
    }

    if (not model or not port) {
        throw utils::SaxParserError(
          _("Origin tag does not have attributes 'model' or 'port'"));
    }

    vpz::Base* orig = new vpz::Origin(model, port);
    push(orig);
}

void
SaxStackVpz::pushDestination(const char** att)
{
    if (m_stack.empty() or not parent()->isOrigin()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* model = nullptr;
    const char* port = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "model") == 0) {
            model = att[i + 1];
        } else if (strcmp(att[i], "port") == 0) {
            port = att[i + 1];
        }
    }

    if (not model or not port) {
        throw utils::SaxParserError(
          _("Destination tag does not have attributes 'model' or 'port'"));
    }

    vpz::Base* dest = new vpz::Destination(model, port);
    push(dest);
}

void
SaxStackVpz::buildConnection()
{
    if (m_stack.empty()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    if (not parent()->isDestination()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto* dest = static_cast<vpz::Destination*>(pop());

    if (not parent()->isOrigin()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto* orig = static_cast<vpz::Origin*>(pop());

    if (not(parent()->isInternalConnection() or
            parent()->isInputConnection() or parent()->isOutputConnection())) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    vpz::Base* cnt = pop();

    if (not parent()->isConnections()) {
        throw utils::SaxParserError(_("Bad file format"));
    }
    vpz::Base* cntx = pop();

    if (not parent()->isModel()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto* model = static_cast<vpz::Model*>(parent());
    vpz::CoupledModel* cpl = model->node()->toCoupled();

    if (cnt->isInternalConnection()) {
        cpl->addInternalConnection(
          orig->model, orig->port, dest->model, dest->port);
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

void
SaxStackVpz::pushDynamics()
{
    if (m_stack.empty() or not parent()->isVpz()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    push(&m_vpz.project().dynamics());
}

void
SaxStackVpz::pushDynamic(const char** att)
{
    if (m_stack.empty() or not parent()->isDynamics()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;
    const char* package = nullptr;
    const char* library = nullptr;
    const char* language = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        } else if (strcmp(att[i], "package") == 0) {
            package = att[i + 1];
        } else if (strcmp(att[i], "library") == 0) {
            library = att[i + 1];
        } else if (strcmp(att[i], "language") == 0) {
            language = att[i + 1];
        }
    }

    if (not name or not library) {
        throw utils::SaxParserError(
          _("Dynamic tag does not have 'name' or 'library' attribute"));
    }

    vpz::Dynamic dyn(name);
    dyn.setLibrary(library);

    if (package) {
        dyn.setPackage(package);
    } else {
        dyn.setPackage("");
    }

    if (language) {
        dyn.setLanguage(language);
    } else {
        dyn.setLanguage("");
    }

    auto* dyns(static_cast<Dynamics*>(parent()));
    dyns->add(dyn);
}

void
SaxStackVpz::pushExperiment(const char** att)
{
    if (m_stack.empty() or not parent()->isVpz()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    vpz::Experiment& exp(m_vpz.project().experiment());
    push(&exp);

    const char* name = nullptr;
    const char* combination = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        } else if (strcmp(att[i], "combination") == 0) {
            combination = att[i + 1];
        }
    }

    exp.setName(name);

    if (combination) {
        exp.setCombination(combination);
    }
}

void
SaxStackVpz::pushConditions()
{
    if (m_stack.empty() or not parent()->isExperiment()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    push(&m_vpz.project().experiment().conditions());
}

void
SaxStackVpz::pushCondition(const char** att)
{
    if (m_stack.empty() or not parent()->isConditions()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    vpz::Conditions& cnds(m_vpz.project().experiment().conditions());

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Condition tag does not have attribute 'name'"));
    }

    vpz::Condition newcondition(name);
    vpz::Condition& cnd(cnds.add(newcondition));
    push(&cnd);
}

void
SaxStackVpz::pushConditionPort(const char** att)
{
    if (m_stack.empty() or not parent()->isCondition()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Condition port tag does not have attribute 'name'"));
    }

    auto* cnd(static_cast<vpz::Condition*>(parent()));
    cnd->add(name);
}

std::vector<std::shared_ptr<value::Value>>&
SaxStackVpz::popConditionPort()
{
    if (m_stack.empty() or not parent()->isCondition()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    auto* cnd(static_cast<vpz::Condition*>(parent()));

    return cnd->lastAddedPort();
}

void
SaxStackVpz::pushViews()
{
    if (m_stack.empty() or not parent()->isExperiment()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    push(&m_vpz.project().experiment().views());
}

void
SaxStackVpz::pushOutputs()
{
    if (m_stack.empty() or not parent()->isViews()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    push(&m_vpz.project().experiment().views().outputs());
}

void
SaxStackVpz::popOutput()
{
    if (m_stack.empty() or not parent()->isOutput()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    pop();
}

void
SaxStackVpz::pushOutput(const char** att)
{
    if (m_stack.empty() or not parent()->isOutputs()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;
    const char* plugin = nullptr;
    const char* location = nullptr;
    const char* package = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        } else if (strcmp(att[i], "plugin") == 0) {
            plugin = att[i + 1];
        } else if (strcmp(att[i], "location") == 0) {
            location = att[i + 1];
        } else if (strcmp(att[i], "package") == 0) {
            package = att[i + 1];
        }
    }

    Outputs& outs(m_vpz.project().experiment().views().outputs());

    Output& result = outs.addStream(name,
                                    location ? location : std::string(),
                                    plugin,
                                    package ? package : std::string());

    push(&result);
}

void
SaxStackVpz::pushView(const char** att)
{
    if (m_stack.empty() or not parent()->isViews()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;
    const char* type = nullptr;
    const char* output = nullptr;
    const char* timestep = nullptr;
    const char* enable = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        } else if (strcmp(att[i], "type") == 0) {
            type = att[i + 1];
        } else if (strcmp(att[i], "output") == 0) {
            output = att[i + 1];
        } else if (strcmp(att[i], "timestep") == 0) {
            timestep = att[i + 1];
        } else if (strcmp(att[i], "enable") == 0) {
            enable = att[i + 1];
        }
    }

    bool enable_b = true;
    if (enable and strcmp(enable, "false") == 0) {
        enable_b = false;
    }
    Views& views(m_vpz.project().experiment().views());

    if (strcmp(type, "timed") == 0) {
        if (not timestep) {
            throw utils::SaxParserError(
              _("View tag does not have a timestep attribute"));
        }
        views.addTimedView(name, charToDouble(timestep), output, enable_b);
    } else {
        std::string typestr(type);
        std::string::size_type begin{ 0 };
        View::Type viewtype = View::NOTHING;

        while (begin != std::string::npos) {
            auto it = typestr.find(',', begin);
            auto tmp = typestr.substr(begin, it - begin);

            if (tmp == "event")
                viewtype = View::INTERNAL | View::CONFLUENT | View::EXTERNAL;
            else if (tmp == "output")
                viewtype |= View::OUTPUT;
            else if (tmp == "internal")
                viewtype |= View::INTERNAL;
            else if (tmp == "external")
                viewtype |= View::EXTERNAL;
            else if (tmp == "confluent")
                viewtype |= View::CONFLUENT;
            else if (tmp == "finish")
                viewtype |= View::FINISH;
            else
                throw utils::SaxParserError(
                  _("View tag does not accept type '%s'"), tmp.c_str());

            if (it != std::string::npos)
                begin = std::min(it + 1, typestr.size());
            else
                begin = std::string::npos;
        }

        views.addEventView(
          name, static_cast<View::Type>(viewtype), output, enable_b);
    }
}

void
SaxStackVpz::pushAttachedView(const char** att)
{
    if (m_stack.empty() or not parent()->isObservablePort()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    pushObservablePortOnView(att);
}

void
SaxStackVpz::pushObservables()
{
    if (m_stack.empty() or not parent()->isViews()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    Observables& obs(m_vpz.project().experiment().views().observables());
    push(&obs);
}

void
SaxStackVpz::pushObservable(const char** att)
{
    if (m_stack.empty() or not parent()->isObservables()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Observable tag does not have attribute 'name'"));
    }

    Views& views(m_vpz.project().experiment().views());
    Observable& obs(views.addObservable(name));
    push(&obs);
}

void
SaxStackVpz::pushObservablePort(const char** att)
{
    if (m_stack.empty() or not parent()->isObservable()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Observable port tag does not have attribute 'name'"));
    }

    auto* out(static_cast<vpz::Observable*>(parent()));
    vpz::ObservablePort& ports(out->add(name));
    push(&ports);
}

void
SaxStackVpz::pushObservablePortOnView(const char** att)
{
    if (m_stack.empty() or not parent()->isObservablePort()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Observable port view tag does not have attribute 'name'"));
    }

    auto* port(static_cast<vpz::ObservablePort*>(parent()));
    port->add(name);
}

void
SaxStackVpz::pushClasses()
{
    if (m_stack.empty() or not parent()->isVpz()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    push(&m_vpz.project().classes());
}

void
SaxStackVpz::pushClass(const char** att)
{
    if (m_stack.empty() or not parent()->isClasses()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    const char* name = nullptr;

    for (int i = 0; att[i] != nullptr; i += 2) {
        if (strcmp(att[i], "name") == 0) {
            name = att[i + 1];
        }
    }

    if (not name) {
        throw utils::SaxParserError(
          _("Class tag does not have attribute 'name'"));
    }

    Class& cls = m_vpz.project().classes().add(name);
    push(&cls);
}

void
SaxStackVpz::popClasses()
{
    if (m_stack.empty() or not parent()->isClasses()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    pop();
}

void
SaxStackVpz::popClass()
{
    if (m_stack.empty() or not parent()->isClass()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    pop();
}

vpz::Base*
SaxStackVpz::pop()
{
    vpz::Base* top = parent();
    m_stack.pop_front();
    return top;
}

const vpz::Base*
SaxStackVpz::top() const
{
    if (m_stack.empty()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    return parent();
}

vpz::Base*
SaxStackVpz::top()
{
    if (m_stack.empty()) {
        throw utils::SaxParserError(_("Bad file format"));
    }

    return parent();
}

Class*
SaxStackVpz::getLastClass() const
{
    std::list<vpz::Base*>::const_iterator it;

    it = std::find_if(m_stack.begin(), m_stack.end(), Base::IsClass());
    return (it != m_stack.end()) ? reinterpret_cast<Class*>(*it) : nullptr;
}

void
SaxStackVpz::checkParentIsVpz() const
{
    if (not parent()->isVpz()) {
        throw utils::SaxParserError(_("Parent is not Vpz element"));
    }
}

void
SaxStackVpz::checkParentIsClass() const
{
    if (not parent()->isClass()) {
        throw utils::SaxParserError(_("Parent is not Class element"));
    }
}

void
SaxStackVpz::checkParentIsSubmodels() const
{
    if (not parent()->isSubmodels()) {
        throw utils::SaxParserError(_("Parent is not Submodels element"));
    }
}

void
SaxStackVpz::checkParentIsStructures() const
{
    if (not parent()->isStructures()) {
        throw utils::SaxParserError(_("Parent is not Structures element"));
    }
}

void
SaxStackVpz::checkParentOfModel() const
{
    if (not parent()->isClass() and not parent()->isSubmodels() and
        not parent()->isStructures()) {
        throw utils::SaxParserError(
          _("Parent of model is not class, submodel or structure"));
    }
}
}
} // namespace vle vpz
