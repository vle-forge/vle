/**
 * @file vle/gvle/QuitBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <gtkmm.h>
#include <libglademm.h>

#ifndef GUI_QUITBOX_HH
#define GUI_QUITBOX_HH

namespace vle { namespace gvle {

class Modeling;

class QuitBox
{
public:
    QuitBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);
    virtual ~QuitBox() { }

    void show();
    void build();

protected:
    class FileTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        FileTreeColumn()
	    {
		add(mName);
	    }

        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

private:
    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                    mDialog;
    FileTreeColumn                  mColumns;
    Gtk::TreeView*                  mTreeView;
    Glib::RefPtr<Gtk::TreeStore>    mRefTreeFile;
    Modeling*                       mModeling;

    std::list < std::string >       mFileModified;

    //Buttons
    Gtk::Button*                    mButtonSave;
    Gtk::Button*                    mButtonCancel;
    Gtk::Button*                    mButtonClose;

    Gtk::Label*                     mMessage;

    void onSave();
    void onCancel();
    void onClose();
};

}} // namespace vle gvle

#endif
