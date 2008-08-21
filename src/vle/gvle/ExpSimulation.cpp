/**
 * @file vle/gvle/ExpSimulation.cpp
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


#include <vle/gvle/ExpSimulation.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/WarningBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Socket.hpp>
//#include <vle/manager/Run.hpp>
#include <vle/manager/JustRun.hpp>
//#include <vle/manager/Manager.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle
{
namespace gvle {

ExpSimulation::ExpSimulation(Modeling* m) :
        mModeling(m),
        mFrameLabel("Simulation"),
        mLabelDuration("Duration :"),
        //mButtonFile("..."),
        mFrameSimulation("Running"),
        mRadioButtonMono(mRadioButtonGroup, "Local - MonoThread"),
        mLabelNbProcess("Number of thread to use : "),
        mRadioButtonMulti(mRadioButtonGroup, "Local MultiThread"),
        mRadioButtonSocket(mRadioButtonGroup, "Socket"),
        mStart("start")//,
        //mPause("pause"),
        //mStop("stop")
{
    AssertI(m);

    if (mModeling->experiment().duration() == 0.0) {
        mModeling->experiment().setDuration(100.0);
    }
    mEntryDuration.set_text(utils::to_string(mModeling->experiment().duration()));
    mHBoxDuration.pack_start(mLabelDuration, false, true, 0);
    mHBoxDuration.pack_start(mEntryDuration, true, true, 0);

    mVBoxLabel.pack_start(mHBoxDuration, false, true, 0);
    mVBoxLabel.pack_start(mLabel2, true, true, 0);
    mVBoxLabel.pack_start(mLabel3, true, true, 0);
    mFrameLabel.set_border_width(3);
    mFrameLabel.add(mVBoxLabel);

    //mHBoxFile.pack_start(*(Gtk::manage(new Gtk::Label("File:"))), false, false);
    //mHBoxFile.pack_start(mEntryFile, true, true);
    //mHBoxFile.pack_start(mButtonFile, false, false);

    /*Group Radio Button Simulation*/
    mHBoxRadio.pack_start(mRadioButtonMono, true, true, 0);
    mHBoxRadio.pack_start(mRadioButtonMulti, true, true, 0);
    mHBoxRadio.pack_start(mRadioButtonSocket, true, true, 0);
    mVBoxRadio.pack_start(mHBoxRadio, false, true, 0);
    mVBoxRadio.pack_start(mLabelRadioButton, false, true, 0);

    mRadioButtonMulti.set_active(true);
    mVBoxRadio.set_border_width(3);

    /*Option MultiThread*/
    mHBoxMulti.pack_start(mLabelNbProcess, true, true, 0);
    mHBoxMulti.pack_start(mNbProcess, true, true, 0);
    mHBoxMulti.hide_all();

    /*Control Buttons Simulation*/
    mButtonControl.pack_end(mStart, true, true, 0);
    //mButtonControl.pack_end(mPause, true, true, 0);
    //mButtonControl.pack_end(mStop, true, true, 0);
    mButtonControl.set_border_width(3);

    mVBoxSimulation.pack_start(mVBoxRadio, false, true, 0);

    mVBoxSimulation.pack_start(mHBoxMulti);

    //mVBoxSimulation.pack_start(mProgressBar, false, true, 0);
    mVBoxSimulation.pack_start(mButtonControl, false, true, 0);
    //mVBoxSimulation.pack_start(mHBoxFile, false, false, 0);
    mFrameSimulation.set_border_width(3);
    mFrameSimulation.add(mVBoxSimulation);

    pack_start(mFrameLabel, true, true, 0);
    pack_start(mFrameSimulation, false, true, 0);

    //mStop.set_sensitive(false);
    //mPause.set_sensitive(false);
    connectSignal();
    set_border_width(3);
    on_radiobutton_local_mono();
}

void ExpSimulation::on_button_start()
{
    /*if (mEntryFile.get_text().empty() == false) {
          if (mRadioButtonLocal.get_active() == true) {
              runLocal(mEntryFile.get_text());
          } else if (mRadioButtonLocalPipe.get_active() == true) {
              runLocalPipe(mEntryFile.get_text());
          } else {
              runSocket(mEntryFile.get_text());
          }
          return;
    }
    */

    if (mModeling->isSaved() == false or mModeling->isModified() == true) {
        gvle::Error("You must save your project before.");
        return;
    } else {
        mModeling->experiment().setDuration(
            utils::to_double(mEntryDuration.get_text()));
        mModeling->saveXML(mModeling->getFileName());
        if (mRadioButtonMono.get_active() == true) {
            runMono(mModeling->getFileName());
        } else if (mRadioButtonMulti.get_active() == true) {
            runMulti(mModeling->getFileName());
        } else {
            runSocket(mModeling->getFileName());
        }
    }
}

