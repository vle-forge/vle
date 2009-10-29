/**
 * @file vle/gvle/conditions/DifferenceEquation/Multiple.hpp
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


#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_MULTIPLE_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_MULTIPLE_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/Mapping.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace conditions {

class Multiple : public ConditionPlugin, public Mapping
{
public:
    Multiple(const std::string& name);

    virtual ~Multiple();

    virtual bool start(vpz::Condition& condition);
    virtual bool start(vpz::Condition&,
	       const std::string&)
	{ return true; }

    std::vector<double>& getVectorValues(const std::string& name)
	{ return m_listValues[name]; }

protected:

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

private:

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

	inline void setParent(Multiple* parent)
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
	Multiple*                    m_parent;

	void buildMenu();

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onRemove();
    };


    /* TreeView to show variable */
    class VariableTreeView : public Gtk::TreeView
    {
    public:
	VariableTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~VariableTreeView();

	inline VariableColumns* getColumns()
	  { return &m_columnsVariable; }

	/**
	 * @brief fill the tree view
	 */
	void init(vpz::Condition& condition);

	/**
	 * @brief fill the list of values
	 */
	void initList(const std::string& name, value::Value* value);

	/**
	 * @brief check the existence of a value
	 */
	bool exist(const std::string& name);

	void setParent(Multiple* parent)
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
	Multiple*                        m_parent;

	/* The dialog window to add a variable */
	Gtk::Dialog*  m_dialog;
	Gtk::Entry*   m_name;
	Gtk::Entry*   m_value;
    };

    Gtk::Dialog*       m_dialog;
    Gtk::SpinButton*   m_spinTime;
    Gtk::CheckButton*  m_checkGlobal;
    Gtk::HBox*         m_hboxTimestep;
    Gtk::ComboBoxText* m_comboUnit;

    VariableTreeView*  m_variableTreeView;
    ValuesTreeView*    m_valuesTreeView;

    std::map < std::string, std::vector < double > > m_listValues;

    void build();

    /**
     * @brief fill the fields for timestep, name, value, init
     * @param condition the current condition
     *
     */
    void fillFields(vpz::Condition& condition);

    /**
     * @brief save the ports in the condition
     * @param condition the current condition
     *
     */
    void assign(vpz::Condition& condition);

    /**
     * @brief fill the values TreeView
     * @param name the name of the current variable
     *
     */
    void buildTreeValues(const std::string& name);

    /*
     * @brief search if the port is already exist
     * @param condtion the current condition
     * @ param the name of the port
     * @return true if the port exist
     */
    bool existPort(vpz::Condition& condition, const std::string& name);

    /**
     * @brief check if a port have a correct syntax
     * @param condition the current condition
     * @param name the name of the port
     * @return true if the port is correct
     */
    bool validPort(vpz::Condition& condition, const std::string& name);

    /*
     * @brief create the set for the init treeview
     *
     */
    void createSetVariable(vpz::Condition& condition);

    /**
     * @brief delete the ports before the update
     * @param condition the current condition
     *
     */
    void deletePorts(vpz::Condition& condition);

    void onClickCheckButton();
};

}}} // namespace vle gvle conditions

#endif

