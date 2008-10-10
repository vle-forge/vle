/**
 * @file vle/utils/test/test1.cpp
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE atomicdynamics_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <vle/utils.hpp>
#include <iostream>

using namespace vle;

struct is_odd
{
    inline bool operator()(const int i) const
    { return i % 2; }
};

struct cout_map
{
    inline void operator()(int x) const
    { std::cout << x << "\n"; }
};

struct clear_string
{
    inline void operator()(std::string& x) const
    { x.clear(); }
};

struct append_string
{
    std::string str;

    void operator()(const std::string& x)
    { str.append(x); }
};

BOOST_AUTO_TEST_CASE(test_algo)
{
    std::vector < int > b(5);
    std::vector < int > out;
    b[0] = 1;
    b[1] = 2;
    b[2] = 2;
    b[3] = 5;
    b[4] = 6;

    utils::copy_if(b.begin(), b.end(), std::back_inserter(out), is_odd());

    BOOST_REQUIRE_EQUAL(out.size(), (std::vector < int >::size_type)2);
    BOOST_REQUIRE_EQUAL(out[0], 1);
    BOOST_REQUIRE_EQUAL(out[1], 5);

    BOOST_REQUIRE_EQUAL(b.size(), (std::vector < int >::size_type)5);
    BOOST_REQUIRE_EQUAL(b[0], 1);
    BOOST_REQUIRE_EQUAL(b[1], 2);
    BOOST_REQUIRE_EQUAL(b[2], 2);
    BOOST_REQUIRE_EQUAL(b[3], 5);
    BOOST_REQUIRE_EQUAL(b[4], 6);

    std::map < std::string, int > m;
    m["toto"] = 1;
    m["tutu"] = 2;
    m["titi"] = 2;

    std::map < std::string, std::string > n;
    n["tutu"] = "ou";
    n["tati"] = "xu";
    n["tete"] = "re";
    n["tita"] = "nt";

    append_string x = utils::for_each(n.begin(), n.end(), append_string());
    BOOST_REQUIRE_EQUAL(x.str, "xurentou");


    std::map < std::string, std::string >::iterator it;
    it = utils::find_if(n.begin(), n.end(),
                        std::bind1st(std::equal_to < std::string >(),
                                    std::string("ou")));
    BOOST_REQUIRE(it != n.end());
}

BOOST_AUTO_TEST_CASE(test_unary_function)
{
    typedef std::map < std::string, std::string > TwoStrings;
    TwoStrings n;
    n["tutu"] = "ou";
    n["tati"] = "xu";
    n["tete"] = "re";
    n["tita"] = "nt";

    {
        std::ostringstream out;

        std::transform(n.begin(), n.end(),
                       std::ostream_iterator < std::string >(out),
                       utils::select1st < TwoStrings::value_type >());

        BOOST_REQUIRE_EQUAL(out.str(), "tatitetetitatutu");
    }

    {
        std::ostringstream out;

        std::transform(n.begin(), n.end(),
                       std::ostream_iterator < std::string >(out),
                       utils::select2nd < TwoStrings::value_type >());

        BOOST_REQUIRE_EQUAL(out.str(), "xurentou");
    }
}
