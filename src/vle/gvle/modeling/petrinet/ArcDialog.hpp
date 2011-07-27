/*
 * @file vle/gvle/modeling/petrinet/ArcDialog.hpp
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


#ifndef VLE_GVLE_MODELING_PETRINET_ARC_DIALOG_HPP
#define VLE_GVLE_MODELING_PETRINET_ARC_DIALOG_HPP

#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <libglademm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

class ArcDialog
{
public:
    ArcDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
              const Arc* arc = 0);

    virtual ~ArcDialog()
    {
    }

    bool inhibitor() const
    {
        return mInhibitorCheckbox->get_active();
    }

    std::string weight() const
    {
        return mWeightEntry->get_text();
    }

    int run();

private:
    Gtk::Dialog* mDialog;
    Gtk::Entry* mWeightEntry;
    Gtk::CheckButton* mInhibitorCheckbox;

    const Arc* mArc;
};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
