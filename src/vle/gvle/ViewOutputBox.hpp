/**
 * @file vle/gvle/ViewOutputBox.hpp
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


#ifndef VLE_GVLE_VIEWOUTPUT_HPP
#define VLE_GVLE_VIEWOUTPUT_HPP

#include <libglademm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>
#include <vle/vpz/Views.hpp>
#include <set>
#include <string>

namespace vle { namespace gvle {

    class ViewOutputBox
    {
    public:
        ViewOutputBox(Glib::RefPtr < Gnome::Glade::Xml > ref,
                      vpz::Views& views);

        virtual ~ViewOutputBox();

        void run();

        /**
         * @brief Call when user want to add a vpz::View.
         */
        void onAddViews();

        /**
         * @brief Call when user want to delete a vpz::View.
         */
        void onRemoveViews();

	/**
	 * @brief Call when user want to rename a vpz::View.
	 */
	void onRenameViews();

	/**
	 * @brief Call when user want to copy a vpz::View
	 */
	void onCopyViews();


    private:
        /** a reference to the origin vpz::Views, filled when user modify and
         * valid the ViewOutputBox.
         */
        vpz::Views&         m_viewsorig;

        /** A copy of the vpz::Views, filled when user modify datas */
        vpz::Views          m_viewscopy;

        Glib::RefPtr < Gnome::Glade::Xml > m_xml;
        Gtk::Dialog*        m_dialog;
        Gtk::ComboBoxText*  m_type;
        Gtk::SpinButton*    m_timestep;
        Gtk::ComboBoxText*  m_format;
        Gtk::Entry*         m_location;
        Gtk::Button*        m_directory;
        Gtk::ComboBoxText*  m_plugin;
        Gtk::TreeView*      m_views;
        Gtk::TextView*      m_data;

        Glib::RefPtr < Gtk::ListStore > m_model;
        Gtk::Menu           m_menu;

        Gtk::TreeModel::iterator m_iter;

        void initViews();
        void fillViews();
        void initMenuPopupViews();
        void sensitive(bool active);
        bool onButtonRealeaseViews(GdkEventButton* event);
        void onCursorChangedViews();
        void onChangedType();
        void onChangedFormat();
        void onClickedDirectory();

        void fillCombobox();
        void assignView(const std::string& name);
        void updateView(const std::string& name);
        void storePrevious();

        std::string buildOutputName(const std::string& name) const;

        struct ViewsColumnRecord : public Gtk::TreeModel::ColumnRecord
        {
            Gtk::TreeModelColumn < std::string > name;
            ViewsColumnRecord() { add(name); }
        } m_viewscolumnrecord;

        std::set < std::string > m_deletedView;
    };

}} // namespace vle gvle

#endif
