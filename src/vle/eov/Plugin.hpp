/*
 * @file vle/eov/Plugin.hpp
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


#ifndef VLE_EOV_PLUGIN_HPP
#define VLE_EOV_PLUGIN_HPP

#include <vle/DllDefines.hpp>
#include <vle/oov/CairoPlugin.hpp>
#include <vle/version.hpp>
#include <sigc++/trackable.h>
#include <gdkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>
#include <glibmm/thread.h>

#define DECLARE_EOV_PLUGIN(x)                                   \
    extern "C" {                                                \
        VLE_API vle::eov::Plugin*                            \
        vle_make_new_eov(vle::oov::CairoPluginPtr cairoplg,     \
                         vle::eov::NetStreamReader* netsr)      \
        {                                                       \
            return new x(cairoplg, netsr);                      \
        }                                                       \
                                                                \
        VLE_API void                                         \
        vle_api_level(vle::uint32_t* major,                     \
                      vle::uint32_t* minor,                     \
                      vle::uint32_t* patch)                     \
        {                                                       \
            *major = VLE_MAJOR_VERSION;                         \
            *minor = VLE_MINOR_VERSION;                         \
            *patch = VLE_PATCH_VERSION;                         \
        }                                                       \
    }

namespace vle { namespace eov {

    class NetStreamReader;

    /**
     * @brief The vle::eov::Plugin is a class to develop graphical user
     * interface around a vle::oov::CairoPlugin. Use the DECLARE_EOV_PLUGIN
     * macro to develop a plugin.
     */
    class VLE_API Plugin : public sigc::trackable
    {
    public:
        /**
         * @brief Default constructor of the eov plugin.
         * @param cairoplugin A reference to the loaded oov::Plugin.
         */
        Plugin(oov::CairoPluginPtr cairoplugin, NetStreamReader* net)
            : m_cairoplugin(cairoplugin), m_net(net) {}

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
        virtual Gtk::Container& widget() = 0;

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

        void onConfigureEvent(GdkEventConfigure* event);

        /**
         * @brief Send to the CairoPlugin a message to redraw is storage image
         * buffer.
         */
        void copy()
        { m_cairoplugin->needCopy(); }

        /**
         * @brief Send to the CairoPlugin a message to initialize all
         * CairoSurface.
         */
        void init()
        { m_cairoplugin->needInit(); }

    protected:
        oov::CairoPluginPtr                 m_cairoplugin;
        NetStreamReader*                    m_net;
        Glib::RefPtr < Gdk::Pixmap >        m_buffer;
        Glib::RefPtr < Gdk::GC >            m_gc;
        Cairo::RefPtr < Cairo::Context >    m_ctx;

        void buildBuffer(int width, int height);
    };

    /**
     * @brief This typedef is used by the Eov plugin factory to automatically
     * destroy plugin at the end of the simulation.
     */
    typedef boost::shared_ptr < Plugin > PluginPtr ;

    /**
     * @brief Define the pointer to function of the eov::Plugin plug-in.
     * @param The reference to the oov::PluginPtr.
     * @param The reference to the NetStreamReader.
     * @return A pointer to the eov::Plugin newly build.
     */
    typedef Plugin* (*EovPluginSlot)(oov::PluginPtr, NetStreamReader*);

}} // namespace vle eov

#endif
