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


#ifndef VLE_GVLE_EDITOR_HPP
#define VLE_GVLE_EDITOR_HPP

#include <vle/gvle/DllDefines.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/gvle/CompleteViewDrawingArea.hpp>
#include <vle/gvle/SimpleViewDrawingArea.hpp>
#include <vle/version.hpp>
#include <gtkmm/textview.h>
#include <gtkmm/viewport.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/notebook.h>
#include <gtkmm/builder.h>
#include <glibmm.h>

namespace vle { namespace gvle {

class GVLE;
class View;

/**
 * @brief Document class used within Gtk::Notebook
 */
class GVLE_API Document : public Gtk::ScrolledWindow
{
public:
    Document(GVLE* gvle, const std::string& filepath);

    Document();

    virtual ~Document() { }

    virtual inline bool isDrawingArea() const
    { return false; }

    inline const std::string& filename() const
    { return mFileName; }

    inline void setFileName(const std::string& filename)
    { mFileName = filename; }

    inline const std::string& filepath() const
    { return mFilePath; }

    inline void setFilePath(const std::string& filepath)
    { mFilePath = filepath; }

    inline bool isModified() const
    { return mModified; }

    inline void setModified(bool modified)
    { mModified = modified; }

    inline const std::string& getTitle() const
    { return mTitle; }

    void setTitle(const std::string& fileName,
                  vpz::BaseModel* model,
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

class GVLE_API DocumentText : public Document
{
public:
    DocumentText(GVLE* gvle, const std::string& filePath, bool newfile = false,
                 bool hasFullName = false);

    DocumentText(const std::string& buffer);

    virtual ~DocumentText();

    void save();
    void saveAs(const std::string& filename);

    inline bool isNew() const
    { return mNew == true; }

    inline bool hasFullName() const
    { return mHasFullName; }

    void updateView();

    virtual void undo();
    virtual void redo();

    void paste();
    void copy();
    void cut();
    void selectAll();

    std::string getBuffer();

private:
    GtkWidget *mView;

    bool           mModified;
    bool           mNew;
    bool           mHasFullName;
    std::string    mIdLang;

    void init(const std::string& buffer);
    void onChanged();
    static void on_changed(GtkWidget *widget, gpointer label);
    std::string guessIdLanguage();
    void applyEditingProperties();
};

class GVLE_API DocumentDrawingArea : public Document
{
public:
    DocumentDrawingArea(GVLE* gvle, const std::string& filePath,
                        View* view, vpz::BaseModel* model);
    virtual ~DocumentDrawingArea();

    inline View* getView() const
    { return mView; }

    inline CompleteViewDrawingArea* getCompleteDrawingArea() const
    { return mCompleteArea; }

    inline SimpleViewDrawingArea* getSimpleDrawingArea() const
    { return mSimpleArea; }

    inline vpz::BaseModel* getModel() const
    { return mModel; }

    virtual inline bool isDrawingArea() const
    { return true; }

    virtual bool isComplete()=0;

    void setHadjustment(double h);
    void setVadjustment(double v);

    void updateView();
    void updateCursor();

    virtual void undo();
    virtual void redo();

protected:
    View*                       mView;
    CompleteViewDrawingArea*    mCompleteArea;
    SimpleViewDrawingArea*      mSimpleArea;
    vpz::BaseModel*       mModel;
    Gtk::Viewport*      mViewport;
    Glib::RefPtr<Gtk::Adjustment>     mAdjustWidth;
    Glib::RefPtr<Gtk::Adjustment>     mAdjustHeight;
};

class GVLE_API DocumentCompleteDrawingArea : public DocumentDrawingArea
{
public:
    DocumentCompleteDrawingArea(GVLE* gvle, const std::string& filePath,
                                View* view, vpz::BaseModel* model);
    virtual ~DocumentCompleteDrawingArea();

    virtual bool isComplete()
    { return true; }

private:
};

class GVLE_API DocumentSimpleDrawingArea : public DocumentDrawingArea
{
public:
    DocumentSimpleDrawingArea(GVLE* gvle, const std::string& filePath,
                              View* view, vpz::BaseModel* model);
    virtual ~DocumentSimpleDrawingArea();

    virtual bool isComplete()
    { return false; }

};

class GVLE_API Editor : public Gtk::Notebook
{
public:
    typedef std::map < std::string, Document* > Documents;

    Editor(BaseObjectType* cobject,
           const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
    virtual ~Editor();

    /**
     * @brief check if the tab is already openend
     */
    void changeFile(const std::string& oldName, const std::string& newName);

    void changeTab(Gtk::Widget *page, gint num);
    void changeAndSignal(Gtk::Widget *page, gint num);
    void refreshTab();

    void closeAllTab();
    void closeTab(const std::string& filepath);
    void closeVpzTab();

    void createBlankNewFile();
    void createBlankNewFile(const std::string& path,
                            const std::string& fileName);

    inline bool existTab(const std::string& name)
    { return mDocuments.find(name) != mDocuments.end(); }

    /**
     * @brief check if a vpz tab is already opened
     */
    bool existVpzTab();

    void focusTab(const std::string& filepath);
    void focusAndSignal(const std::string& filepath);

    const Documents& getDocuments() const
    { return mDocuments; }

    void onRedo();
    void onUndo();

    void openTab(const std::string& filepath);
    void openTabVpz(const std::string& filepath, vpz::CoupledModel* model);

    /**
     * @brief create a tab for the current vpz file with the complete
     * view
     * @param filepath the current file
     * @param model the current model
     */
    void showCompleteView(const::std::string& filepath,
                          vpz::CoupledModel* model);

    /**
     * @brief create a tab for the current vpz file with the simple
     * view
     * @param filepath the current file
     * @param model the current model
     */
    void showSimpleView(const std::string& filepath,
                        vpz::CoupledModel* model);


    /**
     * @brief Apply settings to the source view
     * @param view The SourceView instance to stylish
     */
    void refreshViews();

    /**
     * @brief Modify the title of the tab
     *
     * @param title the new title
     * @param filepath the key in the document map
     *
     */
    void setModifiedTab(const std::string& title,
                        const std::string& newFilePath,
                        const std::string& oldFilePath);

    void setParent(GVLE* gvle)
    { mGVLE = gvle; }

    inline DocumentDrawingArea* getDocumentDrawingArea() const
    {
        Documents::const_iterator it = mDocuments.begin();
        while (it != mDocuments.end() and
               it->second->isDrawingArea() == false) {
            ++it;
        }

        if (it != mDocuments.end()) {
            return dynamic_cast < DocumentDrawingArea* > (it->second);
        } else {
            return 0;
        }
    }

private:
    void onCloseTab(const std::string& filepath);

    Documents   mDocuments;
    GVLE*       mGVLE;
    sigc::connection tabSignal;

    /**
     * @brief add a label for a new tab
     *
     * @param title the title of the label
     * @param filepath the tab's key on the map mDocuments
     *
     * @return a pointeur of the new label
     */
    Gtk::Box* addLabel(const std::string& title,
                        const std::string& filepath);
};

}} // namespace vle gvle

#endif
