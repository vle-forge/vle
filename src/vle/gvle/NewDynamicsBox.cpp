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


#include <vle/gvle/NewDynamicsBox.hpp>

namespace vle { namespace gvle {

NewDynamicsBox::NewDynamicsBox(const Glib::RefPtr < Gtk::Builder >& xml,
        const vle::utils::Package& curr_pack):
    mXml(xml), mCurrPackage(curr_pack)
{
    xml->get_widget("DialogNewDynamics", mDialog);
    xml->get_widget("EntryClassName", mEntryClassName);
    xml->get_widget("EntryNamespace", mEntryNamespace);
    xml->get_widget("ButtonOkNewDynamics", mButtonApply);
    xml->get_widget("ButtonCancelNewDynamics", mButtonCancel);

     mList.push_back(mButtonApply->signal_clicked().connect(
                         sigc::mem_fun(*this, &NewDynamicsBox::onApply)));

     mList.push_back(mButtonCancel->signal_clicked().connect(
                         sigc::mem_fun(*this, &NewDynamicsBox::onCancel)));
}

NewDynamicsBox::~NewDynamicsBox()
{
    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void NewDynamicsBox::onApply()
{
    mDialog->response(Gtk::RESPONSE_OK);
    // No hide_all in gtk 3
    mDialog->hide();
}

void NewDynamicsBox::onCancel()
{
    mDialog->response(Gtk::RESPONSE_CANCEL);
    // No hide_all in gtk 3
    mDialog->hide();
}

int NewDynamicsBox::run()
{
    mEntryNamespace->set_text(mCurrPackage.name());
    mDialog->show_all();
    return mDialog->run();
}

}} // namespace vle gvle
