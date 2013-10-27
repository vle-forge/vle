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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/BooleanBox.hpp>
#include <vle/gvle/MatrixBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TableBox.hpp>
#include <vle/gvle/TreeViewValue.hpp>
#include <vle/gvle/ValueBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

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
        
        mMenuActionGroup = Gtk::ActionGroup::create("TreeViewValue");
        
        /* Partie Menu Principal */
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextMenu", _("Context Menu")));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextRemove", _("_Remove")),
            sigc::mem_fun(*this, &TreeViewValue::on_menu_remove));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextRename", _("_Duplicate")),
            sigc::mem_fun(*this, &TreeViewValue::on_menu_duplicate));
        
        /* Partie Sous Menu */
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextInsert", _("_Insert")));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextBoolean", _("_Boolean")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::BOOLEAN));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextDouble", _("_Double")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::DOUBLE));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextInteger", _("_Integer")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::INTEGER));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextMap", _("_Map")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::MAP));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextMatrix", _("_Matrix")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::MATRIX));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextNull", _("_Null")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::NIL));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextSet", _("_Set")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::SET));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextString", _("_String")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::STRING));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextTable", _("_Table")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::TABLE));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextTuple", _("_Tuple")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::TUPLE));
        
        mMenuActionGroup->add(Gtk::Action::create("TVV_ContextXml", _("_Xml")),
            sigc::bind(sigc::mem_fun(*this, &TreeViewValue::on_menu_insert), Value::XMLTYPE));
        
        
        mUIManager = Gtk::UIManager::create();
        mUIManager->insert_action_group(mMenuActionGroup);
        
        Glib::ustring ui_info =
            "<ui>"
            "  <popup name='TVV_Popup'>"
            "      <menu action='TVV_ContextInsert'>"
            "        <menuitem action='TVV_ContextBoolean'/>"
            "        <menuitem action='TVV_ContextDouble'/>"
            "        <menuitem action='TVV_ContextInteger'/>"
            "        <menuitem action='TVV_ContextMap'/>"
            "        <menuitem action='TVV_ContextMatrix'/>"
            "        <menuitem action='TVV_ContextNull'/>"
            "        <menuitem action='TVV_ContextSet'/>"
            "        <menuitem action='TVV_ContextString'/>"
            "        <menuitem action='TVV_ContextTable'/>"
            "        <menuitem action='TVV_ContextTuple'/>"
            "        <menuitem action='TVV_ContextXml'/>"
            "      </menu>"
            "      <menuitem action='TVV_ContextRemove'/>"
            "      <menuitem action='TVV_ContextRename'/>"
            "  </popup>"
            "</ui>";
    
        try {
            mUIManager->add_ui_from_string(ui_info);
            mMenu = (Gtk::Menu *) (
                mUIManager->get_widget("/TVV_Popup"));
        } catch(const Glib::Error& ex) {
            std::cerr << "building menus failed: TVV_Popup " <<  ex.what();
        }
        
        if (!mMenu)
            std::cerr << "menu not found : TVV_Popup\n";
        
        mMenu->show_all();
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
    append_column(_("Type"), m_Columns.m_col_type);
    append_column(_("Overview"), m_Columns.m_col_view);

    value::VectorValue vector = set.value();
    value::VectorValue::iterator it = vector.begin();

    while (it != vector.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        switch ((*it)->getType()) {
        case value::Value::BOOLEAN:
            row[m_Columns.m_col_type] = _("boolean");
            break;
        case value::Value::INTEGER:
            row[m_Columns.m_col_type] = _("integer");
            break;
        case value::Value::DOUBLE:
            row[m_Columns.m_col_type] = _("double");
            break;
        case value::Value::STRING:
            row[m_Columns.m_col_type] = _("string");
            break;
        case value::Value::SET:
            row[m_Columns.m_col_type] = _("set");
            break;
        case value::Value::MAP:
            row[m_Columns.m_col_type] = _("map");
            break;
        case value::Value::TUPLE:
            row[m_Columns.m_col_type] = _("tuple");
            break;
        case value::Value::TABLE:
            row[m_Columns.m_col_type] = _("table");
            break;
        case value::Value::XMLTYPE:
            row[m_Columns.m_col_type] = _("xml");
            break;
        case value::Value::NIL:
            row[m_Columns.m_col_type] = _("null");
            break;
        case value::Value::MATRIX:
            row[m_Columns.m_col_type] = _("matrix");
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
    append_column(_("Key"), m_Columns.m_col_name);
    append_column(_("Type"), m_Columns.m_col_type);
    append_column(_("Overview"), m_Columns.m_col_view);

    const value::MapValue& values = map.value();
    value::MapValue::const_iterator it = values.begin();
    while (it != values.end()) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());

        row[m_Columns.m_col_name] = it->first;
        switch (it->second->getType()) {
            using namespace value;
        case Value::BOOLEAN:
            row[m_Columns.m_col_type] = _("boolean");
            break;
        case Value::INTEGER:
            row[m_Columns.m_col_type] = _("integer");
            break;
        case Value::DOUBLE:
            row[m_Columns.m_col_type] = _("double");
            break;
        case Value::STRING:
            row[m_Columns.m_col_type] = _("string");
            break;
        case Value::SET:
            row[m_Columns.m_col_type] = _("set");
            break;
        case Value::MAP:
            row[m_Columns.m_col_type] = _("map");
            break;
        case Value::TUPLE:
            row[m_Columns.m_col_type] = _("tuple");
            break;
        case Value::TABLE:
            row[m_Columns.m_col_type] = _("table");
            break;
        case Value::XMLTYPE:
            row[m_Columns.m_col_type] = _("xmltype");
            break;
        case Value::NIL:
            row[m_Columns.m_col_type] = _("null");
            break;
        case Value::MATRIX:
            row[m_Columns.m_col_type] = _("matrix");
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
        mMenu->popup(event->button, event->time);
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

        if (row[m_Columns.m_col_type] == _("map")) {
            value::Map* map = dynamic_cast<value::Map*>(v);
            ValueBox box(map);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("set")) {
            value::Set* set = dynamic_cast<value::Set*>(v);
            ValueBox box(set);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("boolean")) {
            value::Boolean* boolean = dynamic_cast<value::Boolean*>(v);
            BooleanBox box(boolean);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("integer") or
                   row[m_Columns.m_col_type] == _("double") or
                   row[m_Columns.m_col_type] == _("string") or
                   row[m_Columns.m_col_type] == _("tuple")) {
            SimpleTypeBox box(v);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("table")) {
            value::Table* table = dynamic_cast<value::Table*>(v);
            TableBox box(table);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("matrix")) {
            value::Matrix* matrix = dynamic_cast<value::Matrix*>(v);
            MatrixBox box(matrix);
            box.run();
        } else if (row[m_Columns.m_col_type] == _("xml")) {
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
            SimpleTypeBox box(_("Width ?"), "1");
            int w = utils::to < int32_t >(box.run());
            SimpleTypeBox box2(_("Height ?)"), "1");
            int h = utils::to < int32_t >(box2.run());
            vector.insert(it, Table::create(w, h));
        }
        default:
            break;
        break;
        case(Value::XMLTYPE):
        vector.insert(it, Xml::create());
            break;
        case(Value::NIL):
                        vector.insert(it, Null::create());
            break;
        case(Value::MATRIX): {
            SimpleTypeBox box(_("columns ?"), "1");
            int cols = utils::to < int32_t >(box.run());
            SimpleTypeBox box2(_("rows ?"), "1");
            int rows = utils::to < int32_t >(box2.run());
            Matrix* matrix = Matrix::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
            vector.insert(it, matrix);
        }
        break;
        }
        refresh();
    } else if (mValue->getType() ==  value::Value::MAP) {
        value::Map* map = dynamic_cast<value::Map*>(mValue);
        SimpleTypeBox box(_("name ?"), "");
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
            SimpleTypeBox box(_("width ?"), "1");
            int w = utils::to < int32_t >(box.run());
            SimpleTypeBox box2(_("Height ?"), "1");
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
            SimpleTypeBox box(_("columns ?"), "1");
            int cols = utils::to < int32_t >(box.run());
            SimpleTypeBox box2(_("rows ?"), "1");
            int rows = utils::to < int32_t >(box2.run());
            Matrix* matrix = Matrix::create(cols, rows, cols*2, rows*2, cols/5,
                                            rows/5);
            map->add(name, matrix);
        }
        break;
        default:
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

void TreeViewValue::on_menu_duplicate()
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
