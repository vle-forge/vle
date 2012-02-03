/*
 * @file vle/eov/MainWindow.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#ifndef VLE_EOV_MAINWINDOW_HPP
#define VLE_EOV_MAINWINDOW_HPP

#include <vle/DllDefines.hpp>
#include <vle/eov/Plugin.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/utils/SharedLibraryManager.hpp>
#include <gtkmm/window.h>
#include <gtkmm/dialog.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/textview.h>
#include <glibmm/thread.h>
#include <glibmm/timer.h>
#include <gtkmm/builder.h>
#include <map>

namespace vle { namespace eov {

    class NetStreamReader;

    /**
     * @brief A class to manage the Main Window of the Eov application. It use
     * libglademm to load graphical user interface.
     */
    class VLE_API MainWindow : public sigc::trackable
    {
    public:
        /**
         * @brief Build the MainWindow with a libglademm.
         * @param refXml A reference to the glade file.
         * @param port The default port.
         */
        MainWindow(Glib::RefPtr < Gtk::Builder >& refXml,
                   int port);

        /**
         * @brief Nothing to delete.
         */
        virtual ~MainWindow()
        {}

        /**
         * @brief Get a reference to the main Gtk::Window of Eov application.
         * @return A reference to the main Gtk::Window.
         * @throw utils::InternalError if the main Window have problem.
         */
        Gtk::Window& window();

        /**
         * @brief Add a new eov::Window.
         * @param plugin The plugin to attach to the eov::Window.
         */
        void addWindow();

        /**
         * @brief Get a reference to the current mutex.
         * @return A reference to the current mutex.
         */
        Glib::Mutex& mutex()
        { return mMutex; }

        /**
         * @brief Get the default port number.
         * @return The default port number.
         */
        int port() const
        { return mPort; }

    private:
        Glib::Mutex mMutex;
        int mPort;
        int mRefresh;
	Gtk::MenuItem *mMenuNew;
	Gtk::MenuItem *mMenuQuit;
	Gtk::MenuItem *mMenuAbout;
        Gtk::Window* mWindow;
        Gtk::Dialog* mPref;
        Gtk::AboutDialog* mAbout;
        Gtk::SpinButton* mSpinPort;
        Gtk::SpinButton* mSpinRefresh;
        Gtk::TextView* mTextview;
        Glib::RefPtr < Gtk::Builder > mRefXml;
	sigc::connection mConnection;
        utils::SharedLibraryManager slm;
        utils::ModuleManager mModuleMgr;

	std::map < Gtk::Window*, NetStreamReader* > mBufferedStream;

	NetStreamReader* mNet;
	Glib::Thread* mThread;

        /**
         * @brief On new menuitem, we start preference dialog box and try to
         * start a new NetStreamReader.
         */
        void onNew();

        /**
         * @brief On about menuitem, we show the about dialog box.
         */
        void onAbout();

        /**
         * @brief On menu quit, we close all.
         */
        void onQuit();

        /**
         * @brief When close the About dialog box.
         * @param response The response button.
         */
        void onAboutClose(int response);

        /**
         * @brief When close the preference dialog box.
         * @param response  The response button.
         */
        void onPrefButton(int response);

	bool onDeleteEventWindow(GdkEventAny* event);

	/**
	 * @brief Hide new, quit and preference menuitem.
	 */
	void hideMenu();

	/**
	 * @brief Show new, quit and preference menuitem.
	 */
	void showMenu();

	/**
	 * @brief A function, called every 250ms to check if the
	 * NetStreamReader is finish. If it is finish, the menu new, quit are
	 * re-open.
	 */
	bool isStreamReaderFinish();

	struct haveSameGdkWindow
	{
	    GdkWindow* _wnd;
	    haveSameGdkWindow(GdkWindow* wnd) : _wnd(wnd) {}
	    bool operator()(std::pair < Gtk::Window*, NetStreamReader* > x)
	    {
                return x.first->get_window()->gobj() == _wnd;
	    }
	};
    };

}} // namespace vle eov

#endif
