/**
 * @file apps/vle/OptionGroup.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#include <vle/utils/i18n.hpp>
#include <glibmm/optioncontext.h>


namespace vle { namespace apps {

OptionGroup::OptionGroup()
    : Glib::OptionGroup(_("Commands"), _("Descriptions of commands")),
    mManager(false), mSimulator(false), mJustrun(false), mPort(8000),
    mProcess(1), mAllinlocal(false), mSaveVpz(false), mInfos(false),
    mVersion(false), mVerbose(0)
{
    {
        Glib::OptionEntry en;
        en.set_long_name(_("manager"));
        en.set_short_name('m');
        en.set_description(_("Run in manager mode."));
        add_entry(en, mManager);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name(_("simulator"));
        en.set_short_name('s');
        en.set_description(_("Run in simulator mode."));
        add_entry(en, mSimulator);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name(_("justrun"));
        en.set_short_name('j');
        en.set_description(_("Just run the files"));
        add_entry(en, mJustrun);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name(_("port"));
        en.set_short_name('p');
        en.set_description(_("Port to listening in manager or simulator " \
                             "mode."));
        add_entry(en, mPort);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name(_("process"));
        en.set_short_name('o');
        en.set_description(_("Number of process to run VPZ."));
        add_entry(en, mProcess);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name(_("package"));
        en.set_short_name('P');
        en.set_description(_("Select the package."));
        add_entry(en, mPackage);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("allinlocal");
        en.set_short_name('l');
        en.set_description(_("Run all instance into a local created " \
                             "simulator."));
        add_entry(en, mAllinlocal);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("savevpz");
        en.set_short_name('z');
        en.set_description(_("Save all VPZ instance file."));
        add_entry(en, mSaveVpz);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("infos");
        en.set_short_name('i');
        en.set_description(_("Informations of VLE."));
        add_entry(en, mInfos);
    }
    {
        Glib::OptionEntry en;
        en.set_long_name("version");
        en.set_description(_("The VLE Version."));
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

void OptionGroup::check()
{
    if ((mManager and mSimulator) or (mManager and mJustrun) or
        (mSimulator and mJustrun)) {
        throw utils::InternalError(
            _("Cannot start vle in simulator, manager and just run mode"));
    }

    if (not mManager and not mSimulator and not mJustrun)
        mJustrun = true;

    if (mPort == 0) {
        mPort = 8000;
    } else if (mPort > 65535 or mPort < 0) {
        throw utils::InternalError(fmt(
                _("Invalid port %1%. " \
                  "Choose a correct port ie. [1 - 65535]\n")) % mPort);
    }

    if (mProcess == 0) {
        mProcess = 1;
    } else if (mProcess <= 0) {
        throw utils::InternalError(fmt(
                _("Invalid number of process %1%. " \
                  "Choose a correct number of process ie. > 0\n")) % mProcess);
    }

    if (mVerbose < 0 or mVerbose > 3) {
        throw utils::InternalError(fmt(
                _("Invalid verbose %1%. Choose a correcte number [0 - 3]\n")) %
            mVerbose);
    }
}

}} // namespace vle apps

