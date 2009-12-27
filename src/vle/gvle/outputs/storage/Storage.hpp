/**
 * @file vle/gvle/outputs/storage/Storage.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_OUTPUTS_STORAGE_HPP
#define VLE_GVLE_OUTPUTS_STORAGE_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>

namespace vle { namespace gvle { namespace outputs {

class Storage : public OutputPlugin
{
public:
    Storage(const std::string& name);

    virtual ~Storage();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    Gtk::Dialog*        m_dialog;
    Gtk::SpinButton*    m_rows;
    Gtk::SpinButton*    m_columns;
    Gtk::SpinButton*    m_rzrows;
    Gtk::SpinButton*    m_rzcolumns;

    void init(vpz::Output& output);
    void assign(vpz::Output& output);
};

}}} // namespace vle gvle outputs

#endif
