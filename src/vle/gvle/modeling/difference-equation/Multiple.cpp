/**
 * @file vle/gvle/modeling/difference-equation/Multiple.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/modeling/difference-equation/Multiple.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle { namespace modeling {

Multiple::Multiple(const std::string& name) :
    Plugin(name), m_dialog(0), m_buttonSource(0)
{}

Multiple::~Multiple()
{
    if (m_buttonSource) {
        Gtk::VBox* vbox;

        mXml->get_widget("MultiplePluginVBox", vbox);
        vbox->remove(*m_buttonSource);
    }
    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void Multiple::build(bool modeling)
{
    Gtk::VBox* vbox;
    std::string glade = utils::Path::path().
        getModelingGladeFile("DifferenceEquation.glade");

    mXml = Gnome::Glade::Xml::create(glade);
    mXml->get_widget("DialogPluginMultipleBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Multiple");
    mXml->get_widget("MultiplePluginVBox", vbox);

    vbox->pack_start(Variables::build(mXml));
    vbox->pack_start(TimeStep::build(mXml));
    if (modeling) {
        vbox->pack_start(Parameters::build(mXml));

        {
            m_buttonSource = Gtk::manage(
                new Gtk::Button("Compute / InitValue / User section"));
            m_buttonSource->show();
            vbox->pack_start(*m_buttonSource);
            mList.push_back(m_buttonSource->signal_clicked().connect(
                                sigc::mem_fun(*this, &Plugin::onSource)));
        }
    }
    vbox->pack_start(Mapping::build(mXml));
}

bool Multiple::create(graph::AtomicModel& atom,
                    vpz::AtomicModel& model,
                    vpz::Dynamic& dynamic,
                    vpz::Conditions& conditions,
                    vpz::Observables& /*observables*/,
                    const std::string& classname,
                    const std::string& namespace_)
{
    std::string conditionName((fmt("cond_DE_%1%") % atom.getName()).str());

    build(true);

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);
        Multiple::fillFields(condition);
    } else {
        Multiple::fillFields(conditions.get(conditionName));
    }

    mComputeFunction =
        "virtual void compute(const vd::Time& /*time*/)\n"        \
        "{ }\n";
    mInitValueFunction =
        "virtual void initValue(const vd::Time& /*time*/)\n"      \
        "{ }\n";
    mUserFunctions = "";
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(atom, model, dynamic, conditions, classname, namespace_);
        m_dialog->hide_all();
        return true;
    }
    m_dialog->hide_all();
    return false;
}

void Multiple::fillFields(const vpz::Condition& condition)
{
    Variables::fillFields(condition);
    Mapping::fillFields(condition);
}

void Multiple::generateCondition(graph::AtomicModel& atom,
                               vpz::AtomicModel& model,
                               vpz::Conditions& conditions)
{
    std::string conditionName((fmt("cond_DE_%1%") % atom.getName()).str());
    if (conditions.exist(conditionName)) {
        vpz::Condition& condition(conditions.get(conditionName));

	TimeStep::deletePorts(condition);
        Variables::deletePorts(condition);
        Mapping::deletePorts(condition);
        Parameters::deletePorts(condition);

	TimeStep::assign(condition);
        Variables::assign(condition);
        Mapping::assign(condition);
	Parameters::assign(condition);
    } else {
        vpz::Condition condition(conditionName);

	TimeStep::assign(condition);
	Variables::assign(condition);
        Mapping::assign(condition);
	Parameters::assign(condition);
        conditions.add(condition);
    }

    vpz::Strings cond(model.conditions());
    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }
}

void Multiple::generateOutputPorts(graph::AtomicModel& atom)
{
    Variables::Variables_t variables = getVariables();

    for (Variables::Variables_t::const_iterator it = variables.begin();
         it != variables.end(); ++it) {
        if (not atom.existOutputPort(*it)) {
            atom.addOutputPort(*it);
        }
    }
}

