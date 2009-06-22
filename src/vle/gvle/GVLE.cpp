/**
 * @file vle/gvle/GVLE.cpp
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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/About.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ExecutionBox.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/HostsBox.hpp>
#include <vle/gvle/PluginTable.hpp>
#include <vle/gvle/GVLEMenu.hpp>
#include <vle/gvle/PluginPlus.hpp>
#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <boost/filesystem.hpp>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/miscutils.h>

namespace vle { namespace gvle {

Document::Document() :
    Gtk::ScrolledWindow()
{

}

DocumentText::DocumentText(const std::string& filepath, bool newfile) :
    Document(),
    mFilePath(filepath),
    mModified(false),
    mNew(newfile)
{
    mFileName = boost::filesystem::basename(filepath);
    init();
}

DocumentText::~DocumentText()
{
}

void DocumentText::save()
{
    try {
	std::ofstream file(mFilePath.c_str());
	file << mView.get_buffer()->get_text();
	file.close();
	mNew = mModified = false;
    } catch(std::exception& e) {
	throw _("Error while saving file.");
    }
}

void DocumentText::saveAs(const std::string& filename)
{
    mFilePath = filename;
    save();
}

void DocumentText::init()
{
    if (not mNew) {
	std::ifstream file(mFilePath.c_str());
	if (file) {
	    std::stringstream filecontent;
	    filecontent << file.rdbuf();
	    file.close();
	    mView.get_buffer()->insert(mView.get_buffer()->end(), filecontent.str());
	    add(mView);
	} else {
	    throw std::string("cannot open: " + mFileName);
	}
    } else {
	add(mView);
    }
}

DocumentDrawingArea::DocumentDrawingArea(const std::string& filepath,
					 View* view,
					 graph::Model* model) :
    Document(),
    mFilePath(filepath),
    mView(view),
    mModel(model)
{
    mArea = new ViewDrawingArea(mView);
}

DocumentDrawingArea::~DocumentDrawingArea()
{
    delete mModel;
    delete mArea;
}

GVLE::FileTreeView::FileTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject)
{

    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column(_("Files"), mColumns.m_col_name);
    mRefTreeSelection = get_selection();
    mIgnoredFilesList.push_front("build");
}

GVLE::FileTreeView::~FileTreeView()
{
}

void GVLE::FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    Glib::Dir::iterator it;
    for (it = dir.begin(); it != dir.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(), mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
	    row[mColumns.m_col_name] = *it;
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		buildHierarchy(*row, nextpath);
	    }
	}
    }
}

void GVLE::FileTreeView::clear()
{
    mRefTreeModel->clear();
}

void GVLE::FileTreeView::build()
{
    if (not mPackage.empty()) {
	Gtk::TreeIter iter = mRefTreeModel->append();
	Gtk::TreeModel::Row row = *iter;
	row[mColumns.m_col_name] = boost::filesystem::basename(mPackage);
	buildHierarchy(*row, mPackage);
	expand_row(Gtk::TreePath(iter), false);
    }
}

bool GVLE::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

void GVLE::FileTreeView::on_row_activated(
    const Gtk::TreeModel::Path&,
    Gtk::TreeViewColumn*)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
    const Gtk::TreeModel::Row row = *it;
    const std::list<std::string>* lstpath = projectFilePath(row);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(*lstpath));
    if (not isDirectory(absolute_path)) {
	mParent->openTab(absolute_path);
    }
    else {
	if (not row_expanded(Gtk::TreePath(it)))
	    expand_row(Gtk::TreePath(it), false);
	else
	    collapse_row(Gtk::TreePath(it));
    }
}

std::list<std::string>* GVLE::FileTreeView::projectFilePath(
    const Gtk::TreeRow& row)
{
    if (row.parent()) {
	std::list<std::string>* lst =
	    projectFilePath(*row.parent());
	lst->push_back(std::string(row.get_value(mColumns.m_col_name)));
	return lst;
    } else {
	return new std::list<std::string>();
    }
}

GVLE::GVLE(BaseObjectType* cobject,
	   const Glib::RefPtr<Gnome::Glade::Xml> xml):
       Gtk::Window(cobject),
        m_vbox(false, 2),
        m_buttons(2, 4, true),
        //m_labelName("Plugins / Category"),
        m_arrow(m_buttonGroup),
        m_addModels(m_buttonGroup),
        m_addLinks(m_buttonGroup),
        m_addCoupled(m_buttonGroup),
        m_delete(m_buttonGroup),
        m_zoom(m_buttonGroup),
        m_question(m_buttonGroup),
        m_modeling(new Modeling(this)),
        m_currentButton(POINTER),
        m_helpbox(0),
        mCurrentView(0)
{
    mRefXML = xml;
    m_modeling->setGlade(mRefXML);

    mConditionsBox = new ConditionsBox(mRefXML, m_modeling);
    mSimulationBox = new LaunchSimulationBox(mRefXML, m_modeling);
    mPreferencesBox = new PreferencesBox(mRefXML, m_modeling);
    mOpenPackageBox = new OpenPackageBox(mRefXML, m_modeling);
    mOpenVpzBox = new OpenVpzBox(mRefXML, m_modeling);
    mNewProjectBox = new NewProjectBox(mRefXML, m_modeling);
    mSaveVpzBox = new SaveVpzBox(mRefXML, m_modeling);

    mRefXML->get_widget("StatusBarPackageBrowser", mStatusbar);
    mRefXML->get_widget("TextViewLogPackageBrowser", mLog);
    mRefXML->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    mRefXML->get_widget("NotebookPackageBrowser", mNotebook);
    mRefXML->get_widget_derived("MenuBarPackageBrowser", m_menu);
    m_menu->setParent(this);
    m_menu->makeMenus();
    m_menu->onFileMode();
    mRefXML->get_widget("ToolBar", mToolBar);
    mToolBar->insert(m_arrow, 0);
    mToolBar->insert(m_addModels, -1);
    mToolBar->insert(m_addLinks, -1);
    mToolBar->insert(m_addCoupled, -1);
    mToolBar->insert(m_delete, -1);
    mToolBar->insert(m_zoom, -1);
    mToolBar->insert(m_question,-1);
    mRefXML->get_widget_derived("TreeViewModel", mModelTreeBox);
    mModelTreeBox->setModeling(m_modeling);
    mRefXML->get_widget_derived("TreeViewClass", mModelClassBox);
    mModelClassBox->createNewModelBox(m_modeling);
    show();

    mNotebook->signal_switch_page().connect(
	sigc::mem_fun(this, &GVLE::changeTab));

    //loadObserverPlugins(utils::Path::path().getDefaultObserverPluginDir());
    //loadObserverPlugins(utils::Path::path().getUserObserverPluginDir());

    //loadPlugins(utils::Path::path().getDefaultGUIPluginDir());
    //loadPlugins(utils::Path::path().getUserGUIPluginDir());

    makeButtons();

    //m_pluginTable = new PluginTable(this, m_buttonGroup, m_category,
    //       m_plugins);

    //makeComboCategory();

    m_arrow.set_active(true);
    /*m_status.push("Selection");
    //m_scrolledPlugins.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //m_scrolledPlugins.add(*m_pluginTable);
    m_vbox.pack_start(*m_menu, false, false);
    m_vbox.pack_start(m_buttons, false, false);
    m_vbox.pack_start(m_separator, false, false);
    //m_vbox.pack_start(m_labelName, false, false);
    //m_vbox.pack_start(m_comboCategory, false, false);
    //m_vbox.pack_start(m_scrolledPlugins, true, true);
    m_vbox.pack_start(m_status, false, false);
    add(m_vbox);

    show_all();

    if (filename.empty() == false) {
        m_modeling->parseXML(filename);
	}*/

    m_modeling->setModified(false);
}

