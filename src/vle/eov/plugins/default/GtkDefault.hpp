/*
 * @file vle/eov/plugins/default/GtkDefault.hpp
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


#ifndef VLE_EOV_PLUGINS_GTKDEFAULT_HPP
#define VLE_EOV_PLUGINS_GTKDEFAULT_HPP

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/drawingarea.h>
#include <vle/eov/Plugin.hpp>
#include <vle/eov/NetStreamReader.hpp>

namespace vle { namespace eov {

    class GtkDefault : public eov::Plugin
    {
    public:
        GtkDefault(oov::CairoPluginPtr cairoplugin, NetStreamReader* net);

        virtual ~GtkDefault();

        virtual Glib::RefPtr < Gdk::Window > drawingSurface();

        virtual Gtk::Widget& drawingWidget();

        virtual Gtk::Container& widget();

        virtual int width();

        virtual int height();

    private:
        Gtk::VBox           m_vbox;
        Gtk::DrawingArea    m_da;
        Gtk::Label          m_label;
    };

}} // namespace vle eov

DECLARE_EOV_PLUGIN(vle::eov::GtkDefault)

#endif
