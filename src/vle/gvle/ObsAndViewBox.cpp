/**
 * @file vle/gvle/ObsAndViewBox.cpp
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


#include <boost/algorithm/string/trim.hpp>
#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/ViewBox.hpp>

namespace vle
{
namespace gvle {

ObsAndViewBox::ObsAndViewBox(Glib::RefPtr<Gnome::Glade::Xml> xml):
        mXml(xml),
        mAll_Obs_backup(0),
        mViews_backup(0)
{
    xml->get_widget("DialogObsAndView", mDialog);

    //Observable
    xml->get_widget("TreeViewObs", mTreeViewObs);
    mRefTreeObs = Gtk::TreeStore::create(mColumnsObs);
    mTreeViewObs->set_model(mRefTreeObs);
    mTreeViewObs->append_column("ObservablePorts", mColumnsObs.m_col_name);

    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = mTreeViewObs->get_selection();
    refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);

    mTreeViewObs->enable_model_drag_dest();

    mTreeViewObs->signal_drag_data_received().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_data_received)
    );

    xml->get_widget("ObservableAdd", mButton_Add_Obs);
    mButton_Add_Obs->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_add_port));
    xml->get_widget("ObservableDel", mButton_Del_Obs);
    mButton_Del_Obs->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_del_port));

    //Views
    xml->get_widget("TreeViewViews", mTreeViewViews);
    mRefTreeViews = Gtk::TreeStore::create(mColumnsViews);
    mTreeViewViews->set_model(mRefTreeViews);
    mTreeViewViews->append_column("Views", mColumnsViews.m_col_name);

    mTreeViewViews->enable_model_drag_source();

    mTreeViewViews->signal_drag_end().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_drag_end)
    );
    mTreeViewViews->signal_row_activated().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_view_activated)
    );

    xml->get_widget("ViewAdd", mButton_Add_View);
    mButton_Add_View->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_add_view));
    xml->get_widget("ViewDel", mButton_Del_View);
    mButton_Del_View->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_del_view));

    //Buttons
    xml->get_widget("ButtonObsAndViewApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_apply));

    xml->get_widget("ButtonObsAndViewCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_cancel));
}

ObsAndViewBox::~ObsAndViewBox()
{
    delete mTreeViewObs;
    delete mTreeViewViews;
}

void ObsAndViewBox::show(vpz::Observables& obs, std::string name, vpz::Views& views)
{
    mAll_Obs = &obs;
    mObs = &(obs.get(name));
    mViews = &views;

    mAll_Obs_backup = new vpz::Observables(*mAll_Obs);
    mViews_backup = new vpz::Views(*mViews);

    makeObs();
    makeViews();

    mDialog->set_title("Observable "+name);
    mDialog->show_all();
    mDialog->run();
}

void ObsAndViewBox::on_apply()
{
    mDialog->hide_all();
    delete mAll_Obs_backup;
    delete mViews_backup;
}

void ObsAndViewBox::on_cancel()
{
    mDialog->hide_all();

    mAll_Obs->clear();
    mAll_Obs->add(*mAll_Obs_backup);

    mViews->clear();
    mViews->add(*mViews_backup);

    delete mAll_Obs_backup;
    delete mViews_backup;
}

void ObsAndViewBox::on_drag_end(const Glib::RefPtr<Gdk::DragContext >&)
{
    using namespace Gtk;
    using namespace vpz;

    int x,y;
    mTreeViewObs->get_pointer(x,y);
    if (((0 <= x) && (x <= mTreeViewObs->get_width())) && ((0 <= y) && (y <= mTreeViewObs->get_height()))) {
        //Sources
        Glib::RefPtr<TreeSelection> refTreeSelection = mTreeViewViews->get_selection();
        TreeModel::iterator iter = refTreeSelection->get_selected();
        TreeModel::Row row_source = *iter;

        //Destination
        Glib::RefPtr<TreeSelection> refObsSelection = mTreeViewObs->get_selection();
        refObsSelection->set_mode(Gtk::SELECTION_SINGLE);
        TreeModel::iterator iter2 = refObsSelection->get_selected();
        TreeModel::Row row_dest = *iter2;

        ObservablePort& port = mObs->get(row_dest.get_value(mColumnsObs.m_col_name));
        std::string view = row_source.get_value(mColumnsViews.m_col_name);
        if (!port.exist(view)) {
            port.add(view);
            makeObs();
        }
        refObsSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    }
}

void ObsAndViewBox::on_data_received(const Glib::RefPtr<Gdk::DragContext>&, int, int,
                                     const Gtk::SelectionData&, guint, guint)
{
    using namespace Gtk;

    int x,y;
    mTreeViewObs->get_pointer(x,y);
    TreeModel::Path path;
    TreeViewDropPosition pos;
    mTreeViewObs->get_dest_row_at_pos(x, y, path, pos);
    Gtk::TreeModel::iterator it =  mRefTreeObs->get_iter(path);
    Gtk::TreeModel::Row row = *it;
    TreeIter parent = row.parent();
    if (parent) {
        row = *parent;
    }
    mTreeViewObs->set_cursor(TreePath(row));
}

void ObsAndViewBox::on_view_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*)
{
    Gtk::TreeModel::iterator iter = mRefTreeViews->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        vpz::View& view = mViews->get(row.get_value(mColumnsViews.m_col_name));
        ViewBox box(*mViews, &view, &mViews->outputs());
        box.run();
    }
}

void ObsAndViewBox::makeObs()
{
    mRefTreeObs->clear();
    const vpz::ObservablePortList& port_list = mObs->observableportlist();
    vpz::ObservablePortList::const_iterator it_port = port_list.begin();
    while (it_port != port_list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeObs->append());
        row[mColumnsObs.m_col_name] = it_port->first;
        row[mColumnsObs.m_col_type] = it_port->second.getType();

        const vpz::ViewNameList& view_list = it_port->second.viewnamelist();
        vpz::ViewNameList::const_iterator it_view = view_list.begin();
        while (it_view != view_list.end()) {
            Gtk::TreeModel::Row childrow = *(mRefTreeObs->append(row.children()));
            childrow[mColumnsObs.m_col_name] = *it_view;
            childrow[mColumnsObs.m_col_type] = vpz::Base::VIEW;

            ++it_view;
        }

        ++it_port;
    }
    mTreeViewObs->expand_all();
}

void ObsAndViewBox::makeViews()
{
    mRefTreeViews->clear();
    const vpz::ViewList& list = mViews->viewlist();
    vpz::ViewList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeViews->append());
        row[mColumnsViews.m_col_name] = it->first;

        ++it;
    }
}

void ObsAndViewBox::on_add_port()
{
    SimpleTypeBox box("Name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == "" || mObs->exist(name));
    mObs->add(name);
    makeObs();
}

void ObsAndViewBox::on_del_port()
{
    using namespace Gtk;
    using namespace vpz;

    Glib::RefPtr<TreeSelection> tree_selection = mTreeViewObs->get_selection();
    TreeSelection::ListHandle_Path path_list = tree_selection->get_selected_rows();
    Glib::RefPtr<TreeModel> model = mTreeViewObs->get_model();
    TreeModel::Row row;
    std::string data_name;
    Base::type type;

    for (TreeSelection::ListHandle_Path::iterator i = path_list.begin();
            i != path_list.end();
            ++i) {
        row = *(model->get_iter(*i));
        data_name = row.get_value(mColumnsObs.m_col_name);
        type = row.get_value(mColumnsObs.m_col_type);
        if (type == Base::OBSERVABLEPORT) {
            mObs->del(data_name);
        } else if (type == Base::VIEW) {
            TreeModel::Row parent = *(row.parent());
            std::string port_name = parent.get_value(mColumnsObs.m_col_name);
            if (mObs->exist(port_name)) {
                mObs->get(port_name).del(data_name);
            }
        }
    }
    makeObs();
}

void ObsAndViewBox::on_add_view()
{
    SimpleTypeBox box("Name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == "" || mViews->exist(name));

    mViews->add(vpz::View(name));
    makeViews();
}

void ObsAndViewBox::on_del_view()
{
    using namespace vpz;

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = mTreeViewViews->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string target = row.get_value(mColumnsViews.m_col_name);
            mViews->del(target);

            Observable* obs;
            ObservablePort* obs_port;

            const ObservableList& obs_list = mAll_Obs->observablelist();
            ObservableList::const_iterator it_obs = obs_list.begin();
            while (it_obs != obs_list.end()) {
                obs = &(mAll_Obs->get(it_obs->first));

                const ObservablePortList& obs_port_list = obs->observableportlist();
                ObservablePortList::const_iterator it_port = obs_port_list.begin();
                while (it_port != obs_port_list.end()) {
                    obs_port = &(obs->get(it_port->first));

                    if (obs_port->exist(target)) {
                        obs_port->del(target);
                    }

                    ++it_port;
                }
                ++it_obs;
            }
            makeObs();
            makeViews();
        }
    }
}

}
} // namespace vle gvle
