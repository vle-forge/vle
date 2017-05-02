/*
 * @file vle/oov/plugins/FileType.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <gauthier.quesnel@inra.fr>
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

#include "FileType.hpp"

namespace vle {
namespace oov {
namespace plugin {

CSV::~CSV()
{
}

std::string
CSV::extension() const
{
    return ".csv";
}

void
CSV::writeSeparator(std::ostream& out)
{
    out << ';';
}

void
CSV::writeHead(std::ostream& out, const std::vector<std::string>& heads)
{
    if (not heads.empty()) {
        std::vector<std::string>::const_iterator it = heads.begin();
        out << *it;
        it++;

        while (it != heads.end()) {
            out << ";\"" << *it << "\"";
            ++it;
        }
    }
    out << '\n';
}

Text::~Text()
{
}

std::string
Text::extension() const
{
    return ".dat";
}

void
Text::writeSeparator(std::ostream& out)
{
    out << '\t';
}

void
Text::writeHead(std::ostream& out, const std::vector<std::string>& heads)
{
    out << '#';
    for (std::vector<std::string>::const_iterator it = heads.begin();
         it != heads.end();
         ++it) {
        out << "\"" << *it << "\"\t";
    }
    out << '\n';
}

Rdata::~Rdata()
{
}

std::string
Rdata::extension() const
{
    return ".rdata";
}

void
Rdata::writeSeparator(std::ostream& out)
{
    out << '\t';
}

void
Rdata::writeHead(std::ostream& out, const std::vector<std::string>& heads)
{
    for (std::vector<std::string>::const_iterator it = heads.begin();
         it != heads.end();
         ++it) {
        out << "\"" << *it << "\"\t";
    }
    out << '\n';
}
}
}
} // namespace vle oov plugin
