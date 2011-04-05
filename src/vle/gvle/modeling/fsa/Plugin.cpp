/*
 * @file vle/gvle/modeling/fsa/Plugin.cpp
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


#include <vle/gvle/modeling/fsa/Plugin.hpp>
#include <vle/gvle/modeling/fsa/SourceDialog.hpp>
#include <vle/gvle/modeling/fsa/TimeStepDialog.hpp>
#include <vle/utils/Template.hpp>
#include <vle/gvle/Message.hpp>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <boost/regex.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

const std::string PluginFSA::TEMPLATE_DEFINITION =
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag Statechart (generic)@@"                                  \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};{{width}}|{{height}};s:"                       \
    "{{for i in states}}"                                               \
    "{{states^i}}|"                                                     \
    "{{end for}}"                                                       \
    ";t:"                                                               \
    "{{for i in transitions}}"                                          \
    "{{transitions^i}}|"                                                \
    "{{end for}}"                                                       \
    ";T:"                                                               \
    "{{timeStep}}"                                                      \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/fsa/Statechart.hpp>\n\n"                   \
    "//@@begin:include@@\n"                                             \
    "{{include}}"                                                       \
    "//@@end:include@@\n"                                               \
    "namespace vd = vle::devs;\n"                                       \
    "namespace vf = vle::extension::fsa;\n"                             \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public vf::Statechart\n"                     \
    "{\n"                                                               \
    "    enum state_t {{statesenum}};\n\n"                              \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& init,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : vf::Statechart(init, evts)\n"                            \
    "    {\n"                                                           \
    "//@@begin:constructorUser@@\n"                                     \
    "{{constructorUser}}"                                               \
    "\n"                                                                \
    "//@@end:constructorUser@@\n"                                       \
    "      // structure\n"                                              \
    "      states(this)"                                                \
    "{{for i in states2}}"                                              \
    " << {{states2^i}}"                                                 \
    "{{end for}}"                                                       \
    ";\n\n"                                                             \
    "{{for i in src}}"                                                  \
    "      transition(this, {{src^i}}, {{dst^i}}){{clauses^i}}"         \
    ";\n"                                                               \
    "{{end for}}"                                                       \
    "\n"                                                                \
    "{{for i in states3}}"                                              \
    "      state(this, {{states3^i}}){{clauses2^i}}"                    \
    ";\n"                                                               \
    "{{end for}}"                                                       \
    "\n"                                                                \
    "      initialState({{initial}});\n"                                \
    "{{if bTimeStep}}"                                                  \
    "      timeStep({{timeStep}});\n"                                   \
    "{{end if}}"                                                        \
    "    }\n"                                                           \
    "\n"                                                                \
    "    virtual ~{{classname}}()\n"                                    \
    "    {}\n\n"                                                        \
    "private:\n"                                                        \
    "{{for i in actions}}"                                              \
    "\n"                                                                \
    "//@@begin:action({{actionNames^i}})@@\n"                           \
    "{{actions^i}}"                                                     \
    "//@@end:action@@\n"                                                \
    "{{end for}}"                                                       \
    "{{for i in eventActions}}"                                         \
    "\n"                                                                \
    "//@@begin:eventAction({{eventActionNames^i}})@@\n"                 \
    "{{eventActions^i}}"                                                \
    "//@@end:eventAction@@\n"                                           \
    "{{end for}}"                                                       \
    "{{for i in guards}}"                                               \
    "\n"                                                                \
    "//@@begin:guard({{guardNames^i}})@@\n"                             \
    "{{guards^i}}"                                                      \
    "//@@end:guard@@\n"                                                 \
    "{{end for}}"                                                       \
    "{{for i in afters}}"                                               \
    "\n"                                                                \
    "//@@begin:after({{afterNames^i}})@@\n"                             \
    "{{afters^i}}"                                                      \
    "//@@end:after@@\n"                                                 \
    "{{end for}}"                                                       \
    "{{for i in whens}}"                                                \
    "\n"                                                                \
    "//@@begin:when({{whenNames^i}})@@\n"                               \
    "{{whens^i}}"                                                       \
    "//@@end:when@@\n"                                                  \
    "{{end for}}"                                                       \
    "{{for i in sends}}"                                                \
    "\n"                                                                \
    "//@@begin:send({{sendNames^i}})@@\n"                               \
    "{{sends^i}}"                                                       \
    "//@@end:send@@\n"                                                  \
    "{{end for}}"                                                       \
    "{{for i in activities}}"                                           \
    "\n"                                                                \
    "//@@begin:activity({{activityNames^i}})@@\n"                       \
    "{{activities^i}}"                                                  \
    "//@@end:activity@@\n"                                              \
    "{{end for}}"                                                       \
    "//@@begin:definitionUser@@\n"                                      \
    "{{definitionUser}}"                                                \
    "\n"                                                                \
    "//@@end:definitionUser@@\n"                                        \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";

const Glib::ustring PluginFSA::UI_DEFINITION =
    "<ui>"
    "    <toolbar name='Toolbar'>"
    "        <toolitem action='Select' />"
    "        <toolitem action='AddState' />"
    "        <toolitem action='AddTransition' />"
    "        <toolitem action='Delete' />"
    "        <toolitem action='Help' />"
    "    </toolbar>"
    "</ui>";

PluginFSA::PluginFSA(const std::string& name)
    : ModelingPlugin(name), mDialog(0), mStatechart(0)
{
    std::string glade = utils::Path::path().getModelingGladeFile(
            "FSA.glade");

    mXml = Gnome::Glade::Xml::create(glade);
    mXml->get_widget("StatechartDialog", mDialog);
    mDialog->set_title("FSA - Statechart");

    mXml->get_widget_derived("StatechartDrawingArea", mView);

    mXml->get_widget("IncludeSourceButton", mIncludeButton);
    mXml->get_widget("UserSourceButton", mUserButton);
    mXml->get_widget("TimeStepButton", mTimeStepButton);

    mList.push_back(mIncludeButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginFSA::onIncludeSource)));
    mList.push_back(mUserButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginFSA::onUserSource)));
    mList.push_back(mTimeStepButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginFSA::onTimeStep)));

    {
        Gtk::HBox* hbox;
        mXml->get_widget("StatechartHBox", hbox);
        mActionGroup = Gtk::ActionGroup::create();
        mUIManager = Gtk::UIManager::create();
        createActions();
        mUIManager->insert_action_group(mActionGroup);
        mDialog->add_accel_group(mUIManager->get_accel_group());
        createUI();
        mToolbar = dynamic_cast < Gtk::Toolbar* > (
                mUIManager->get_widget("/Toolbar"));
        mToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
        mToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL);
        mToolbar->set_size_request(100, 50);
        hbox->pack_start(*mToolbar, false, false);
    }
}

PluginFSA::~PluginFSA()
{
    destroy();

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void PluginFSA::createActions()
{
    Gtk::RadioAction::Group toolsGroup;

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Select", Gtk::Stock::INDEX,
            _("Select"),
            _("Select state or transition (F1)")),
        Gtk::AccelKey("F1"), sigc::mem_fun(this, &PluginFSA::onSelect));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddState", Gtk::Stock::ADD,
            _("Add state"),
            _("Add state (F2)")),
        Gtk::AccelKey("F2"),
        sigc::mem_fun(this, &PluginFSA::onAddState));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddTransition",
            Gtk::Stock::DISCONNECT,
            _("Add transition"),
            _("Add transition (F3)")),
        Gtk::AccelKey("F3"),
        sigc::mem_fun(this, &PluginFSA::onAddTransition));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Delete", Gtk::Stock::DELETE,
            _("Delete"),
            _("Delete state or transition (F4)")),
        Gtk::AccelKey("F4"),
        sigc::mem_fun(this, &PluginFSA::onDelete));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Help", Gtk::Stock::HELP,
            _("Help"), _("Help (F5)")),
        Gtk::AccelKey("F5"),
        sigc::mem_fun(this, &PluginFSA::onHelp));
}

void PluginFSA::createUI()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        mUIManager->add_ui_from_string(UI_DEFINITION);
    } catch (const Glib::Error& ex) {
        throw utils::InternalError(fmt(
                _("FSA modeling plugin building menus failed: %1%")) %
            ex.what());
    }
#else
    std::auto_ptr < Glib::Error > ex;
    mUIManager->add_ui_from_string(UI_DEFINITION, ex);
    if (ex.get()) {
        throw utils::InternalError(fmt(
                _("FSA modeling plugin building menus failed: %1%")) %
            ex->what());
    }
#endif
}

void PluginFSA::destroy()
{
    delete mStatechart;
    mStatechart = 0;
}

void PluginFSA::onSelect()
{
    mView->setState(StatechartDrawingArea::SELECT);
}

void PluginFSA::onAddState()
{
    mView->setState(StatechartDrawingArea::ADD_STATE);
}

void PluginFSA::onAddTransition()
{
    mView->setState(StatechartDrawingArea::ADD_TRANSITION);
}

void PluginFSA::onDelete()
{
    mView->setState(StatechartDrawingArea::DELETE);
}

void PluginFSA::onHelp()
{
    mView->setState(StatechartDrawingArea::HELP);
}

void PluginFSA::onIncludeSource()
{
    SourceDialog dialog(mXml);

    dialog.add("Include", mInclude);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mInclude.assign(dialog.buffer("Include"));
    }
}

void PluginFSA::onTimeStep()
{
    TimeStepDialog dialog(mXml);

    if (dialog.run(mTimeStep) == Gtk::RESPONSE_ACCEPT) {
        mTimeStep.assign(dialog.timeStep());
    }
}

void PluginFSA::onUserSource()
{
    SourceDialog dialog(mXml);

    dialog.add("Constructor", mConstructorUser);
    dialog.add("Definition", mDefinitionUser);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mConstructorUser.assign(dialog.buffer("Constructor"));
        mDefinitionUser.assign(dialog.buffer("Definition"));
    }
}

bool PluginFSA::create(graph::AtomicModel& atom,
    vpz::AtomicModel& model,
    vpz::Dynamic& /*dynamic*/,
    vpz::Conditions& /*conditions*/,
    vpz::Observables& observables,
    const std::string& classname,
    const std::string& namespace_)
{
    strings_t inputPorts;
    strings_t outputPorts;

    for (graph::ConnectionList::const_iterator it =
             atom.getInputPortList().begin();
         it != atom.getInputPortList().end(); ++it) {
        inputPorts.push_back(it->first);
    }

    for (graph::ConnectionList::const_iterator it =
             atom.getOutputPortList().begin();
         it != atom.getOutputPortList().end(); ++it) {
        outputPorts.push_back(it->first);
    }

    mStatechart = new Statechart(classname, inputPorts, outputPorts);
    mView->setStatechart(mStatechart);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(classname, namespace_);
        generateStructure(atom);
        generateObservables(atom, model, observables);
        mDialog->hide();
        destroy();
        return true;
    } else {
        mDialog->hide();
        destroy();
        return false;
    }
}

