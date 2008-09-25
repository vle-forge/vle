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

namespace vle { namespace gvle {

ObsAndViewBox::ObsAndViewBox(Glib::RefPtr<Gnome::Glade::Xml> xml):
        mXml(xml),
        mAll_Obs_backup(0),
        mViews_backup(0),
	mSelected(false)
{
    xml->get_widget("DialogObsAndView", mDialog);

    //Observable
    xml->get_widget("TreeViewObs", mTreeViewObs);
    mRefTreeObs = Gtk::TreeStore::create(mColumnsObs);
    mTreeViewObs->set_model(mRefTreeObs);
    mTreeViewObs->append_column("ObservablePorts", mColumnsObs.m_col_name);

    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
        mTreeViewObs->get_selection();

    mTreeViewObs->enable_model_drag_dest();

    mTreeViewObs->signal_drag_data_received().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_data_received));

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

void ObsAndViewBox::show(vpz::Observables& obs, std::string name,
                         vpz::Views& views)
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
    mValid = true;
    mDialog->hide_all();
    delete mAll_Obs_backup;
    delete mViews_backup;
}

void ObsAndViewBox::on_cancel()
{
    mValid = false;
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

    int x, y;
    mTreeViewObs->get_pointer(x, y);
    if (mSelected and (0 <= x and x <= mTreeViewObs->get_width())
        and (0 <= y and y <= mTreeViewObs->get_height())) {
        Glib::RefPtr<TreeSelection> srcSelect(mTreeViewViews->get_selection());
        Glib::RefPtr<TreeSelection> dstSelect(mTreeViewObs->get_selection());

        srcSelect->set_mode(Gtk::SELECTION_SINGLE);
        dstSelect->set_mode(Gtk::SELECTION_SINGLE);

        if (srcSelect and dstSelect) {
            TreeModel::iterator iter = srcSelect->get_selected();
            TreeModel::iterator iter2 = dstSelect->get_selected();
            if (iter and iter2) {
                TreeModel::Row row_source = *iter;
                TreeModel::Row row_dest = *iter2;

                ObservablePort& port(
                    mObs->get(row_dest.get_value(mColumnsObs.m_col_name)));
                std::string view(row_source.get_value(mColumnsViews.m_col_name));

                if (not port.exist(view)) {
                    port.add(view);
                    makeObs();
                }
            }
        }
    }
}

void ObsAndViewBox::on_data_received(const Glib::RefPtr<Gdk::DragContext>&,
				     int, int,
                                     const Gtk::SelectionData&, guint, guint)
{
    using namespace Gtk;

    int x,y;
    mTreeViewObs->get_pointer(x, y);

    TreeModel::Path path;
    TreeViewDropPosition pos;

    if (mTreeViewObs->get_dest_row_at_pos(x, y, path, pos)) {
        if (!path.empty()) {
            Gtk::TreeModel::iterator it =  mRefTreeObs->get_iter(path);
            if (it) {
                Gtk::TreeModel::Row row = *it;
                TreeIter parent = row.parent();

                if (parent) {
                    row = *parent;
                }
                mSelected = true;
                mTreeViewObs->set_cursor(TreePath(row));
            }
        } else  {
            mSelected = false;
        }
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
    SimpleTypeBox box("Name of the Observable port ?");
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty()) {
        mObs->add(name);
        makeObs();
    }
}

void ObsAndViewBox::on_del_port()
{
    using namespace Gtk;
    using namespace vpz;

    Glib::RefPtr<TreeSelection> tree_selection = mTreeViewObs->get_selection();
    TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    Glib::RefPtr<TreeModel> model = mTreeViewObs->get_model();

    for (TreeSelection::ListHandle_Path::iterator i = lst.begin();
            i != lst.end(); ++i) {
        TreeModel::Row row( *(model->get_iter(*i)));
        std::string data_name(row.get_value(mColumnsObs.m_col_name));
        Base::type type(row.get_value(mColumnsObs.m_col_type));

        if (type == Base::OBSERVABLEPORT) {
            mObs->del(data_name);
        } else if (type == Base::VIEW) {
            TreeModel::Row parent(*(row.parent()));
            std::string port_name(parent.get_value(mColumnsObs.m_col_name));
            if (mObs->exist(port_name)) {
                mObs->get(port_name).del(data_name);
            }
        }
    }
    makeObs();
}

}} // namespace vle gvle