void ExpSimulation::on_button_pause()
{
}

void ExpSimulation::on_button_stop()
{
}

void ExpSimulation::on_radiobutton_local_mono()
{
    mLabelRadioButton.set_text("Local execution - Debug only");
    mHBoxMulti.hide_all();
}

void ExpSimulation::on_radiobutton_local_multi()
{
    mLabelRadioButton.set_text("Local execution - another process");
    mHBoxMulti.show_all();
}

void ExpSimulation::on_radiobutton_socket()
{
    mLabelRadioButton.set_text("External execution - another process");
    mHBoxMulti.hide_all();
}

void ExpSimulation::connectSignal()
{
    mStart.signal_clicked().connect(sigc::mem_fun(*this,
                                    &ExpSimulation::on_button_start));
    /*mPause.signal_clicked().connect(sigc::mem_fun(*this,
            &ExpSimulation::on_button_pause));
    mStop.signal_clicked().connect(sigc::mem_fun(*this,
            &ExpSimulation::on_button_stop));
    */
    mRadioButtonMono.signal_clicked().connect(sigc::mem_fun(*this,
            &ExpSimulation::on_radiobutton_local_mono));
    mRadioButtonMulti.signal_clicked().connect(sigc::mem_fun(*this,
            &ExpSimulation::on_radiobutton_local_multi));
    mRadioButtonSocket.signal_clicked().connect(sigc::mem_fun(*this,
            &ExpSimulation::on_radiobutton_socket));

    /*mButtonFile.signal_clicked().connect(sigc::mem_fun(*this,
      &ExpSimulation::on_click_button_file));*/
}

void ExpSimulation::localPipeSimulationStarted()
{
    mLabel2.set_text((boost::format(
                          "Execution started using PID : %1%") % mChildPid).str());
}

void ExpSimulation::runMono(const std::string& /*name*/)
{
    using namespace manager;

    JustRunMono run(std::cerr);
    std::vector< const char* > args;
    args.push_back(mModeling->vpz().filename().c_str());
    run(args);

    /*old : try {
      manager::Simulator::run(name);
      } catch(const std::invalid_argument& e) {
      gvle::Error((boost::format(
      "Simulation: File problem. %1%") % e.what()).str());
      } catch(const utils::FileError& e) {
      gvle::Error((boost::format(
      "Simulation: File access error. %1%") % e.what()).str());
      } catch(const utils::ParseError& e) {
      gvle::Error((boost::format(
      "Simulation: Parse XML error. %1%") % e.what()).str());
      } catch(const std::exception& e) {
      gvle::Error((boost::format(
      "Unkwon error ?!?. %1%") % e.what()).str());
      }*/
}

void ExpSimulation::runMulti(const std::string& /*name*/)
{
    using namespace manager;

    int process = utils::to_int(mNbProcess.get_text());
    if (process > 0) {
        JustRunThread run(std::cerr, process);
        std::vector< const char* > args;
        args.push_back(mModeling->vpz().filename().c_str());
        run(args);
    }


    //old : std::list < std::string > localArgv;
    //localArgv.push_back("vle");
    //localArgv.push_back(name);

    //Glib::spawn_async(utils::Path::path().getDefaultBinDir(), localArgv,
    //Glib::SpawnFlags(0), sigc::slot<void>(), &mChildPid);

    //localPipeSimulationStarted();
}

void ExpSimulation::runSocket(const std::string& /*name*/)
{
    /*utils::net::Client* client = 0;
      std::string buffer;

      try {
    client = new utils::net::Client("localhost",
       mModeling->getSocketPort());
    buffer = Glib::file_get_contents(name);
    client->send_int(buffer.size());
    client->send_buffer(buffer);
      } catch(const utils::InternalError& e) {
       gvle::Error((boost::format(
       "Error running socket.\n%1%") % buffer).str());
      }

      delete client;*/
}

void ExpSimulation::on_click_button_file()
{
    Gtk::FileChooserDialog file("VPZ file",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filter;
    filter.set_name("Vle Project gZipped");
    filter.add_pattern("*.vpz");
    file.add_filter(filter);

    /*if (file.run() == Gtk::RESPONSE_OK ) {
      mEntryFile.set_text(file.get_filename());
      }*/
}

}
} // namespace vle gvle
