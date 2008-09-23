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
#include <vle/utils.hpp>
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
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::BOOLEAN)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Double", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::DOUBLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Integer", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::INTEGER)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Map", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::MAP)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Matrix", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::MATRIX)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Null", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::NIL)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Set", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::SET)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("String", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::STRING)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Table", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::TABLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Tuple", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::TUPLE)));
        insert.push_back(Gtk::Menu_Helpers::MenuElem("Xml", sigc::bind(
                             sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), ValueBase::XMLTYPE)));

        pMenuItem->set_submenu(*pMenu);

        items.push_back(Gtk::Menu_Helpers::MenuElem("Remove",
                        sigc::mem_fun(*this, &TreeViewValue::on_menu_remove)));
    }
}

TreeViewValue::~TreeViewValue()
{
}

void TreeViewValue::makeTreeView(value::SetFactory& set)
{
    mValue = &set;

    m_refTreeModel->clear();
    remove_all_columns();
    //Add the TreeView's view columns:
    append_column("Type", m_Columns.m_col_type);
    append_column("Overview", m_Columns.m_col_view);

    value::VectorValue vector = set.getValue();
    value::VectorValue::iterator it = vector.begin();

    while (it != vector.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        switch ((*it)->getType()) {
        case value::ValueBase::BOOLEAN:
            row[m_Columns.m_col_type] = "boolean";
            break;
        case value::ValueBase::INTEGER:
            row[m_Columns.m_col_type] = "integer";
            break;
        case value::ValueBase::DOUBLE:
            row[m_Columns.m_col_type] = "double";
            break;
        case value::ValueBase::STRING:
            row[m_Columns.m_col_type] = "string";
            break;
        case value::ValueBase::SET:
            row[m_Columns.m_col_type] = "set";
            break;
        case value::ValueBase::MAP:
            row[m_Columns.m_col_type] = "map";
            break;
        case value::ValueBase::TUPLE:
            row[m_Columns.m_col_type] = "tuple";
            break;
        case value::ValueBase::TABLE:
            row[m_Columns.m_col_type] = "table";
            break;
        case value::ValueBase::XMLTYPE:
            row[m_Columns.m_col_type] = "xml";
            break;
        case value::ValueBase::NIL:
            row[m_Columns.m_col_type] = "null";
            break;
        case value::ValueBase::MATRIX:
            row[m_Columns.m_col_type] = "matrix";
            break;
        default:
            break;
        }
        row[m_Columns.m_col_view] = boost::trim_copy((*it)->toString()).substr(0, GVLE::overview_max);
        row[m_Columns.m_col_value] = *it;

        it++;
    }
}

void TreeViewValue::makeTreeView(value::MapFactory& map)
{
    mValue = &map;

    m_refTreeModel->clear();
    remove_all_columns();
    //Add the TreeView's view columns:
    append_column("Key", m_Columns.m_col_name);
    append_column("Type", m_Columns.m_col_type);
    append_column("Overview", m_Columns.m_col_view);

    const value::MapValue& values = map.getValue();
    value::MapValue::const_iterator it = values.begin();
    while (it != values.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        row[m_Columns.m_col_name] = it->first;
        switch (it->second->getType()) {
            using namespace value;
        case ValueBase::BOOLEAN:
            row[m_Columns.m_col_type] = "boolean";
            break;
        case ValueBase::INTEGER:
            row[m_Columns.m_col_type] = "integer";
            break;
        case ValueBase::DOUBLE:
            row[m_Columns.m_col_type] = "double";
            break;
        case ValueBase::STRING:
            row[m_Columns.m_col_type] = "string";
            break;
        case ValueBase::SET:
            row[m_Columns.m_col_type] = "set";
            break;
        case ValueBase::MAP:
            row[m_Columns.m_col_type] = "map";
            break;
        case ValueBase::TUPLE:
            row[m_Columns.m_col_type] = "tuple";
            break;
        case ValueBase::TABLE:
            row[m_Columns.m_col_type] = "table";
            break;
        case ValueBase::XMLTYPE:
            row[m_Columns.m_col_type] = "xmltype";
            break;
        case ValueBase::NIL:
            row[m_Columns.m_col_type] = "null";
            break;
        case ValueBase::MATRIX:
            row[m_Columns.m_col_type] = "matrix";
            break;
        default:
            break;
        }

        row[m_Columns.m_col_view] = boost::trim_copy(it->second->toString()).substr(0, GVLE::overview_max);
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
        value::ValueBase* v =  &*row.get_value(m_Columns.m_col_value);

        if (row[m_Columns.m_col_type] == "map") {
            value::MapFactory* map = dynamic_cast<value::MapFactory*>(v);
            ValueBox box(map);
            box.run();
        } else if (row[m_Columns.m_col_type] == "set") {
            value::SetFactory* set = dynamic_cast<value::SetFactory*>(v);
            ValueBox box(set);
            box.run();
        } else if (row[m_Columns.m_col_type] == "boolean") {
            value::BooleanFactory* boolean = dynamic_cast<value::BooleanFactory*>(v);
            BooleanBox box(boolean);
            box.run();
        } else if (row[m_Columns.m_col_type] == "integer" or
                   row[m_Columns.m_col_type] == "double" or
                   row[m_Columns.m_col_type] == "string" or
                   row[m_Columns.m_col_type] == "tuple") {
            SimpleTypeBox box(v);
            box.run();
        } else if (row[m_Columns.m_col_type] == "table") {
            value::TableFactory* table = dynamic_cast<value::TableFactory*>(v);
            TableBox box(table);
            box.run();
        } else if (row[m_Columns.m_col_type] == "matrix") {
            value::MatrixFactory* matrix = dynamic_cast<value::MatrixFactory*>(v);
            MatrixBox box(matrix);
            box.run();
        } else if (row[m_Columns.m_col_type] == "xml") {
            value::XMLFactory* xml = dynamic_cast<value::XMLFactory*>(v);
            XmlTypeBox box(xml);
            box.run();
        }
        value::ValueBase* val =  &*row.get_value(m_Columns.m_col_value);
        row[m_Columns.m_col_view] = boost::trim_copy(val->toString()).substr(0, GVLE::overview_max);
        refresh();
    }
}

