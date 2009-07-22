/**
 * @file vle/gvle/CoupledModelBox.hpp
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


#ifndef GUI_COUPLEDMODELBOX_HPP
#define GUI_COUPLEDMODELBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <vle/graph/CoupledModel.hpp>

namespace vle
{
namespace graph {

class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

class CoupledModelBox : public Gtk::TreeView
{
public:
    CoupledModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);

    void show(graph::CoupledModel* model);

protected:
class ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:

        ModelColumns() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

private:

    class InputPortTreeView : public Gtk::TreeView
    {
    public:
	InputPortTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~InputPortTreeView();

	void build();
	void setModel(graph::CoupledModel* model)
	    { mModel = model; }

    protected:
	virtual bool on_button_press_event(GdkEventButton *ev);
	virtual void onAdd();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

    private:
	graph::CoupledModel* mModel;
	Gtk::Menu mMenuPopup;
	ModelColumns mColumnsInputPort;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelInputPort;

	int mColumnIn;
	bool mValidateRetry;
	std::string mOldName;
	Gtk::CellRendererText* mCellRenderer;
	Glib::ustring mInvalidTextForRetry;
    };

    class OutputPortTreeView : public Gtk::TreeView
    {
    public:
	OutputPortTreeView(BaseObjectType* cobject,
			   const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~OutputPortTreeView();

	void build();
	void setModel(graph::CoupledModel* model)
	    { mModel = model; }
    protected:
	virtual bool on_button_press_event(GdkEventButton *ev);

	virtual void onAdd();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

    private:
	graph::CoupledModel* mModel;
	Gtk::Menu mMenuPopup;
	ModelColumns mColumnsOutputPort;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelOutputPort;

	int mColumnOut;
	bool mValidateRetry;
	std::string mOldName;
	Gtk::CellRendererText* mCellRenderer;
	Glib::ustring mInvalidTextForRetry;
    };

    void on_validate();

    Glib::RefPtr<Gnome::Glade::Xml>        mXml;
    Modeling*                              mModeling;
    graph::CoupledModel*                   mModel;

    Gtk::Dialog*                           mDialog;
    Gtk::Label*                            mModelName;
    Gtk::Label*                            mModelNbChildren;


    InputPortTreeView*                     mInputPorts;
    OutputPortTreeView*                    mOutputPorts;


    Gtk::TreeView*                         mTreeViewOutput;
    ModelColumns                           mColumnsOutput;
    Glib::RefPtr<Gtk::ListStore>           mRefListOutput;

    Gtk::Button*                           mButtonValidate;

};

}
} // namespace vle gvle

#endif
