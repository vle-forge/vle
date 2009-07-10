/**
 * @file vle/gvle/conditions/DifferenceEquation/GenericPlugin.hpp
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

#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_GENERICPLUGIN_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_GENERICPLUGIN_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/Mapping.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace conditions {

class GenericPlugin
{
public:
    GenericPlugin();

    virtual ~GenericPlugin();

    /**
     * @brief create the port timestep
     *
     */
    void createTimeStep(vpz::Condition& condition);

    /*
     * @brief create the set for the init treeview
     *
     */
    void createSetInit(vpz::Condition& condition);

protected:
    class InitColumns : public Gtk::TreeModelColumnRecord
    {
    public:

	InitColumns() {
	    add(m_col_value);
	}

	Gtk::TreeModelColumn<double> m_col_value;
    };

    /* TreeView which shows the init values */
    class InitTreeView : public Gtk::TreeView
    {
    public:
	InitTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~InitTreeView();

	inline InitColumns* getColumns()
	  { return &m_columnsInit; }

	/**
	 * @brief fill the tree view
	 */
	void init(vpz::Condition& condition);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);

	//Signal handler for popup menu items:
	void onAdd();
	void onRemove();
	void onUp();
	void onDown();

    private:
	Gtk::Menu                    m_menuPopup;
	InitColumns                  m_columnsInit;
	Glib::RefPtr<Gtk::ListStore> m_refTreeInit;
    };

    Gtk::SpinButton*   m_spinTime;
    Gtk::Entry*        m_entryName;
    Gtk::Entry*        m_entryValue;
    Gtk::CheckButton*  m_checkGlobal;
    Gtk::HBox*         m_hboxTimestep;
    Gtk::ComboBoxText* m_comboUnit;

    InitTreeView*      m_initTreeView;

    /**
     * @brief create the box with the timestep
     *
     */
    void buildGeneric();

    /**
     * @brief fill the fields for timestep, name, value, init
     * @param condition the current condition
     *
     */
    void fillFieldsGeneric(vpz::Condition& condition);

    /**
     * @brief save the ports in the condition
     * @param condition the current condition
     *
     */
    void assignGeneric(vpz::Condition& condition);

    /**
     * @brief search if the port is already exist
     * @param condtion the current condition
     * @param the name of the port
     * @return true if the port exist
     *
     */
    bool existPort(vpz::Condition& condition, std::string name);

    /**
     * @brief delete the ports before the update
     * @param condition the current condition
     *
     */
    void deletePortsGeneric(vpz::Condition& condition);

    /**
     * @brief check if a port have a correct syntax
     * @param condition the current condition
     * @param name the name of the port
     * @return true if the port is correct
     */
    bool validPortGeneric(vpz::Condition& condition,const std::string& name);

    void onClickCheckButton();
};

}}} // namespace vle gvle conditions

#endif




