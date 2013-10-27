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


#include <boost/algorithm/string/trim.hpp>
#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>

namespace vle { namespace gvle {

ObsAndViewBox::ObsAndViewBox(const Glib::RefPtr < Gtk::Builder >& xml):
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

    mTreeViewObs->signal_button_press_event().connect_notify(
        sigc::mem_fun(*this, &ObsAndViewBox::on_button_press));

    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
        mTreeViewObs->get_selection();

    //Views
    xml->get_widget("TreeViewViews", mTreeViewViews);
    mRefTreeViews = Gtk::TreeStore::create(mColumnsViews);
    mTreeViewViews->set_model(mRefTreeViews);
    mTreeViewViews->append_column("Views", mColumnsViews.m_col_name);

    //Selections
    mSrcSelect = mTreeViewViews->get_selection();
    mDstSelect = mTreeViewObs->get_selection();
    mSrcSelect->set_mode(Gtk::SELECTION_MULTIPLE);
    mDstSelect->set_mode(Gtk::SELECTION_MULTIPLE);

    //Buttons
    xml->get_widget("ButtonAddViews", mButtonAdd);
    mButtonAdd->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_add));

    xml->get_widget("ButtonDelViews", mButtonDelete);
    mButtonDelete->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_del_view));

    xml->get_widget("ButtonObsAndViewApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_apply));

    xml->get_widget("ButtonObsAndViewCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &ObsAndViewBox::on_cancel));

    
    mPopupActionGroup = Gtk::ActionGroup::create("ObsAndViewBox");
    mPopupActionGroup->add(Gtk::Action::create("OaVB_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("OaVB_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &ObsAndViewBox::on_add_port));
    
    mPopupActionGroup->add(Gtk::Action::create("OaVB_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &ObsAndViewBox::on_del_port));
    
    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='OaVB_Popup'>"
        "    <menuitem action='OaVB_ContextAdd'/>"
        "    <menuitem action='OaVB_ContextRemove'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/OaVB_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: OaVB_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : OaVB_Popup\n";
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

    mDialog->set_title(_("Observable ")+name);
    mDialog->show_all();
    mDialog->run();
    mDialog->hide();
}

void ObsAndViewBox::on_apply()
{
    mValid = true;
    delete mAll_Obs_backup;
    delete mViews_backup;
}

void ObsAndViewBox::on_cancel()
{
    mValid = false;

    mAll_Obs->clear();
    mAll_Obs->add(*mAll_Obs_backup);

    mViews->clear();
    mViews->add(*mViews_backup);

    delete mAll_Obs_backup;
    delete mViews_backup;
}

void ObsAndViewBox::on_button_press(GdkEventButton* event)
{
    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
        mMenuPopup->popup(event->button, event->time);
    }
}

void ObsAndViewBox::on_add()
{
    using namespace Gtk;
    using namespace vpz;
    
    std::vector<TreePath> v1 = mDstSelect->get_selected_rows();
    std::list<TreePath> lstDst (v1.begin(), v1.end());
//    TreeSelection::ListHandle_Path lstDst = mDstSelect->get_selected_rows();
    Glib::RefPtr<TreeModel> modelDst = mTreeViewObs->get_model();

    for (std::list<TreePath>::iterator iDst = lstDst.begin();
            iDst != lstDst.end(); ++iDst) {
        TreeModel::Row rowDst( *(modelDst->get_iter(*iDst)));
        std::string data_name(rowDst.get_value(mColumnsObs.m_col_name));
        Base::type type(rowDst.get_value(mColumnsObs.m_col_type));

        if (type == Base::VLE_VPZ_OBSERVABLEPORT) {

            ObservablePort& port(
                mObs->get(rowDst.get_value(mColumnsObs.m_col_name)));

            std::vector<TreePath> v2 = mSrcSelect->get_selected_rows();
            std::list<TreePath> lstSrc (v2.begin(), v2.end());
            //TreeSelection::ListHandle_Path lstSrc = mSrcSelect->get_selected_rows();
            Glib::RefPtr<TreeModel> modelSrc = mTreeViewViews->get_model();

            for (std::list<TreePath>::iterator iSrc = lstSrc.begin();
                 iSrc != lstSrc.end(); ++iSrc) {
                TreeModel::Row rowSrc( *(modelSrc->get_iter(*iSrc)));

                std::string view(rowSrc.get_value(mColumnsViews.m_col_name));

                if (not port.exist(view)) {
                    port.add(view);
                }
            }
        }
    }
    makeObs();
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
            childrow[mColumnsObs.m_col_type] = vpz::Base::VLE_VPZ_VIEW;

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
    SimpleTypeBox box(_("Name of the Observable port ?"), "");
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty() and not mObs->exist(name)) {
        mObs->add(name);
        makeObs();
    }
}

void ObsAndViewBox::on_del_port()
{
    using namespace Gtk;
    using namespace vpz;

    Glib::RefPtr<TreeSelection> tree_selection = mTreeViewObs->get_selection();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    //TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    Glib::RefPtr<TreeModel> model = mTreeViewObs->get_model();

    for (std::list<TreePath>::iterator i = lst.begin();
            i != lst.end(); ++i) {
        TreeModel::Row row( *(model->get_iter(*i)));
        std::string data_name(row.get_value(mColumnsObs.m_col_name));
        Base::type type(row.get_value(mColumnsObs.m_col_type));

        if (type == Base::VLE_VPZ_OBSERVABLEPORT) {
            mObs->del(data_name);
        } else if (type == Base::VLE_VPZ_VIEW) {
            TreeModel::Row parent(*(row.parent()));
            std::string port_name(parent.get_value(mColumnsObs.m_col_name));
            if (mObs->exist(port_name)) {
                mObs->get(port_name).del(data_name);
            }
        }
    }
    makeObs();
}

void ObsAndViewBox::on_del_view()
{
    using namespace Gtk;
    using namespace vpz;

    Glib::RefPtr<TreeSelection> tree_selection = mTreeViewObs->get_selection();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    //TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    Glib::RefPtr<TreeModel> model = mTreeViewObs->get_model();

    for (std::list<TreePath>::iterator i = lst.begin();
            i != lst.end(); ++i) {
        TreeModel::Row row( *(model->get_iter(*i)));
        std::string data_name(row.get_value(mColumnsObs.m_col_name));
        Base::type type(row.get_value(mColumnsObs.m_col_type));

        if (type == Base::VLE_VPZ_VIEW) {
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
