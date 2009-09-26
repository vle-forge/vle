/**
 * @file vle/gvle/Modeling.hpp
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


#ifndef GUI_MODELING_HPP
#define GUI_MODELING_HPP

#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/CoupledModelBox.hpp>
#include <vle/gvle/CutCopyPaste.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/ImportModelBox.hpp>
#include <vle/gvle/ImportClassesBox.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/PluginFactory.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Rand.hpp>
#include <libglademm/xml.h>
#include <map>
#include <string>
#include <vector>
#include <set>

namespace vle { namespace gvle {

class AtomicModelBox;
class ImportModelBox;

class View;

/**
 * @brief This class contains all information on the opened VLE project. It
 * store the graph::Model hierarchy, the gui::GModel hierarchy and all
 * information on the project.
 */
class Modeling
{
public:
    /** define associative map from graph::model name and is reference. */
    typedef std::map < std::string, graph::Model * > MapStringModel;

    /** define vector of current view. */
    typedef std::vector < View * > ListView;

    /** define set of string represent current models names. */
    typedef std::set < std::string > SetString;

    /**
     * create a new Modeling with a ptr.
     *
     * @param gvle to get information on graphics interface.
     * @param filename project file to load.
     */
    Modeling(GVLE* gvle, const std::string& filename = std::string());

    /**
     * delete all models and view.
     */
    ~Modeling();

    /**
     * delete all models, and view.
     */
    void clearModeling();

    void setGlade(Glib::RefPtr < Gnome::Glade::Xml > xml);

    Glib::RefPtr < Gnome::Glade::Xml > getGlade() const;


    /********************************************************************
     *
     * START, XML LOADING AND SAVE
     *
     ********************************************************************/


    /**
     * delete all models and views and restart with one view.
     *
     */
    void start();

    /**
     * parse XML project file to get filename of XML files structures and
     * graphics.
     *
     * @param name project file name to parse.
     */
    void parseXML(const std::string& name);

    /**
     * write XML project file and XML files structures and graphics.
     *
     * @param name project file name to write.
     * @param top a ptr to the top GModel of tree.
     * @param compress you want to gzip ?
     */
    void saveXML(const std::string& name);


    /********************************************************************
     *
     * SET / GET TOP GMODEL
     *
     ********************************************************************/


    /**
     * set the top model ; If model already exist, delete it.
     *
     * @param cp
     */
    void setTopModel(graph::CoupledModel* cp);

    /**
     * return the top model.
     *
     * @return top model of tree.
     */
    inline graph::CoupledModel* getTopModel() {
        return mTop;
    }

    /**
     * return list of class models available from Modeling.
     *
     * @return list of small-models.
     */
    inline const vpz::Classes& getClassModel() const {
        return mVpz.project().classes();
    }

    /**
     * get a class model graph::CoupledModel with name.
     *
     * @param std::string name get graph::CoupledModel
     * @return graph::CoupledModel reference to class model, NULL if
     * class model doesn't exist.
     */
    inline graph::Model* getClassModel(const std::string& name)
	{ return mVpz.project().classes().get(name).model(); }

    /**
     * get the AtomicModelList associate to the class
     *
     * @param name of class model to delete
     */
    inline vpz::AtomicModelList& getAtomicModelClass(std::string className);

    /**
     * add a new list of atomic model for a class
     *
     * @param name of the class
     * @param new_atom : the new list
     */
    inline void addAtomicModelClass(std::string className,
				    graph::AtomicModel* new_atom)
	{ vpz().project().classes().get(className).atomicModels().add(
		new_atom, vpz::AtomicModel("", "", "")); }





    /********************************************************************
     *
     * TWO FUNCTIONS TO CREATE A MAP OF STRING MODEL FROM A MODEL GRAPH
     *
     ********************************************************************/


    /**
     * parse graph structure with top is m and return a map string model of
     * all Model in graph ; this function call a recursive function.
     *
     * @param m model represent top of graph.
     * @return a map string model of all model in graph.
     */
    MapStringModel buidMapString(graph::Model* m);

