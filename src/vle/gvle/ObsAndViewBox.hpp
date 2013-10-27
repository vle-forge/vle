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

#ifndef VLE_GVLE_OBSANDVIEWBOX_HPP
#define VLE_GVLE_OBSANDVIEWBOX_HPP

#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Views.hpp>

#include <gtkmm/treeview.h>
#include <gtkmm/button.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treestore.h>
#include <gtkmm/builder.h>
#include <gtkmm/uimanager.h>
#include <iostream>

namespace vle { namespace gvle {

    class Modeling;

    class ObsAndViewBox
    {
    public:
        ObsAndViewBox(const Glib::RefPtr < Gtk::Builder >& xml);

        ~ObsAndViewBox();

        void show(vpz::Observables& obs, std::string name, vpz::Views& views);

        /**
         * @brief Return true if the user select the Gtk::REPONSE_OK, false
         * otherwise.
         * @return
         */
        bool valid() const { return mValid; }

    protected:
        class ModelColumnsObs : public Gtk::TreeModel::ColumnRecord
        {
        public:
            ModelColumnsObs()
            {
                add(m_col_name);
                add(m_col_type);
            }
            Gtk::TreeModelColumn<Glib::ustring>      m_col_name;
            Gtk::TreeModelColumn<vpz::Base::type>    m_col_type;
        };

        class ModelColumnsView : public Gtk::TreeModel::ColumnRecord
        {
        public:
            ModelColumnsView()
            {
                add(m_col_name);
            }
            Gtk::TreeModelColumn<Glib::ustring>      m_col_name;
        };

    private:
        Glib::RefPtr < Gtk::Builder >         mXml;
        Gtk::Dialog*                          mDialog;

        //Data
        vpz::Observables*                     mAll_Obs;
        vpz::Observable*                      mObs;
        vpz::Views*                           mViews;

        //Backup
        vpz::Observables*                     mAll_Obs_backup;
        vpz::Views*                           mViews_backup;

        //Observable
        Gtk::TreeView*                        mTreeViewObs;
        Glib::RefPtr<Gtk::TreeStore>          mRefTreeObs;
        ModelColumnsObs                       mColumnsObs;
        Gtk::Menu*                            mMenuPopup;
	Glib::RefPtr <Gtk::UIManager>         mPopupUIManager;
	Glib::RefPtr <Gtk::ActionGroup>       mPopupActionGroup;
	
        Glib::RefPtr<Gtk::TreeSelection>      mSrcSelect;

        //Views
        Gtk::TreeView*                        mTreeViewViews;
        Glib::RefPtr<Gtk::TreeStore>          mRefTreeViews;
        ModelColumnsView                      mColumnsViews;
        Glib::RefPtr<Gtk::TreeSelection>      mDstSelect;

        //Buttons
        Gtk::Button*                          mButtonAdd;
        Gtk::Button*                          mButtonDelete;
        Gtk::Button*                          mButtonApply;
        Gtk::Button*                          mButtonCancel;

        bool                                  mSelected;
        bool                                  mValid;

        void on_apply();
        void on_cancel();
        void on_button_press(GdkEventButton* event);

        void on_add();

        void makeObs();
        void makeViews();

        void on_add_port();
        void on_del_port();

        void on_del_view();
    };

}} // namespace vle gvle

#endif
