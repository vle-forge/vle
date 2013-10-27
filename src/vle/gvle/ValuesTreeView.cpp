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


#include <vle/gvle/ValuesTreeView.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/BooleanBox.hpp>
#include <vle/gvle/MatrixBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TableBox.hpp>
#include <vle/gvle/TupleBox.hpp>
#include <vle/gvle/ValueBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

ValuesTreeView::ValuesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject)
{
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);
    buildMenu();
}

ValuesTreeView::~ValuesTreeView()
{
}

void ValuesTreeView::buildMenu()
{
    mMenuActionGroup = Gtk::ActionGroup::create("buildMenu");
        
        /* Partie Menu Principal */
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextMenu", _("Context Menu")));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextRemove", _("_Remove")),
            sigc::mem_fun(*this, &ValuesTreeView::on_menu_remove));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextRename", _("_Duplicate")),
            sigc::mem_fun(*this, &ValuesTreeView::on_menu_duplicate));
        
        /* Partie Sous Menu */
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextInsert", _("Insert")));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextBoolean", _("_Boolean")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::BOOLEAN));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextDouble", _("_Double")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::DOUBLE));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextInteger", _("_Integer")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::INTEGER));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextMap", _("_Map")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::MAP));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextMatrix", _("_Matrix")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::MATRIX));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextNull", _("_Null")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::NIL));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextSet", _("_Set")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::SET));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextString", _("_String")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::STRING));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextTable", _("_Table")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::TABLE));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextTuple", _("_Tuple")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::TUPLE));
        
        mMenuActionGroup->add(Gtk::Action::create("VTView_ContextXml", _("_Xml")),
            sigc::bind(sigc::mem_fun(*this, &ValuesTreeView::on_menu_insert), value::Value::XMLTYPE));
        
        
        mUIManager = Gtk::UIManager::create();
        mUIManager->insert_action_group(mMenuActionGroup);
        
        Glib::ustring ui_info =
            "<ui>"
            "  <popup name='VTView_Popup'>"
            "      <menu action='VTView_ContextInsert'>"
            "        <menuitem action='VTView_ContextBoolean'/>"
            "        <menuitem action='VTView_ContextDouble'/>"
            "        <menuitem action='VTView_ContextInteger'/>"
            "        <menuitem action='VTView_ContextMap'/>"
            "        <menuitem action='VTView_ContextMatrix'/>"
            "        <menuitem action='VTView_ContextNull'/>"
            "        <menuitem action='VTView_ContextSet'/>"
            "        <menuitem action='VTView_ContextString'/>"
            "        <menuitem action='VTView_ContextTable'/>"
            "        <menuitem action='VTView_ContextTuple'/>"
            "        <menuitem action='VTView_ContextXml'/>"
            "      </menu>"
            "      <menuitem action='VTView_ContextRemove'/>"
            "      <menuitem action='VTView_ContextRename'/>"
            "  </popup>"
            "</ui>";
    
        try {
            mUIManager->add_ui_from_string(ui_info);
            mMenu = (Gtk::Menu *) (
                mUIManager->get_widget("/VTView_Popup"));
        } catch(const Glib::Error& ex) {
            std::cerr << "building menus failed: VTView_Popup \n" <<  ex.what() << "\n\n";
        } 
	
        if (!mMenu)
            std::cerr << "menu not found : VTView_Popup \n\n";
        
    
    mMenu->show_all();
}

void ValuesTreeView::clear()
{
    remove_all_columns();
}