    /**
     * This recursive function is call by parseStructures ; It parse graph
     * structure with m is current node and add each node in msm.
     *
     * @param m model node.
     * @param msm string model map.
     */
    void parseStructures(graph::Model* m, MapStringModel& msm);


    /********************************************************************
     *
     * VIEW
     *
     ********************************************************************/


    /**
     * show dynamics for a plugin
     *
     * @param name the name of GModel dynamics to show.
     */
    void showDynamics(const std::string& name);

    /**
     * add a view to modeling for the GModel model.
     *
     * @param model a ptr to the GModel to show.
     */
    void addView(graph::Model* model);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     * @return int index in list view for founded view or first index with
     * null value
     */
    void addView(graph::CoupledModel* model);

    /**
     * add a view to the current class
     *
     * @param model to ptr to the GModel assiociate to the class
     */
    void addViewClass(graph::Model* model, std::string name);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     */

    void addViewClass(graph::CoupledModel* model, std::string name);

    /**
     * Return True if a View of the parameter already exist.
     * @param model reference to GCoupledModel to find.
     * @return Return True if the View exist , False otherwise
     */
    bool existView(graph::CoupledModel* model);

    /**
     * Return the View that reference the model.
     * @param model reference to GCoupledModel to find.
     * @return Return the View if exist , NULL otherwise
     */
    View* findView(graph::CoupledModel* model);

    /**
     * delete a view from model view.
     *
     * @param index num of window to delete.
     */
    void delViewIndex(size_t index);

    /**
     * delete all view where coupled model is equal to cm.
     *
     * @param cm a ptr to graph::CoupledModel to delete view.
     */
    void delViewOnModel(const graph::CoupledModel* cm);

    /**
     * delete all view of modeling.
     *
     */
    void delViews();


    void refreshViews();

    void EditCoupledModel(graph::CoupledModel* model);

    /**
     * @brief Assign a new titles to all gvle::View.
     */
    void setTitles();

    /**
     * @brief Assign a new titles to all gvle::View and an asterisk to inform
     * that this model is modified.
     */
    void setModifiedTitles();


    /********************************************************************
     *
     * MANAGE GVLE INTERFACE
     *
     ********************************************************************/


    /**
     * get current button selected in GVLE panel.
     *
     * @return selected button.
     */
    inline GVLE::ButtonType getCurrentButton() const
	{ return mGVLE->getCurrentButton(); }

    /**
     * return a ptr to application GVLE.
     *
     * @return ptr to application GVLE.
     */
    inline GVLE* getGVLE()
	{ setModified(true); return mGVLE; }

    inline void hideGVLE()
	{ mGVLE->hide(); }

    inline const Editor::Documents& getDocuments() const
	{ return mGVLE->getEditor()->getDocuments(); }

    /********************************************************************
     *
     * OTHER USEFULL FUNCTION
     *
     ********************************************************************/


    /**
     * return true if a model with name 'name' already exist in tree.
     *
     * @param name string to search.
     * @return true if a model with name 'name' is found.
     */
    bool exist(const std::string& name) const;

    /**
     * return true if a model with ptr = m exist in tree.
     *
     * @param m model to find.
     * @return true if a model with ptr equal to m is found.
     */
    bool exist(const graph::Model* m) const;


    /********************************************************************
     *
     * CUT COPY AND PASTE
     *
     ********************************************************************/


    /**
     * clone the ListGModel if has no connection with external model.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void cut(graph::ModelList& lst, graph::CoupledModel* gc, std::string className);

    /**
     * detach the list of GModel of GCoupledModel parent.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void copy(graph::ModelList& lst, graph::CoupledModel* gc, std::string className);

    /**
     * paste the current list GModel into GCoupledModel ; rename
     * models to elimiante duplicated name.
     *
     * @param gc paste selected GModel under this GCoupledModel.
     */
    void paste(graph::CoupledModel* gc, std::string className);


    /********************************************************************
     *
     * COMPRESSED, MODIFIED AND SAVED
     *
     ********************************************************************/


