/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/utils/unit-test.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/vle.hpp>
#include <sstream>

using namespace vle;

const char *str = "# this file is a test\n"
    "test { # comment\n"
    "a = 1, 2, 3;"
    "b = \"1\", \"2\", \"3\";"
    "}\n";

void ParserStr()
{
    std::istringstream in(str);
    vle::utils::Parser parser(in);

    EnsuresEqual(parser.root().blocks.size(), 1);

    const vle::utils::Block& block = parser.root().getBlock("test");
    EnsuresEqual(block.name, "test");
    EnsuresEqual(block.strings.size(), 3);
    EnsuresEqual(block.reals.size(), 3);

    {
        typedef vle::utils::Block::Reals::const_iterator Iterator;
        std::pair < Iterator, Iterator > r = block.reals.equal_range("a");
        EnsuresEqual(block.reals.count("a"), 3);
        EnsuresEqual(r.first++->second, 1);
        EnsuresEqual(r.first++->second, 2);
        EnsuresEqual(r.first++->second, 3);
    }
    {
        typedef vle::utils::Block::Strings::const_iterator Iterator;
        std::pair < Iterator, Iterator > r = block.strings.equal_range("b");
        EnsuresEqual(block.strings.count("b"), 3);
        EnsuresEqual(r.first++->second, "1");
        EnsuresEqual(r.first++->second, "2");
        EnsuresEqual(r.first++->second, "3");
    }
}

int main()
{
    ParserStr();

    return unit_test::report_errors();
}
