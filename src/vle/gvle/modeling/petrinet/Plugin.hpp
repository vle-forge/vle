/*
 * @file vle/gvle/modeling/petrinet/Plugin.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_MODELING_PETRINET_PLUGIN_HPP
#define VLE_GVLE_MODELING_PETRINET_PLUGIN_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/petrinet/PetriNetDrawingArea.hpp>
#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/actiongroup.h>
#include <gtkmm/dialog.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <libglademm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

class PluginPetriNet : public ModelingPlugin
{
public:
    PluginPetriNet(const std::string& name);
    virtual ~PluginPetriNet();

    virtual bool create(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

    virtual bool modify(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

    virtual bool start(vpz::Condition& condition);

    virtual bool start(vpz::Condition&, const std::string&)
    {
        return true;
    }

private:
    void createActions();
    void createUI();
    void destroy();

    void generateObservables(graph::AtomicModel& atom,
                             vpz::AtomicModel& model,
                             vpz::Observables& observables);
    void generateSource(const std::string& classname,
                        const std::string& namespace_);
    void generateStructure(graph::AtomicModel& atom);

    void onAddArc();
    void onAddPlace();
    void onAddTransition();
    void onDelete();
    void onHelp();
    void onSelect();

    void parseConf(const strings_t& lst, std::string& classname,
                   std::string& namespace_);
    void parseFunctions(const std::string& buffer);
    void parsePlaces(const strings_t& lst);
    void parseTransitions(const strings_t& lst);
    void parseArcs(const strings_t& lst);

    static const Glib::ustring UI_DEFINITION;
    static const std::string TEMPLATE_DEFINITION;

    Glib::RefPtr < Gnome::Glade::Xml > mXml;
    Gtk::Dialog* mDialog;
    PetriNetDrawingArea* mView;

    Gtk::Toolbar* mToolbar;
    Glib::RefPtr < Gtk::UIManager >    mUIManager;
    Glib::RefPtr < Gtk::ActionGroup >  mActionGroup;

    std::list < sigc::connection >     mList;

    PetriNet* mPetriNet;
};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