GVLE::~GVLE()
{
    //delPlugins();

    //delete m_pluginTable;
    delete m_modeling;

    delete mConditionsBox;
    delete mSimulationBox;
    delete mPreferencesBox;
    delete mOpenPackageBox;
    delete mOpenVpzBox;
    delete mNewProjectBox;
    delete mSaveVpzBox;
}

void GVLE::show()
{
    mPackage = vle::utils::Path::path().getPackageDir();
    set_title(std::string(_("Browsing package: ")).append(
			   boost::filesystem::basename(mPackage)));
    buildPackageHierarchy();
    show_all();
}

void GVLE::buildPackageHierarchy()
{
    mPackage = vle::utils::Path::path().getPackageDir();
    mFileTreeView->clear();
    mFileTreeView->setPackage(mPackage);
    mFileTreeView->build();
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and
	boost::filesystem::extension(name) == ".vpz") {
	m_modeling->parseXML(name);
	m_menu->onViewMode();
    }
    m_modeling->setModified(false);
}

void GVLE::redrawModelTreeBox()
{
    assert(m_modeling->getTopModel());
    mModelTreeBox->parseModel(m_modeling->getTopModel());
}

void GVLE::redrawModelClassBox()
{
    mModelClassBox->parseClass();
}

bool GVLE::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
	onMenuQuit();
	return true;
    }
    return false;
}