void PluginFSA::generateObservables(graph::AtomicModel& atom,
    vpz::AtomicModel& model,
    vpz::Observables& observables)
{
    std::string observableName((fmt("obs_FSA_%1%") % atom.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));

        if (not observable.exist("state")) {
            observable.add("state");
        }
    } else {
        vpz::Observable observable(observableName);

        observable.add("state");
        observables.add(observable);
    }
    if (model.observables().empty()) {
        model.setObservables(observableName);
    }
}

void PluginFSA::generateSource(const std::string& classname,
    const std::string& namespace_)
{
    utils::Template tpl_(TEMPLATE_DEFINITION);

    tpl_.stringSymbol().append("namespace", namespace_);
    tpl_.stringSymbol().append("classname", classname);

    tpl_.stringSymbol().append("width",
        boost::lexical_cast < std::string > (
            mStatechart->width()));
    tpl_.stringSymbol().append("height",
        boost::lexical_cast < std::string > (
            mStatechart->height()));

    // states
    tpl_.listSymbol().append("states");
    for (states_t::const_iterator it = mStatechart->states().begin();
         it != mStatechart->states().end(); ++it) {
        tpl_.listSymbol().append("states", it->second->toString());
    }
    {
        std::string str = "{ ";

        for (states_t::const_iterator it = mStatechart->states().begin();
             it != mStatechart->states().end(); ++it) {
            if (it != mStatechart->states().begin()) {
                str += ", ";
            }
            str += it->first;
        }
        str += " }";
        tpl_.stringSymbol().append("statesenum", str);
    }
    tpl_.listSymbol().append("states2");
    for (states_t::const_iterator it = mStatechart->states().begin();
         it != mStatechart->states().end(); ++it) {
        tpl_.listSymbol().append("states2", it->first);
    }
    tpl_.listSymbol().append("states3");
    tpl_.listSymbol().append("clauses2");
    for (states_t::const_iterator it = mStatechart->states().begin();
         it != mStatechart->states().end(); ++it) {
        std::string clause = mStatechart->clause(it->second);

        if (not clause.empty()) {
            tpl_.listSymbol().append("states3", it->first);
            tpl_.listSymbol().append("clauses2", clause);
        }
    }
    tpl_.stringSymbol().append("initial", mStatechart->initialState());

    // transitions
    tpl_.listSymbol().append("transitions");
    for (transitions_t::const_iterator it =
             mStatechart->transitions().begin();
         it != mStatechart->transitions().end(); ++it) {
        tpl_.listSymbol().append("transitions", (*it)->toString());
    }
    tpl_.listSymbol().append("src");
    tpl_.listSymbol().append("dst");
    tpl_.listSymbol().append("clauses");
    for (transitions_t::const_iterator it =
             mStatechart->transitions().begin();
         it != mStatechart->transitions().end(); ++it) {
        tpl_.listSymbol().append("src", (*it)->source());
        tpl_.listSymbol().append("dst", (*it)->destination());
        tpl_.listSymbol().append("clauses", mStatechart->clause(*it));
    }

    // actions
    tpl_.listSymbol().append("actions");
    tpl_.listSymbol().append("actionNames");
    for (buffers_t::const_iterator it = mStatechart->actions().begin();
         it != mStatechart->actions().end(); ++it) {
        tpl_.listSymbol().append("actions", it->second);
        tpl_.listSymbol().append("actionNames", it->first);
    }

    // eventActions
    tpl_.listSymbol().append("eventActions");
    tpl_.listSymbol().append("eventActionNames");
    for (buffers_t::const_iterator it = mStatechart->eventActions().begin();
         it != mStatechart->eventActions().end(); ++it) {
        tpl_.listSymbol().append("eventActions", it->second);
        tpl_.listSymbol().append("eventActionNames", it->first);
    }

    // guards
    tpl_.listSymbol().append("guards");
    tpl_.listSymbol().append("guardNames");
    for (buffers_t::const_iterator it = mStatechart->guards().begin();
         it != mStatechart->guards().end(); ++it) {
        tpl_.listSymbol().append("guards", it->second);
        tpl_.listSymbol().append("guardNames", it->first);
    }

    // afters
    tpl_.listSymbol().append("afters");
    tpl_.listSymbol().append("afterNames");
    for (buffers_t::const_iterator it = mStatechart->afters().begin();
         it != mStatechart->afters().end(); ++it) {
        tpl_.listSymbol().append("afters", it->second);
        tpl_.listSymbol().append("afterNames", it->first);
    }

    // whens
    tpl_.listSymbol().append("whens");
    tpl_.listSymbol().append("whenNames");
    for (buffers_t::const_iterator it = mStatechart->whens().begin();
         it != mStatechart->whens().end(); ++it) {
        tpl_.listSymbol().append("whens", it->second);
        tpl_.listSymbol().append("whenNames", it->first);
    }

    // sends
    tpl_.listSymbol().append("sends");
    tpl_.listSymbol().append("sendNames");
    for (buffers_t::const_iterator it = mStatechart->sends().begin();
         it != mStatechart->sends().end(); ++it) {
        tpl_.listSymbol().append("sends", it->second);
        tpl_.listSymbol().append("sendNames", it->first);
    }

    // activities
    tpl_.listSymbol().append("activities");
    tpl_.listSymbol().append("activityNames");
    for (buffers_t::const_iterator it = mStatechart->activities().begin();
         it != mStatechart->activities().end(); ++it) {
        tpl_.listSymbol().append("activities", it->second);
        tpl_.listSymbol().append("activityNames", it->first);
    }

    // includes
    tpl_.stringSymbol().append("include", mInclude);

    // constructor and definition user
    tpl_.stringSymbol().append("constructorUser", mConstructorUser);
    tpl_.stringSymbol().append("definitionUser", mDefinitionUser);

    // time-step
    tpl_.boolSymbol().append("bTimeStep", not mTimeStep.empty());
    tpl_.stringSymbol().append("timeStep", mTimeStep);

    std::ostringstream out;
    tpl_.process(out);

    mSource = out.str();
}

