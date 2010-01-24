/**
 * @file vle/gvle/ObservableBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef GUI_OBSERVABLEBOX_HPP
#define GUI_OBSERVABLEBOX_HPP

#include <gtkmm.h>
#include <vle/vpz/Observable.hpp>
#include <vle/vpz/Views.hpp>
#include <vle/gvle/TreeViewObservable.hpp>

namespace vle
{
namespace gvle {

class ObservableBox : public Gtk::Dialog
{
public:
    ObservableBox(vpz::Observable& obs, vpz::Views& views);

    virtual ~ObservableBox();

    void run();

private:
    TreeViewObservable* mTreeView;

    //Values
    vpz::Observable* mObs;
    vpz::Views* mViews;

    //backuo
    vpz::Observable* mBackup_obs;
    vpz::Views* mBackup_views;
};

}
} // namespace vle gvle

#endif
