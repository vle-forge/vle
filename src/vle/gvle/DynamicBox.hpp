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


#ifndef VLE_GVLE_DYNAMICBOX_HPP
#define VLE_GVLE_DYNAMICBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <vle/vpz/Dynamic.hpp>

namespace vle {

    namespace vpz {
        class AtomicModel;
        class Dynamic;
        class Conditions;
        class Observables;
        class AtomicModel;
    }
}

namespace vle { namespace gvle {

    class GVLE;

    class DynamicBox : public sigc::trackable
    {
    public:
        DynamicBox(const Glib::RefPtr < Gtk::Builder >& xml,
                   GVLE* gvle,
                   vpz::AtomicModel& atom,
                   vpz::Dynamic& dynamic,
                   vpz::Conditions& conditions,
                   vpz::Observables& observables);

        ~DynamicBox();

        void show(vpz::Dynamic* dyn);

        /**
         * @brief Return true if the user select the Gtk::REPONSE_OK, false
         * otherwise.
         * @return
         */
        bool valid() const { return mValid; }

    private:
        Glib::RefPtr < Gtk::Builder >       mXml;
        GVLE*                               mGVLE;
        vpz::AtomicModel&                   mAtom;
        vpz::Dynamic&                       mDynamic;
        vpz::Conditions&                    mConditions;
        vpz::Observables&                   mObservables;
        Gtk::Dialog*                        mDialog;
        Gtk::ComboBoxText*                  mComboLibrary;
        Gtk::ComboBoxText*                  mComboPackage;
        Gtk::Button*                        mButtonNew;
        vpz::Dynamic*                       mDyn;

        Gtk::Box* mBoxDynamicLibrary;
        Gtk::Box* mBoxDynamicPackage;

        std::list < sigc::connection >      mList;

        bool                                mValid;

        void makeComboLibrary();
        void makeComboPackage();

        /**
         * @brief When combobox package is changed, we update the combobox
         * library.
         */
        void onComboPackageChange();
        void on_apply();
        void on_cancel();
        void onNewLibrary();
        int execPlugin(const std::string& pluginname,
                       const std::string& classname,
                       const std::string& namespace_);
    };

}} // namespace vle gvle

#endif
