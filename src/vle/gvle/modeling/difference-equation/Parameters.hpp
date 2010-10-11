/*
 * @file vle/gvle/modeling/difference-equation/Parameters.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_PARAMETERS_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_PARAMETERS_HPP

#include <vle/vpz/Condition.hpp>
#include <libglademm.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

namespace vle { namespace gvle { namespace modeling { namespace de {

class Parameters
{
public:
    Parameters() : m_parametersTreeView(0), m_externalVariablesTreeView(0) { }
    virtual ~Parameters() { }

    struct Parameters_t
    {
        std::vector < std::string > mNames;
        std::map < std::string, double > mValues;
    };

    typedef std::vector < std::pair < std::string, bool > > ExternalVariables_t;

    void assign(vpz::Condition& condition);
    Gtk::Widget& build(Glib::RefPtr<Gnome::Glade::Xml> ref);
    Gtk::Widget& buildParameters(Glib::RefPtr<Gnome::Glade::Xml> ref);
    void deletePorts(vpz::Condition& condition);
    void fillFields(const Parameters_t& parameters);
    void fillFields(const Parameters_t& parameters,
		    const ExternalVariables_t& variables);

    Parameters_t getParameters() const
    { return m_parametersTreeView->getParameters(); }

    ExternalVariables_t getExternalVariables() const
    { if (m_externalVariablesTreeView) {
            return m_externalVariablesTreeView->getExternalVariables();
        } else {
            return ExternalVariables_t();
        }
    }

private:
    class ParameterColumns : public Gtk::TreeModelColumnRecord
    {
    public:
	ParameterColumns() {
	    add(m_col_name);
	    add(m_col_value);
	}

	Gtk::TreeModelColumn < std::string > m_col_name;
	Gtk::TreeModelColumn < std::string > m_col_value;
    };

    class ParametersTreeView : public Gtk::TreeView
    {
    public:
	ParametersTreeView(BaseObjectType* cobject,
                          const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~ParametersTreeView();

	Parameters_t getParameters() const;

	/**
	 * @brief fill the tree view
	 */
	void init(const Parameters_t& parameters);

	/**
	 * @brief check the existence of a value
	 */
	bool exist(const std::string& name);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);

	//Signal handler for popup menu items:
	void onSelect();
	void onAdd();
	void onRemove();
	void onEdit();

    private:
	Gtk::Menu                        m_menuPopup;
	ParameterColumns                 m_columnsParameter;
	Glib::RefPtr<Gtk::ListStore>     m_refTreeParameter;
	Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

	/* The dialog window to add a variable */
	Gtk::Dialog*  m_dialog;
	Gtk::Entry*   m_name;
	Gtk::Entry*   m_value;
    };

    class ExternalVariableColumns : public Gtk::TreeModelColumnRecord
    {
    public:
	ExternalVariableColumns() {
	    add(m_col_sync);
	    add(m_col_name);
	}

	Gtk::TreeModelColumn < bool > m_col_sync;
	Gtk::TreeModelColumn < std::string > m_col_name;
    };

    class ExternalVariablesTreeView : public Gtk::TreeView
    {
    public:
	ExternalVariablesTreeView(
	    BaseObjectType* cobject,
	    const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~ExternalVariablesTreeView();

	ExternalVariables_t getExternalVariables() const;

	/**
	 * @brief fill the tree view
	 */
	void init(const ExternalVariables_t& variables);

	/**
	 * @brief check the existence of a value
	 */
	bool exist(const std::string& name);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);

	//Signal handler for popup menu items:
	void onSelect();
	void onAdd();
	void onRemove();
	void onEdit();

    private:
	Gtk::Menu                        m_menuPopup;
	ExternalVariableColumns          m_columnsVariable;
	Glib::RefPtr<Gtk::ListStore>     m_refTreeVariable;
	Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

	/* The dialog window to add a variable */
	Gtk::Dialog*  m_dialog;
	Gtk::Entry*   m_name;
    };

    Gtk::HBox*                  m_hbox;
    ParametersTreeView*         m_parametersTreeView;
    ExternalVariablesTreeView*  m_externalVariablesTreeView;
    std::vector < std::string > m_oldParameters;
};

}}}} // namespace vle gvle modeling de

#endif