Gtk::RadioToolButton* GVLE::getButtonRef(ButtonType button)
{
    switch (button)
    {
    case POINTER:
	return &m_arrow;
    case ADDMODEL:
	return &m_addModels;
    case ADDLINK:
	return &m_addLinks;
    case DELETE:
	return &m_delete;
    case ADDCOUPLED:
	return &m_addCoupled;
    case ZOOM:
	return &m_zoom;
    case QUESTION:
	return &m_question;
    default:
	return NULL;
    }
}

/*
void GVLE::loadPlugins(const std::string& rep)
{
if (!Glib::Module::get_supported()) {
    gvle::Error("Your system don't allow dynamic class loading.\n"
                   "Try GNU/Linux");
} else if (!Glib::file_test(rep, Glib::FILE_TEST_IS_DIR)) {
    gvle::Error((fmt("%1% is not a directory") % rep).str());
} else {
    Glib::Dir repertoire(rep);
    Glib::DirIterator it = repertoire.begin();
    while (it != repertoire.end()) {
        std::string path(Glib::build_filename(rep, *it));
        if (Glib::file_test(path, Glib::FILE_TEST_IS_REGULAR)) {
            Plugin* p = loadPlugin(path);
            if (p and m_plugins.find(p->getFormalismName()) ==
                m_plugins.end()) {
                m_plugins[p->getFormalismName()] = p;

                std::list < std::string > lst = p->getCategory();
                std::list < std::string >::iterator it = lst.begin();
                while (it != lst.end()) {
                    m_category.insert(std::pair < std::string, std::string >
                        (*it, p->getFormalismName()));
                    ++it;
                }
            }
        }
        ++it;
    }
}
}*/
/*
void GVLE::loadObserverPlugins(const std::string& rep)
{
if (!Glib::Module::get_supported()) {
   gvle::Error("Your system don't allow dynamic class loading.\n"
                  "Try GNU/Linux");
} else if (!Glib::file_test(rep, Glib::FILE_TEST_IS_DIR)) {
   gvle::Error((fmt("%1% is not a directory") % rep).str());
} else {
   Glib::Dir repertoire(rep);
   Glib::DirIterator it = repertoire.begin();
   while (it != repertoire.end()) {
       std::string path(Glib::build_filename(rep, *it));
       if (Glib::file_test(path, Glib::FILE_TEST_IS_REGULAR)) {
           ObserverPlugin* p = loadObserverPlugin(path);
           if (p and m_observerPlugins.find(p->getObserverName()) ==
               m_observerPlugins.end()) {
               m_observerPlugins[p->getObserverName()] = p;

               std::list < std::string > lst = p->getCategory();
               std::list < std::string >::iterator it = lst.begin();
               while (it != lst.end()) {
                   m_observerCategory.insert(
  std::pair < std::string, std::string >
                       (*it, p->getObserverName()));
                   ++it;
               }
           }
       }
       ++it;
   }
}
}*/
/*
Plugin* GVLE::loadPlugin(const std::string& file)
{
void*   makeNewPlugin = NULL;
Plugin* call = NULL;

Glib::Module mod(file);
if (mod == false) {
  TraceImportant((fmt("Error opening file %1%, %2%") % file %
                  Glib::Module::get_last_error()).str());
} else {
  mod.make_resident();
  if (mod.get_symbol("makeNewPlugin", makeNewPlugin) == false) {
      TraceImportant((fmt(
          "Error in file %1%, function 'makeNewPlugin' not found: %2%") %
              file % Glib::Module::get_last_error()).str());
  } else {
      try {
          call = ((Plugin*(*)())(makeNewPlugin))();
          if (call == NULL) {
              TraceImportant((fmt(
                  "Error in file %1%, function 'makeNewPlugin':"
                  "problem allocation a new Plugin: %2%") %
                  file % Glib::Module::get_last_error()).str());
          } else {
              PluginPlus* pp = dynamic_cast < PluginPlus* >(call);
              if (pp != NULL) {
                  pp->setModeling(m_modeling);
              }
              if (call->init() == false) {
                  TraceImportant((fmt(
                      "Error in file %1%, function 'init' return false")
                         % file).str());
                  delete call;
                  call = NULL;
              }
          }
      } catch(const utils::FileError& e) {
          gvle::Error(e.what());
          return NULL;
      }
  }
}
return call;
}*/
/*
ObserverPlugin* GVLE::loadObserverPlugin(const std::string& file)
{
void*           makeNewPlugin = NULL;
ObserverPlugin* call = NULL;

Glib::Module mod(file);
if (mod == false) {
 TraceImportant((fmt("Error opening file %1%, %2%") %
     file % Glib::Module::get_last_error()).str());
} else {
 mod.make_resident();
 if (mod.get_symbol("makeNewPlugin", makeNewPlugin) == false) {
     TraceImportant((fmt(
         "Error in file %1%, function 'makeNewPlugin' not found: %2%") %
             file % Glib::Module::get_last_error()).str());
 } else {
     try {
         call = ((ObserverPlugin*(*)())(makeNewPlugin))();
         if (call == NULL) {
             TraceImportant((fmt(
                 "Error in file %1%, function 'makeNewPlugin':"
                 "problem allocation a new Plugin: %2%") %
                 file % Glib::Module::get_last_error()).str());
         } else {
             if (call->init() == false) {
                 TraceImportant((fmt(
                     "Error in file %1%, function 'init' return false")
                        % file).str());
                 delete call;
                 call = NULL;
             }
         }
     } catch(const utils::FileError& e) {
         gvle::Error(e.what());
         return NULL;
     }
 }
}
return call;
}*/
/*
Plugin* GVLE::getPlugin(const std::string& name)
{
   MapPlugin::const_iterator   it = m_plugins.find(name);
   Plugin* plugin = 0;

   if (it != m_plugins.end() and (*it).second != 0) {
       plugin = (*it).second;
   }

   return plugin;
}*/
/*
ObserverPlugin* GVLE::getObserverPlugin(const std::string& name)
{
  MapObserverPlugin::const_iterator   it = m_observerPlugins.find(name);
  ObserverPlugin* plugin = 0;

  if (it != m_observerPlugins.end() and (*it).second != 0) {
      plugin = (*it).second;
  }

  return plugin;
}*/
/*
void GVLE::delPlugins()
{
 MapPlugin::iterator itp;
 for (itp = m_plugins.begin(); itp != m_plugins.end(); ++itp)
     delete (*itp).second;

 m_plugins.clear();
}*/

