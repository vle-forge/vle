/**
 * @file vle/gvle/TreeViewObservable.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TreeViewObservable.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

TreeViewObservable::TreeViewObservable()
{
    m_refTreeModel = Gtk::TreeStore::create(m_Columns);
    set_model(m_refTreeModel);

    remove_all_columns();
    append_column(_("name"), m_Columns.m_col_name);

    //makeTreeView();
    /*
    {
    using namespace Gtk::Menu_Helpers;

    Gtk::Menu::MenuList* items = &mMenu_no_port.items();
    items->push_back( Gtk::Menu_Helpers::MenuElem("Insert Port",
     sigc::mem_fun(*this, &TreeViewObservable::on_menu_insert_port) ) );
    items->push_back( Gtk::Menu_Helpers::MenuElem("Remove",
     sigc::mem_fun(*this, &TreeViewObservable::on_menu_remove) ) );


    items = &mMenu_port.items();
    items->push_back( Gtk::Menu_Helpers::MenuElem("Insert Port",
     sigc::mem_fun(*this, &TreeViewObservable::on_menu_insert_port) ) );
    items->push_back( Gtk::Menu_Helpers::MenuElem("Insert View",
     sigc::mem_fun(*this, &TreeViewObservable::on_menu_insert_view) ) );
    items->push_back( Gtk::Menu_Helpers::MenuElem("Remove",
     sigc::mem_fun(*this, &TreeViewObservable::on_menu_remove) ) );
    }
    */
    show_all_children();
}

void TreeViewObservable::on_row_activated(const Gtk::TreeModel::Path& /* path */,
        Gtk::TreeViewColumn* /*column*/)
{
    //Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    //if (iter) {
    //Gtk::TreeModel::Row row = *iter;
    //vpz::ObservablePort* parent = row.get_value(m_Columns.m_col_parent);
    //std::cout << "parent = " << parent->name() << "\n";


        //Gtk::TreeModel::Row row = *iter;
        //vpz::Base* value = row.get_value(m_Columns.m_col_value);
        //if (value->isView()) {
        /*
          vpz::View* view = dynamic_cast<vpz::View*>(row.get_value(m_Columns.m_col_value));
          ViewBox box(view, &mViews->outputs());
          box.run();

          }
        */
    //}
}

/*
bool TreeViewObservable::on_button_press_event(GdkEventButton* event){
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){
Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
if(refSelection)
{
 Gtk::TreeModel::iterator iter = refSelection->get_selected();
 if(iter)
   {
mMenu_port.popup(event->button, event->time);
   }
 else {
   mMenu_no_port.popup(event->button, event->time);
 }
}
  }
  return TreeView::on_button_press_event(event);
}
*/

void TreeViewObservable::makeTreeView(vpz::Observable& obs)
{
    mObservable = &obs;

    m_refTreeModel->clear();

    const vpz::ObservablePortList& list = mObservable->observableportlist();
    vpz::ObservablePortList::const_iterator it_port = list.begin();
    while (it_port != list.end()) {
        //std::cout << it_port->first << "\n";
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.m_col_name] = it_port->first;
        row[m_Columns.m_col_parent] = &(mObservable->get(it_port->first));
        //row[m_Columns.m_col_value] = &(mObservable->get(it_port->first));

        const vpz::ViewNameList& viewlist = it_port->second.viewnamelist();
        vpz::ViewNameList::const_iterator it_view = viewlist.begin();
        while (it_view != viewlist.end()) {
            //std::cout << "\t" << *it_view << "\n";
            Gtk::TreeModel::Row childrow = *(m_refTreeModel->append(row.children()));
            childrow[m_Columns.m_col_name] = *it_view;
            childrow[m_Columns.m_col_parent] = &(mObservable->get(it_port->first));
            //childrow[m_Columns.m_col_value] = &( mViews->get(*it_view) );

            ++it_view;
        }

        ++it_port;
    }
    expand_all();
}
/*
void TreeViewObservable::on_menu_insert_port() {
  SimpleTypeBox box("Name ?");
  std::string name;
  do {
name = box.run();
boost::trim(name);
  } while ( name == "" || mObservable->exist(name) );
  mObservable->add(name);
  makeTreeView();
}

void TreeViewObservable::on_menu_insert_view() {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
  if(refSelection)
{
Gtk::TreeModel::iterator iter = refSelection->get_selected();
if(iter)
 {
   Gtk::TreeModel::Row row = *iter;
   vpz::ObservablePort* parent = row.get_value(m_Columns.m_col_parent);
   SimpleTypeBox box("Name ?");
   std::string name;
   do {
name = box.run();
boost::trim(name);
   } while ( name == "" || parent->exist(name) );
   mViews->add( vpz::View(name) );
   parent->add(name);
   makeTreeView();
 }
}
}


void TreeViewObservable::on_menu_remove() {
  Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
  if(refSelection)
{
Gtk::TreeModel::iterator iter = refSelection->get_selected();
if(iter)
 {
   Gtk::TreeModel::Row row = *iter;
   vpz::Base* value = row.get_value(m_Columns.m_col_value);
   switch ( value->getType() ) {
   case vle::vpz::Base::OBSERVABLEPORT:
mObservable->del( row.get_value(m_Columns.m_col_name) );
break;
   case vle::vpz::Base::VIEW:
row.get_value(m_Columns.m_col_parent)->del(row.get_value(m_Columns.m_col_name));
break;
   default: break;
   }
   makeTreeView();
 }
}
}
*/

}
} // namespace vle gvle