void PluginFSA::generateStructure(graph::AtomicModel& atom)
{
    for (strings_t::const_iterator it = mStatechart->inputPorts().begin();
         it != mStatechart->inputPorts().end(); ++it) {
        if (not atom.existInputPort(*it)) {
            atom.addInputPort(*it);
        }
    }

    for (strings_t::const_iterator it = mStatechart->outputPorts().begin();
         it != mStatechart->outputPorts().end(); ++it) {
        if (not atom.existOutputPort(*it)) {
            atom.addOutputPort(*it);
        }
    }
}

bool PluginFSA::modify(graph::AtomicModel& atom,
    vpz::AtomicModel& model,
    vpz::Dynamic& /*dynamic*/,
    vpz::Conditions& /*conditions*/,
    vpz::Observables& observables,
    const std::string& conf,
    const std::string& buffer)
{
    std::string namespace_;
    std::string classname;
    strings_t lst;

    boost::split(lst, conf, boost::is_any_of(";"));
    parseConf(lst, classname, namespace_);

    strings_t inputPorts;
    strings_t outputPorts;

    for (graph::ConnectionList::const_iterator it =
             atom.getInputPortList().begin();
         it != atom.getInputPortList().end(); ++it) {
        inputPorts.push_back(it->first);
    }
    for (graph::ConnectionList::const_iterator it =
             atom.getOutputPortList().begin();
         it != atom.getOutputPortList().end(); ++it) {
        outputPorts.push_back(it->first);
    }

    mStatechart = new Statechart(classname, inputPorts, outputPorts);

    // width & height
    strings_t wh;

    boost::split(wh, lst[2], boost::is_any_of("|"));
    mStatechart->width(boost::lexical_cast < int > (wh[0]));
    mStatechart->height(boost::lexical_cast < int > (wh[1]));

    parseStates(lst);
    parseTransitions(lst);
    parseFunctions(buffer);

    mInclude.assign(parseFunction(buffer, "//@@begin:include@@",
            "//@@end:include@@", "include"));
    mConstructorUser.assign(parseFunction(buffer,
            "//@@begin:constructorUser@@",
            "//@@end:constructorUser@@",
            "constructorUser"));
    mDefinitionUser.assign(parseFunction(buffer,
            "//@@begin:definitionUser@@",
            "//@@end:definitionUser@@",
            "definitionUser"));
    mView->setStatechart(mStatechart);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(classname, namespace_);
        generateStructure(atom);
        generateObservables(atom, model, observables);
        mDialog->hide();
        destroy();
        return true;
    } else {
        mDialog->hide();
        destroy();
        return false;
    }
    return true;
}

