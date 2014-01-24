/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef GUI_MODELDESCRIPTIONBOX_HH
#define GUI_MODELDESCRIPTIONBOX_HH

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <string>
#include <set>

namespace vle
{
namespace vpz {

class BaseModel;

}
} // namespace graph

namespace vle
{
namespace gvle {

/**
 * @brief A Gtk::Dialog to show the description of a graph::Model.
 */
class ModelDescriptionBox : public Gtk::Dialog
{
public :
    /**
     * A dialog box to change or add a new name and description for a
     * model. If a model is specified, then running this Dialog can change
     * its name and/or description.
     *
     * @param lst list of all model name in current modeling
     * @param model to assign new string or null if no model to affect
     */
    ModelDescriptionBox(const std::set < std::string > & lst,
                        vpz::BaseModel * model = 0);

    /**
     * return name written by user
     * @return a string representing name
     */
    inline std::string getName() const {
        return m_entry->get_text();
    }

    /**
     * run dialog and verify if no problem in input of window
     * @return true if model name and description is accepted, otherwise
     * false
     */
    bool run();

private :
    const std::set < std::string >& m_lst;
    Gtk::Entry*                     m_entry;
    vpz::BaseModel*                   m_model;
};

}
} // namespace vle gvle

#endif