void GVLE::makeButtons()
{
    Glib::RefPtr<Gdk::Pixbuf> r;
    Gtk::Image* img = NULL;

    try {
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("arrow.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_arrow.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("model.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_addModels.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("coupled.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_addCoupled.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("links.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_addLinks.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("delete.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_delete.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("zoom.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_zoom.set_icon_widget(*img);
        r = Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile("question.png"));
        img = Gtk::manage(new Gtk::Image(r));
	m_question.set_icon_widget(*img);
    } catch (const Glib::FileError& e) {
        gvle::Error(e.what());
    }

    //m_arrow.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_arrow, _("Move models, open coupled models, show "
                                  "dynamics of models. (F1)"));
    //m_addModels.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addModels, _("Add empty atomics models. You can "
                                      "specify inputs, outputs, inits and states "
                                      "ports. Select dynamics plugins file and "
                                      "XML write your XML Dynamics. (F2)"));
    //m_addLinks.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addLinks, _("Add connections between models. (F3)"));
    //m_addCoupled.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addCoupled, _("Add coupled models. (F4)"));
    //m_delete.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_delete, _("Delete connections or models. (F5)"));
    //m_zoom.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_zoom, _("Zoom into coupled models. (F6)"));
    //m_question.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_question, _("Show XML Dynamics if atomic model is "
                                     "selected, show XML Structures if coupled "
                                     "model. (F7)"));

    m_arrow.set_active(false);
    m_addModels.set_active(false);
    m_addLinks.set_active(false);
    m_addCoupled.set_active(false);
    m_delete.set_active(false);
    m_zoom.set_active(false);
    m_question.set_active(false);

    m_arrow.add_accelerator("clicked", this->get_accel_group(),
			    Gtk::AccelKey("F1").get_key(),
			    static_cast< Gdk::ModifierType >(0),
	                    Gtk::ACCEL_MASK);
    m_addModels.add_accelerator("clicked", this->get_accel_group(),
				Gtk::AccelKey("F2").get_key(),
				static_cast< Gdk::ModifierType >(0),
				Gtk::ACCEL_MASK);
    m_addLinks.add_accelerator("clicked", this->get_accel_group(),
			       Gtk::AccelKey("F3").get_key(),
			       static_cast< Gdk::ModifierType >(0),
			       Gtk::ACCEL_MASK);
    m_addCoupled.add_accelerator("clicked", this->get_accel_group(),
				 Gtk::AccelKey("F4").get_key(),
				 static_cast< Gdk::ModifierType >(0),
				 Gtk::ACCEL_MASK);
    m_delete.add_accelerator("clicked", this->get_accel_group(),
			     Gtk::AccelKey("F5").get_key(),
			     static_cast< Gdk::ModifierType >(0),
			     Gtk::ACCEL_MASK);
    m_zoom.add_accelerator("clicked", this->get_accel_group(),
			   Gtk::AccelKey("F6").get_key(),
			   static_cast< Gdk::ModifierType >(0),
			   Gtk::ACCEL_MASK);
    m_question.add_accelerator("clicked", this->get_accel_group(),
			       Gtk::AccelKey("F7").get_key(),
			       static_cast< Gdk::ModifierType >(0),
			       Gtk::ACCEL_MASK);

    m_buttons.attach(m_arrow, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_addModels, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_addLinks, 2, 3, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_addCoupled, 3, 4, 0, 1, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_delete, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_zoom, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);
    m_buttons.attach(m_question, 2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 0);

    m_arrow.signal_clicked().connect(sigc::mem_fun(*this, &GVLE::onArrow));
    m_addModels.signal_clicked().connect(sigc::mem_fun(*this,
                                         &GVLE::onAddModels));
    m_addLinks.signal_clicked().connect(sigc::mem_fun(*this,
                                        &GVLE::onAddLinks));
    m_addCoupled.signal_clicked().connect(sigc::mem_fun(*this,
                                          &GVLE::onAddCoupled));
    m_delete.signal_clicked().connect(sigc::mem_fun(*this, &GVLE::onDelete));
    m_zoom.signal_clicked().connect(sigc::mem_fun(*this, &GVLE::onZoom));
    m_question.signal_clicked().connect(sigc::mem_fun(*this,
                                        &GVLE::onQuestion));
}

