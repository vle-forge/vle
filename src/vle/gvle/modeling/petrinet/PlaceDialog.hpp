/*
 * @file vle/gvle/modeling/petrinet/PlaceDialog.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_MODELING_PETRINET_PLACE_DIALOG_HPP
#define VLE_GVLE_MODELING_PETRINET_PLACE_DIALOG_HPP

#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <boost/algorithm/string.hpp>
#include <libglademm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

class PlaceDialog
{
public:
    PlaceDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                const PetriNet& petrinet, const Place* place = 0);
    virtual ~PlaceDialog();

    bool delay() const
    {
        return mDelayCheckbox->get_active();
    }

    std::string delayValue() const
    {
        return mDelayEntry->get_text();
    }

    std::string initialMarking() const
    {
        return mMarkingEntry->get_text();
    }

    std::string name() const
    {
        std::string entryName = mNameEntry->get_text();
        boost::trim(entryName);
        return entryName;
    }

    bool output() const
    {
        return mOutputCheckbox->get_active();
    }

    std::string outputPortName() const
    {
        std::string entryOutput = mOutputEntry->get_entry()->get_text();
        boost::trim(entryOutput);
        return entryOutput;
    }

    int run();

private:
    void onChangeName();
    void onDelay();
    void onInput();
    void onOutput();

    Gtk::Dialog* mDialog;
    Gtk::Entry* mNameEntry;
    Gtk::Image* mStatusName;
    // output
    Gtk::HBox* mOutputHBox;
    Gtk::CheckButton* mOutputCheckbox;
    Gtk::ComboBoxEntryText* mOutputEntry;
    // marking
    Gtk::Entry* mMarkingEntry;
    // delay
    Gtk::CheckButton* mDelayCheckbox;
    Gtk::Entry* mDelayEntry;
    Gtk::Button* mOkButton;
    const PetriNet& mPetriNet;
    const Place* mPlace;
    std::string mInitialName;

    std::string mTrimName;
    std::string mTrimOutput;
    std::list < sigc::connection > mList;
};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
