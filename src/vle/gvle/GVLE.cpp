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
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/HostsBox.hpp>
#include <vle/gvle/PluginTable.hpp>
#include <vle/gvle/GVLEMenu.hpp>
#include <vle/gvle/PluginPlus.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <gtkmm/filechooserwidget.h>

using namespace vle;

namespace vle
{
namespace gvle {

GVLE::GVLE(const std::string& filename) :
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
        m_helpbox(0)
{
    mRefXML = Gnome::Glade::Xml::create(
                  utils::Path::path().getGladeFile("gvle.glade"));

    m_modeling->setGlade(mRefXML);

    mExpBox = new ExperimentBox(mRefXML, m_modeling);
    mConditionsBox = new ConditionsBox(mRefXML, m_modeling);
    mProjectBox = new ProjectBox(mRefXML, &m_modeling->vpz().project());
    mSimulationBox = new LaunchSimulationBox(mRefXML, m_modeling);

    //loadObserverPlugins(utils::Path::path().getDefaultObserverPluginDir());
    //loadObserverPlugins(utils::Path::path().getUserObserverPluginDir());

    //loadPlugins(utils::Path::path().getDefaultGUIPluginDir());
    //loadPlugins(utils::Path::path().getUserGUIPluginDir());

    makeButtons();

    m_menu = new GVLEMenu(this);

    //m_pluginTable = new PluginTable(this, m_buttonGroup, m_category,
    //       m_plugins);

    //makeComboCategory();

    m_arrow.set_active(true);
    m_status.push("Selection");
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
    }
}

GVLE::~GVLE()
{
    //delPlugins();

    //delete m_pluginTable;
    delete m_modeling;

    delete mExpBox;
    delete mConditionsBox;
    delete mProjectBox;
    delete mSimulationBox;
}
/*
void GVLE::loadPlugins(const std::string& rep)
{
if (!Glib::Module::get_supported()) {
    gvle::Error("Your system don't allow dynamic class loading.\n"
                   "Try GNU/Linux");
} else if (!Glib::file_test(rep, Glib::FILE_TEST_IS_DIR)) {
    gvle::Error((boost::format("%1% is not a directory") % rep).str());
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
   gvle::Error((boost::format("%1% is not a directory") % rep).str());
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
  TraceImportant((boost::format("Error opening file %1%, %2%") % file %
                  Glib::Module::get_last_error()).str());
} else {
  mod.make_resident();
  if (mod.get_symbol("makeNewPlugin", makeNewPlugin) == false) {
      TraceImportant((boost::format(
          "Error in file %1%, function 'makeNewPlugin' not found: %2%") %
              file % Glib::Module::get_last_error()).str());
  } else {
      try {
          call = ((Plugin*(*)())(makeNewPlugin))();
          if (call == NULL) {
              TraceImportant((boost::format(
                  "Error in file %1%, function 'makeNewPlugin':"
                  "problem allocation a new Plugin: %2%") %
                  file % Glib::Module::get_last_error()).str());
          } else {
              PluginPlus* pp = dynamic_cast < PluginPlus* >(call);
              if (pp != NULL) {
                  pp->setModeling(m_modeling);
              }
              if (call->init() == false) {
                  TraceImportant((boost::format(
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
 TraceImportant((boost::format("Error opening file %1%, %2%") %
     file % Glib::Module::get_last_error()).str());
} else {
 mod.make_resident();
 if (mod.get_symbol("makeNewPlugin", makeNewPlugin) == false) {
     TraceImportant((boost::format(
         "Error in file %1%, function 'makeNewPlugin' not found: %2%") %
             file % Glib::Module::get_last_error()).str());
 } else {
     try {
         call = ((ObserverPlugin*(*)())(makeNewPlugin))();
         if (call == NULL) {
             TraceImportant((boost::format(
                 "Error in file %1%, function 'makeNewPlugin':"
                 "problem allocation a new Plugin: %2%") %
                 file % Glib::Module::get_last_error()).str());
         } else {
             if (call->init() == false) {
                 TraceImportant((boost::format(
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
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("arrow.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_arrow.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("model.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_addModels.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("coupled.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_addCoupled.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("links.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_addLinks.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("delete.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_delete.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("zoom.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_zoom.add(*img);
        r = Gdk::Pixbuf::create_from_file(utils::Path::path().getPixmapsFile("question.png"));
        img = Gtk::manage(new Gtk::Image(r));
        m_question.add(*img);
    } catch (const Glib::FileError& e) {
        gvle::Error(e.what());
    }

    m_arrow.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_arrow, "Move models, open coupled models, show"
                       " dynamics of models");
    m_addModels.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addModels, "Add empty atomics models. You can"
                       " specify inputs, outputs, inits and states ports."
                       " Select dynamics plugins file and XML write your"
                       " XML Dynamics.");
    m_addLinks.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addLinks, "Add connections between models.");
    m_addCoupled.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_addCoupled, "Add coupled models.");
    m_delete.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_delete, "Delete connections or models");
    m_zoom.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_zoom, "Zoom into coupled models.");
    m_question.set_relief(Gtk::RELIEF_NONE);
    m_tooltips.set_tip(m_question, "Show XML Dynamics if atomic model is"
                       " selected, show XML Structures if coupled model.");
    m_arrow.set_mode(false);
    m_addModels.set_mode(false);
    m_addLinks.set_mode(false);
    m_addCoupled.set_mode(false);
    m_delete.set_mode(false);
    m_zoom.set_mode(false);
    m_question.set_mode(false);

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
    m_status.push("Selection");
}

void GVLE::onAddModels()
{
    m_currentButton = ADDMODEL;
    m_status.push("Add models");
}

void GVLE::onAddLinks()
{
    m_currentButton = ADDLINK;
    m_status.push("Add links");
}

void GVLE::onDelete()
{
    m_currentButton = DELETE;
    m_status.push("Delete object");
}

void GVLE::onAddCoupled()
{
    m_currentButton = ADDCOUPLED;
    m_status.push("Coupled Model");
}

void GVLE::onZoom()
{
    m_currentButton = ZOOM;
    m_status.push("Zoom");
}

void GVLE::onQuestion()
{
    m_currentButton = QUESTION;
    m_status.push("Question");
}

void GVLE::onMenuNew()
{
    if (m_modeling->isModified() == false) {
        m_modeling->delNames();
        m_modeling->start();
        m_modeling->redrawModelTreeBox();
    } else if (gvle::Question("Do you really want destroy model ?")) {
        m_modeling->delNames();
        m_modeling->start();
        m_modeling->redrawModelTreeBox();
    }
}

void GVLE::onMenuLoad()
{
    if (m_modeling->isModified() == false or
            gvle::Question("Do you really want load a new Model ?\n"
                           "Current model will be destroy and not save")) {
        Gtk::FileChooserDialog file(*this, "VPZ file",
                                    Gtk::FILE_CHOOSER_ACTION_OPEN);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Gtk::FileFilter filter;
        filter.set_name("Vle Project gZipped");
        filter.add_pattern("*.vpz");
        file.add_filter(filter);

        if (file.run() == Gtk::RESPONSE_OK) {
            m_modeling->parseXML(file.get_filename());
        }
    }
}

void GVLE::onMenuSave()
{
    std::vector<std::string> vec;
    m_modeling->vpz_is_correct(vec);

    if (vec.size() == 0) {
        //if (m_modeling->isModified()) {
        if (m_modeling->isSaved()) {
            m_modeling->saveXML(m_modeling->getFileName());
        }
        //}
        else {
            onMenuSaveAs();
        }
    } else {
        //vpz is incorrect
        std::string error = "Vpz incorrect :\n";
        std::vector<std::string>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            error += *it + "\n";

            ++it;
        }
        Error(error);
    }
}

void GVLE::onMenuSaveAs()
{
    std::vector<std::string> vec;
    m_modeling->vpz_is_correct(vec);

    if (vec.size() != 0) {
        //vpz is correct
        std::string error = "Vpz incorrect :\n";
        std::vector<std::string>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            error += *it + "\n";

            ++it;
        }
        Error(error);
        return;
    }

    Gtk::FileChooserWidget filechooser(Gtk::FILE_CHOOSER_ACTION_SAVE);
    Gtk::FileFilter filter;
    filter.set_name("Vle Project gZipped");
    filter.add_pattern("*.vpz");
    filechooser.add_filter(filter);

    Gtk::CheckButton button("compress vpz with gzip");
    Gtk::Dialog dialog("Save as...");
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    Gtk::VBox* vbox = dialog.get_vbox();
    vbox->pack_start(filechooser, true, true);
    vbox->pack_start(button, false, false);

    dialog.show_all();

    if (dialog.run() == Gtk::RESPONSE_OK) {
        std::string filename(filechooser.get_filename());
        vpz::Vpz::fixExtension(filename);
        m_modeling->saveXML(filename);
    }
}

void GVLE::onShowModelTreeView()
{
    m_modeling->toggleModelTreeBox();
}

void GVLE::onShowClassModelTreeView()
{
    m_modeling->toggleClassModelTreeBox();
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

void GVLE::onSimulationBox()
{
    if (m_modeling->isSaved()) {
        mSimulationBox->show();
    } else {
        gvle::Error("Save or load a project before simulation");
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
    mExpBox->show();
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

void GVLE::onProjectBox()
{
    mProjectBox->show();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

std::string valuetype_to_string(value::ValueBase::type type)
{
    switch (type) {
    case(value::ValueBase::BOOLEAN):
                    return "boolean";
        break;
    case(value::ValueBase::INTEGER):
                    return "integer";
        break;
    case(value::ValueBase::DOUBLE):
                    return "double";
        break;
    case(value::ValueBase::STRING):
                    return "string";
        break;
    case(value::ValueBase::SET):
                    return "set";
        break;
    case(value::ValueBase::MAP):
                    return "map";
        break;
    case(value::ValueBase::TUPLE):
                    return "tuple";
        break;
    case(value::ValueBase::TABLE):
                    return "table";
        break;
    case(value::ValueBase::XMLTYPE):
                    return "xml";
        break;
    case(value::ValueBase::NIL):
                    return "null";
        break;
    case(value::ValueBase::MATRIX):
                    return "matrix";
        break;
    default:
        return "(no value)";
        break;
    }
}
}
} // namespace vle gvle
