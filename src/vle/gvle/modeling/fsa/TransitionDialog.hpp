/*
 * @file vle/gvle/modeling/fsa/TransitionDialog.hpp
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


#ifndef VLE_GVLE_MODELING_FSA_TRANSITION_DIALOG_HPP
#define VLE_GVLE_MODELING_FSA_TRANSITION_DIALOG_HPP

#include <vle/gvle/modeling/fsa/Statechart.hpp>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <libglademm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

class TransitionDialog
{
public:
    TransitionDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                     const Transition* transition, Statechart* statechart);

    virtual ~TransitionDialog();

    std::string action() const
    {
        return mActionEntry->get_entry()->get_text();
    }

    std::string after() const
    {
        return mAfterEntry->get_entry()->get_text();
    }

    std::string event() const
    {
        return mEventEntry->get_entry()->get_text();
    }

    std::string guard() const
    {
        return mGuardEntry->get_entry()->get_text();
    }

    const strings_t& inputPorts() const
    {
        return mStatechart->inputPorts();
    }

    const strings_t& outputPorts() const
    {
        return mStatechart->outputPorts();
    }

    std::string send() const
    {
        if (mSendEntry->get_entry()->get_text().empty()) {
            return mOutputPortEntry->get_entry()->get_text();
        } else {
            return mSendEntry->get_entry()->get_text();
        }
    }

    function_type typeSend() const
    {
        return mSendEntry->get_entry()->get_text().empty() ?
               CONSTANT : FUNCTION;
    }

    std::string when() const
    {
        return mWhenEntry->get_entry()->get_text();
    }

    int run();

    bool isValidName(std::string name)
    {
        size_t i = 0;
        if (isdigit(name[0])) {
            return false;
        } else {
            while (i < name.length()) {
                if (!isalnum(name[i])) {
                    return false;
                }
                i++;
            }
            return true;
        }
    }

private:
    void onAction();
    void onActionSource();
    void onAfter();
    void onAfterSource();
    void onGuard();
    void onGuardSource();
    void onSend();
    void onSendSource();
    void onWhen();
    void onWhenSource();

    Glib::RefPtr < Gnome::Glade::Xml > mXml;
    const Transition* mTransition;
    Statechart* mStatechart;

    Gtk::Dialog* mDialog;
    Gtk::VBox* mComboVBox;
    // event
    Gtk::ComboBoxEntryText* mEventEntry;
    // after
    Gtk::ComboBoxEntryText* mAfterEntry;
    Gtk::Button* mAfterButton;
    // when
    Gtk::ComboBoxEntryText* mWhenEntry;
    Gtk::Button* mWhenButton;
    // guard
    Gtk::ComboBoxEntryText* mGuardEntry;
    Gtk::Button* mGuardButton;
    // action
    Gtk::ComboBoxEntryText* mActionEntry;
    Gtk::Button* mActionButton;
    // send
    Gtk::HBox* mSendHBox;
    Gtk::ComboBoxEntryText* mOutputPortEntry;
    Gtk::ComboBoxEntryText* mSendEntry;
    Gtk::Button* mSendButton;

    Gtk::Button* mOkButton;

    std::string mTrimAction;
    std::string mTrimAfter;
    std::string mTrimWhen;
    std::string mTrimSend;
    std::string mTrimGuard;
    std::list < sigc::connection > mList;
};

}
}
}
}    // namespace vle gvle modeling fsa

#endif
