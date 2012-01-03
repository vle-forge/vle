/*
 * @file vle/gvle/modeling/petrinet/ArcDialog.cpp
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


#include <vle/gvle/modeling/petrinet/ArcDialog.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

ArcDialog::ArcDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                     const Arc* arc) : mArc(arc)
{
    xml->get_widget("ArcDialog", mDialog);
    xml->get_widget("ArcWeightEntry", mWeightEntry);
    xml->get_widget("InhibitorArcCheckbox", mInhibitorCheckbox);
}

int ArcDialog::run()
{
    if (mArc) {
        mWeightEntry->set_text((fmt("%1%") % mArc->weight()).str());
        mInhibitorCheckbox->set_active(mArc->inhibitor());
    } else {
        mWeightEntry->set_text("1");
        mInhibitorCheckbox->set_active(false);
    }
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}
}
}
}
}    // namespace vle gvle modeling petrinet
