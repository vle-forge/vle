/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <gtkmm/builder.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treestore.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/dialog.h>

#ifndef GUI_QUITBOX_HH
#define GUI_QUITBOX_HH

namespace vle { namespace gvle {

// class Modeling;
class GVLE;

class QuitBox
{
public:
    QuitBox(const Glib::RefPtr < Gtk::Builder >& xml, GVLE* app);
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
    Glib::RefPtr < Gtk::Builder >   mXml;

    Gtk::Dialog*                    mDialog;
    FileTreeColumn                  mColumns;
    Gtk::TreeView*                  mTreeView;
    Glib::RefPtr<Gtk::TreeStore>    mRefTreeFile;
    GVLE*                           mApp;

    std::list < std::string >       mFileModified;

    //Buttons
    Gtk::Button*                    mButtonSave;
    Gtk::Button*                    mButtonCancel;
    Gtk::Button*                    mButtonClose;

    Gtk::Label*                     mMessage;

    void onSave();
    void onClose();
};

}} // namespace vle gvle

#endif
