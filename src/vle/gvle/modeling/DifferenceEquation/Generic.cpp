/**
 * @file vle/gvle/modeling/DifferenceEquation/Generic.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/gvle/modeling/DifferenceEquation/Generic.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle { namespace modeling {

Generic::Generic(const std::string& name) :
    Plugin(name), m_dialog(0), m_buttonSource(0)
{}

Generic::~Generic()
{
    Gtk::VBox* vbox;

    mXml->get_widget("GenericPluginVBox", vbox);
    vbox->remove(*m_buttonSource);

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void Generic::build()
{
    Gtk::VBox* vbox;
    std::string glade = utils::Path::path().
        getModelingGladeFile("DifferenceEquation.glade");

    mXml = Gnome::Glade::Xml::create(glade);
    mXml->get_widget("DialogPluginGenericBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Generic");
    mXml->get_widget("GenericPluginVBox", vbox);

    vbox->pack_start(NameValue::build(mXml));
    vbox->pack_start(TimeStep::build(mXml));
    vbox->pack_start(Parameters::buildParameters(mXml));

    {
        m_buttonSource = Gtk::manage(
            new Gtk::Button("Compute / InitValue / User section"));
        m_buttonSource->show();
        vbox->pack_start(*m_buttonSource);
        mList.push_back(m_buttonSource->signal_clicked().connect(
                            sigc::mem_fun(*this, &Plugin::onSource)));
    }
}

bool Generic::create(graph::AtomicModel& atom,
                    vpz::AtomicModel& model,
                    vpz::Dynamic& dynamic,
                    vpz::Conditions& conditions,
                    vpz::Observables& /*observables*/,
                    const std::string& classname,
                    const std::string& namespace_)
{
    std::string conditionName((fmt("cond_DE_%1%") % atom.getName()).str());

    build();

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);

        Generic::fillFields(condition);
    } else {
        Generic::fillFields(conditions.get(conditionName));
    }
    mComputeFunction =
        "virtual double compute(const vd::Time& /*time*/)\n"        \
        "{ return 0; }\n";
    mInitValueFunction =
        "virtual double initValue(const vd::Time& /*time*/)\n"      \
        "{ return 0; }\n";
    mUserFunctions = "";

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(atom, model, dynamic, conditions, classname, namespace_, true);
        m_dialog->hide_all();
        return true;
    }
    m_dialog->hide_all();
    return false;
}

void Generic::fillFields(const vpz::Condition& condition)
{
    NameValue::fillFields(condition);
    TimeStep::fillFields(condition);
}

void Generic::generateCondition(graph::AtomicModel& atom,
                               vpz::AtomicModel& model,
                               vpz::Conditions& conditions)
{
    std::string conditionName((fmt("cond_DE_%1%") % atom.getName()).str());
    if (conditions.exist(conditionName)) {
        vpz::Condition& condition(conditions.get(conditionName));

        NameValue::deletePorts(condition);
        TimeStep::deletePorts(condition);
        Parameters::deletePorts(condition);

        NameValue::assign(condition);
        TimeStep::assign(condition);
        Parameters::assign(condition);
    } else {
        vpz::Condition condition(conditionName);

        NameValue::assign(condition);
        TimeStep::assign(condition);
        Parameters::assign(condition);
        conditions.add(condition);
    }

    vpz::Strings cond(model.conditions());
    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }
}

void Generic::generateOutputPorts(graph::AtomicModel& atom)
{
    if (not atom.existOutputPort("update")) {
        atom.addOutputPort("update");
    }
}

void Generic::generateVariables(utils::Template& tpl_)
{
    tpl_.stringSymbol().append("varname", getVariableName());
}

std::string Generic::getTemplate() const
{
    return
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag DifferenceEquationGeneric@@"                              \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};par:"                                          \
    "{{for i in par}}"                                                  \
    "{{par^i}},{{val^i}}|"                                              \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/DifferenceEquation.hpp>\n\n"               \
    "namespace vd = vle::devs;\n"                                       \
    "namespace ve = vle::extension;\n"                                  \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public ve::DifferenceEquation::Generic\n"     \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& atom,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : ve::DifferenceEquation::Generic(atom, evts)\n"            \
    "    {\n"                                                           \
    "{{for i in par}}"                                                  \
    "        {{par^i}} = vv::toDouble(evts.get(\"{{par^i}}\"));\n"      \
    "{{end for}}"                                                       \
    "        {{varname}} = createVar(\"{{varname}}\");\n"               \
    "    }\n"                                                           \
    "\n"                                                                \
    "    virtual ~{{classname}}()\n"                                    \
    "    {}\n"                                                          \
    "\n"                                                                \
    "//@@begin:compute@@\n"                                             \
    "{{compute}}"                                                       \
    "//@@end:compute@@\n\n"                                             \
    "//@@begin:initValue@@\n"                                           \
    "{{initValue}}"                                                     \
    "//@@end:initValue@@\n\n"                                             \
    "private:\n"                                                        \
    "//@@begin:user@@\n"                                                \
    "{{userFunctions}}"                                                 \
    "//@@end:user@@\n\n"                                                \
    "{{for i in par}}"                                                  \
    "    double {{par^i}};\n"                                           \
    "{{end for}}"                                                       \
    "    Var {{varname}};\n"                                            \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";
}

bool Generic::modify(graph::AtomicModel& atom,
                    vpz::AtomicModel& model,
                    vpz::Dynamic& dynamic,
                    vpz::Conditions& conditions,
                    vpz::Observables& /*observables*/,
                    const std::string& conf,
                    const std::string& buffer)
{
    std::string namespace_;
    std::string classname;
    Parameters::Parameters_t parameters;
    Parameters::ExternalVariables_t externalVariables;

    parseConf(conf, classname, namespace_, parameters, externalVariables);
    parseFunctions(buffer);
    std::string conditionName((fmt("cond_DE_%1%") % atom.getName()).str());

    build();

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);

        Generic::fillFields(condition);
	Parameters::fillFields(parameters);
    } else {
        Generic::fillFields(conditions.get(conditionName));
	Parameters::fillFields(parameters);
    }

    backup();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(atom, model, dynamic, conditions, classname, namespace_, true);
        m_dialog->hide_all();
        return true;
    }
    m_dialog->hide_all();
    return false;
}

}}} // namespace vle gvle modeling

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::Generic)

