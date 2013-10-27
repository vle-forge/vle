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


#ifndef GUI_CONNECTIONBOX_HPP
#define GUI_CONNECTIONBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <vle/gvle/ComboboxString.hpp>

namespace vle
{
namespace vpz {

class CoupledModel;
class BaseModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

/**
 * @brief A Gtk::Window to allow user to connect Model Atomic or Coupled.
 */
class ConnectionBox : public Gtk::Dialog
{
public:
    ConnectionBox(vpz::CoupledModel* parent, vpz::BaseModel* src,
                  vpz::BaseModel* dst);

    /**
     * Assign to string name value of combobox input
     *
     * @param name: string to assign value
     */
    void getSelectedInputPort(std::string& name) const {
        name.assign(m_comboInput.get_active_string());
    }

    /**
     * Assign to string name value of combobox output
     *
     * @param name: string to assign value
     */
    void getSelectedOutputPort(std::string& name) const {
        name.assign(m_comboOutput.get_active_string());
    }

private:
    /**
     * Assign to combo box all inputs ports of model src if src equal
     * parent or ouputs otherwise.
     *
     * @param parent vpz::CoupledModel parent of src or equal
     * @param src children of parent
     */
    void assingComboInputPort(vpz::CoupledModel* parent,
                              vpz::BaseModel* src);

    /**
     * Assign to combo box all outputs ports of model dst if src equal
     * parent or inputs otherwise.
     *
     * @param parent vpz::CoupledModel parent of src or equal
     * @param src children of parent
     */
    void assingComboOutputPort(vpz::CoupledModel* parent,
                               vpz::BaseModel* dst);

private:
    Gtk::Box                   m_vbox;
    Gtk::Box                   m_hbox;
    Gtk::Label                  m_label;
    Gtk::Frame                  m_left;
    Gtk::Box                   m_left2;
    Gtk::Label                  m_labelInput;
    gvle::ComboBoxString     m_comboInput;
    Gtk::Frame                  m_right;
    Gtk::Box                   m_right2;
    Gtk::Label                  m_labelOutput;
    gvle::ComboBoxString     m_comboOutput;
};

}
} // namespace vle gvle

#endif
