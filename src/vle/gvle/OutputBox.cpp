/**
 * @file vle/gvle/OutputBox.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/gvle/OutputBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <vle/utils/Path.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

OutputBox::OutputBox(vpz::Output& output):
        Gtk::Dialog("?",true,true),
        mOutput(&output),
        mBackupData(output.data())
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    set_title("Output " + output.name());

    //Format
    mHbox1 = new Gtk::HBox();
    mLabelFormat = new Gtk::Label("format : ");
    mFormat = new Gtk::ComboBoxText();
    mFormat->append_text("distant");
    mFormat->append_text("local");
    switch (output.format()) {
    case vle::vpz::Output::LOCAL:
        mFormat->set_active_text("local");
        break;
    case vle::vpz::Output::DISTANT:
        mFormat->set_active_text("distant");
        break;
    }
    mHbox1->pack_start(*mLabelFormat);
    mHbox1->pack_start(*mFormat);
    get_vbox()->pack_start(*mHbox1);

    //Location
    mHbox2 = new Gtk::HBox();
    mLabelLocation = new Gtk::Label("location : ");
    mEntryLocation = new Gtk::Entry();
    mEntryLocation->set_text(output.location());
    mHbox2->pack_start(*mLabelLocation);
    mHbox2->pack_start(*mEntryLocation);
    get_vbox()->pack_start(*mHbox2);

    //Plugin
    mHbox3 = new Gtk::HBox();
    mLabelPlugin = new Gtk::Label("plugin : ");
    mPlugin = new Gtk::ComboBoxText();

    {
        using namespace utils;
        Path::PathList paths = Path::path().getStreamDirs();
        Path::PathList::iterator it = paths.begin();
        while (it != paths.end()) {
            if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
                Glib::Dir rep(*it);
                Glib::DirIterator in = rep.begin();
                while (in != rep.end()) {
#ifdef G_OS_WIN32
                    if (((*in).find("lib") == 0) && ((*in).rfind(".dll") == (*in).size() - 4)) {
                        mPlugin->append_text((*in).substr(3, (*in).size() - 7));
                    }
#else
                    if (((*in).find("lib") == 0) && ((*in).rfind(".so") == (*in).size() - 3)) {
                        mPlugin->append_text((*in).substr(3, (*in).size() - 6));
                    }
#endif
                    in++;
                }
            }
            it++;
        }
    }
    mPlugin->set_active_text(output.plugin());
    mHbox3->pack_start(*mLabelPlugin);
    mHbox3->pack_start(*mPlugin);
    get_vbox()->pack_start(*mHbox3);

    //Data
    mHbox4 = new Gtk::HBox();
    mData = new Gtk::Button("Edit Data");
    mData->signal_clicked().connect(sigc::mem_fun(*this, &OutputBox::edit_data));
    mHbox4->pack_start(*mData);
    get_vbox()->pack_start(*mHbox4);

    set_size_request(220,150);
    show_all();
}

OutputBox::~OutputBox()
{
    hide_all();
    delete mHbox1;
    delete mFormat;
    delete mLabelFormat;
    delete mHbox2;
    delete mLabelLocation;
    delete mEntryLocation;
    delete mHbox3;
    delete mLabelPlugin;
    delete mPlugin;
    delete mHbox4;
    delete mData;
}

void OutputBox::run()
{
    switch (Gtk::Dialog::run()) {
    case Gtk::RESPONSE_APPLY:
        if (mFormat->get_active_text() == "local") {
            //local
            mOutput->setLocalStream(mEntryLocation->get_text() , mPlugin->get_active_text());
        } else {
            //distant
            mOutput->setDistantStream(mEntryLocation->get_text() , mPlugin->get_active_text());
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        mOutput->setData(mBackupData);
        break;
    }
}

void OutputBox::edit_data()
{
    std::string data = mOutput->data();
    XmlTypeBox box(data);
    box.run();
    mOutput->setData(data);
}

}
} // namespace vle gvle
