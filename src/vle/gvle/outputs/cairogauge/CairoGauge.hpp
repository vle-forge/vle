/**
 * @file vle/gvle/outputs/cairogauge/CairoGauge.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_OUTPUTS_CAIROGAUGE_HPP
#define VLE_GVLE_OUTPUTS_CAIROGAUGE_HPP

#include <vle/gvle/OutputPlugin.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>

namespace vle { namespace gvle { namespace outputs {

class CairoGauge : public OutputPlugin
{
public:
    CairoGauge(const std::string& name);

    virtual ~CairoGauge();

    virtual bool start(vpz::Output& output, vpz::View& view);

private:
    Gtk::Dialog*        m_dialog;
    Gtk::SpinButton*    m_min;
    Gtk::SpinButton*    m_max;

    void init(vpz::Output& output);
    void assign(vpz::Output& output);
};

}}} // namespace vle gvle outputs

#endif
