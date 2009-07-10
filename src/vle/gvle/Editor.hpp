/**
 * @file vle/gvle/Editor.hpp
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


#ifndef VLE_GVLE_EDITOR_HPP
#define VLE_GVLE_EDITOR_HPP

#include <vle/graph/Model.hpp>
#include <vle/version.hpp>
#include <gtkmm.h>
#include <gdkmm/gc.h>
#include <gdkmm/types.h>
#include <libglademm/xml.h>

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtksourceviewmm-2.0/gtksourceviewmm.h>
#endif

namespace vle { namespace gvle {

    class GVLE;
    class View;
    class ViewDrawingArea;

/**
 * @brief Document class used within Gtk::Notebook
 */
class Document : public Gtk::ScrolledWindow {
public:

    Document(GVLE* gvle, const std::string& filepath);

    virtual ~Document();

    virtual inline bool isDrawingArea()
	{ return false; }

    inline std::string filename()
	{ return mFileName; }

    inline const std::string filepath() const
	{ return mFilePath; }

    inline void setFilePath(std::string filepath)
	{ mFilePath = filepath; }

    inline bool isModified()
	{ return mModified; }

    inline void setModified(bool modified)
	{ mModified = modified; }

    inline std::string getTitle()
	{ return mTitle; }

    void setTitle(std::string title, graph::Model* model,
		  bool modified);

    virtual void updateView() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;

protected:
    GVLE*          mGVLE;
    bool           mModified;
    std::string    mTitle;

private:
    std::string    mFilePath;
    std::string    mFileName;
};

class DocumentText : public Document {
public:
    DocumentText(GVLE* gvle, const std::string& filePath, bool newfile = false);
    ~DocumentText();

    void save();
    void saveAs(const std::string& filename);

    inline bool isNew() const
    { return mNew == true; }

    void updateView();

    void undo();
    void redo();

private:
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::SourceView mView;
#else
    Gtk::TextView  mView;
#endif
    bool           mModified;
    bool           mNew;

    void init();
    void onChanged();
    std::string getIdLanguage();
    void applyEditingProperties();
};

class DocumentDrawingArea : public Document {
public:
    DocumentDrawingArea(GVLE* gvle, const std::string& filePath,
			View* view, graph::Model* model);
    ~DocumentDrawingArea();

     inline View* getView() const
	{ return mView; }

    inline ViewDrawingArea* getDrawingArea() const
	{ return mArea; }

    inline graph::Model* getModel() const
	{ return mModel; }

    virtual inline bool isDrawingArea() const
	{ return true; }

    void setHadjustment(double h);
    void setVadjustment(double v);

    void updateView();

    void undo();
    void redo();

private:
    View*               mView;
    ViewDrawingArea*    mArea;
    graph::Model*       mModel;
    Gtk::Viewport*      mViewport;
    Gtk::Adjustment     mAdjustWidth;
    Gtk::Adjustment     mAdjustHeight;
};


class Editor : public Gtk::Notebook
{
public:
    typedef std::map < std::string, Document* > Documents;

    Editor(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);

    void setParent(GVLE* gvle)
    { mGVLE = gvle; }

    /* methods to manage tabs */
    void focusTab(const std::string& filepath);
    void openTab(const std::string& filepath);
    void openTabVpz(const std::string& filepath, graph::CoupledModel* model);
    void closeTab(const std::string& filepath);
    void closeVpzTab();
    void closeAllTab();
    void changeTab(GtkNotebookPage* page, int num);

    /**
     * @brief check if the tab is already openend
     */
    inline bool existTab(const std::string& name)
	{ return mDocuments.find(name) != mDocuments.end(); }

    /**
     * @brief check if a vpz tab is already opened
     */
    bool existVpzTab();

    /**
     * @brief Modify the title of the tab
     *
     * @param title the new title
     * @param filepath the key in the document map
     *
     */
    void setModifiedTab(const std::string title, const std::string filepath);

    void createBlankNewFile();
    void closeFile();

    /**
     * @brief Apply settings to the source view
     * @param view The SourceView instance to stylish
     */
    void refreshViews();

    void onUndo();
    void onRedo();

    Documents& getDocumentsList()
    { return mDocuments; }

private:
    Documents   mDocuments;
    GVLE*       mGVLE;

    /**
     * @brief add a label for a new tab
     *
     * @param title the title of the label
     * @param filepath the tab's key on the map mDocuments
     *
     * @return a pointeur of the new label
     */
    Gtk::HBox* addLabel(const std::string& title,
			const std::string& filepath);

};

}}

#endif