void PluginFSA::parseConf(const strings_t& lst,
    std::string& classname,
    std::string& namespace_)
{
// namespace
    namespace_ = std::string(lst[0], 10, lst[0].size() - 10);

// classname
    classname = std::string(lst[1], 6, lst[1].size() - 6);

// time-step
    mTimeStep.assign(lst[5], 2, lst[5].size() - 2);
}

std::string PluginFSA::parseFunction(const std::string& buffer,
    const std::string& begin,
    const std::string& end,
    const std::string& name)
{
    boost::regex tagbegin(begin, boost::regex::grep);
    boost::regex tagend(end, boost::regex::grep);

    boost::sregex_iterator it(buffer.begin(), buffer.end(), tagbegin);
    boost::sregex_iterator jt(buffer.begin(), buffer.end(), tagend);
    boost::sregex_iterator itend;

    if (it == itend or jt == itend) {
        throw utils::ArgError(fmt(_("FSA plugin error, " \
                    "no begin or end tag (%1%)")) % name);
    }

    if ((*it)[0].second >= (*jt)[0].first) {
        throw utils::ArgError(fmt(_("FSA plugin error, " \
                    "bad tag (%1%)")) % name);
    }

    return std::string((*it)[0].second + 1, (*jt)[0].first);
}

void PluginFSA::parseFunction(const std::string& buffer,
    const std::string& begin,
    const std::string& end,
    strings_t& names,
    strings_t& buffers)
{
    boost::regex tagbegin(begin, boost::regex::grep);
    boost::regex tagend(end, boost::regex::grep);

    boost::sregex_iterator it(buffer.begin(), buffer.end(), tagbegin);
    boost::sregex_iterator jt(buffer.begin(), buffer.end(), tagend);
    boost::sregex_iterator itend;

    while (it != itend and jt != itend) {
        boost::regex nameend(")", boost::regex::grep);
        boost::sregex_iterator kt((*it)[0].second, (*jt)[0].first, nameend);

        names.push_back(std::string((*it)[0].second, (*kt)[0].first));
        buffers.push_back(std::string((*kt)[0].second + 3, (*jt)[0].first));
        ++it;
        ++jt;
    }
}

