/**
 * @file vle/gvle/TreeViewValue.cpp
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


#include <vle/gvle/BooleanBox.hpp>
#include <vle/gvle/MatrixBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TableBox.hpp>
#include <vle/gvle/TreeViewValue.hpp>
#include <vle/gvle/ValueBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {
TreeViewValue::TreeViewValue()
{
    //Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);

    {
        using namespace Gtk::Menu_Helpers;
        using namespace value;
        //Fill popup menu:
        Gtk::Menu::MenuList& items = mMenu.items();

        items.push_back(MenuElem("Insert"));

        Gtk::MenuItem* pMenuItem = &items.back();
        Gtk::Menu* pMenu = Gtk::manage(new Gtk::Menu());
        MenuList insert = pMenu->items();
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Boolean", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::BOOLEAN)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Double", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::DOUBLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Integer", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::INTEGER)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Map", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::MAP)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Matrix", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::MATRIX)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Null", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::NIL)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Set", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::SET)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("String", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::STRING)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Table", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::TABLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Tuple", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::TUPLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Xml", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::XMLTYPE)));

        pMenuItem->set_submenu(*pMenu);

        items.push_back(Gtk::Menu_Helpers::MenuElem("Remove",
                        sigc::mem_fun(*this, &TreeViewValue::on_menu_remove)));
    }
}

TreeViewValue::~TreeViewValue()
{
}

void TreeViewValue::makeTreeView(value::Set& set)
{
    mValue = &set;

    m_refTreeModel->clear();
    remove_all_columns();
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
        row[m_Columns.m_col_view] = boost::trim_copy((*it)->writeToString()).substr(0, GVLE::overview_max);
        row[m_Columns.m_col_value] = *it;

        it++;
    }
}

void TreeViewValue::makeTreeView(value::Map& map)
{
    mValue = &map;

    m_refTreeModel->clear();
    remove_all_columns();
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

        row[m_Columns.m_col_view] = boost::trim_copy(it->second->writeToString()).substr(0, GVLE::overview_max);
        row[m_Columns.m_col_value] = it->second;
        it++;
    }
}

bool TreeViewValue::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
        mMenu.popup(event->button, event->time);
    }
    return TreeView::on_button_press_event(event);
}

void TreeViewValue::on_row_activated(const Gtk::TreeModel::Path& path,
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
            value::Boolean* boolean = dynamic_cast<value::Boolean*>(v);
            BooleanBox box(boolean);
            box.run();
        } else if (row[m_Columns.m_col_type] == "integer" or
                   row[m_Columns.m_col_type] == "double" or
                   row[m_Columns.m_col_type] == "string" or
                   row[m_Columns.m_col_type] == "tuple") {
            SimpleTypeBox box(v);
            box.run();
        } else if (row[m_Columns.m_col_type] == "table") {
            value::Table* table = dynamic_cast<value::Table*>(v);
            TableBox box(table);
            box.run();
        } else if (row[m_Columns.m_col_type] == "matrix") {
            value::Matrix* matrix = dynamic_cast<value::Matrix*>(v);
            MatrixBox box(matrix);
            box.run();
        } else if (row[m_Columns.m_col_type] == "xml") {
            value::Xml* xml = dynamic_cast<value::Xml*>(v);
            XmlTypeBox box(xml);
            box.run();
        }
        value::Value* val =  &*row.get_value(m_Columns.m_col_value);
        row[m_Columns.m_col_view] = boost::trim_copy(val->writeToString()).substr(0, GVLE::overview_max);
        refresh();
    }
}

void TreeViewValue::on_menu_insert(value::Value::type type)
{
    if (mValue->getType() ==  value::Value::SET) {
        value::Set* set = dynamic_cast<value::Set*>(mValue);
        value::VectorValue& vector = set->value();
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
        case(Value::TUPLE):
                        vector.insert(it, Tuple::create());
            break;
        case(Value::TABLE): {
            SimpleTypeBox box("Width ?");
            int w = utils::to_int(box.run());
            SimpleTypeBox box2("Height ?");
            int h = utils::to_int(box2.run());
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
            SimpleTypeBox box("columns ?");
            int cols = utils::to_int(box.run());
            SimpleTypeBox box2("rows ?");
            int rows = utils::to_int(box2.run());
            Matrix* matrix = Matrix::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
            vector.insert(it, matrix);
        }
        break;
        }
        refresh();
    } else if (mValue->getType() ==  value::Value::MAP) {
        value::Map* map = dynamic_cast<value::Map*>(mValue);
        SimpleTypeBox box("name ?");
        std::string name;
        do {
            name = box.run();
        } while (map->existValue(name));

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
                        map->addClone(name, Set::create());
            break;
        case(Value::MAP):
                        map->addClone(name, Map::create());
            break;
        case(Value::TUPLE):
                        map->addClone(name, Tuple::create());
            break;
        case(Value::TABLE): {
            SimpleTypeBox box("width ?");
            int w = utils::to_int(box.run());
            SimpleTypeBox box2("Height ?");
            int h = utils::to_int(box2.run());
            map->addClone(name, Table::create(w, h));
        }
        break;
        case(Value::XMLTYPE):
        map->addXml(name, "");
            break;
        case(Value::NIL):
            map->addClone(name, Null::create());
            break;
        case(Value::MATRIX): {
            SimpleTypeBox box("columns ?");
            int cols = utils::to_int(box.run());
            SimpleTypeBox box2("rows ?");
            int rows = utils::to_int(box2.run());
            Matrix* matrix = Matrix::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
            map->addClone(name, matrix);
        }
        break;
        }
        refresh();
    }
}

void TreeViewValue::on_menu_remove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            value::Value* base = &*row.get_value(m_Columns.m_col_value);

            if (mValue->getType() ==  value::Value::SET) {
                value::Set* set = dynamic_cast<value::Set*>(mValue);
                value::VectorValue& vector = set->value();

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
                value::Map* m  = dynamic_cast<value::Map*>(mValue);
                value::MapValue& map = m->value();
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

void TreeViewValue::refresh()
{
    if (mValue->isSet()) {
        makeTreeView(*(dynamic_cast<value::Set*>(mValue)));
    } else if (mValue->isMap()) {
        makeTreeView(*(dynamic_cast<value::Map*>(mValue)));
    }
}

}
} // namespace vle gvle
