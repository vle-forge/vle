/**
 * @file vle/gvle/conditions/file/File.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/gvle/conditions/file/File.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gdkmm/cursor.h>
#include <gtkmm/stock.h>
#include <libglademm/xml.h>
#include <cassert>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::assign;
using namespace vle;

namespace vle { namespace gvle { namespace conditions {

FileLoc::FileLoc(const std::string& name)
    : ConditionPlugin(name), m_dialog(0), m_location(0), m_filechooserbutton(0)
{
}

FileLoc::~FileLoc()
{
    m_cntFileChooserButtonClicked.disconnect();
}

void FileLoc::build()
{
    std::string glade = utils::Path::path().getConditionGladeFile("file.glade");
    Glib::RefPtr<Gnome::Glade::Xml> ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialog", m_dialog);
    ref->get_widget("entryLocation", m_location);
    ref->get_widget("buttonLocation", m_filechooserbutton);

    assert(m_dialog);
    assert(m_location);
    assert(m_filechooserbutton);

    m_cntFileChooserButtonClicked = m_filechooserbutton->signal_clicked().connect(
        sigc::mem_fun( *this, &FileLoc::onClickedFileChooserButton));
}

bool FileLoc::start(vpz::Condition& condition)
{
    build();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(condition);
    }

    m_dialog->hide();

    return true;
}

bool FileLoc::start(vpz::Condition& condition, const std::string& port)
{
    build();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(condition, port);
    }

    m_dialog->hide();

    return true;
}

void FileLoc::assign(vpz::Condition& condition)
{
    condition.addValueToPort("filename",
			     value::String::create(m_location->get_text()));
}

void FileLoc::assign(vpz::Condition& condition, const std::string& port)
{
    condition.addValueToPort(port,
			     value::String::create(m_location->get_text()));
}

void FileLoc::onClickedFileChooserButton()
{
    Gtk::FileChooserDialog file("File", Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*m_dialog);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    // set filters
    map<string,string> filters;
    insert( filters )
        ( "*.*", _("All files") )
        ( "*.ini", _("Config files") )
        ( "*.data", _("Data files") )
        ( "*.txt", _("Text files") )
        ( "*.csv", _("csv files") );

    for ( map<string,string> :: const_iterator it = filters.begin();
	  it != filters.end();
	  ++it) {

	Gtk::FileFilter filter;
	filter.set_name(it->second);
	filter.add_pattern(it->first);
	file.add_filter(filter);
    }

    // set the current folder depending on the mode of gvle
    string packageName = utils::Package::package().name();

    if (not packageName.empty()){
	string packageDataDirName = utils::Path::path().getPackageDataDir();
	file.set_current_folder (Glib::ustring(packageDataDirName));
    }

    if (file.run() == Gtk::RESPONSE_OK) {
        m_location->set_text(file.get_filename());
    }

    file.hide();
}

}}} // namespace vle gvle conditions

DECLARE_GVLE_CONDITIONPLUGIN(vle::gvle::conditions::FileLoc)