void GVLE::makeComboCategory()
{
    /*
      MapCategory::iterator it = m_category.begin();
      while (it != m_category.end()) {
          m_comboCategory.append_string((*it).first);
          size_t sz = m_category.count((*it).first);
          for(size_t i = 0; i < sz; ++i) {
              ++it;
          }
      }
      m_comboCategory.append_string("All");
      m_comboCategory.signal_changed().connect(sigc::mem_fun(*this,
                  &GVLE::on_combocategory_changed));
      on_combocategory_changed();
    */
}

void GVLE::on_combocategory_changed()
{
    /*
      std::string selected = m_comboCategory.get_active_string();
      if (selected == "All") {
        m_pluginTable->showAllCategory();
      } else {
        m_pluginTable->showCategory(selected);
      }
    */
}
/*
void GVLE::onPluginButton(const std::string& name)
{
m_pluginSelected.assign(name);
m_status.push(name);
m_currentButton = PLUGINMODEL;
}*/

void GVLE::onArrow()
{
    m_currentButton = POINTER;
    m_status.push(_("Selection"));
}

void GVLE::onAddModels()
{
    m_currentButton = ADDMODEL;
    m_status.push(_("Add models"));
}

void GVLE::onAddLinks()
{
    m_currentButton = ADDLINK;
    m_status.push(_("Add links"));
}

