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


#ifndef VLE_GVLE_FILETREEVIEW_HPP
#define VLE_GVLE_FILETREEVIEW_HPP

#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <glibmm/ustring.h>
#include <gtkmm/uimanager.h>

namespace vle { namespace gvle {

class GVLE;

/**
 * @brief Tree model columns used in FileTreeView.
 */
class FileModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    FileModelColumns()
    {
        add(mColname);
    }

    Gtk::TreeModelColumn < Glib::ustring > mColname;
};

/**
 * @brief FileTreeView used for display file hierarchy.
 */
class FileTreeView : public Gtk::TreeView
{
public:
    FileTreeView(BaseObjectType* cobject,
                 const Glib::RefPtr < Gtk::Builder >& refGlade);

    virtual ~FileTreeView();

    void build();

    void clear();

    void refresh();

    Gtk::TreeModel::iterator getFileRow(
        std::vector <std::string> path,
        Gtk::TreeModel::Children child);

    void showRow(std::string path);

    inline void setPackage(const std::string& package)
    { mPackage = package; }

    void setParent(GVLE* parent)
    { mParent = parent; }

protected:
    bool on_button_press_event(GdkEventButton* event);
    bool onSelectHighlightOnly(const Glib::RefPtr<Gtk::TreeModel>& model,
                               const Gtk::TreeModel::Path& path, bool info);
    bool onSelect(const Glib::RefPtr<Gtk::TreeModel>& model,
                  const Gtk::TreeModel::Path& path, bool info);
    bool on_foreach(const Gtk::TreeModel::Path&,
                    const Gtk::TreeModel::iterator& iter);
    void onEdition(const Glib::ustring& pathString,
                   const Glib::ustring& newName);
    void onEditionStarted(Gtk::CellEditable* cellEditable,
                          const Glib::ustring& /* path */);
    bool onEvent(GdkEvent* event);
    void onOpen();
    void onNewFile();
    void onNewDirectory();
    void onCopy();
    void onPaste();
    void onRemove();
    void onRemoveCallBack(const Gtk::TreeModel::iterator& it);
    void onRename();

private:
    /**
     * Create the hierarchy with the directories
     *
     * @param parent the parent's of the current row
     * @param dirname the name of the parent directory
     */
    void buildHierarchyDirectory(const Gtk::TreeModel::Row* parent,
                                 const std::string& dirname);

    /**
     * Create the hierarchy with the files
     *
     * @param parent the parent's of the current row
     * @param dirname the name of the parent directory
     *
     */
    void buildHierarchyFile(const Gtk::TreeModel::Row* parent,
                            const std::string& dirname);

    void buildHierarchy(const Gtk::TreeModel::Row* parent,
                        const std::string& dirname);
    bool isDirectory(const std::string& dirname);
    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn*  column);
    void projectFilePath(const Gtk::TreeRow& row,
                         std::list<std::string>& lst);

    void refreshHierarchy(const Gtk::TreeModel::Row* parent,
                          const std::string& dirname);
    void refreshHierarchyDirectory(const Gtk::TreeModel::Row* parent,
                                   const std::string& dirname);
    void refreshHierarchyFile(const Gtk::TreeModel::Row* parent,
                              const std::string& dirname);
    void removeFiles(const Gtk::TreeModel::Row* parent,
                     const std::list < std::string >& entries);

    GVLE*                            mParent;
    Gtk::Menu*                       mMenuPopup;
    FileModelColumns                 mColumns;
    std::string                      mPackage;
    Glib::RefPtr<Gtk::TreeStore>     mTreeModel;
    Glib::RefPtr<Gtk::TreeSelection> mTreeSelection;
    Gtk::TreeModel::Path             mRecentSelectedPath;
    std::list<std::string>           mIgnoredFilesList;

    Glib::ustring                    mOldName;
    std::string                      m_search;
    std::string                      mAbsolutePath;
    std::string                      mFilePath;
    std::string                      mFileName;

    //Cell
    int                              mColumnName;
    bool                             mValidateRetry;
    Gtk::CellRendererText*           mCellrenderer;
    Glib::ustring                    mInvalidTextForRetry;
    guint32                          mDelayTime;
    
    Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
};

}} // namespace vle gvle

#endif