    /**
     * Return true if model is modified since last new or loading.
     *
     * @return true if model is modified, otherwise false.
     */
    inline bool isModified() const {
        return mIsModified;
    }

    /**
     * Return true if model is saved.
     *
     * @return true if model is saved, otherwise.
     */
    inline bool isSaved() const {
        return mIsSaved;
    }

    /**
     * Set document modification.
     *
     * @param modified true if document is modified, otherwise false.
     */
    void setModified(bool modified);

    /**
     * Set document save.
     *
     * @param saved true if document is saved, otherwise false.
     */
    inline void setSaved(bool saved) {
        mIsSaved = saved;
    }

    /**
     * Get file name of current document.
     *
     * @return file name of current document.
     */
    inline const std::string& getFileName() const {
        return mFileName;
    }

    /**
     * Set current file name for document.
     *
     * @param name filename of current document.
     */
    inline void setFileName(const std::string& name) {
        mFileName.assign(name);
    }

    /**
     * Get compressed vpz format.
     *
     * @return true if vpz is compressed.
     */
    inline bool isCompressed() const {
        return mIsCompressed;
    }

    /**
     * Set compressed vpz format, true for gzip, false to text.
     *
     * @param compress true to compress vpz file, false otherwise.
     */
    inline void setCompressed(bool compress) {
        mIsCompressed = compress;
    }

    void exportCoupledModel(graph::CoupledModel* model, vpz::Vpz* dst, std::string className);

    void importModel(graph::CoupledModel* parent, vpz::Vpz* src);

    void importModelToClass(vpz::Vpz* src, std::string& className);

    void importClasses(vpz::Vpz* src);

    void exportClass(graph::Model* model, vpz::Class classe, vpz::Vpz* dst);

    /********************************************************************
     *
     * MANAGE MODELS NAME
     *
     ********************************************************************/


    /**
     * return true if model name exist in models names list.
     *
     * @param name name to test existing.
     * @return true if model exist in list, otherwise false.
     */
    bool existModelName(const std::string& name) const;

    /**
     * test if name is a correct name i.e. only ascii char, only char:
     * [[A-Z][a-z]]* and not exist in model name.
     *
     * @param name std::string to test validity.
     * @return true if name is correct, false otherwise.
     */
    bool isCorrectName(const Glib::ustring& name) const;

    /**
     * get a new string using Gtk::Dialog class.
     *
     * @param name output parameter fill by correct new name.
     * @return true if name is ok, false otherwise.
     */
    bool getNewName(std::string& name) const;

    /**
     * delete a string from list.
     *
     * @param name model name to delete.
     */
    void delName(const std::string& name);

    /**
     * delete all string from list.
     */
    void delNames();

    /**
     * add a model name into list.
     *
     * @param name a new name to add.
     */
    void addName(const std::string& name);

    /**
     * Get a model name with prefix name ; Name must include only ascii
     * char.
     * @code
     * if name equal :
     *    "toto" then, return "toto_"
     *    "toto" and "toto_" exist, return "toto[A-z]_"
     * if name is empty,
     *    return "[A-z]*8_" minimum, add [A-z] if model exist
     * @endcode
     *
     * @param name a prefix for model name.
     * @return a prefix for a model like "toto_".
     */
    std::string getNameWithPrefix(const std::string& name);

    /**
     * return list of model name.
     *
     * @return list of model name.
     */
    const SetString& getNames() const;



    /********************************************************************
     *
     * MANAGE MEASURE - INTERFACE FOR MEASURES CLASS
     *
     ********************************************************************/

    const vpz::Experiment& experiment() const
    { return mVpz.project().experiment(); }

    vpz::Experiment& experiment()
    { setModified(true); return mVpz.project().experiment(); }

    const vpz::Views& views() const
    { return mVpz.project().experiment().views(); }

    vpz::Views& views()
    { setModified(true); return mVpz.project().experiment().views(); }

    const vpz::Outputs& outputs() const
    { return mVpz.project().experiment().views().outputs(); }

