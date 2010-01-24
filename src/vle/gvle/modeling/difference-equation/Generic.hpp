/**
 * @file vle/gvle/modeling/difference-equation/Generic.hpp
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_GENERIC_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_GENERIC_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/difference-equation/NameValue.hpp>
#include <vle/gvle/modeling/difference-equation/Plugin.hpp>
#include <vle/utils/Template.hpp>
#include <gtkmm/dialog.h>

namespace vle { namespace gvle { namespace modeling {

class Generic : public Plugin, public NameValue
{
public:
    Generic(const std::string& name);
    virtual ~Generic();

    virtual bool create(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

    virtual std::string getMode() const
    { return "port"; }

    virtual bool modify(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

    virtual bool start(vpz::Condition& condition);

    virtual bool start(vpz::Condition&, const std::string&)
    { return true; }

private:
    Gtk::Dialog*         m_dialog;
    Gtk::Button*         m_buttonSource;

    std::list < sigc::connection > mList;

    void assign(vpz::Condition& condition);
    void build(bool modeling);
    void fillFields(const vpz::Condition& condition);
    virtual void generateCondition(graph::AtomicModel& atom,
                                   vpz::AtomicModel& model,
                                   vpz::Conditions& conditions);
    virtual void generateOutputPorts(graph::AtomicModel& atom);
    virtual void generateVariables(utils::Template& tpl_);
    std::string getTemplate() const;
};

}}} // namespace vle gvle modeling

#endif
