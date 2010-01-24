/**
 * @file vle/gvle/ReplicasBox.cpp
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


#include <vle/gvle/ReplicasBox.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Path.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ReplicasBox::ReplicasBox(Glib::RefPtr < Gnome::Glade::Xml > ref) :
        mNbReplicas(1),
        mRefXML(ref),
        mRand(0)
{
    mRefXML->get_widget("DialogReplicas", mDialog);
    mRefXML->get_widget("treeviewReplica", mTreeViewValue);
    mRefXML->get_widget("spinbuttonNumber", mSpinButtonNumber);
    mRefXML->get_widget("addButtonReplica", mButtonIncrement);
    mRefXML->get_widget("delButtonReplica", mButtonDecrement);
    mRefXML->get_widget("buttonRandomReplica", mButtonRandom);

    mButtonIncrement->signal_clicked().connect(
        sigc::mem_fun(*this, &ReplicasBox::onIncrement));
    mButtonDecrement->signal_clicked().connect(
        sigc::mem_fun(*this, &ReplicasBox::onDecrement));
    mButtonRandom->signal_clicked().connect(
        sigc::mem_fun(*this, &ReplicasBox::onRandom));
    mSpinButtonNumber->signal_value_changed().connect(
        sigc::mem_fun(*this, &ReplicasBox::onSpinButtonNumberChange));

    mListStore = Gtk::ListStore::create(mColumn);
    mTreeViewValue->set_model(mListStore);
    mTreeViewValue->append_column_editable(_("Value"), mColumn.mValue);

    mRand = new vle::utils::Rand();
}

ReplicasBox::~ReplicasBox()
{
    delete mRand;
}

bool ReplicasBox::run()
{
    bool r = false;
    mDialog->show_all();

    Gtk::TreeModel::Row row = *(mListStore->append());
    if (row)
        row[mColumn.mValue] = 0;


    if (mDialog->run() == Gtk::RESPONSE_OK) {
        r = true;
    }

    mDialog->hide_all();
    return r;
}

bool ReplicasBox::run(const std::vector < guint32 >& lst)
{
    bool r = false;
    clear();
    mDialog->show_all();

    for (std::vector < guint32 >::const_iterator it = lst.begin();
            it != lst.end(); ++it) {
        Gtk::TreeModel::Row row = *(mListStore->append());
        if (row)
            row[mColumn.mValue] = *it;
    }

    if (mDialog->run() == Gtk::RESPONSE_OK) {
        r = true;
    }

    mDialog->hide_all();
    return r;
}

void ReplicasBox::clear()
{
    mListStore->clear();
}

void ReplicasBox::onIncrement()
{
    Gtk::TreeModel::Children children = mListStore->children();
    Gtk::TreeModel::iterator it = children.begin();

    if (it) {
        Gtk::TreeModel::Row row = *it;
        int value = row[mColumn.mValue];
        ++it;

        while (it != children.end()) {
            Gtk::TreeModel::Row row = *it;
            row[mColumn.mValue] = ++value;
            ++it;
        }
    }
}

void ReplicasBox::onDecrement()
{
    Gtk::TreeModel::Children children = mListStore->children();
    Gtk::TreeModel::iterator it = children.begin();

    if (it) {
        Gtk::TreeModel::Row row = *it;
        int value = row[mColumn.mValue];
        ++it;

        while (it != children.end()) {
            Gtk::TreeModel::Row row = *it;
            row[mColumn.mValue] = --value;
            ++it;
        }
    }
}

void ReplicasBox::onRandom()
{
    Gtk::TreeModel::Children children = mListStore->children();
    Gtk::TreeModel::iterator it = children.begin();

    if (it) {
        while (it != children.end()) {
            Gtk::TreeModel::Row row = *it;
            row[mColumn.mValue] = mRand->getInt();
            ++it;
        }
    }
}

void ReplicasBox::onSpinButtonNumberChange()
{
    int nb = (int)mSpinButtonNumber->get_value();

    if (nb > mNbReplicas) {
        const int todo = nb - mNbReplicas;
        for (int i = 0; i < todo; ++i) {
            Gtk::TreeModel::Row row = *(mListStore->append());
            if (row)
                row[mColumn.mValue] = 0;
        }
    } else {
        int todo = mNbReplicas - nb;
        while (todo > 0) {
            Gtk::TreeModel::Children children = mListStore->children();
            Gtk::TreeModel::iterator it = children.begin();
            mListStore->erase(it);
            --todo;
        }
    }
    mNbReplicas = nb;
}

}
} // namespace vle gvle
