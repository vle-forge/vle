/*
 * @file apps/oov/OptionGroup.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
        en.set_description(_("Run in daemon mode."));
        add_entry(en, mDaemon);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("port");
        en.set_short_name('p');
        en.set_description(_("Port to listening ."));
        add_entry(en, mPort);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("infos");
        en.set_short_name('i');
        en.set_description(_("Informations of OOV."));
        add_entry(en, mInfos);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("version");
        en.set_description(_("The OOV Version."));
        add_entry(en, mVersion);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("verbose");
        en.set_short_name('v');
        en.set_description(_(
            "Verbose mode 0 - 3. [default 0]\n"
            "\t(0) no trace and no long exception\n"
            "\t(1) small simulation trace and long exception\n"
            "\t(2) long simulation trace\n"
            "\t(3) all simulation trace\n"));
        add_entry(en, mVerbose);
    }
}

void CommandOptionGroup::check()
{
    if (mPort == 0) {
        mPort = 8000;
    } else if (mPort > 65535 or mPort < 0) {
        throw utils::InternalError(fmt(
                _("Invalid port %1%. Choose a correct port ie. [1 - 65535]\n")) %
            mPort);
    }
}

}} // namespace vle apps