void TreeViewValue::on_menu_insert(value::ValueBase::type type)
{
    if (mValue->getType() ==  value::ValueBase::SET) {
        value::SetFactory* set = dynamic_cast<value::SetFactory*>(mValue);
        value::VectorValue& vector = set->getValue();
        value::VectorValue::iterator it;

        Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
        if (refSelection) {
            Gtk::TreeModel::iterator iter = refSelection->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                value::Value base = row.get_value(m_Columns.m_col_value);
                it = std::find(vector.begin(), vector.end(), base);
            } else {
                it = vector.end();
            }
        }
        switch (type) {
            using namespace value;
        case(ValueBase::BOOLEAN):
                        vector.insert(it, BooleanFactory::create());
            break;
        case(ValueBase::INTEGER):
                        vector.insert(it, IntegerFactory::create());
            break;
        case(ValueBase::DOUBLE):
                        vector.insert(it, DoubleFactory::create());
            break;
        case(ValueBase::STRING):
                        vector.insert(it, StringFactory::create());
            break;
        case(ValueBase::SET):
                        vector.insert(it, SetFactory::create());
            break;
        case(ValueBase::MAP):
                        vector.insert(it, MapFactory::create());
            break;
        case(ValueBase::TUPLE):
                        vector.insert(it, TupleFactory::create());
            break;
        case(ValueBase::TABLE): {
            SimpleTypeBox box("Width ?");
            int w = utils::to_int(box.run());
            SimpleTypeBox box2("Height ?");
            int h = utils::to_int(box2.run());
            vector.insert(it, TableFactory::create(w, h));
        }
        break;
        case(ValueBase::XMLTYPE):
                        vector.insert(it, XMLFactory::create());
            break;
        case(ValueBase::NIL):
                        vector.insert(it, NullFactory::create());
            break;
        case(ValueBase::MATRIX): {
            SimpleTypeBox box("columns ?");
            int cols = utils::to_int(box.run());
            SimpleTypeBox box2("rows ?");
            int rows = utils::to_int(box2.run());
            Matrix matrix = MatrixFactory::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
            vector.insert(it, matrix);
        }
        break;
        }
        refresh();
    } else if (mValue->getType() ==  value::ValueBase::MAP) {
        value::MapFactory* map = dynamic_cast<value::MapFactory*>(mValue);
        SimpleTypeBox box("name ?");
        std::string name;
        do {
            name = box.run();
        } while (map->existValue(name));

        switch (type) {
            using namespace value;
        case(ValueBase::BOOLEAN):
                        map->setBooleanValue(name, false);
            break;
        case(ValueBase::INTEGER):
                        map->setIntValue(name, 0);
            break;
        case(ValueBase::DOUBLE):
                        map->setDoubleValue(name, 0);
            break;
        case(ValueBase::STRING):
                        map->setStringValue(name, "");
            break;
        case(ValueBase::SET):
                        map->addCloneValue(name, SetFactory::create());
            break;
        case(ValueBase::MAP):
                        map->addCloneValue(name, MapFactory::create());
            break;
        case(ValueBase::TUPLE):
                        map->addCloneValue(name, TupleFactory::create());
            break;
        case(ValueBase::TABLE): {
            SimpleTypeBox box("width ?");
            int w = utils::to_int(box.run());
            SimpleTypeBox box2("Height ?");
            int h = utils::to_int(box2.run());
            map->addCloneValue(name, TableFactory::create(w, h));
        }
        break;
        case(ValueBase::XMLTYPE):
                        map->setXMLValue(name, "");
            break;
        case(ValueBase::NIL):
                        map->addCloneValue(name, NullFactory::create());
            break;
        case(ValueBase::MATRIX): {
            SimpleTypeBox box("columns ?");
            int cols = utils::to_int(box.run());
            SimpleTypeBox box2("rows ?");
            int rows = utils::to_int(box2.run());
            Matrix matrix = MatrixFactory::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
            map->addCloneValue(name, matrix);
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
            value::ValueBase* base = &*row.get_value(m_Columns.m_col_value);

            if (mValue->getType() ==  value::ValueBase::SET) {
                value::SetFactory* set = dynamic_cast<value::SetFactory*>(mValue);
                value::VectorValue& vector = set->getValue();

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

            } else if (mValue->getType() ==  value::ValueBase::MAP) {
                value::MapFactory* factory = dynamic_cast<value::MapFactory*>(mValue);
                value::MapValue& map = factory->getValue();
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
        makeTreeView(*(dynamic_cast<value::SetFactory*>(mValue)));
    } else if (mValue->isMap()) {
        makeTreeView(*(dynamic_cast<value::MapFactory*>(mValue)));
    }
}

}
} // namespace vle gvle