    vpz::Outputs& outputs()
    { setModified(true); return mVpz.project().experiment().views().outputs(); }

    /********************************************************************
     *
     * DYNAMICS - CONDITIONS - OBSERVABLES
     *
     ********************************************************************/

    vpz::Dynamics& dynamics()
    { setModified(true); return mVpz.project().dynamics(); }

    vpz::Observables& observables()
    {
        setModified(true);
        return mVpz.project().experiment().views().observables();
    }

    vpz::Conditions& conditions()
    {
        setModified(true);
        return mVpz.project().experiment().conditions();
    }

    inline vpz::AtomicModel& get_model(graph::AtomicModel* atom, const std::string& className = "")
    {
        setModified(true);
	if (className == "") {
	    return mVpz.project().model().atomicModels().get(atom);
	} else {
	    return mVpz.project().classes().get(className).atomicModels().get(atom);
	}
    }

    const vpz::Strings* get_conditions(graph::AtomicModel* atom);

    /********************************************************************
     *
     * SOCKET INFORMATION
     *
     ********************************************************************/


    /**
     * Return currently use port to connect VLE and GVLE.
     *
     * @return a port number between [0-65536], default is 8000.
     */
    inline int getSocketPort() const {
        return mSocketPort;
    }

    /**
     * Set a new port to connect VLE and GVLE.
     *
     * @param socket change used port by a new if port number [0-65535].
     */
    inline void setSocketPort(int socket) {
        if (socket > 0 and socket < 65535) mSocketPort = socket;
    }


    /********************************************************************
     *
     * CREATE GATOMICMODEL AND GCOUPLEDMODEL
     *
     ********************************************************************/

    graph::CoupledModel* newCoupledModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    graph::AtomicModel* newAtomicModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    graph::Model* newPluginModel(
        graph::CoupledModel* parent,
        const std::string& name,
        const std::string& description,
        int x, int y);

    void renameModel(graph::Model* model,
                     const std::string& name,
                     const std::string& description);

    void delModel(graph::Model* model, std::string className);

    inline const vpz::Vpz& vpz() const {
        return mVpz;
    }

    inline vpz::Vpz& vpz()
    { setModified(true); return mVpz; }

    void vpz_is_correct(std::vector<std::string>& vec);

    /********************************************************************
     *
     * MANAGE THE DRAWING SETTINGS
     *
     ********************************************************************/

    inline void setForegroundColor(Color color)
    { mForegroundColor = color; }

    inline const Color getForegroundColor() const
    { return mForegroundColor; }

    inline void setBackgroundColor(const Color color)
    { mBackgroundColor = color; }

    inline const Color getBackgroundColor() const
    { return mBackgroundColor; }

    inline void setSelectedColor(const Color color)
    { mSelectedColor = color; }

    inline const Color getSelectedColor() const
    { return mSelectedColor; }

    inline void setCoupledColor(const Color color)
    { mCoupledColor = color; }

    inline const Color getCoupledColor() const
    { return mCoupledColor; }

    inline void setAtomicColor(const Color color)
    { mAtomicColor = color; }

    inline const Color getAtomicColor() const
    { return mAtomicColor; }

    inline void setConnectionColor(const Color color)
    { mConnectionColor = color; }

    inline const Color getConnectionColor() const
    { return mConnectionColor; }

    inline void setFont(const std::string font)
    { mFont = font; }

    inline const std::string getFont() const
    { return mFont; }

    inline void setFontSize(const double size)
    { mFontSize = size; }

    inline double getFontSize() const
    { return mFontSize; }

    inline void setLineWidth(const double width)
    { mLineWidth = width; }

    inline double getLineWidth() const
    { return mLineWidth; }

    inline utils::Rand* getRand() const
    { return mRand; }


    /********************************************************************
     *
     * MANAGE THE EDITOR SETTINGS
     *
     ********************************************************************/
    inline void setHighlightSyntax(bool syntax)
    { mHighlightSyntax = syntax; }

    inline bool getHighlightSyntax()
    { return mHighlightSyntax; }

