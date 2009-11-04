/**
 * @file vle/gvle/conditions/DifferenceEquation/Variables.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_VARIABLES_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_VARIABLES_HPP

#include <vle/vpz/Condition.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace conditions {

class Variables
{
public:
    Variables() { }
    virtual ~Variables() { }

    std::vector<double>& getVectorValues(const std::string& name)
	{ return m_listValues[name]; }

protected:
    typedef std::vector < std::string > Variables_t;

    void assign(vpz::Condition& condition);
    Gtk::Widget& build(Glib::RefPtr<Gnome::Glade::Xml> ref);
    void deletePorts(vpz::Condition& condition);
    void fillFields(const vpz::Condition& condition);
    Variables_t getVariables() const
    { return m_variablesTreeView->getVariables(); }

private:
    void buildTreeValues(const std::string& name);

    class ValueColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ValueColumns() {
            add(m_col_value);
        }

        Gtk::TreeModelColumn<double> m_col_value;
    };

    class VariableColumns : public Gtk::TreeModelColumnRecord
    {
    public:

	VariableColumns() {
	    add(m_col_name);
	    add(m_col_value);
	}

	Gtk::TreeModelColumn < std::string > m_col_name;
	Gtk::TreeModelColumn < std::string > m_col_value;
    };

    /* TreeView to show the values of a variable */
    class ValuesTreeView : public Gtk::TreeView
    {
    public:
	ValuesTreeView(BaseObjectType* cobject,
		       const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~ValuesTreeView();

	void clear();
	void makeTreeView();

	/**
	 * @brief set the current variable for these values
	 *
	 */
	inline void setVariable(const std::string& name)
	  { m_variable = name; }

	inline void setParent(Variables* parent)
	  { m_parent = parent; }

	inline ValueColumns* getColumns()
	  { return &m_columnsValues; }

    protected:
	virtual bool on_button_press_event(GdkEventButton* e);

    private:
	ValueColumns                 m_columnsValues;
	Gtk::Menu                    mMenu;
	Glib::RefPtr<Gtk::ListStore> m_refTreeValues;
	std::string                  m_variable;
	Variables*                   m_parent;

	void buildMenu();

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onRemove();
    };


    /* TreeView to show variable */
    class VariablesTreeView : public Gtk::TreeView
    {
    public:
	VariablesTreeView(BaseObjectType* cobject,
                          const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~VariablesTreeView();

	inline VariableColumns* getColumns()
	  { return &m_columnsVariable; }

	Variables_t getVariables() const;

	/**
	 * @brief fill the tree view
	 */
	void init(const vpz::Condition& condition);

	/**
	 * @brief fill the list of values
	 */
	void initList(const std::string& name, value::Value* value);

	/**
	 * @brief check the existence of a value
	 */
	bool exist(const std::string& name);

	void setParent(Variables* parent)
	    { m_parent = parent; }

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);

	//Signal handler for popup menu items:
	void onSelect();
	void onAdd();
	void onRemove();
	void onEdit();

    private:
	Gtk::Menu                        m_menuPopup;
	VariableColumns                  m_columnsVariable;
	Glib::RefPtr<Gtk::ListStore>     m_refTreeVariable;
	Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;
	Variables*                       m_parent;

	/* The dialog window to add a variable */
	Gtk::Dialog*  m_dialog;
	Gtk::Entry*   m_name;
	Gtk::Entry*   m_value;
    };

    Gtk::Frame*        m_frame;
    VariablesTreeView* m_variablesTreeView;
    ValuesTreeView*    m_valuesTreeView;

    std::map < std::string, std::vector < double > > m_listValues;

    bool validPort(const vpz::Condition& condition, const std::string& name);

};

}}} // namespace vle gvle conditions

#endif

