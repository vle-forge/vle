/**
 * @file vle/gvle/ViewBox.cpp
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


#include <vle/gvle/Message.hpp>
#include <vle/gvle/OutputBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/ViewBox.hpp>
#include <vle/utils/Tools.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ViewBox::ViewBox(vpz::Views& views, vpz::View* view, vpz::Outputs* outputs) :
        Gtk::Dialog("?",true,true),
        mViews(views),
        mView(view),
        mOutputs(outputs),
        mOutputs_backup(new vpz::Outputs(*outputs))
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    set_title("View " + view->name());

    mFrame = new Gtk::Frame("Output");
    mVbox = new Gtk::VBox();
    mHbox2 = new Gtk::HBox();
    mLabel2 = new Gtk::Label("output : ");
    mAddOutput = new Gtk::Button("Add");
    mEdOutput = new Gtk::Button("Edit");
    mDelOutput = new Gtk::Button("Delete");
    mOutput = new Gtk::ComboBoxText();

    mHbox2_1 = new Gtk::HBox();
    mHbox2_1->pack_start(*mLabel2);
    mHbox2_1->pack_start(*mOutput);
    mVbox->pack_start(*mHbox2_1);

    mHbox2_2 = new Gtk::HBox();
    mHbox2_2->pack_start(*mAddOutput);
    mAddOutput->signal_clicked().connect(
        sigc::mem_fun(*this, &ViewBox::add_output));
    mHbox2_2->pack_start(*mEdOutput);
    mEdOutput->signal_clicked().connect(
        sigc::mem_fun(*this, &ViewBox::edit_output));
    mHbox2_2->pack_start(*mDelOutput);
    mDelOutput->signal_clicked().connect(
        sigc::mem_fun(*this, &ViewBox::del_output));
    mVbox->pack_start(*mHbox2_2);

    mFrame->add(*mVbox);
    get_vbox()->pack_start(*mFrame);

    mHbox3 = new Gtk::HBox();
    mLabel3 = new Gtk::Label("type : ");
    mType = new Gtk::ComboBoxText();
    mType->signal_changed().connect(
        sigc::mem_fun(*this, &ViewBox::display_timestep));
    mHbox3->pack_start(*mLabel3);
    mHbox3->pack_start(*mType);
    get_vbox()->pack_start(*mHbox3);

    mHbox4 = new Gtk::HBox();
    mLabel4 = new Gtk::Label("timestep : ");
    mTimeStep = new Gtk::Entry();
    mTimeStep->set_text(utils::to_string(view->timestep()));
    mHbox4->pack_start(*mLabel4);
    mHbox4->pack_start(*mTimeStep);
    get_vbox()->pack_start(*mHbox4);

    makeOutputs();
    makeType();

    set_size_request(270,210);
    show_all_children();
    display_timestep();
}

ViewBox::~ViewBox()
{
    hide_all();
    delete mHbox2_1;
    delete mHbox2_2;
    delete mFrame;
    delete mVbox;
    delete mLabel2;
    delete mAddOutput;
    delete mEdOutput;
    delete mDelOutput;
    delete mOutput;
    delete mHbox3;
    delete mLabel3;
    delete mType;
    delete mHbox4;
    delete mLabel4;
    delete mTimeStep;
    delete mOutputs_backup;
}

void ViewBox::run()
{
    int rep;

    for (;;) {
        rep = Gtk::Dialog::run();
        if (mOutput->get_active_text() != "" || rep == Gtk::RESPONSE_CANCEL)
            break;
        else {
            Error("Please link an Output to this View");
        }
    }

    switch (rep) {
    case Gtk::RESPONSE_APPLY:
        {
            vpz::View copy(*mView);
            mViews.del(copy.name());
            mView = 0;

            if (mType->get_active_text() == "Timed") {
                mViews.addTimedView(copy.name(),
                                    utils::to_double(mTimeStep->get_text()),
                                    mOutput->get_active_text());
            } else if (mType->get_active_text() == "Event") {
                mViews.addEventView(copy.name(),
                                    mOutput->get_active_text());
            } else if (mType->get_active_text() == "Finish") {
                mViews.addFinishView(copy.name(),
                                     mOutput->get_active_text());
            }
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        mOutputs->clear();
        vpz::OutputList list = mOutputs_backup->outputlist();
        vpz::OutputList::const_iterator it = list.begin();
        while (it != list.end()) {
            mOutputs->add(it->second);
            ++it;
        }
        break;
    }
}

void ViewBox::makeOutputs()
{
    mOutput->clear_items();
    std::vector < std::string > list = mOutputs->outputsname();
    std::vector < std::string >::const_iterator it = list.begin();
    while (it != list.end()) {
        mOutput->append_text(*it);
        ++it;
    }
    mOutput->set_active_text(mView->output());
}

void ViewBox::makeType()
{
    mType->append_text("Timed");
    mType->append_text("Event");
    mType->append_text("Finish");
    switch (mView->type()) {
    case vle::vpz::View::TIMED:
        mType->set_active_text("Timed");
        break;
    case vle::vpz::View::EVENT:
        mType->set_active_text("Event");
        break;
    case vle::vpz::View::FINISH:
        mType->set_active_text("Finish");
        break;
    }
}

void ViewBox::display_timestep()
{
    if (mType->get_active_text() == "Timed") {
        mHbox4->show_all();
    } else if (mType->get_active_text() == "Event") {
        mHbox4->hide_all();
    } else if (mType->get_active_text() == "Finish") {
        mHbox4->hide_all();
    }
}

void ViewBox::add_output()
{
    SimpleTypeBox box("name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == "" || mOutputs->exist(name));
    vpz::Output out;
    out.setName(name);
    out.setLocalStream("", "default");
    mOutputs->add(out);
    makeOutputs();
}

void ViewBox::edit_output()
{
    if (mOutputs->exist(mOutput->get_active_text())) {
        OutputBox box(mOutputs->get(mOutput->get_active_text()));
        box.run();
    }
}

void ViewBox::del_output()
{
    mOutputs->del(mOutput->get_active_text());
    makeOutputs();
}

}
} // namespace vle gvle
