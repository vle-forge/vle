/**
 * @file apps/oov/OptionGroup.cpp
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


#include <apps/oov/OptionGroup.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <glibmm/optioncontext.h>



namespace vle { namespace apps {

CommandOptionGroup::CommandOptionGroup() :
    Glib::OptionGroup("commandgroup", "Commands of OOV",
                      "Description of commands"),
    mDaemon(false),
    mPort(8000),
    mInfos(false),
    mVersion(false),
    mVerbose(0)
{
    {
        Glib::OptionEntry en;
        en.set_long_name("daemon");
        en.set_short_name('d');
        en.set_description("Run in daemon mode.");
        add_entry(en, mDaemon);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("port");
        en.set_short_name('p');
        en.set_description("Port to listening .");
        add_entry(en, mPort);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("infos");
        en.set_short_name('i');
        en.set_description("Informations of OOV.");
        add_entry(en, mInfos);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("version");
        en.set_description("The OOV Version.");
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

void CommandOptionGroup::check()
{
    if (mPort == 0) {
        mPort = 8000;
    } else if (mPort > 65535 or mPort < 0) {
        throw utils::InternalError(boost::format(
                "Invalid port %1%. Choose a correct port ie. [1 - 65535]\n") %
            mPort);
    }
}

}} // namespace vle apps