void GVLE::onDelete()
{
    m_currentButton = DELETE;
    m_status.push(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    m_currentButton = ADDCOUPLED;
    m_status.push(_("Coupled Model"));
}

void GVLE::onZoom()
{
    m_currentButton = ZOOM;
    m_status.push(_("Zoom"));
}

void GVLE::onQuestion()
{
    m_currentButton = QUESTION;
    m_status.push(_("Question"));
}

void GVLE::newFile()
{
    try {
	DocumentText* doc = new DocumentText(_("untitled file"), true);
	mDocuments.insert(
	    std::make_pair < std::string, DocumentText* >(_("untitled file"), doc));
	mNotebook->append_page(*doc, doc->filename());
    } catch (std::exception& e) {
	std::cout << e.what() << std::endl;
    }
    show_all_children();
}

void GVLE::onMenuNew()
{
    if (m_modeling->isModified() == false) {
        m_modeling->delNames();
        m_modeling->start();
        m_modeling->redrawModelTreeBox();
    } else if (gvle::Question(_("Do you really want destroy model ?"))) {
        m_modeling->delNames();
        m_modeling->start();
        m_modeling->redrawModelTreeBox();
    }
}

void GVLE::onMenuNewProject()
{
    mNewProjectBox->show();
}


void GVLE::openFile()
{
    Gtk::FileChooserDialog file(_("Choose a file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string selected_file = file.get_filename();
	openTab(selected_file);
    }
}
void GVLE::onMenuOpenPackage()
{
    closeAllTab();
    mOpenPackageBox->show();
    if (utils::Path::path().package() != "")
      m_menu->onPackageMode();
}

void GVLE::onMenuOpenVpz()
{
    if (m_modeling->isModified() == false or
	gvle::Question(_("Do you really want load a new Model ?\nCurrent"
			 "model will be destroy and not save"))) {
	try {
	    mOpenVpzBox->show();
	} catch(utils::InternalError) {
	    Error(_("No experiments in the package ") +
		    utils::Path::path().package());
	}
    }
}

void GVLE::onMenuLoad()
{
    if (m_modeling->isModified() == false or
            gvle::Question(_("Do you really want load a new Model ?\nCurrent"
                             "model will be destroy and not save"))) {
        Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Gtk::FileFilter filter;
        filter.set_name(_("Vle Project gZipped"));
        filter.add_pattern("*.vpz");
        file.add_filter(filter);

        if (file.run() == Gtk::RESPONSE_OK) {
	    closeAllTab();
            m_modeling->parseXML(file.get_filename());
	    utils::Path::path().setPackage("");
	    m_menu->onGlobalMode();
        }
    }
}

void GVLE::saveFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mNotebook->get_nth_page(page));
	if (not doc->isNew()) {
	    doc->save();
	} else {
	    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	    if (file.run() == Gtk::RESPONSE_OK) {
		std::string filename(file.get_filename());
		doc->saveAs(filename);
	    }
	}
	buildPackageHierarchy();
    }
}

void GVLE::onMenuSave()
{
    std::vector<std::string> vec;
    m_modeling->vpz_is_correct(vec);

    if (vec.size() != 0) {
        //vpz is correct
        std::string error = _("Vpz incorrect :\n");
        std::vector<std::string>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            error += *it + "\n";

            ++it;
        }
        Error(error);
        return;
    }

    if (m_modeling->isSaved()) {
	m_modeling->saveXML(m_modeling->getFileName());
    } else if (utils::Path::path().package() != "") {
	mSaveVpzBox->show();
    } else {
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.set_transient_for(*this);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::FileFilter filter;
	filter.set_name(_("Vle Project gZipped"));
	filter.add_pattern("*.vpz");
	file.add_filter(filter);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    vpz::Vpz::fixExtension(filename);
	    m_modeling->saveXML(filename);
	}
    }
}

void GVLE::saveFileAs()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mNotebook->get_nth_page(page));
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    doc->saveAs(filename);
	}
	buildPackageHierarchy();
    }
}

void GVLE::closeFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Gtk::Widget* tab = mNotebook->get_nth_page(page);
	Documents::iterator it = mDocuments.begin();
	while (it != mDocuments.end()) {
	    if (it->second->isDrawingArea())
	    {
		if (dynamic_cast<DocumentDrawingArea*>(
			it->second)->getDrawingArea() == tab) {
		    closeTab(it->first);
		    break;
		}
	    } else {
		if (it->second == tab) {
		    closeTab(it->first);
		    break;
		}
	    }
	    ++it;
	}
    }
}

