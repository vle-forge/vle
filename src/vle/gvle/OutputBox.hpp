/**
 * @file vle/gvle/OutputBox.hpp
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


#ifndef GUI_OUTPUTBOX_HPP
#define GUI_OUTPUTBOX_HPP

#include <gtkmm.h>
#include <vle/vpz/Output.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

class OutputBox : public Gtk::Dialog
{
public:
    OutputBox(vpz::Output& output);

    virtual ~OutputBox();

    void run();

private:
    vpz::Output* mOutput;

    //Format
    Gtk::HBox* mHbox1;
    Gtk::Label* mLabelFormat;
    Gtk::ComboBoxText* mFormat;

    //Location
    Gtk::HBox* mHbox2;
    Gtk::Label* mLabelLocation;
    Gtk::Entry* mEntryLocation;

    //Plugin
    Gtk::HBox* mHbox3;
    Gtk::Label* mLabelPlugin;
    Gtk::ComboBoxText* mPlugin;

    //Data
    Gtk::HBox* mHbox4;
    Gtk::Button* mData;

    //Backup
    const std::string mBackupData;

    void edit_data();
};

}
} // namespace vle gvle

#endif
