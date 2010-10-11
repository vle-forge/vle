/*
 * @file vle/gvle/modeling/petrinet/Plugin.cpp
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


#include <vle/gvle/modeling/petrinet/Plugin.hpp>
#include <vle/utils/Template.hpp>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <boost/regex.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

const std::string PluginPetriNet::TEMPLATE_DEFINITION =
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag PetriNet@@"                                              \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};{{width}}|{{height}};p:"                       \
    "{{for i in places}}"                                               \
    "{{places^i}}|"                                                     \
    "{{end for}}"                                                       \
    ";t:"                                                               \
    "{{for i in transitions}}"                                          \
    "{{transitions^i}}|"                                                \
    "{{end for}}"                                                       \
    ";a:"                                                               \
    "{{for i in arcs}}"                                                 \
    "{{arcs^i}}|"                                                       \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/PetriNet.hpp>\n\n"                         \
    "namespace vd = vle::devs;\n"                                       \
    "namespace ve = vle::extension;\n"                                  \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public ve::PetriNet\n"                       \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& init,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : ve::PetriNet(init, evts)\n"                              \
    "    {\n"                                                           \
    "    }\n\n"                                                         \
    "    virtual ~{{classname}}()\n"                                    \
    "    {}\n\n"                                                        \
    "    virtual void build() {\n"                                      \
    "{{for i in simplePlaces}}"                                         \
    "        addPlace(\"{{simplePlaces^i}}\", {{placeDelays^i}});\n"    \
    "{{end for}}"                                                       \
    "{{for i in outputPlaces}}"                                         \
    "        addOutputPlace(\"{{outputPlaces^i}}\", "                   \
    "\"{{outputPortPlaces^i}}\");\n"                                    \
    "{{end for}}"                                                       \
    "{{for i in initialMarkings}}"                                      \
    "        addInitialMarking(\"{{initialMarkingPlaces^i}}\", "        \
    "{{initialMarkings^i}});\n"                                         \
    "{{end for}}"                                                       \
    "{{for i in simpleTransitions}}"                                    \
    "        addTransition(\"{{simpleTransitions^i}}\", "               \
    "{{simpleTransitionDelays^i}}, "                                    \
    "{{simpleTransitionPriorities^i}});\n"                              \
    "{{end for}}"                                                       \
    "{{for i in inputTransitions}}"                                     \
    "        addInputTransition(\"{{inputTransitions^i}}\", "           \
    "\"{{inputPortTransitions^i}}\", "                                  \
    "{{inputTransitionDelays^i}});\n"                                   \
    "{{end for}}"                                                       \
    "{{for i in outputTransitions}}"                                    \
    "        addOutputTransition(\"{{outputTransitions^i}}\", "         \
    "\"{{outputPortTransitions^i}}\", "                                 \
    "{{outputTransitionDelays^i}});\n"                                  \
    "{{end for}}"                                                       \
    "{{for i in arcSrcs}}"                                              \
    "        addArc(\"{{arcSrcs^i}}\", \"{{arcDsts^i}}\", {{weights^i}});\n" \
    "{{end for}}"                                                       \
    "    }\n\n"                                                         \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";

const Glib::ustring PluginPetriNet::UI_DEFINITION =
    "<ui>"
    "    <toolbar name='Toolbar'>"
    "        <toolitem action='Select' />"
    "        <toolitem action='AddPlace' />"
    "        <toolitem action='AddTransition' />"
    "        <toolitem action='AddArc' />"
    "        <toolitem action='Delete' />"
    "        <toolitem action='Help' />"
    "    </toolbar>"
    "</ui>";

PluginPetriNet::PluginPetriNet(const std::string& name)
    : ModelingPlugin(name), mDialog(0), mPetriNet(0)
{
    std::string glade = utils::Path::path().getModelingGladeFile(
            "PetriNet.glade");

    mXml = Gnome::Glade::Xml::create(glade);
    mXml->get_widget("PetriNetDialog", mDialog);
    mDialog->set_title("PetriNet");

    mXml->get_widget_derived("PetriNetDrawingArea", mView);

    {
        Gtk::HBox* hbox;

        mXml->get_widget("PetriNetHBox", hbox);
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

PluginPetriNet::~PluginPetriNet()
{
    destroy();

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void PluginPetriNet::createActions()
{
    Gtk::RadioAction::Group toolsGroup;

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Select", Gtk::Stock::INDEX,
            _("Select"),
            _("Select arc or state or transition (F1)")),
        Gtk::AccelKey("F1"), sigc::mem_fun(this, &PluginPetriNet::onSelect));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddPlace", Gtk::Stock::ADD,
            _("Add place"),
            _("Add place (F2)")),
        Gtk::AccelKey("F2"),
        sigc::mem_fun(this, &PluginPetriNet::onAddPlace));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddTransition",
            Gtk::Stock::REMOVE,
            _("Add transition"),
            _("Add transition (F3)")),
        Gtk::AccelKey("F3"),
        sigc::mem_fun(this, &PluginPetriNet::onAddTransition));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddArc",
            Gtk::Stock::DISCONNECT,
            _("Add arc"),
            _("Add arc (F4)")),
        Gtk::AccelKey("F4"),
        sigc::mem_fun(this, &PluginPetriNet::onAddArc));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Delete", Gtk::Stock::DELETE,
            _("Delete"),
            _("Delete arc or state or transition (F5)")),
        Gtk::AccelKey("F5"),
        sigc::mem_fun(this, &PluginPetriNet::onDelete));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Help", Gtk::Stock::HELP,
            _("Help"), _("Help (F6)")),
        Gtk::AccelKey("F6"),
        sigc::mem_fun(this, &PluginPetriNet::onHelp));
}

void PluginPetriNet::createUI()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        mUIManager->add_ui_from_string(UI_DEFINITION);
    } catch (const Glib::Error& ex) {
        throw utils::InternalError(fmt(
                _(
                    "PetriNet modeling plugin building menus failed: %1%"))
            %
            ex.what());
    }
#else
    std::auto_ptr < Glib::Error > ex;
    mUIManager->add_ui_from_string(UI_DEFINITION, ex);
    if (ex.get()) {
        throw utils::InternalError(fmt(
                _(
                    "PetriNet modeling plugin building menus failed: %1%"))
            %
            ex->what());
    }
#endif
}

void PluginPetriNet::destroy()
{
    delete mPetriNet;
    mPetriNet = 0;
}

void PluginPetriNet::onAddArc()
{
    mView->setState(PetriNetDrawingArea::ADD_ARC);
}

void PluginPetriNet::onAddPlace()
{
    mView->setState(PetriNetDrawingArea::ADD_PLACE);
}

void PluginPetriNet::onAddTransition()
{
    mView->setState(PetriNetDrawingArea::ADD_TRANSITION);
}

void PluginPetriNet::onDelete()
{
    mView->setState(PetriNetDrawingArea::DELETE);
}

void PluginPetriNet::onHelp()
{
    mView->setState(PetriNetDrawingArea::HELP);
}

void PluginPetriNet::onSelect()
{
    mView->setState(PetriNetDrawingArea::SELECT);
}

bool PluginPetriNet::create(graph::AtomicModel& atom,
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

    mPetriNet = new PetriNet(classname, inputPorts, outputPorts);
    mView->setPetriNet(mPetriNet);

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

void PluginPetriNet::generateObservables(graph::AtomicModel& atom,
    vpz::AtomicModel& model,
    vpz::Observables& observables)
{
    std::string observableName((fmt("obs_PetriNet_%1%") %
                                atom.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));

        if (not observable.exist("token")) {
            observable.add("token");
        }
        if (not observable.exist("marking")) {
            observable.add("marking");
        }
    } else {
        vpz::Observable observable(observableName);

        observable.add("token");
        observable.add("marking");
        observables.add(observable);
    }
    if (model.observables().empty()) {
        model.setObservables(observableName);
    }
}

void PluginPetriNet::generateSource(const std::string& classname,
    const std::string& namespace_)
{
    utils::Template tpl_(TEMPLATE_DEFINITION);

    tpl_.stringSymbol().append("namespace", namespace_);
    tpl_.stringSymbol().append("classname", classname);

    tpl_.stringSymbol().append("width",
        boost::lexical_cast < std::string > (
            mPetriNet->width()));
    tpl_.stringSymbol().append("height",
        boost::lexical_cast < std::string > (
            mPetriNet->height()));

    // states
    tpl_.listSymbol().append("places");
    for (places_t::const_iterator it = mPetriNet->places().begin();
         it != mPetriNet->places().end(); ++it) {
        tpl_.listSymbol().append("places", it->second->toString());
    }

    // transitions
    tpl_.listSymbol().append("transitions");
    for (transitions_t::const_iterator it = mPetriNet->transitions().begin();
         it != mPetriNet->transitions().end(); ++it) {
        tpl_.listSymbol().append("transitions", it->second->toString());
    }

    // arcs
    tpl_.listSymbol().append("arcs");
    for (arcs_t::const_iterator it = mPetriNet->arcs().begin();
         it != mPetriNet->arcs().end(); ++it) {
        tpl_.listSymbol().append("arcs", (*it)->toString());
    }

    tpl_.listSymbol().append("simplePlaces");
    tpl_.listSymbol().append("placeDelays");
    tpl_.listSymbol().append("outputPlaces");
    tpl_.listSymbol().append("outputPortPlaces");
    tpl_.listSymbol().append("initialMarkings");
    tpl_.listSymbol().append("initialMarkingPlaces");
    for (places_t::const_iterator it = mPetriNet->places().begin();
         it != mPetriNet->places().end(); ++it) {
        Place* place = it->second;

        if (place->output()) {
            tpl_.listSymbol().append("outputPlaces", place->name());
            tpl_.listSymbol().append(
                "outputPortPlaces",
                (fmt("%1%") % place->outputPortName()).str());
        } else {
            tpl_.listSymbol().append("simplePlaces", place->name());
            if (place->delay()) {
                tpl_.listSymbol().append(
                    "placeDelays", (fmt("%1%") % place->delayValue()).str());
            } else {
                tpl_.listSymbol().append("placeDelays", "0");
            }
        }
        if (place->initialMarking() > 0) {
            tpl_.listSymbol().append("initialMarkingPlaces", place->name());
            tpl_.listSymbol().append(
                "initialMarkings",
                (fmt("%1%") % place->initialMarking()).str());
        }
    }

    tpl_.listSymbol().append("simpleTransitions");
    tpl_.listSymbol().append("simpleTransitionDelays");
    tpl_.listSymbol().append("simpleTransitionPriorities");
    tpl_.listSymbol().append("inputTransitions");
    tpl_.listSymbol().append("inputPortTransitions");
    tpl_.listSymbol().append("inputTransitionDelays");
    tpl_.listSymbol().append("outputTransitions");
    tpl_.listSymbol().append("outputPortTransitions");
    tpl_.listSymbol().append("outputTransitionDelays");
    for (transitions_t::const_iterator it = mPetriNet->transitions().begin();
         it != mPetriNet->transitions().end(); ++it) {
        Transition* transition = it->second;

        if (transition->input()) {
            tpl_.listSymbol().append("inputTransitions", transition->name());
            tpl_.listSymbol().append("inputPortTransitions",
                transition->inputPortName());
            if (transition->delay()) {
                tpl_.listSymbol().append(
                    "inputTransitionDelays",
                    (fmt("%1%") % transition->delayValue()).str());
            } else {
                tpl_.listSymbol().append("inputTransitionDelays", "0");
            }
        } else if (transition->output()) {
            tpl_.listSymbol().append("outputTransitions", transition->name());
            tpl_.listSymbol().append("outputPortTransitions",
                transition->outputPortName());
            if (transition->delay()) {
                tpl_.listSymbol().append(
                    "outputTransitionDelays",
                    (fmt("%1%") % transition->delayValue()).str());
            } else {
                tpl_.listSymbol().append("outputTransitionDelays", "0");
            }
        } else {
            tpl_.listSymbol().append("simpleTransitions", transition->name());
            if (transition->delay()) {
                tpl_.listSymbol().append(
                    "simpleTransitionDelays",
                    (fmt("%1%") % transition->delayValue()).str());
            } else {
                tpl_.listSymbol().append("simpleTransitionDelays", "0");
            }
            tpl_.listSymbol().append(
                "simpleTransitionPriorities",
                (fmt("%1%") % transition->priority()).str());
        }
    }

    tpl_.listSymbol().append("arcSrcs");
    tpl_.listSymbol().append("arcDsts");
    tpl_.listSymbol().append("weights");
    for (arcs_t::const_iterator it = mPetriNet->arcs().begin();
         it != mPetriNet->arcs().end(); ++it) {
        tpl_.listSymbol().append("arcSrcs", (*it)->source());
        tpl_.listSymbol().append("arcDsts", (*it)->destination());
        if ((*it)->inhibitor()) {
            tpl_.listSymbol().append("weights", "0");
        } else {
            tpl_.listSymbol().append("weights",
                (fmt("%1%") % (*it)->weight()).str());
        }
    }

    std::ostringstream out;
    tpl_.process(out);

    mSource = out.str();
}

void PluginPetriNet::generateStructure(graph::AtomicModel& atom)
{
    for (strings_t::const_iterator it = mPetriNet->inputPorts().begin();
         it != mPetriNet->inputPorts().end(); ++it) {
        if (not atom.existInputPort(*it)) {
            atom.addInputPort(*it);
        }
    }

    for (strings_t::const_iterator it = mPetriNet->outputPorts().begin();
         it != mPetriNet->outputPorts().end(); ++it) {
        if (not atom.existOutputPort(*it)) {
            atom.addOutputPort(*it);
        }
    }
}

bool PluginPetriNet::modify(graph::AtomicModel& atom,
    vpz::AtomicModel& model,
    vpz::Dynamic& /*dynamic*/,
    vpz::Conditions& /*conditions*/,
    vpz::Observables& observables,
    const std::string& conf,
    const std::string& /*buffer*/)
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

    mPetriNet = new PetriNet(classname, inputPorts, outputPorts);

    // width & height
    strings_t wh;

    boost::split(wh, lst[2], boost::is_any_of("|"));
    mPetriNet->width(boost::lexical_cast < int > (wh[0]));
    mPetriNet->height(boost::lexical_cast < int > (wh[1]));

    parsePlaces(lst);
    parseTransitions(lst);
    parseArcs(lst);

    mView->setPetriNet(mPetriNet);

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

