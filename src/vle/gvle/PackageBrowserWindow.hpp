/**
 * @file vle/gvle/PackageBrowserWindow.hpp
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

#ifndef VLE_GVLE_PACKAGEBROWSER_HPP
#define VLE_GVLE_PACKAGEBROWSER_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <map>
#include <string>

namespace vle { namespace gvle {
    class Modeling;
    class PackageBrowserMenu;

/**
 * @brief Document class used within Gtk::Notebook
 */
/*class Document : public Gtk::ScrolledWindow {
public:
    Document(const std::string& filePath, bool newfile = false);
    ~Document();

    void save();
    void saveAs(const std::string& filename);

    inline const std::string filename() const
    { return mFileName; }

    inline const std::string filepath() const
    { return mFilePath; }

    inline bool isNew() const
    { return mNew == true; }

private:
    Gtk::TextView  mView;
    std::string    mFilePath;
    std::string    mFileName;
    bool           mModified;
    bool           mNew;

    void init();
    };*/

class PackageBrowserWindow {
public:
    /* define a map with a file path and its buffer */
    //typedef std::map < std::string, Document* > Documents;

    /* Constructor */
    PackageBrowserWindow(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling);

    /* Show the package management window */
    void show();
    void hide();
    bool is_visible() { return mWindow->is_visible(); }

    /* methodes to manage tabs */
    void focusTab(const std::string& filepath);
    void openTab(const std::string& filepath);
    void closeTab(const std::string& filepath);

    /* methods to manage the project throw menu Package */
    void configureProject();
    void buildProject();
    void cleanProject();
    void packageProject();

    /* methods to manage files throw menu File */
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();

private:
    /**
     * @brief Tree model columns used in FileTreeView
     */
    class FileModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
	FileModelColumns()
	    { add(m_col_name); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    /**
     * @brief FileTreeView used for display file hierarchy
     */
    class FileTreeView : public Gtk::TreeView
    {
    public:
	FileTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
        virtual ~FileTreeView();

        void build();

	void clear();

	inline void setPackage(const std::string& package)
        { mPackage = package; }

	void setParent(PackageBrowserWindow* parent)
	    { mParent = parent; }

        Glib::ustring getSelected()
        { return (*mRefTreeSelection->get_selected())[mColumns.m_col_name]; }
    private:
	void buildHierarchy(const Gtk::TreeModel::Row& parent,
			    const std::string& dirname);
	bool isDirectory(const std::string& dirname);
	void on_row_activated(const Gtk::TreeModel::Path& path,
			      Gtk::TreeViewColumn*  column);
	std::list<std::string>* projectFilePath(const Gtk::TreeRow& row);

	PackageBrowserWindow*            mParent;
	FileModelColumns                 mColumns;
	std::string                      mPackage;
	Glib::RefPtr<Gtk::TreeStore>     mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;
    };

    /**
     * PackageBrowserWindow attributes
     */
    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;
    Gtk::Window*                         mWindow;
    Gtk::TextView*                       mLog;
    Gtk::Statusbar*                      mStatusbar;
    FileTreeView*                        mFileTreeView;
    Gtk::Notebook*                       mNotebook;
    PackageBrowserMenu*                  mMenuBar;
    std::string                          mPackage;
    //Documents                            mDocuments;

    void buildPackageHierarchy();
};

}} //namespace vle gvle

#endif
