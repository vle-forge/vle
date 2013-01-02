/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_SIMPLETYPEBOX_HPP
#define VLE_GVLE_SIMPLETYPEBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace gvle {

    class VLE_API SimpleTypeBox : public Gtk::Dialog
    {
    public:
        SimpleTypeBox(value::Value* base);
        SimpleTypeBox(const std::string& title,
		      const std::string& value);

        virtual ~SimpleTypeBox();

        std::string run();

        /**
         * @brief Return true if the user have selected the Gtk::Button valid.
         * False otherwise.
         *
         * @return true if Gtk::Dialog is true, false otherwise.
         */
        bool valid() const { return mValid; }

    private:
        value::Value* mBase;
        Gtk::Entry* mEntry;
        bool mValid;

        void makeDialog();
    };

}} // namespace vle gvle

#endif
