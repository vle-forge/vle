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

#ifndef VLE_GVLE_DYNAMICSBOX_HPP
#define VLE_GVLE_DYNAMICSBOX_HPP

#include <gtkmm/treemodel.h>
#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>
#include <gtkmm/cellrenderercombo.h>
#include <gtkmm/liststore.h>
#include <gtkmm/uimanager.h>

#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/PluginFactory.hpp>

namespace vle { namespace gvle {

class Modeling;

class DynamicsBox
{
public:
    DynamicsBox(Modeling& modeling,
                vle::gvle::PluginFactory&,
                Glib::RefPtr < Gtk::Builder > ref,
                vpz::Dynamics& dynamics);

    virtual ~DynamicsBox();

    void run();

    /**
     * @brief When a choice is made using a combo.
     */
    void onCellrendererChoiceEditedPackage(
        const Glib::ustring& pathstring,
        const Glib::ustring& newtext);
    /**
     * @brief When a choice is made using a combo.
     */
    void onCellrendererChoiceEditedLibrary(
        const Glib::ustring& pathstring,
        const Glib::ustring& newtext);

    /**
     * @brief To add a vpz::Dynamic.
     */
    void onAddDynamic();

    /**
     * @brief To delete a vpz::Dynamic.
     */
    void onRemoveDynamic();

    /**
     * @brief To rename a vpz::Dynamic.
     */
    void onRenameDynamic();

    /**
     * @brief To copy a vpz::Dynamic
     */
    void onCopyDynamic();

    // Signal handler for text cell
    virtual void onEditionStarted(
        Gtk::CellEditable* celleditatble,
        const Glib::ustring& path);
    virtual void onEdition(
        const Glib::ustring& path,
        const Glib::ustring& newstring);

private:
    typedef std::vector < std::pair <std::string, std::string> > renameList;
    typedef std::map< std::string, Glib::RefPtr<Gtk::ListStore> > listStoreList;

    struct DynamicsModelColumns : public Gtk::TreeModel::ColumnRecord
    {
        DynamicsModelColumns()
        { add(mName); add(mPackage); add(mLibrary);
            add(mPackages); add(mLibraries); }

        Gtk::TreeModelColumn<Glib::ustring> mName;
        Gtk::TreeModelColumn<Glib::ustring> mPackage;
        Gtk::TreeModelColumn< Glib::RefPtr<Gtk::TreeModel> > mPackages;
        Gtk::TreeModelColumn<Glib::ustring> mLibrary;
        Gtk::TreeModelColumn< Glib::RefPtr<Gtk::TreeModel> > mLibraries;

    } mDynamicsColumns;

    struct PackageModelColumns : public Gtk::TreeModel::ColumnRecord
    {
        PackageModelColumns()
        { add(mContent); }

        Gtk::TreeModelColumn<Glib::ustring> mContent;
    } mPackageColumns;

    struct LibraryModelColumns : public Gtk::TreeModel::ColumnRecord
    {
        LibraryModelColumns()
        { add(mContent); }

        Gtk::TreeModelColumn<Glib::ustring> mContent;
    } mLibraryColumns;

    Modeling&                           mModeling;
    PluginFactory&                      mPluginFactory;

    vpz::Dynamics                       mDynsCopy;

    Glib::RefPtr < Gtk::Builder >       mXml;
    Gtk::Dialog*                        mDialog;
    Gtk::TreeView*                      mDynamics;

    int                                 mColumnName;
    int                                 mColumnPackage;
    int                                 mColumnLibrary;
    Glib::ustring                       mOldName;

    std::list < sigc::connection >      mList;

    Glib::RefPtr<Gtk::ListStore>        mDynamicsListStore;
    Glib::RefPtr<Gtk::ListStore>        mPackagesListStore;
    listStoreList                       mLibrariesListStore;

    Gtk::TreeModel::Row                 mRowPackage;
    Gtk::TreeModel::Row                 mRowLibrary;

    Gtk::Menu*                          mMenu;
    Glib::RefPtr <Gtk::UIManager> mUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mActionGroup;
	
    Gtk::TreeModel::Children::iterator  mIter;


    renameList                          mRenameList;

    std::set < std::string >            mDeletedDynamics;

    void fillLibrariesListStore();
    void initDynamicsColumnName();
    void initDynamicsColumnPackage();
    void initDynamicsColumnLibrary();
    void fillDynamics();

    void initMenuPopupDynamics();

    bool onButtonRealeaseDynamics(GdkEventButton*
                                  event);
    void applyRenaming();

    void onClickColumn(int numColum);

    bool isValidName(std::string name)
    {
        size_t i = 0;
        while (i < name.length()) {
            if (!isalnum(name[i])) {
                if (name[i] != '_') {
                    return false;
                }
            }
            i++;
        }
        return true;
    }
};

}} // namespace vle gvle

#endif
