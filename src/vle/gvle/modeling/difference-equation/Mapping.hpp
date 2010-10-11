/*
 * @file vle/gvle/modeling/difference-equation/Mapping.hpp
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_MAPPING_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_MAPPING_HPP

#include <vle/vpz/Condition.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/expander.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling { namespace de {

class Mapping
{
public:
    Mapping() { }
    virtual ~Mapping();

    void assign(vpz::Condition& condition);
    Gtk::Widget& build(Glib::RefPtr<Gnome::Glade::Xml> ref);
    void deletePorts(vpz::Condition& condition);
    void fillFields(const vpz::Condition& condition);
    std::string getMode() const;

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

    class MappingTreeView : public Gtk::TreeView
    {
    public:
	MappingTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
	virtual ~MappingTreeView() { }

	inline MappingColumns* getColumns()
	  { return &m_columnsMapping; }

	void init(const vpz::Condition& condition);

    protected:
	virtual bool on_button_press_event(GdkEventButton* ev);
	void onAdd();
	void onRemove();
	void onEdit();

    private:
	Gtk::Menu                    m_menuPopup;
	MappingColumns               m_columnsMapping;
	Glib::RefPtr<Gtk::ListStore> m_refTreeMapping;

	Gtk::Dialog* m_dialog;
	Gtk::Entry*  m_port;
	Gtk::Entry*  m_id;

	int m_columnPort;
	int m_columnId;
    };

private:
    Gtk::Expander*     m_expander;
    MappingTreeView*   m_mappingTreeView;
    Gtk::HBox*         m_hboxMode;
    Gtk::RadioButton*  m_RadioButtonName;
    Gtk::RadioButton*  m_RadioButtonPort;
    Gtk::RadioButton*  m_RadioButtonMapping;

    void createMap(vpz::Condition& condition);
    bool validPort(const vpz::Condition& condition,
                   const std::string& name);

    void onClickRadioButton();
};

}}}} // namespace vle gvle modeling de

#endif
