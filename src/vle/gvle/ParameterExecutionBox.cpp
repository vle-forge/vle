/**
 * @file vle/gvle/ParameterExecutionBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
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


#include <vle/gvle/ParameterExecutionBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>

using namespace vle;

namespace vle
{
namespace gvle {

ParameterExecutionBox::ParameterExecutionBox(Modeling* modeling) :
        Gtk::Dialog("Execution box", true, true),
        mModeling(modeling)
{
    assert(modeling);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    Gtk::Button* okButton = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    okButton->set_flags(Gtk::CAN_DEFAULT);
    okButton->grab_default();

    buildInterface();

    mEntryPort->set_width_chars(5);
    mEntryPort->set_max_length(5);
    mEntryPort->set_activates_default();
    mEntryPath->set_activates_default();
}

bool ParameterExecutionBox::run()
{
    show_all();
    for (;;) {
        int response = Gtk::Dialog::run();
        if (response == Gtk::RESPONSE_OK) {
            int port = utils::toInt(mEntryPort->get_text());
            if (0 < port and port < 65535) {
                mModeling->setSocketPort(port);
                return true;
            }
        } else {
            return false;
        }
    }
}

void ParameterExecutionBox::buildInterface()
{
    Gtk::Frame* local = Gtk::manage(new Gtk::Frame(_("Local execution")));
    Gtk::Label* labelPath = Gtk::manage(new Gtk::Label(_("VLE binary path:")));
    Gtk::Button* buttonPath = Gtk::manage(new Gtk::Button(_(" ... ")));
    Gtk::HBox* HBoxPath = Gtk::manage(new Gtk::HBox());
    mEntryPath = Gtk::manage(new Gtk::Entry());
    //mEntryPath->set_text(
    //Glib::build_filename(utils::Path::path().getDefaultBinDir(), "vle"));
    HBoxPath->pack_start(*labelPath, false, false);
    HBoxPath->pack_start(*mEntryPath, true, true);
    HBoxPath->pack_start(*buttonPath, false, false);
    HBoxPath->set_border_width(3);
    local->add(*HBoxPath);
    local->set_border_width(3);

    Gtk::Frame* remote = Gtk::manage(new Gtk::Frame(_("Remote execution")));
    Gtk::Label* port = Gtk::manage(new Gtk::Label(_("Network port:")));
    Gtk::HBox* HBoxRemote = Gtk::manage(new Gtk::HBox());
    mEntryPort = Gtk::manage(new Gtk::Entry());
    mEntryPort->set_text(utils::toString(mModeling->getSocketPort()));
    HBoxRemote->pack_start(*port, false, false);
    HBoxRemote->pack_start(*mEntryPort, true, true);
    HBoxRemote->set_border_width(3);
    remote->add(*HBoxRemote);
    remote->set_border_width(3);

    get_vbox()->pack_start(*local, false, false, 5);
    get_vbox()->pack_start(*remote, false, false, 5);

    buttonPath->signal_clicked().connect(
        sigc::mem_fun(*this, &ParameterExecutionBox::onClickButtonPath));
}

void ParameterExecutionBox::onClickButtonPath()
{
    Gtk::FileChooserDialog file(*this, _("Select VLE binary"),
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    if (file.run() == Gtk::RESPONSE_OK) {
        mEntryPath->set_text(file.get_filename());
    }
}

}
} // namespace vle gvle
