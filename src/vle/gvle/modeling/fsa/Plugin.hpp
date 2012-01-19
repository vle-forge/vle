/*
 * @file vle/gvle/modeling/fsa/Plugin.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_MODELING_FSA_PLUGIN_HPP
#define VLE_GVLE_MODELING_FSA_PLUGIN_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/fsa/StatechartDrawingArea.hpp>
#include <vle/gvle/modeling/fsa/Statechart.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/actiongroup.h>
#include <gtkmm/dialog.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/builder.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {


class PluginFSA : public ModelingPlugin
{
public:
    PluginFSA(const std::string& name);
    virtual ~PluginFSA();

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

    void onAddState();
    void onAddTransition();
    void onDelete();
    void onHelp();
    void onIncludeSource();
    void onSelect();
    void onTimeStep();
    void onUserSource();

    void parseConf(const strings_t& lst, std::string& classname,
                   std::string& namespace_);
    void parseFunction(const std::string& buffer,
                       const std::string& begin,
                       const std::string& end,
                       strings_t& names,
                       strings_t& buffers);
    std::string parseFunction(const std::string& buffer,
                              const std::string& begin,
                              const std::string& end,
                              const std::string& name);
    void parseFunctions(const std::string& buffer);
    void parseStates(const strings_t& lst);
    void parseTransitions(const strings_t& lst);

    static const Glib::ustring UI_DEFINITION;
    static const std::string TEMPLATE_DEFINITION;

    Glib::RefPtr < Gtk::Builder > mXml;
    Gtk::Dialog* mDialog;
    StatechartDrawingArea* mView;
    Cairo::RefPtr < Cairo::Context >      mContext;

    Gtk::Toolbar* mToolbar;
    Glib::RefPtr < Gtk::UIManager >    mUIManager;
    Glib::RefPtr < Gtk::ActionGroup >  mActionGroup;

    Gtk::Button* mIncludeButton;
    Gtk::Button* mTimeStepButton;
    Gtk::Button* mUserButton;

    std::list < sigc::connection >     mList;

    Statechart* mStatechart;
    std::string mInclude;
    std::string mConstructorUser;
    std::string mDefinitionUser;
    std::string mTimeStep;
};

}
}
}
}    // namespace vle gvle modeling fsa

#endif
