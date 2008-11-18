/**
 * @file vle/eov/Plugin.hpp
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


#ifndef VLE_EOV_PLUGIN_HPP
#define VLE_EOV_PLUGIN_HPP

#include <sigc++/trackable.h>
#include <gdkmm/window.h>
#include <gtkmm/widget.h>
#include <glibmm/thread.h>
#include <vle/oov/CairoPlugin.hpp>

namespace vle { namespace eov {

    class NetStreamReader;

    /**
     * @brief The vle::eov::Plugin is a class to develop graphical user
     * interface around a vle::oov::CairoPlugin. Use the DECLARE_EOV_PLUGIN
     * macro to develop a plugin.
     */
    class Plugin : public sigc::trackable
    {
    public:
        /**
         * @brief Default constructor of the eov plugin.
         * @param cairoplugin A reference to the loaded oov::Plugin.
         */
        Plugin(oov::CairoPluginPtr cairoplugin, NetStreamReader* net) :
            m_cairoplugin(cairoplugin),
            m_net(net) {}

        virtual ~Plugin() {}

        /**
         * @brief Get the drawing surface from a component of the Gtk widget
         * where the CairoPlugin must draw.
         * @return A reference to the drawing surface.
         */
        virtual Glib::RefPtr < Gdk::Window > drawingSurface() = 0;

	/**
	 * @brief Get the drawing widget.
	 * @return A reference to the drawingWidget.
	 */
	virtual Gtk::Widget& drawingWidget() = 0;

        /**
         * @brief Get the widget of the plugin.
         * @return A reference to the main widget, a container for instance.
         */
        virtual Gtk::Widget& widget() = 0;

        /**
         * @brief Return the width of the drawing surface.
         * @return the width.
         */
        virtual int width() = 0;

        /**
         * @brief Return the height of the drawing surface.
         * @return the height.
         */
        virtual int height() = 0;

        /**
         * @brief Copy the Surface from the oov::CairoPlugin to the drawing
         * surface of the eov::Plugin. Use as slot function for expose event
         * signal.
         * @brief event A reference to the expose event structure.
         */
        void onExposeEvent(GdkEventExpose* event);

        /**
         * @brief Send to the CairoPlugin a message to redraw is storage image
         * buffer.
         */
        void copy()
        { m_cairoplugin->needCopy(); }

    protected:
        oov::CairoPluginPtr                 m_cairoplugin;
        NetStreamReader*                    m_net;
        Glib::RefPtr < Gdk::Pixmap >        m_buffer;
        Glib::RefPtr < Gdk::GC >            m_gc;
        Cairo::RefPtr < Cairo::Context >    m_ctx;
    };

    /**
     * @brief This typedef is used by the Eov plugin factory to automatically
     * destroy plugin at the end of the simulation.
     */
    typedef boost::shared_ptr < Plugin > PluginPtr ;


#define DECLARE_EOV_PLUGIN(x) \
    extern "C" { vle::eov::Plugin* makeNewEovPlugin( \
        vle::oov::CairoPluginPtr cairoplg, vle::eov::NetStreamReader* netsr) { \
            return new x(cairoplg, netsr); } }

}} // namespace vle eov

#endif
