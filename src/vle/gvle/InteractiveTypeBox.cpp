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


#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/Observables.hpp>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/Views.hpp>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

InteractiveTypeBox::InteractiveTypeBox(const std::string& title,
				       vpz::Base* base,
				       const std::string& currentValue) :
    Gtk::Dialog(title, true),
    mValid(false),
    mState(false),
    mBase(base),
    mCoupledModel(0),
    mCurrentValue(currentValue)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    mButtonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    set_default_response(Gtk::RESPONSE_OK);
    makeDialog();
    mHBox->pack_start(mImage);
    
    // Passage GTK-3 mettre show, parceque plus le hide_all n'exite plus ???
    show_all();
}

InteractiveTypeBox::InteractiveTypeBox(const std::string& title,
				       vpz::CoupledModel* coupledModel,
				       const std::string& currentValue) :
    Gtk::Dialog(title, true),
    mValid(false),
    mState(false),
    mBase(0),
    mCoupledModel(coupledModel),
    mCurrentValue(currentValue)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    mButtonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    set_default_response(Gtk::RESPONSE_OK);
    makeDialog();
    mHBox->pack_start(mImage);
    
    // Passage GTK-3 mettre show, parceque plus le hide_all n'exite plus ???
    show_all();
}

InteractiveTypeBox::~InteractiveTypeBox()
{
    // Passage gtk 3 : hide_all n'existe plus !
    hide();
    delete mEntry;
}

void InteractiveTypeBox::makeDialog()
{
    mHBox = new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);
    mEntry = new Gtk::Entry();
    mEntry->set_editable(true);
    mEntry->set_activates_default(true);
    mEntry->signal_changed().connect(
	sigc::mem_fun(*this, &InteractiveTypeBox::onChange));

    if (!mCurrentValue.empty())
	mEntry->set_text(mCurrentValue);

    mHBox->pack_start(*mEntry);
    get_vbox()->add(*mHBox);
    mImage.set(Gtk::StockID(Gtk::Stock::NO), Gtk::IconSize(1));
    mButtonOk->set_sensitive(false);
}

std::string InteractiveTypeBox::run()
{
    mValid = Gtk::Dialog::run() == Gtk::RESPONSE_OK;

    if (mValid) {
	return mEntry->get_text();
    } else {
	return std::string();
    }
}

void InteractiveTypeBox::onChange()
{
    if (isValid())
	mButtonOk->set_sensitive(true);
    else
	mButtonOk->set_sensitive(false);
}

bool InteractiveTypeBox::isValid()
{
    mState = false;
    if (mBase) {
	switch (mBase->getType()) {
	case vpz::Base::VLE_VPZ_CLASSES:
	    mState = not (dynamic_cast< vpz::Classes* >(mBase))->exist(mEntry->get_text());
	    break;
	case vpz::Base::VLE_VPZ_CONDITIONS:
	    mState = not (dynamic_cast< vpz::Conditions* >(mBase))->exist(mEntry->get_text());
	    break;
	case vpz::Base::VLE_VPZ_DYNAMICS:
	    mState = not (dynamic_cast< vpz::Dynamics* >(mBase))->exist(mEntry->get_text());
	    break;
	case vpz::Base::VLE_VPZ_OBSERVABLES:
	    mState = not (dynamic_cast< vpz::Observables* >(mBase))->exist(mEntry->get_text());
	    break;
	case vpz::Base::VLE_VPZ_OUTPUTS:
	    mState = not (dynamic_cast< vpz::Outputs* >(mBase))->exist(mEntry->get_text());
	    break;
	case vpz::Base::VLE_VPZ_VIEWS:
	    mState = not (dynamic_cast< vpz::Views* >(mBase))->exist(mEntry->get_text());
	    break;
	default:
	    Error(_("Unknown argument type."));
	    return false;
	}
    } else {
	mState = not mCoupledModel->exist(mEntry->get_text());
    }

    if (mState && not (mEntry->get_text().empty())) {
	mImage.set(Gtk::StockID(Gtk::Stock::YES), Gtk::IconSize(1));
	return true;
    } else {
	mImage.set(Gtk::StockID(Gtk::Stock::NO), Gtk::IconSize(1));
	return false;
    }
}

}} //namespace vle gvle
