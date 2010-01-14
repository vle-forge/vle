/**
 * @file vle/gvle/conditions/DifferenceEquation/NameValue.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_NAMEVALUE_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_NAMEVALUE_HPP

#include <vle/vpz/Condition.hpp>
#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling {

class NameValue
{
public:
    NameValue() { }
    virtual ~NameValue() { }

    std::string getVariableName() const
	{ return m_entryName->get_text(); }

protected:
    class InitColumns : public Gtk::TreeModelColumnRecord
    {
    public:
	InitColumns() {
	    add(m_col_value);
	}

	Gtk::TreeModelColumn<double> m_col_value;
    };

    class InitTreeView : public Gtk::TreeView
    {
    public:
	InitTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~InitTreeView();

	const InitColumns& getColumns() const { return m_columnsInit; }

	void init(const vpz::Condition& condition);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);

	void onAdd();
	void onRemove();
	void onUp();
	void onDown();

    private:
	Gtk::Menu                    m_menuPopup;
	InitColumns                  m_columnsInit;
	Glib::RefPtr<Gtk::ListStore> m_refTreeInit;
    };

    void assign(vpz::Condition& condition);
    Gtk::Widget& build(Glib::RefPtr<Gnome::Glade::Xml> ref);
    void deletePorts(vpz::Condition& condition);
    void fillFields(const vpz::Condition& condition);

private:
    void createSetInit(vpz::Condition& condition);
    bool validPort(const vpz::Condition& condition,
                   const std::string& name);

    void onClickCheckButton();

    Gtk::HBox*         m_hbox;
    Gtk::Entry*        m_entryName;
    Gtk::Entry*        m_entryValue;
    InitTreeView*      m_initTreeView;
};

}}} // namespace vle gvle modeling

#endif




