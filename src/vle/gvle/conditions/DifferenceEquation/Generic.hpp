/**
 * @file vle/gvle/conditions/DifferenceEquation/Generic.hpp
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


#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_GENRIC_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_GENRIC_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/GenericPlugin.hpp>
#include <gtkmm/dialog.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace conditions {

class Generic : public ConditionPlugin, public GenericPlugin
{
public:
    Generic(const std::string& name);

    virtual ~Generic();

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