void Multiple::generateVariables(utils::Template& tpl_)
{
    tpl_.listSymbol().append("var");

    Variables::Variables_t variables = getVariables();

    for (Variables::Variables_t::const_iterator it = variables.begin();
         it != variables.end(); ++it) {
        tpl_.listSymbol().append("var", *it);
    }
}

std::string Multiple::getTemplate() const
{
    return
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag DifferenceEquationMultiple@@"                              \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};par:"                                          \
    "{{for i in par}}"                                                  \
    "{{par^i}},{{val^i}}|"                                              \
    "{{end for}}"                                                       \
    ";sync:"                                                            \
    "{{for i in sync}}"                                                 \
    "{{sync^i}}|"                                                       \
    "{{end for}}"                                                       \
    ";nosync:"                                                          \
    "{{for i in nosync}}"                                               \
    "{{nosync^i}}|"                                                     \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/DifferenceEquation.hpp>\n\n"               \
    "namespace vd = vle::devs;\n"                                       \
    "namespace ve = vle::extension;\n"                                  \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public ve::DifferenceEquation::Multiple\n"   \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& atom,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : ve::DifferenceEquation::Multiple(atom, evts)\n"          \
    "    {\n"                                                           \
    "{{for i in par}}"                                                  \
    "        {{par^i}} = vv::toDouble(evts.get(\"{{par^i}}\"));\n"      \
    "{{end for}}"                                                       \
    "{{for i in var}}"                                                  \
    "        {{var^i}} = createVar(\"{{var^i}}\");\n"                   \
    "{{end for}}"                                                       \
    "{{for i in sync}}"                                                 \
    "        {{sync^i}} = createSync(\"{{sync^i}}\");\n"                \
    "{{end for}}"                                                       \
    "{{for i in nosync}}"                                               \
    "        {{nosync^i}} = createNosync(\"{{nosync^i}}\");\n"          \
    "{{end for}}"                                                       \
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
    "//@@end:initValue@@\n\n"                                           \
    "private:\n"                                                        \
    "//@@begin:user@@\n"                                                \
    "{{userFunctions}}"                                                 \
    "//@@end:user@@\n\n"                                                \
    "{{for i in par}}"                                                  \
    "    double {{par^i}};\n"                                           \
    "{{end for}}"                                                       \
    "{{for i in var}}"                                                  \
    "    Var {{var^i}};\n"                                              \
    "{{end for}}"                                                       \
    "{{for i in sync}}"                                                 \
    "    Sync {{sync^i}};\n"                                            \
    "{{end for}}"                                                       \
    "{{for i in nosync}}"                                               \
    "    Nosync {{nosync^i}};\n"                                        \
    "{{end for}}"                                                       \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";
}

bool Multiple::modify(graph::AtomicModel& atom,
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

    build(true);

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);

        TimeStep::fillFields(condition);
        Multiple::fillFields(condition);
	Parameters::fillFields(parameters, externalVariables);
    } else {
        TimeStep::fillFields(conditions.get(conditionName));
        Multiple::fillFields(conditions.get(conditionName));
	Parameters::fillFields(parameters, externalVariables);
    }

    backup();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(atom, model, dynamic, conditions, classname, namespace_);
        m_dialog->hide_all();
        return true;
    }
    m_dialog->hide_all();
    return false;
}

bool Multiple::start(vpz::Condition& condition)
{
    build(false);
    Multiple::fillFields(condition);
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	Multiple::assign(condition);
    }
    m_dialog->hide();
    return true;
}

void Multiple::assign(vpz::Condition& condition)
{
    Mapping::deletePorts(condition);
    TimeStep::deletePorts(condition);
    Variables::deletePorts(condition);

    Mapping::assign(condition);
    TimeStep::assign(condition);
    Variables::assign(condition);
}

}}} // namespace vle gvle modeling

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::Multiple)

