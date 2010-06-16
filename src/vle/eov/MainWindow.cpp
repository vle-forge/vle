/**
 * @file vle/eov/MainWindow.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/eov/MainWindow.hpp>
#include <vle/eov/Window.hpp>
#include <vle/eov/NetStreamReader.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <gtkmm/menuitem.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/messagedialog.h>
#include <boost/format.hpp>
#include <vle/version.hpp>

namespace vle { namespace eov {

MainWindow::MainWindow(Glib::RefPtr < Gnome::Glade::Xml > refXml, int port)
    : mPort(port), mRefresh(100), mMenuNew(0), mMenuQuit(0), mMenuAbout(0),
    mWindow(0), mPref(0), mAbout(0), mSpinPort(0), mSpinRefresh(0),
    mTextview(0), mRefXml(refXml), mNet(0), mThread(0)
{
    mRefXml->get_widget("imagemenuitem_new", mMenuNew);
    mRefXml->get_widget("imagemenuitem_quit", mMenuQuit);
    mRefXml->get_widget("imagemenuitem_about", mMenuAbout);
    mRefXml->get_widget("window_eov", mWindow);
    mRefXml->get_widget("spinbutton_port", mSpinPort);
    mRefXml->get_widget("spinbutton_refresh", mSpinRefresh);
    mRefXml->get_widget("textview", mTextview);
    mRefXml->get_widget("dialog_pref_eov", mPref);

    mPref->signal_response().connect(
        sigc::mem_fun(*this, &MainWindow::onPrefButton));

    mMenuNew->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::onNew));
    mMenuQuit->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::onQuit));
    mMenuAbout->signal_activate().connect(
        sigc::mem_fun(*this, &MainWindow::onAbout));
}

Gtk::Window& MainWindow::window()
{
    if (mWindow == 0) {
        throw utils::InternalError(_("EOV main window is not initialized"));
    }
    return *mWindow;
}

void MainWindow::addWindow()
{
    Window* wnd = new Window(mMutex, mRefresh);
    mNet = new NetStreamReader(mPort, *wnd);

    wnd->signal_delete_event().connect(sigc::mem_fun(*this,
		&MainWindow::onDeleteEventWindow));

    mBufferedStream[wnd] = mNet;
    wnd->show_all();
}

bool MainWindow::onDeleteEventWindow(GdkEventAny* event)
{
    std::map < Gtk::Window*, NetStreamReader* >::iterator it;
    it = std::find_if(mBufferedStream.begin(),
	    mBufferedStream.end(),
	    haveSameGdkWindow(event->window));

    if (it != mBufferedStream.end()) {
	if (it->second->isFinish()) {
	    it->first->hide();
	    delete it->first;
	    delete it->second;
	    mBufferedStream.erase(it);
	}
    }

    return true;
}

void MainWindow::onNew()
{
    mSpinPort->set_value(mPort);
    mSpinRefresh->set_value(mRefresh);
    if (mPref->run() ==  Gtk::RESPONSE_APPLY) {
        try {
            /*
	     * 10 ms is the minimum between refresh the Gtk::DrawinArea widget.
             */
	    mRefresh = (int)std::max(
			 std::floor(std::abs(mSpinRefresh->get_value())),
                         10.);
            addWindow();

	    mThread = Glib::Thread::create(
			    sigc::mem_fun(*mNet,
				    &eov::NetStreamReader::process), true);
	    mConnection = Glib::signal_timeout().connect(
			    sigc::mem_fun(*this,
				    &MainWindow::isStreamReaderFinish), 500);
	    hideMenu();
        } catch(const std::exception& e) {
            Gtk::MessageDialog a(_("Cannot open an Eov stream"), false,
                                 Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            a.run();
            return;
        }

        Glib::RefPtr < Gtk::TextBuffer > buffer = mTextview->get_buffer();
        buffer->insert(buffer->begin(), (fmt(_(
                    "%1%: Eov Stream listen on port %2% at %3%ms\n")) %
                   utils::DateTime::simpleCurrentDate() % mPort %
                   mRefresh).str());
        mTextview->set_editable(false);
        mTextview->show_all();
    }
}

void MainWindow::onAbout()
{
    if (not mAbout) {
        mRefXml->get_widget("aboutdialog_eov", mAbout);
        if (not mAbout) {
            throw utils::InternalError(_("EOV Glade file have problem"));
        }
        mAbout->signal_response().connect(
            sigc::mem_fun(*this, &MainWindow::onAboutClose));

        std::string extra(VLE_EXTRA_VERSION);
        if (extra.empty()) {
            mAbout->set_version(VLE_VERSION);
        } else {
            std::string version(VLE_VERSION);
            version += extra;
            mAbout->set_version(version);
        }
    }
    mAbout->run();
}

void MainWindow::onQuit()
{
    if (mThread) {
	mThread->join();
    }

    delete mNet;
    mWindow->hide();
}

void MainWindow::onAboutClose(int /* response */)
{
    mAbout->hide();
}

void MainWindow::onPrefButton(int response)
{
    if (response == Gtk::RESPONSE_APPLY) {
        mPort = (int)std::floor(std::abs(mSpinPort->get_value()));
        mRefresh = (int)std::floor(std::abs(mSpinRefresh->get_value()));
    }
    mPref->hide();
}

void MainWindow::hideMenu()
{
    mMenuNew->set_sensitive(false);
    mMenuQuit->set_sensitive(false);
}

void MainWindow::showMenu()
{
    mMenuNew->set_sensitive(true);
    mMenuQuit->set_sensitive(true);
}

bool MainWindow::isStreamReaderFinish()
{
    if (mNet and mNet->isFinish()) {
	mThread = 0;

	Glib::RefPtr < Gtk::TextBuffer > buffer = mTextview->get_buffer();
	if (not mNet->finishError().empty()) {
            buffer->insert(buffer->begin(), (fmt(_("/!\\ Error: %1%\n")) %
                                             mNet->finishError()).str());
	}

        buffer->insert(buffer->begin(),
                       (fmt(_( "%1%: Eov Stream close\n")) %
                        utils::DateTime::simpleCurrentDate()).str());
        showMenu();
	return false;
    }
    return true;
}

}} // namespace vle eov
