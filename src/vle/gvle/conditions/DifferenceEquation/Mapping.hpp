/**
 * @file vle/gvle/conditions/DifferenceEquation/Mapping.hpp
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


#ifndef VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_MAPPING_HPP
#define VLE_GVLE_CONDITIONS_DIFFERENCEEQUATION_MAPPING_HPP

#include <vle/gvle/ConditionPlugin.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace conditions {

class Mapping
{
public:
    Mapping();

    virtual ~Mapping();

    /*
     * @brief create the map for the mapping treeview
     * @param condition the condition which have the port mapping
     *
     */
    void createMap(vpz::Condition& condition);

protected:
    class MappingColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        MappingColumns() {
            add(m_col_port);
            add(m_col_id);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_port;
        Gtk::TreeModelColumn<Glib::ustring> m_col_id;
    };

    /* TreeView which shows the mapping value */
    class MappingTreeView : public Gtk::TreeView
    {
    public:
	MappingTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~MappingTreeView();

	inline MappingColumns* getColumns()
	  { return &m_columnsMapping; }

	void setSizeColumn(int size);

	void init(vpz::Condition& condition);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);
	void onAdd();
	void onRemove();
	void onEdit();

    private:
	Gtk::Menu                    m_menuPopup;
	MappingColumns               m_columnsMapping;
	Glib::RefPtr<Gtk::ListStore> m_refTreeMapping;


	/* The dialog window to add a couple */
	Gtk::Dialog* m_dialog;
	Gtk::Entry*  m_port;
	Gtk::Entry*  m_id;


	/* Columns */
	int m_columnPort;
	int m_columnId;
    };

    /* Port Mapping */
    MappingTreeView*   m_mappingTreeView;

    /* Port Mode */
    Gtk::HBox*         m_hboxMode;
    Gtk::RadioButton*  m_RadioButtonName;
    Gtk::RadioButton*  m_RadioButtonPort;
    Gtk::RadioButton*  m_RadioButtonMapping;

    /**
     * @brief create the box with the mode
     *
     */
    void buildButtonMapping();

     /**
     * @brief fill the fields for mode and mapping
     * @param condition the current condition
     *
     */
    void fillFieldsMapping(vpz::Condition& condition);

    /**
     * @brief save the ports in the condition
     * @param condition the current condition
     *
     */
    void assignMode(vpz::Condition& condition);

    /**
     * @brief search if the port is already exist
     * @param condtion the current condition
     * @param the name of the port
     * @return true if the port exist
     *
     */
    bool existPortMapping(vpz::Condition& condition, const std::string& name);

    /**
     * @brief delete all ports before the update
     * @param condition the current condition
     *
     */
    void deletePortsMapping(vpz::Condition& condition);

    /**
     * @brief check if a port have a correct syntax
     * @param condition the current condition
     * @param name the name of the port
     * @return true if the port is correct
     */
    bool validPortMapping(vpz::Condition& condition, const std::string& name);

    /**
     * @brief return the name of the current RadioButton
     *
     */
    const std::string getCurrentRadioButton();

    void onClickRadioButton();
};

}}} // namespace vle gvle conditions

#endif



