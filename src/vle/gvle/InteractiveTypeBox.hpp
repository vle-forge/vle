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


#ifndef VLE_GVLE_INTERACTIVETYPEBOX_HPP
#define VLE_GVLE_INTERACTIVETYPEBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/CoupledModel.hpp>

namespace vle { namespace gvle {

    class InteractiveTypeBox : public Gtk::Dialog
    {
    public:
        InteractiveTypeBox(const std::string& title,
	                   vpz::Base* base = 0,
			   const std::string& currentValue = std::string());

        InteractiveTypeBox(const std::string& title,
	                   vpz::CoupledModel* coupledModel = 0,
			   const std::string& currentValue = "");

        virtual ~InteractiveTypeBox();

        std::string run();

        /**
         * @brief Return true if the user have selected the Gtk::Button valid.
         * False otherwise.
         *
         * @return true if Gtk::Dialog is true, false otherwise.
         */
        bool valid() const { return mValid; }

    private:
	// Gtk widgets
	Gtk::Box*               mHBox;
        Gtk::Entry*              mEntry;
	Gtk::Button*             mButtonOk;
	Gtk::Image               mImage;

	// Utils
        bool                     mValid;
	bool                     mState;
	vpz::Base*               mBase;

	vpz::CoupledModel*     mCoupledModel;
	std::string              mCurrentValue;

	// Methods
        void makeDialog();
	bool isValid();
	void onChange();
    };

}} // namespace vle gvle

#endif