void GVLE::onMenuQuit()
{
    if (m_modeling->isModified() == true and
	gvle::Question(_("Changes have been made,\n"
                         "Do you want the model to be saved?"))) {
	std::vector<std::string> vec;
	m_modeling->vpz_is_correct(vec);
	if (vec.size() != 0) {
	    //vpz is correct
	    std::string error = _("incorrect VPZ:\n");
	    std::vector<std::string>::const_iterator it = vec.begin();
	    while (it != vec.end()) {
		error += *it + "\n";

		++it;
	    }
	    Error(error);
	    return;
	} else {
	    onMenuSave();
	}
    }
    hide();
}


void GVLE::onShowModelTreeView()
{
    m_modeling->toggleModelTreeBox();
}

void GVLE::onShowModelClassView()
{
    m_modeling->toggleModelClassBox();
}

void GVLE::onShowPackageBrowserWindow()
{
    togglePackageBrowserWindow();
}

void GVLE::onCloseAllViews()
{
    m_modeling->delViews();
}

void GVLE::onIconifyAllViews()
{
    m_modeling->iconifyViews();
}

void GVLE::onDeiconifyAllViews()
{
    m_modeling->deiconifyViews();
}

void GVLE::onPreferences()
{
    mPreferencesBox->show();
}

void GVLE::showPackageBrowserWindow()
{
    /*if(not vle::utils::Path::path().package().empty()) {
	mPackageBrowserWindow->show();
	}*/
}

void GVLE::hidePackageBrowserWindow()
{
    // mPackageBrowserWindow->hide();
}

void GVLE::togglePackageBrowserWindow()
{
    /*if (mPackageBrowserWindow->is_visible()) {
	hidePackageBrowserWindow();
    } else {
	showPackageBrowserWindow();
	}*/
}

void GVLE::onSimulationBox()
{
    if (m_modeling->isSaved()) {
        mSimulationBox->show();
    } else {
        gvle::Error(_("Save or load a project before simulation"));
    }
}

void GVLE::onParameterExecutionBox()
{
    ParameterExecutionBox* box = new ParameterExecutionBox(m_modeling);
    box->run();
    delete box;
}

void GVLE::onExperimentsBox()
{
    ExperimentBox box(mRefXML, m_modeling);
    box.run();
}

void GVLE::onConditionsBox()
{
    mConditionsBox->show();
}

void GVLE::onHostsBox()
{
    HostsBox* box = new HostsBox(mRefXML);
    box->run();
    delete box;
}

void GVLE::onHelpBox()
{
    if (m_helpbox == 0)
        m_helpbox = new HelpBox;

    m_helpbox->show_all();
}

