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


#ifndef VLE_GVLE_ABOUT_HPP
#define VLE_GVLE_ABOUT_HPP

#include <gtkmm/builder.h>

namespace vle { namespace gvle {

/**
 * @brief modal About dialog box.
 *
 */
class About
{
public:
    /**
     * Buid a modal About dialog box.
     */
    About(const Glib::RefPtr < Gtk::Builder >& refXml);

    /**
     * @brief Nothing to delete.
     */
    ~About();

    /**
     * @brief Show the dialog box and wait for user's actions.
     */
    void run();

private:
    About(const About& about);
    About& operator=(const About& about);

    class Pimpl;

    Pimpl* mImpl;
};

}} // namespace vle gvle

#endif
