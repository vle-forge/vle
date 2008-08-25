/**
 * @file apps/gvle/main.cpp
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


#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <gtkmm/main.h>
#include <glibmm/optioncontext.h>
#include <glibmm/thread.h>
#include <iostream>
#include <fstream>
#include <string>

namespace vle { namespace gvle {

class GVLEOptionGroup : public Glib::OptionGroup
{
public:
    GVLEOptionGroup() :
	Glib::OptionGroup("gvle_group", "description of GVLE options",
			  "help description of GVLE group"),
	mInfo(false),
	mVersion(false),
	mLevel(0)
    {
        Glib::OptionEntry entry1;
        entry1.set_long_name("info");
        entry1.set_short_name('i');
        entry1.set_description("Information on GVLE.");
        add_entry(entry1, mInfo);

        Glib::OptionEntry entry2;
        entry2.set_long_name("version");
        entry2.set_short_name('v');
        entry2.set_description("Version information.");
        add_entry(entry2, mVersion);

	Glib::OptionEntry entry3;
	entry3.set_long_name("verbose level");
	entry3.set_short_name('V');
	entry3.set_description("[int] [0-3] min to max verbose.");
	add_entry(entry3, mLevel);
    }

    bool mInfo;    /// information on VLE program.
    bool mVersion; /// information on VLE version.
    int mLevel;    /// verbose level
};

}} // namespace vle gvle

int main(int argc, char** argv)
{
    vle::utils::init();

    Gtk::Main application(&argc, &argv);
    
    Glib::OptionContext context;
    vle::gvle::GVLEOptionGroup group;
    context.set_main_group(group);
    context.set_help_enabled(true);

    try {
        context.parse(argc, argv);
    } catch (const Glib::Error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    vle::utils::Trace::trace().setLogFile(
        vle::utils::Trace::getLogFilename("gvle.log"));
    vle::utils::Trace::trace().setLevel(
        static_cast < vle::utils::Trace::Level >(group.mLevel));

    if (group.mInfo) {
        std::cerr << "GVLE - the Gui of VLE\n";
        vle::utils::printInformations(std::cerr);
    } else if (group.mVersion) {
        std::cerr << "GVLE - the Gui of VLE\n";
	vle::utils::printVersion(std::cerr);
    } else {
	try {
	    vle::gvle::GVLE* g = 0;
	    if (argc > 1)
		g = new vle::gvle::GVLE(argv[1]);
	    else
		g = new vle::gvle::GVLE;
	    application.run(*g);
	    delete g;
        } catch(const Glib::Exception& e) {
            std::cerr << "\n/!\\ eov Glib error reported: " <<
                vle::utils::demangle(typeid(e)) << "\n" << e.what();
            return EXIT_FAILURE;
        } catch(const std::exception& e) {
            std::cerr << "\n/!\\ eov exception reported: " <<
                vle::utils::demangle(typeid(e)) << "\n" << e.what();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
