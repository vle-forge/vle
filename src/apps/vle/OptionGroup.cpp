/**
 * @file apps/vle/OptionGroup.cpp
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


#include <apps/vle/OptionGroup.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <glibmm/optioncontext.h>



namespace vle { namespace apps {

CommandOptionGroup::CommandOptionGroup() :
    VLEOptionGroup("commandgroup", "Commands of VLE",
                   "Description of commands"),
    mManager(false),
    mSimulator(false),
    mJustrun(false),
    mPort(8000),
    mProcess(1)
{
    {
        Glib::OptionEntry en;
        en.set_long_name("manager");
        en.set_short_name('m');
        en.set_description("Run in manager mode.");
        add_entry(en, mManager);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("simulator");
        en.set_short_name('s');
        en.set_description("Run in simulator mode.");
        add_entry(en, mSimulator);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("justrun");
        en.set_short_name('j');
        en.set_description("Just run the files");
        add_entry(en, mJustrun);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("port");
        en.set_short_name('p');
        en.set_description("Port to listening in manager or simulator mode.");
        add_entry(en, mPort);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("process");
        en.set_short_name('o');
        en.set_description("Number of process to run VPZ.");
        add_entry(en, mProcess);
    }
}

void CommandOptionGroup::check()
{
    if ((mManager and mSimulator) or (mManager and mJustrun) or
        (mSimulator and mJustrun)) {
        throw utils::InternalError(boost::format(
                    "Choose a correct mode not: %1% %2% %3%\n") %
                    (mManager ? "manager" : "") % (mSimulator ? "simulator" : "") %
                    (mJustrun ? "justrun": ""));
    }

    if (not mManager and not mSimulator and not mJustrun)
        mJustrun = true;

    if (mPort == 0) {
        mPort = 8000;
    } else if (mPort > 65535 or mPort < 0) {
        throw utils::InternalError(boost::format(
                "Invalid port %1%. Choose a correct port ie. [1 - 65535]\n") %
            mPort);
    }

    if (mProcess == 0) {
        mProcess = 1;
    } else if (mProcess <= 0) {
        throw utils::InternalError(boost::format(
                "Invalid %1%. Choose a correct number of process ie. > 0\n") %
            mProcess);
    }
}

ManagerOptionGroup::ManagerOptionGroup() :
    VLEOptionGroup("mangroup",
                   "Manager options",
                   "Description of manager options"),
    mAllinlocal(false),
    mSaveVpz(false)
{
    {
        Glib::OptionEntry en;
        en.set_long_name("allinlocal");
        en.set_short_name('l');
        en.set_description("Run all instance into a local created simulator.");
        add_entry(en, mAllinlocal);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("savevpz");
        en.set_short_name('z');
        en.set_description("Save all VPZ instance file.");
        add_entry(en, mSaveVpz);
    }
}

void ManagerOptionGroup::check()
{
}

GlobalOptionGroup::GlobalOptionGroup() :
    VLEOptionGroup("globalgroup", "Global options",
                   "Description of global options"),
    mInfos(false),
    mVersion(false),
    mVerbose(0)
{
    {
        Glib::OptionEntry en;
        en.set_long_name("infos");
        en.set_short_name('i');
        en.set_description("Informations of VLE.");
        add_entry(en, mInfos);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("version");
        en.set_description("The VLE Version.");
        add_entry(en, mVersion);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("verbose");
        en.set_short_name('v');
        en.set_description(
            "Verbose mode 0 - 3. [default 0]\n"
            "\t(0) no trace and no long exception\n"
            "\t(1) small simulation trace and long exception\n"
            "\t(2) long simulation trace\n"
            "\t(3) all simulation trace\n");
        add_entry(en, mVerbose);
    }
}

void GlobalOptionGroup::check()
{
    if (mVerbose < 0 or mVerbose > 3) {
        throw utils::InternalError(boost::format(
                "Invalid verbose %1%. Choose a correcte number [0 - 3]\n") %
            mVerbose);
    }
}

}} // namespace vle apps