void PluginFSA::parseFunctions(const std::string& buffer)
{
    // actions
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:action(", "//@@end:action@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->action(*itn++, *itf++);
        }
    }

    // eventActions
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer,
            "//@@begin:eventAction(",
            "//@@end:eventAction@@",
            names,
            buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->eventAction(*itn++, *itf++);
        }
    }

    // guards
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:guard(", "//@@end:guard@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->guard(*itn++, *itf++);
        }
    }

    // afters
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:after(", "//@@end:after@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->after(*itn++, *itf++);
        }
    }

    // whens
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:when(", "//@@end:when@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->when(*itn++, *itf++);
        }
    }

    // sends
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:send(", "//@@end:send@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->send(*itn++, *itf++);
        }
    }

    // activities
    {
        strings_t names;
        strings_t buffers;

        parseFunction(buffer, "//@@begin:activity(", "//@@end:activity@@",
            names, buffers);

        strings_t::const_iterator itn = names.begin();
        strings_t::const_iterator itf = buffers.begin();

        while (itn != names.end()) {
            mStatechart->activity(*itn++, *itf++);
        }
    }
}

void PluginFSA::parseStates(const strings_t& lst)
{
    std::string state_lst(lst[3], 2, lst[3].size() - 2);
    strings_t states;

    boost::split(states, state_lst, boost::is_any_of("|"));
    for (strings_t::const_iterator it = states.begin();
         it != states.end(); ++it) {
        if (not it->empty()) {
            mStatechart->addState(*it);
        }
    }
}

void PluginFSA::parseTransitions(const strings_t& lst)
{
    std::string transition_lst(lst[4], 2, lst[4].size() - 2);
    strings_t transitions;

    boost::split(transitions, transition_lst, boost::is_any_of("|"));
    for (strings_t::const_iterator it = transitions.begin();
         it != transitions.end(); ++it) {
        if (not it->empty()) {
            mStatechart->addTransition(*it);
        }
    }
}

bool PluginFSA::start(vpz::Condition& /*condition*/)
{
    return true;
}

}
}
}
}    // namespace vle gvle modeling fsa

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::fsa::PluginFSA)
