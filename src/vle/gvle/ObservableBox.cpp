/**
 * @file vle/gvle/ObservableBox.cpp
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


#include <vle/gvle/ObservableBox.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ObservableBox::ObservableBox(vpz::Observable& obs, vpz::Views& views):
        Gtk::Dialog("Observable",true,true),
        mObs(&obs),
        mViews(&views),
        mBackup_obs(new vpz::Observable(obs)),
        mBackup_views(new vpz::Views(views))
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    //mTreeView = new TreeViewObservable(obs, views);
    //get_vbox()->pack_start(*mTreeView);

    show_all_children();
}

ObservableBox::~ObservableBox()
{
    hide_all();
    delete mBackup_obs;
    delete mBackup_views;
}

void ObservableBox::run()
{
    if (Gtk::Dialog::run() == Gtk::RESPONSE_CANCEL) {
        using namespace vpz;

        //Observable
        const ObservablePortList& portlist = mObs->observableportlist();
        ObservablePortList::const_iterator it = portlist.begin();
        while (it != portlist.end()) {
            mObs->del(it->first);
            ++it;
        }
        const ObservablePortList& portlist_backup = mBackup_obs->observableportlist();
        it = portlist_backup.begin();
        while (it != portlist_backup.end()) {
            mObs->add(it->second);
            ++it;
        }

        //Views
        mViews->clear();
        mViews->add(*mBackup_views);
    }
}

}
} // namespace vle gvle
