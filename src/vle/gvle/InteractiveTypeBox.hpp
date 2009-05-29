/**
 * @file vle/gvle/InteractiveTypeBox.hpp
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


#ifndef VLE_GVLE_INTERACTIVETYPEBOX_HPP
#define VLE_GVLE_INTERACTIVETYPEBOX_HPP

#include <gtkmm.h>
#include <vle/vpz/Base.hpp>
#include <vle/graph/CoupledModel.hpp>

namespace vle { namespace gvle {

    class InteractiveTypeBox : public Gtk::Dialog
    {
    public:
        InteractiveTypeBox(const std::string& title,
	                   vpz::Base* base = 0,
			   std::string currentValue = "");

        InteractiveTypeBox(const std::string& title,
	                   graph::CoupledModel* coupledModel = 0,
			   std::string currentValue = "");

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
	Gtk::HBox*               mHBox;
        Gtk::Entry*              mEntry;
	Gtk::Button*             mButtonOk;
	Gtk::Image               mImage;

	// Utils
        bool                     mValid;
	bool                     mState;
	vpz::Base*               mBase;
	graph::CoupledModel*     mCoupledModel;
	std::string&             mCurrentValue;

	// Methods
        void makeDialog();
	bool isValid();
	void onChange();
    };

}} // namespace vle gvle

#endif