    inline void setHighlightBrackets(bool brackets)
    { mHighlightBrackets = brackets; }

    inline bool getHighlightBrackets()
    { return mHighlightBrackets; }

    inline void setHighlightLine(bool line)
    { mHighlightLine = line; }

    inline bool getHighlightLine()
    { return mHighlightLine; }

    inline void setLineNumbers(bool numbers)
    { mLineNumbers = numbers; }

    inline bool getLineNumbers()
    { return mLineNumbers; }

    inline void setRightMargin(bool margin)
    { mRightMargin = margin; }

    inline bool getRightMargin()
    { return mRightMargin; }

    inline void setAutoIndent(bool auto_indent)
    { mAutoIndent = auto_indent; }

    inline bool getAutoIndent()
    { return mAutoIndent; }

    inline void setIndentOnTab(bool indent_tab)
    { mIndentOnTab = indent_tab; }

    inline bool getIndentOnTab()
    { return mIndentOnTab; }

    inline void setIndentSize(int size)
    { mIndentSize = size; }

    inline int getIndentSize()
    { return mIndentSize; }

    inline void setSmartHomeEnd(bool smart)
    { mSmartHomeEnd = smart; }

    inline bool getSmartHomeEnd()
    { return mSmartHomeEnd; }

    inline void setFontEditor(const std::string& font)
    { mFontEditor = font; }

    inline std::string& getFontEditor()
    { return mFontEditor; }

    /********************************************************************
     *
     * MANAGE OUTPUT AND CONDITION PLUG-INS
     *
     ********************************************************************/

    /**
     * @brief Get a constant reference to the PluginFactory.
     * @return A constant reference.
     */
    const PluginFactory& pluginFactory() const
    { return mPluginFactory; }

    /**
     * @brief Get a reference to the PluginFactory.
     * @return A reference.
     */
    PluginFactory& pluginFactory()
    { return mPluginFactory; }

private:
    vpz::Vpz                    mVpz;
    graph::CoupledModel*        mTop;
    std::string                 mCurrentClass;
    GVLE*                       mGVLE;
    utils::Rand*                mRand;
    ListView                    mListView;
    CutCopyPaste                mCutCopyPaste;
    bool                        mIsModified;
    bool                        mIsSaved;
    bool                        mIsCompressed;
    std::string                 mFileName;
    SetString                   mModelsNames;
    int                         mSocketPort;
    AtomicModelBox*             mAtomicBox;
    ImportModelBox*             mImportModelBox;
    ImportClassesBox*           mImportClassesBox;
    CoupledModelBox*            mCoupledBox;
    PluginFactory               mPluginFactory;

    // Drawing attributes
    Color                       mForegroundColor;
    Color                       mBackgroundColor;
    Color                       mSelectedColor;
    Color                       mCoupledColor;
    Color                       mAtomicColor;
    Color                       mConnectionColor;
    std::string                 mFont;
    double                      mFontSize;
    double                      mLineWidth;

    // Editor attributes
    bool mHighlightSyntax;
    bool mHighlightBrackets;
    bool mHighlightLine;
    bool mLineNumbers;
    bool mRightMargin;
    bool mAutoIndent;
    bool mIndentOnTab;
    int  mIndentSize;
    bool mSmartHomeEnd;
    std::string mFontEditor;

    Glib::RefPtr < Gnome::Glade::Xml >  mRefXML;

    void export_atomic_model(vpz::Vpz* dst, graph::AtomicModel* atom, std::string className = "");
    void export_coupled_model(vpz::Vpz* dst, graph::CoupledModel* atom, std::string className = "");

    void import_atomic_model(vpz::Vpz* src, graph::AtomicModel* atom, std::string className = "");
    void import_coupled_model(vpz::Vpz* src, graph::CoupledModel* atom, std::string className = "");
};

void parse_recurs(graph::Model*, vpz::Vpz*, int level = 0);
void parse_model(vpz::AtomicModelList& list);

} } // namespace vle gvle

#endif
