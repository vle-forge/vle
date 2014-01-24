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


#include <gtkmm/aboutdialog.h>
#include <vle/gvle/About.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/version.hpp>

namespace vle { namespace gvle {

class About::Pimpl
{
public:
    Pimpl(const Glib::RefPtr < Gtk::Builder >& refXml);

    ~Pimpl();

    void onAboutClose(int response);

    Gtk::AboutDialog* dialog() const;

private:
    Gtk::AboutDialog* mAbout;
    sigc::connection mConnectionResponse;
};

About::Pimpl::Pimpl(const Glib::RefPtr < Gtk::Builder >& refXml)
{
    refXml->get_widget("DialogAbout", mAbout);

    std::string extra(VLE_EXTRA_VERSION);
    if (extra.empty()) {
        mAbout->set_version(VLE_VERSION);
    } else {
        mAbout->set_version((fmt("%1%-%2%") % VLE_VERSION % extra).str());
    }

    mAbout->set_logo(Gdk::Pixbuf::create_from_file(
            utils::Path::path().getPixmapFile(
#ifdef BOOST_WINDOWS
                "icon128x128.png"
#else
                "yellowvle.svg"
#endif
                )));

    mConnectionResponse = mAbout->signal_response().connect(
        sigc::mem_fun(*this, &About::Pimpl::onAboutClose));
}

About::Pimpl::~Pimpl()
{
    mConnectionResponse.disconnect();
}

void About::Pimpl::onAboutClose(int /*response*/)
{
    mAbout->hide();
}

Gtk::AboutDialog* About::Pimpl::dialog() const
{
    return mAbout;
}

About::About(const Glib::RefPtr < Gtk::Builder >& refXml)
    : mImpl(new Pimpl(refXml))
{
}

About::~About()
{
    delete mImpl;
}

void About::run()
{
    mImpl->dialog()->run();
}

}} // namespace vle gvle
