/*
 * @file vle/gvle/outputs/file/File.cpp
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


#include <vle/gvle/outputs/file/File.hpp>
#include <vle/utils/Path.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <libglademm/xml.h>
#include <cassert>

namespace vle { namespace gvle { namespace outputs {

File::File(const std::string& name)
    : OutputPlugin(name), mDialog(0), mRadioFile(0), mRadioStdOut(0),
      mRadioErrOut(0), mCheckJulianDay(0), mComboType(0), mComboFile(0),
      mHBoxFile(0), mHBoxLocale(0)
{
    std::string glade = utils::Path::path().getOutputGladeFile("file.glade");
    Glib::RefPtr < Gnome::Glade::Xml > ref = Gnome::Glade::Xml::create(glade);

    ref->get_widget("dialogFileOutputPlugin", mDialog);
    ref->get_widget("radiobuttonFileFileOutputPlugin", mRadioFile);
    ref->get_widget("radiobuttonStdOutFileOutputPlugin", mRadioStdOut);
    ref->get_widget("radiobuttonErrOutFileOutputPlugin", mRadioErrOut);
    ref->get_widget("checkbuttonJulianDay", mCheckJulianDay);

    Gtk::RadioButton::Group group = mRadioFile->get_group();
    mRadioStdOut->set_group(group);
    mRadioErrOut->set_group(group);

    ref->get_widget("hboxTypeFileOutputPlugin", mHBoxFile);
    ref->get_widget("hboxLocaleFileOutputPlugin", mHBoxLocale);

    mComboType = Gtk::manage(new Gtk::ComboBoxText());
    mComboFile = Gtk::manage(new Gtk::ComboBoxEntryText());
    mComboType->show();
    mComboFile->show();

    mHBoxFile->pack_start(*mComboType, true, true);
    mHBoxLocale->pack_start(*mComboFile, true, true);
}

File::~File()
{
}

bool File::start(vpz::Output& output, vpz::View& /* view */)
{
    assert(output.plugin() == "file");

    init(output);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        assign(output);
    }

    mDialog->hide();

    return true;
}

void File::init(vpz::Output& output)
{
    mComboType->clear_items();
    mComboType->append_text("text");
    mComboType->append_text("csv");
    mComboType->append_text("rdata");
    mComboType->set_active_text("text");

    mComboFile->clear_items();
    mComboFile->append_text("C");
    mComboFile->append_text("user");
    mComboFile->set_active_text("C");

    mRadioFile->set_active();

    const value::Value* init = output.data();
    if (init and init->isMap()) {
        const value::Map* map = value::toMapValue(init);
        if (map->exist("type")) {
            std::string type = map->getString("type");

            if (type != "csv" and type != "rdata" and type != "text") {
                throw utils::InternalError(fmt(
                        _("Output file plugin: unknow type `%1%'")) % type);
            }

            mComboType->set_active_text(type);
        }

        if (map->exist("locale")) {
            std::string loc = map->getString("locale");
            if (not loc.empty() and loc != "user" and loc != "C") {
                mComboFile->append_text(loc);
            }

            mComboFile->set_active_text(loc);
        }

        if (map->exist("output")) {
            std::string type = map->getString("output");

            if (type == "out") {
                mRadioStdOut->set_active();
            } else if (type == "error") {
                mRadioErrOut->set_active();
            } else {
                throw utils::ArgError(fmt(
                        _("Output file plugin: unknow output type `%1%'")) %
                        type);
            }
        } else {
            mRadioFile->set_active();
        }

	if (map->exist("julian-day")) {
            bool type = map->getBoolean("julian-day");

            if (type) {
                mCheckJulianDay->set_active(true);
            } else {
                mCheckJulianDay->set_active(false);
            }
	}
    }
}

void File::assign(vpz::Output& output)
{
    value::Map* map = new value::Map();
    map->addString("type", mComboType->get_active_text());
    map->addString("locale", mComboFile->get_active_text());

    if (not mRadioFile->get_active()) {
        map->addString("output", mRadioStdOut->get_active() ? "out" : "error");
    }

    if (mCheckJulianDay->get_active()) {
        map->addBoolean("julian-day", true);
    }
    else {
        map->addBoolean("julian-day", false);
    }

    output.setData(map);
}

}}} // namespace vle gvle outputs

DECLARE_GVLE_OUTPUTPLUGIN(vle::gvle::outputs::File)
