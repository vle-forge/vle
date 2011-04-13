/*
 * @file vle/gvle/DynamicsBox.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_GVLE_DYNAMICSBOX_HPP
#define VLE_GVLE_DYNAMICSBOX_HPP

#include <vle/gvle/TreeViewConditions.hpp>

namespace vle { namespace gvle {

    class Modeling;

    class DynamicsBox
    {
    public:
        DynamicsBox(Modeling& modeling,
                    Glib::RefPtr < Gnome::Glade::Xml > ref,
			        vpz::Dynamics& dynamics);

        virtual ~DynamicsBox();

        void run();

        /**
         * @brief Call when user want to add a vpz::Dynamic.
         */
        void onAddDynamic();

        /**
         * @brief Call when user want to delete a vpz::Dynamic.
         */
        void onRemoveDynamic();

        /**
         * @brief Call when user want to rename a vpz::Dynamic.
         */
        void onRenameDynamic();

        /**
         * @brief Call when user want to copy a vpz::Dynamic
         */
        void onCopyDynamic();

        // Signal handler for text area
        virtual void onEditionStarted(
            Gtk::CellEditable* cellEditatble,
            const Glib::ustring& path);
        virtual void onEdition(
            const Glib::ustring& pathString,
            const Glib::ustring& newName);

    private:
        Modeling&                           mModeling;

        /** A copy of the vpz::Dynamics, filled when user modify datas */
        vpz::Dynamics                       mDynsCopy;

        Glib::RefPtr < Gnome::Glade::Xml >  mXml;
        Gtk::Dialog*                        mDialog;
        Gtk::TreeView*                      mDynamics;
        Gtk::ComboBox*                      mPackage;
        Gtk::ComboBox*                      mLibrary;
        Gtk::Entry*                         mLocationHost;
        Gtk::SpinButton*                    mLocationPort;
        Gtk::ComboBox*                      mLanguage;

        Gtk::CellRendererText*              mCellRenderer;
        int                                 mColumnName;
        int                                 mColumnPackage;
        int                                 mColumnLibrary;
        std::string                         mOldName;
        bool                                mValidateRetry;
        Glib::ustring                       mInvalidTextForRetry;

        std::list < sigc::connection >      mList;

        Glib::RefPtr<Gtk::ListStore>        mDynamicsListStore;
        Glib::RefPtr<Gtk::ListStore>        mPackageListStore;
        Glib::RefPtr<Gtk::ListStore>        mLanguageListStore;
        Glib::RefPtr<Gtk::ListStore>        mLibraryListStore;

        Gtk::TreeModel::Row                 mRowLanguage;
        Gtk::TreeModel::Row                 mRowPackage;
        Gtk::TreeModel::Row                 mRowLibrary;

        Gtk::Menu                           mMenu;

        Gtk::TreeModel::Children::iterator  mIter;

        typedef std::vector < std::pair < std::string,
                                          std::string > > renameList;
        renameList                          mRenameList;

        void initDynamics();
        void fillDynamics();
        void initMenuPopupDynamics();
        void sensitive(bool active);
        bool onButtonRealeaseDynamics(GdkEventButton*
                                      event);
        void onCursorChangedDynamics();
        void onChangedPackage();
        void onChangedLibrary();
        void onChangedModel();

        void assignDynamic(const std::string& name);
        void updateDynamic(const std::string& name);
        void storePrevious();
        void applyRenaming();

        void initPackage();
        void fillPackage();
        void setPackageStr(Glib::ustring str);
        Glib::ustring getPackageStr();

        void initLanguage();
        void fillLanguage();
        void setLanguageStr(Glib::ustring str);
        Glib::ustring getLanguageStr();

        void initLibrary();
        void fillLibrary();
        void setLibraryStr(Glib::ustring str);
        Glib::ustring getLibraryStr();

        void onClickColumn(int numColum);


        struct DynamicsModelColumns : public Gtk::TreeModel::ColumnRecord
        {
            DynamicsModelColumns()
            { add(mName); add(mPackage); add(mLibrary); }

            Gtk::TreeModelColumn<Glib::ustring> mName;
            Gtk::TreeModelColumn<Glib::ustring> mPackage;
            Gtk::TreeModelColumn<Glib::ustring> mLibrary;
        } mDynamicsColumns;

        struct PackageModelColumns : public Gtk::TreeModel::ColumnRecord
        {
            PackageModelColumns()
            { add(mContent); }

            Gtk::TreeModelColumn<Glib::ustring> mContent;
        } mPackageColumns;

        struct LanguageModelColumns : public Gtk::TreeModel::ColumnRecord
        {
            LanguageModelColumns()
            { add(mContent); }

            Gtk::TreeModelColumn<Glib::ustring> mContent;
        } mLanguageColumns;

        struct LibraryModelColumns : public Gtk::TreeModel::ColumnRecord
        {
            LibraryModelColumns()
            { add(mContent); }

            Gtk::TreeModelColumn<Glib::ustring> mContent;
        } mLibraryColumns;

        std::set < std::string >mDeletedDynamics;
    };
}} // namespace vle gvle

#endif