void ValuesTreeView::makeTreeView(value::Set& set)
{
    mValue = &set;

    m_refTreeModel->clear();
    clear();
    //Add the TreeView's view columns:
    append_column("Type", m_Columns.m_col_type);
    append_column("Overview", m_Columns.m_col_view);

    value::VectorValue vector = set.value();
    value::VectorValue::iterator it = vector.begin();

    while (it != vector.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        switch ((*it)->getType()) {
        case value::Value::BOOLEAN:
            row[m_Columns.m_col_type] = "boolean";
            break;
        case value::Value::INTEGER:
            row[m_Columns.m_col_type] = "integer";
            break;
        case value::Value::DOUBLE:
            row[m_Columns.m_col_type] = "double";
            break;
        case value::Value::STRING:
            row[m_Columns.m_col_type] = "string";
            break;
        case value::Value::SET:
            row[m_Columns.m_col_type] = "set";
            break;
        case value::Value::MAP:
            row[m_Columns.m_col_type] = "map";
            break;
        case value::Value::TUPLE:
            row[m_Columns.m_col_type] = "tuple";
            break;
        case value::Value::TABLE:
            row[m_Columns.m_col_type] = "table";
            break;
        case value::Value::XMLTYPE:
            row[m_Columns.m_col_type] = "xml";
            break;
        case value::Value::NIL:
            row[m_Columns.m_col_type] = "null";
            break;
        case value::Value::MATRIX:
            row[m_Columns.m_col_type] = "matrix";
            break;
        default:
            break;
        }
        row[m_Columns.m_col_view] =
	    boost::trim_copy((*it)->writeToString()).substr(0,
                                                            GVLE::overview_max);
        row[m_Columns.m_col_value] = *it;

        it++;
    }
}

void ValuesTreeView::makeTreeView(value::Map& map)
{
    mValue = &map;

    m_refTreeModel->clear();
    clear();
    //Add the TreeView's view columns:
    append_column("Key", m_Columns.m_col_name);
    append_column("Type", m_Columns.m_col_type);
    append_column("Overview", m_Columns.m_col_view);

    const value::MapValue& values = map.value();
    value::MapValue::const_iterator it = values.begin();
    while (it != values.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        row[m_Columns.m_col_name] = it->first;
        switch (it->second->getType()) {
            using namespace value;
        case Value::BOOLEAN:
            row[m_Columns.m_col_type] = "boolean";
            break;
        case Value::INTEGER:
            row[m_Columns.m_col_type] = "integer";
            break;
        case Value::DOUBLE:
            row[m_Columns.m_col_type] = "double";
            break;
        case Value::STRING:
            row[m_Columns.m_col_type] = "string";
            break;
        case Value::SET:
            row[m_Columns.m_col_type] = "set";
            break;
        case Value::MAP:
            row[m_Columns.m_col_type] = "map";
            break;
        case Value::TUPLE:
            row[m_Columns.m_col_type] = "tuple";
            break;
        case Value::TABLE:
            row[m_Columns.m_col_type] = "table";
            break;
        case Value::XMLTYPE:
            row[m_Columns.m_col_type] = "xmltype";
            break;
        case Value::NIL:
            row[m_Columns.m_col_type] = "null";
            break;
        case Value::MATRIX:
            row[m_Columns.m_col_type] = "matrix";
            break;
        default:
            break;
        }
        row[m_Columns.m_col_view] =
	    boost::trim_copy(it->second->writeToString()).substr(
                0, GVLE::overview_max);
        row[m_Columns.m_col_value] = it->second;
        it++;
    }
}