void GVLE::onViewOutputBox()
{
    ViewOutputBox box(*m_modeling, mRefXML, m_modeling->views());
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::setTitle(const Glib::ustring& name)
{
    Glib::ustring title("gvle");
    if (not name.empty()) {
        title += " - ";
        title += Glib::path_get_basename(name);
    }
    set_title(title);
}

void GVLE::setModifiedTitle()
{
    Glib::ustring current("* ");
    current += get_title();

    set_title(current);
}

std::string valuetype_to_string(value::Value::type type)
{
    switch (type) {
    case(value::Value::BOOLEAN):
        return "boolean";
        break;
    case(value::Value::INTEGER):
        return "integer";
        break;
    case(value::Value::DOUBLE):
        return "double";
        break;
    case(value::Value::STRING):
        return "string";
        break;
    case(value::Value::SET):
        return "set";
        break;
    case(value::Value::MAP):
        return "map";
        break;
    case(value::Value::TUPLE):
        return "tuple";
        break;
    case(value::Value::TABLE):
        return "table";
        break;
    case(value::Value::XMLTYPE):
        return "xml";
        break;
    case(value::Value::NIL):
        return "null";
        break;
    case(value::Value::MATRIX):
        return "matrix";
        break;
    default:
        return "(no value)";
        break;
    }
}

void GVLE::focusTab(const std::string& filepath)
{
    int page = mNotebook->page_num(*mDocuments.find(filepath)->second);
    mNotebook->set_current_page(page);
}

void GVLE::openTab(const std::string& filepath)
{
    if(boost::filesystem::extension(filepath) != ".vpz") {
	try {
	    if (mDocuments.find(filepath) == mDocuments.end()) {
		DocumentText* doc = new DocumentText(filepath);
		mDocuments.insert(
		    std::make_pair < std::string, DocumentText* >(filepath, doc));
		int page = mNotebook->append_page(*doc,
			doc->filename() + boost::filesystem::extension(filepath));
		show_all_children();
		mNotebook->set_current_page(page);
	    } else {
		focusTab(filepath);
	    }
	} catch (std::exception& e) {
	    mLog->get_buffer()->insert(
		mLog->get_buffer()->end(), e.what());
	}
	m_menu->onFileMode();
    } else {
	m_modeling->parseXML(filepath);
    }
}

void GVLE::openTabVpz(const std::string& filepath, graph::CoupledModel* model)
{
    if (model != NULL) {
	DocumentDrawingArea* doc = new DocumentDrawingArea(filepath,
							  m_modeling->findView(model),
							  model);
	mCurrentView = doc->getView();
	ViewDrawingArea* area = doc->getDrawingArea();
	std::string tabName = model->getName();
	graph::Model* parent = model->getParent();
	while (parent != NULL) {
	    tabName += "/" + parent->getName();
	    parent = parent->getParent();
	}
	mDocuments.insert(
	  std::make_pair < std::string, DocumentDrawingArea* >(tabName, doc));
	int page = mNotebook->append_page(*area, model->getName());
	show_all_children();
	mNotebook->set_current_page(page);
    } else {
	DocumentDrawingArea* doc = new DocumentDrawingArea(filepath,
							  m_modeling->findView(model),
							  m_modeling->getTopModel());
	mCurrentView = doc->getView();
	ViewDrawingArea* area = doc->getDrawingArea();
	std::string tabName = m_modeling->getTopModel()->getName();
	mDocuments.insert(
	  std::make_pair < std::string, DocumentDrawingArea* >(tabName, doc));
	int page = mNotebook->append_page(*area,
					  m_modeling->getTopModel()->getName());
	show_all_children();
	mNotebook->set_current_page(page);
    }
    m_menu->onViewMode();
}


void GVLE::closeTab(const std::string& filepath)
{
    Documents::iterator it = mDocuments.find(filepath);
    if (it != mDocuments.end()) {
	int page = mNotebook->page_num(*it->second);
	mNotebook->remove_page(page);
	mDocuments.erase(filepath);

	mNotebook->set_current_page(--page);
	redrawModelTreeBox();
	redrawModelClassBox();
    }
}

void GVLE::closeAllTab()
{
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	int page = mNotebook->page_num(*it->second);
	mNotebook->remove_page(page);
	mDocuments.erase(it->first);

	mNotebook->set_current_page(--page);
	++it;
    }
}

void GVLE::changeTab(GtkNotebookPage* /*page*/, int num)
{
    Gtk::Widget* tab = mNotebook->get_nth_page(num);
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	if (it->second->isDrawingArea()) {
	    DocumentDrawingArea* area =
		    dynamic_cast< DocumentDrawingArea *>(it->second);
	    if (tab == dynamic_cast< DocumentDrawingArea *>(it->second)->getDrawingArea())
	    {
		std::cout << "Couc\n";
		fflush(stdout);
		mCurrentView = area->getView();
		m_menu->onViewMode();
		break;
	    }
	}
	else{
	    if (it->second == tab) {
		mCurrentView = 0;
		m_menu->onFileMode();
		break;
	    }
	}
	++it;
    }
}

void GVLE::configureProject()
{
    std::string out, err;
    utils::CMakePackage::configure(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::buildProject()
{
    std::string out, err;
    utils::CMakePackage::build(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::cleanProject()
{
    std::string out, err;
    utils::CMakePackage::clean(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::packageProject()
{
    std::string out, err;
    utils::CMakePackage::package(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::onCutModel()
{
    mCurrentView->onCutModel();
}

void GVLE::onCopyModel()
{
    mCurrentView->onCopyModel();
}

void GVLE::onPasteModel()
{
    mCurrentView->onPasteModel();
}

void GVLE::clearCurrentModel()
{
    mCurrentView->clearCurrentModel();
}

void GVLE::importModel()
{
    mCurrentView->importModel();
}

void GVLE::exportCurrentModel()
{
    mCurrentView->exportCurrentModel();
}

void GVLE::exportGraphic()
{
    mCurrentView->exportGraphic();
}

void GVLE::addCoefZoom()
{
    mCurrentView->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    mCurrentView->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    mCurrentView->setCoefZoom(coef);
}

}} // namespace vle gvle
