/**
 * @file vle/gvle/ModelClassBox.hpp
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


#ifndef GUI_MODELCLASSBOX_HPP
#define GUI_MODELCLASSBOX_HPP

#include <libglademm.h>
#include <gtkmm.h>
#include <vle/vpz/Classes.hpp>
#include <vle/gvle/NewModelClassBox.hpp>

namespace vle
{
namespace gvle {

class Modeling;

class ModelClassBox : public Gtk::TreeView
{
public:
    ModelClassBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);

    ~ModelClassBox();

    void show();

    void hide();

    bool is_visible() {
        return mWindow->is_visible();
    }

protected:

class ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    ModelColumns mColumns;

private:

    class ClassTreeView : public Gtk::TreeView
    {
    public:
	ClassTreeView(BaseObjectType* cobject,
		      const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~ClassTreeView();

	void build();
	void setModel(Modeling* modeling)
	    { mModeling = modeling; }

	void setBox(NewModelClassBox* newModelBox)
	    { mNewModelBox = newModelBox; }


    protected:
	virtual bool on_button_press_event(GdkEventButton *ev);
	virtual void onAdd();
	virtual void onRemove();
	virtual void onRename();

    private:
	Modeling* mModeling;
	Gtk::Menu mMenuPopup;
	ModelColumns mColumns;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModel;
	NewModelClassBox* mNewModelBox;
    };

    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    NewModelClassBox*                    mNewModelBox;
    Gtk::Menu                            mMenuPopup;

    //Backup
    vpz::ClassList*                      mClasses_backup;

    Gtk::Window*                         mWindow;
    Gtk::TreeView*                       mTreeView;
    Glib::RefPtr<Gtk::ListStore>         mRefTreeModel;

    ClassTreeView*                       mClass;
};

}
} // namespace vle gvle

#endif
