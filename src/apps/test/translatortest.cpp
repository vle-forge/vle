/** 
 * @file main.cpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 18 mai 2006 17:55:16 +0200
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <glibmm/fileutils.h>
#include <glibmm/spawn.h>
#include <iostream>

using namespace vle;


void translate(const char* filename)
{
    try {
        std::cerr << "Read: " << filename;
        vpz::Vpz* file = new vpz::Vpz(filename);
        if (file->hasNoVLE()) {
            file->expandTranslator();
        }

        std::string newfile(filename);
        newfile += "-tr";
        std::cerr << "\nWrite under: " << newfile;
        file->write(newfile);

        delete file;
    } catch(const std::exception& e) {
        std::cerr << "\n/!\\ Error reported: " << utils::demangle(typeid(e)) <<
            "\n" << e.what();
    }
    std::cerr << std::endl;
}

int main(int argc, char* argv[])
{
    vle::utils::Trace::trace().set_level(3);

    for (int i = 1; i < argc; ++i) {
        translate(argv[i]);
    }

    return EXIT_SUCCESS;
}
