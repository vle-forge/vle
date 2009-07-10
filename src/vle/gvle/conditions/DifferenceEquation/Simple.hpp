/**
 * @file vle/gvle/conditions/DifferenceEquation/Simple.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_SIMPLE_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_SIMPLE_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/GenericPlugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/Mapping.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>

namespace vle { namespace gvle { namespace conditions {

class Simple : public ConditionPlugin, public GenericPlugin, public Mapping
{

public:
    Simple(const std::string& name);

    virtual ~Simple();

    virtual bool start(vpz::Condition& condition);
    virtual bool start(vpz::Condition&,
	       const std::string&)
	{ return true; }

private:

    Gtk::Dialog*       m_dialog;

    void build();

    /**
     * @brief fill the fields if the ports already exist
     * @param condition the current condition
     *
     */
    void fillFields(vpz::Condition& condition);

    /**
     * @brief save the ports in the condition
     * @param condition the current condition
     *
     */
    void assign(vpz::Condition& condition);
};

}}} // namespace vle gvle conditions

#endif