bool ValuesTreeView::on_button_press_event(GdkEventButton* event)
{
    if (mCondition and event->type == GDK_BUTTON_PRESS and event->button == 3) {
        mMenu->popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

void ValuesTreeView::on_row_activated(const Gtk::TreeModel::Path& path,
                                     Gtk::TreeViewColumn* /*column*/)
{
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        value::Value* v =  &*row.get_value(m_Columns.m_col_value);

        if (row[m_Columns.m_col_type] == "map") {
            value::Map* map = dynamic_cast<value::Map*>(v);
            ValueBox box(map);
            box.run();
        } else if (row[m_Columns.m_col_type] == "set") {
            value::Set* set = dynamic_cast<value::Set*>(v);
            ValueBox box(set);
            box.run();
        } else if (row[m_Columns.m_col_type] == "boolean") {
            value::Boolean* boolean =
		dynamic_cast<value::Boolean*>(v);
            BooleanBox box(boolean);
            box.run();
        } else if (row[m_Columns.m_col_type] == "integer" or
                   row[m_Columns.m_col_type] == "double" or
                   row[m_Columns.m_col_type] == "string") {
            SimpleTypeBox box(v);
            box.run();
        } else if (row[m_Columns.m_col_type] == "tuple") {
            value::Tuple* tuple = dynamic_cast<value::Tuple*>(v);
            TupleBox box(tuple);
            box.run();
        } else if (row[m_Columns.m_col_type] == "table") {
            value::Table* table = dynamic_cast<value::Table*>(v);
            TableBox box(table);
            box.run();
        } else if (row[m_Columns.m_col_type] == "matrix") {
            value::Matrix* matrix =
		dynamic_cast<value::Matrix*>(v);
            MatrixBox box(matrix);
            box.run();
        } else if (row[m_Columns.m_col_type] == "xml") {
            value::Xml* xml = dynamic_cast<value::Xml*>(v);
            XmlTypeBox box(xml);
            box.run();
        }
        value::Value* val =  &*row.get_value(m_Columns.m_col_value);
        row[m_Columns.m_col_view] =
	    boost::trim_copy(val->writeToString()).substr(0,
                                                          GVLE::overview_max);
        refresh();
    }
}

void ValuesTreeView::on_menu_insert(value::Value::type type)
{
    if (mValue->getType() ==  value::Value::SET) {
        value::Set& set = mValue->toSet();
        value::VectorValue& vector = set.value();
        value::VectorValue::iterator it;

        Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
        if (refSelection) {
            Gtk::TreeModel::iterator iter = refSelection->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                value::Value* base = row.get_value(m_Columns.m_col_value);
                it = std::find(vector.begin(), vector.end(), base);
            } else {
                it = vector.end();
            }
        }
        switch (type) {
            using namespace value;
        case(Value::BOOLEAN):
            vector.insert(it, Boolean::create());
            break;
        case(Value::INTEGER):
            vector.insert(it, Integer::create());
            break;
        case(Value::DOUBLE):
            vector.insert(it, Double::create());
            break;
        case(Value::STRING):
            vector.insert(it, String::create());
            break;
        case(Value::SET):
            vector.insert(it, Set::create());
            break;
        case(Value::MAP):
            vector.insert(it, Map::create());
            break;
        case(Value::TUPLE): {
            SimpleTypeBox box("Size ?", "1");
            int s = utils::to < int32_t >(box.run());
            if(s<1) {
                gvle::Error(_("The size can not be null or negative"));
                break;
            }
            value::Tuple* tmp = Tuple::create();
            tmp->value().resize(s);
            vector.insert(it, tmp);
        }
            break;
        case(Value::TABLE): {
            SimpleTypeBox box("Width ?", "1");
            int w = utils::to < int32_t >(box.run());
            if (w < 1) {
                gvle::Error(_("The size can not be null or negative"));
                break;
            }
            SimpleTypeBox box2("Height ?", "1");
            int h = utils::to < int32_t >(box2.run());
            if (h < 1) {
                gvle::Error(_("The size can not be null or negative"));
                break;
            }
            vector.insert(it, Table::create(w, h));
        }
            break;
        case(Value::XMLTYPE):
            vector.insert(it, Xml::create());
            break;
        case(Value::NIL):
            vector.insert(it, Null::create());
            break;
        case(Value::MATRIX): {
            SimpleTypeBox box("Columns ?", "1");
            int cols = utils::to < int32_t >(box.run());
            if (cols < 1) {
                gvle::Error(_("The size can not be null or negative"));
                break;
            }
            SimpleTypeBox box2("Rows ?", "1");
            int rows = utils::to < int32_t >(box2.run());
            if (rows < 1) {
                gvle::Error(_("The size can not be null or negative"));
                break;
            }
            Matrix* matrix = Matrix::create(cols, rows, cols * 2, rows * 2,
                                            cols / 5, rows / 5);
            vector.insert(it, matrix);
        }
            break;
        default:
            break;
        }
        refresh();
    } else if (mValue->getType() ==  value::Value::MAP) {
        value::Map* map = dynamic_cast<value::Map*>(mValue);
        SimpleTypeBox box("name ?", "");
        std::string name;
        do {
            name = box.run();
        } while (map->exist(name));

        switch (type) {
            using namespace value;
        case(Value::BOOLEAN):
            map->addBoolean(name, false);
            break;
        case(Value::INTEGER):
            map->addInt(name, 0);
            break;
        case(Value::DOUBLE):
            map->addDouble(name, 0);
            break;
        case(Value::STRING):
            map->addString(name, "");
            break;
        case(Value::SET):
            map->addSet(name);
            break;
        case(Value::MAP):
            map->addMap(name);
            break;
        case(Value::TUPLE):
            map->addTuple(name);
            break;
        case(Value::TABLE): {
            SimpleTypeBox box("Width ?", "1");
            int w = utils::to < int32_t >(box.run());
            SimpleTypeBox box2("Height ?", "1");
            int h = utils::to < int32_t >(box2.run());
            map->add(name, Table::create(w, h));
        }
        break;
        case(Value::XMLTYPE):
        map->addXml(name, "");
        break;
        case(Value::NIL):
        map->add(name, Null::create());
        break;
        case(Value::MATRIX): {
            SimpleTypeBox box("Columns ?", "1");
            int cols = utils::to < int32_t >(box.run());
            SimpleTypeBox box2("Rows ?", "1");
            int rows = utils::to < int32_t >(box2.run());
            Matrix* matrix = Matrix::create(cols, rows,
                                            cols * 2, rows * 2,
                                            cols / 5, rows / 5);
            map->add(name, matrix);
        }
        break;
        default:
        break;
        }
        refresh();
    }
}

