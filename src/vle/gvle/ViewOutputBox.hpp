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


#ifndef VLE_GVLE_VIEWOUTPUT_HPP
#define VLE_GVLE_VIEWOUTPUT_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/liststore.h>
#include <vle/vpz/Views.hpp>
#include <set>
#include <string>

namespace vle { namespace gvle {

    class Modeling;
    class GVLE;

    class ViewOutputBox
    {
    public:
        ViewOutputBox(Modeling& modeling, GVLE* gvle,
                      const Glib::RefPtr < Gtk::Builder >& ref,
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

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

    private:
        Modeling&      m_modeling;
        GVLE*          m_GVLE;

        /** A copy of the vpz::Views, filled when user modify datas */
        vpz::Views          m_viewscopy;

        Glib::RefPtr < Gtk::Builder > m_xml;
        Gtk::Dialog*        m_dialog;
        Gtk::ComboBoxText*  m_type;
        Gtk::Entry*         m_timestep;
        Gtk::ComboBoxText*  m_format;
        Gtk::Entry*         m_location;
        Gtk::Button*        m_directory;
        Gtk::Button*        m_editplugin;
        Gtk::ComboBoxText*  m_plugin;
        Gtk::TreeView*      m_views;
        Gtk::TextView*      m_data;

	Gtk::CellRendererText* m_cellrenderer;
	int                    m_columnName;
	std::string            m_oldName;
	bool                   m_validateRetry;
	Glib::ustring          m_invalidTextForRetry;

        sigc::connection m_cntViewButtonRelease;
        sigc::connection m_cntViewCursorChanged;
        sigc::connection m_cntTypeChanged;
        sigc::connection m_cntFormatChanged;
        sigc::connection m_cntDirectoryClicked;
        sigc::connection m_cntEditPluginClicked;
	sigc::connection m_cntPluginClicked;

        Glib::RefPtr < Gtk::ListStore > m_model;
        Gtk::Menu*          m_menu;
	Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
        
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
        void onEditPlugin();
	void onChangedPlugin();

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