void PluginPetriNet::parseConf(const strings_t& lst,
    std::string& classname,
    std::string& namespace_)
{
// namespace
    namespace_ = std::string(lst[0], 10, lst[0].size() - 10);

// classname
    classname = std::string(lst[1], 6, lst[1].size() - 6);
}

void PluginPetriNet::parsePlaces(const strings_t& lst)
{
    std::string place_lst(lst[3], 2, lst[3].size() - 2);
    strings_t places;

    boost::split(places, place_lst, boost::is_any_of("|"));
    for (strings_t::const_iterator it = places.begin();
         it != places.end(); ++it) {
        if (not it->empty()) {
            mPetriNet->addPlace(*it);
        }
    }
}

void PluginPetriNet::parseTransitions(const strings_t& lst)
{
    std::string transition_lst(lst[4], 2, lst[4].size() - 2);
    strings_t transitions;

    boost::split(transitions, transition_lst, boost::is_any_of("|"));
    for (strings_t::const_iterator it = transitions.begin();
         it != transitions.end(); ++it) {
        if (not it->empty()) {
            mPetriNet->addTransition(*it);
        }
    }
}

void PluginPetriNet::parseArcs(const strings_t& lst)
{
    std::string arc_lst(lst[5], 2, lst[5].size() - 2);
    strings_t arcs;

    boost::split(arcs, arc_lst, boost::is_any_of("|"));
    for (strings_t::const_iterator it = arcs.begin();
         it != arcs.end(); ++it) {
        if (not it->empty()) {
            mPetriNet->addArc(*it);
        }
    }
}

bool PluginPetriNet::start(vpz::Condition& /*condition*/)
{
    return true;
}

}
}
}
}    // namespace vle gvle modeling petrinet

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::petrinet::PluginPetriNet)