void ValuesTreeView::on_menu_remove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
            value::Value* base = &*row.get_value(m_Columns.m_col_value);

            if (mValue->getType() ==  value::Value::SET) {
                value::Set& set = mValue->toSet();
                value::VectorValue& vector = set.value();
                value::VectorValue::iterator it = vector.begin();

                while (it != vector.end()) {
                    if (&**it == base)
                        break;
                    ++it;
                }

                if (it != vector.end()) {
                    vector.erase(it);
                    refresh();
                }

            } else if (mValue->getType() ==  value::Value::MAP) {
                value::Map& mp = mValue->toMap();
                value::MapValue& map = mp.value();
                std::string name = "" + row[m_Columns.m_col_name];
                value::MapValue::iterator it = map.find(name);

                if (it != map.end()) {
                    map.erase(it);
                    refresh();
                }

            }
        }
    }
}

void ValuesTreeView::on_menu_duplicate()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            Gtk::TreeModel::Row row = *iter;
            value::Value* base = &*row.get_value(m_Columns.m_col_value);

            if (mValue->getType() ==  value::Value::SET) {
                value::Set& set = mValue->toSet();
                value::VectorValue& vector = set.value();
                value::VectorValue::iterator it = vector.end();

                vector.insert(it, base->clone());

                refresh();
            } else if (mValue->getType() ==  value::Value::MAP) {
                value::Map& mp = mValue->toMap();
                value::MapValue& map = mp.value();
                std::string name = "" + row[m_Columns.m_col_name];
                int number = 1;
                std::string copy;
                value::MapValue::iterator it;
                do {
                    copy = name
                        + "_"
                        + boost::lexical_cast< std::string >(number);
                    ++number;
                    it = map.find(copy);
                } while (it != map.end());

                map[copy] = base->clone();

                refresh();
            }
        }
    }
}

void ValuesTreeView::refresh()
{
    if (mValue->isSet()) {
        makeTreeView(*(dynamic_cast<value::Set*>(mValue)));
    } else if (mValue->isMap()) {
        makeTreeView(*(dynamic_cast<value::Map*>(mValue)));
    }
}

} } // namespace vle gvle
