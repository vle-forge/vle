/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_library_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>
#include <numeric>
#include <vle/utils/Algo.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vle.hpp>

using namespace vle;

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

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

    utils::copyIf(b.begin(), b.end(), std::back_inserter(out), is_odd());

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

    append_string x = utils::forEach(n.begin(), n.end(), append_string());
    BOOST_REQUIRE_EQUAL(x.str, "xurentou");


    std::map < std::string, std::string >::iterator it;
    it = utils::findIf(n.begin(), n.end(),
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

BOOST_AUTO_TEST_CASE(test_generator)
{
    vle::utils::Rand r(123456789);

    r.getBool();
    r.getInt();
    r.getInt(-100, 100);
    r.getDouble();
    r.getDouble(-1.0, 1.0);
}

BOOST_AUTO_TEST_CASE(test_uniform)
{
    const std::size_t szmax(1000);
    std::vector < uint32_t > vec1(szmax, 0);
    std::vector < double > vec2(szmax, 0);
    vle::utils::Rand r(123456789);

    boost::uniform_int < > distrib1(0, 10);
    boost::uniform_real < > distrib2(0.0, 10.0);

    boost::variate_generator < boost::mt19937&,
        boost::uniform_int < > > gen1(r.gen(), distrib1);
    boost::variate_generator < boost::mt19937&,
        boost::uniform_real < > > gen2(r.gen(), distrib2);

    vle::utils::generate(vec1.begin(), vec1.end(), gen1);

    double result1 = ((double)std::accumulate(vec1.begin(), vec1.end(),
                                              (uint32_t)0)) / (double)szmax;
    BOOST_REQUIRE_CLOSE(result1, 5.0, 10);

    vle::utils::generate(vec2.begin(), vec2.end(), gen2);

    double result2 = std::accumulate(vec2.begin(), vec2.end(), (double)0.0) /
        (double) szmax;
    BOOST_REQUIRE_CLOSE(result2, 5.0, 10);

    {
        std::vector < uint32_t > vec1_test(vec1);
        BOOST_REQUIRE(vec1 == vec1_test);

        for (std::vector < uint32_t >::iterator it = vec1_test.begin();
             it != vec1_test.end(); ++it) {
            *it = r.getInt(0, 10);
        }
        BOOST_REQUIRE(vec1 != vec1_test);

        vle::utils::generate(vec1_test.begin(), vec1_test.end(), gen1);
        BOOST_REQUIRE(vec1 != vec1_test);

        boost::uniform_int < > distrib3(0, 10);
        boost::variate_generator < boost::mt19937&,
            boost::uniform_int < > > gen3(r.gen(), distrib3);

        vle::utils::generate(vec1_test.begin(), vec1_test.end(), gen3);
        BOOST_REQUIRE(vec1 != vec1_test);
    }
}

BOOST_AUTO_TEST_CASE(test_normal)
{
    const std::size_t szmax(1000);
    std::vector < double > n1(szmax, 0), n2(szmax, 0);

    vle::utils::Rand r(123456789);
    boost::normal_distribution < >dist(1, 1);
    boost::variate_generator < boost::mt19937&,
        boost::normal_distribution < > > gen(r.gen(), dist);

    vle::utils::generate(n1.begin(), n1.end(), gen);

    for (std::vector < double >::iterator it = n2.begin();
         it != n2.end(); ++it) {
        *it = r.normal(1, 1);
    }

    BOOST_REQUIRE_CLOSE(std::accumulate(n1.begin(), n1.end(), 0.0) /
                        (double)szmax, 1.0, 10);
    BOOST_REQUIRE_CLOSE(std::accumulate(n2.begin(), n2.end(), 0.0) /
                        (double)szmax, 1.0, 10);
}

BOOST_AUTO_TEST_CASE(date_time)
{
    BOOST_REQUIRE_EQUAL(vle::utils::DateTime::year((2451545)),
			2000u);
    BOOST_REQUIRE_EQUAL(vle::utils::DateTime::month((2451545)),
			1u);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::dayOfMonth((2451545)), 1);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::dayOfYear((2451545)), 1);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::dayOfWeek((2451545)), 6);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::weekOfYear((2451547)), 1);

    BOOST_REQUIRE(vle::utils::DateTime::isLeapYear((2451545)));
    BOOST_REQUIRE(not vle::utils::DateTime::isLeapYear(
		      (2451911)));

    BOOST_REQUIRE(not vle::utils::DateTime::isValidYear(
		      (-1)));

    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aYear((2451545)), 366);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aMonth((2451545)), 31);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aMonth((2451576)), 29);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aMonth((2451942)), 28);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aWeek(), 7);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::aDay(), 1);

    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::years((2451545), 1), 366);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::months((2451545), 2), 60);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::weeks(2), 14);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::days(2), 2);

    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::convertUnit("year"),
        vle::utils::DATE_TIME_UNIT_YEAR);
    BOOST_REQUIRE_EQUAL(
	vle::utils::DateTime::convertUnit("month"),
	vle::utils::DATE_TIME_UNIT_MONTH);
    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::convertUnit("week"),
        vle::utils::DATE_TIME_UNIT_WEEK);
    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::convertUnit("day"),
        vle::utils::DATE_TIME_UNIT_DAY);

    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::duration(
            (2451545), 1, vle::utils::DATE_TIME_UNIT_YEAR), 366);
    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::duration(
            (2451545), 1, vle::utils::DATE_TIME_UNIT_MONTH), 31);
    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::duration(
            (2451545), 1, vle::utils::DATE_TIME_UNIT_WEEK), 7);
    BOOST_REQUIRE_EQUAL(
        vle::utils::DateTime::duration(
            (2451545), 1, vle::utils::DATE_TIME_UNIT_DAY), 1);
}

BOOST_AUTO_TEST_CASE(julian_date)
{
    BOOST_TEST_MESSAGE("\nJulian day number\n");
    BOOST_REQUIRE_EQUAL(2452192,
			vle::utils::DateTime::toJulianDayNumber("2001-10-9"));

    BOOST_TEST_MESSAGE("\nJulian day\n");
    BOOST_REQUIRE_EQUAL(vle::utils::DateTime::toJulianDay(2454115.05486),
                        vle::utils::DateTime::toJulianDay(
			    vle::utils::DateTime::toJulianDay(
                                vle::utils::DateTime::toJulianDay(
				    2454115.05486))));

    BOOST_REQUIRE_EQUAL(vle::utils::DateTime::toJulianDay(
			    vle::utils::DateTime::toJulianDay(2454115.05486)),
                        vle::utils::DateTime::toJulianDay(
			    vle::utils::DateTime::toJulianDay(
                                vle::utils::DateTime::toJulianDay(
				    vle::utils::DateTime::toJulianDay(
					2454115.05486)))));
}

BOOST_AUTO_TEST_CASE(to_time_function)
{
    long year, month, day, hours, minutes, seconds;
    double date = 2452192.191273148;

    vle::utils::DateTime::toTime(date, year, month, day, hours, minutes,
                                 seconds);

    BOOST_REQUIRE_EQUAL(year, 2001);
    BOOST_REQUIRE_EQUAL(month, 10);
    BOOST_REQUIRE_EQUAL(day, 9);
    BOOST_REQUIRE_EQUAL(hours, 4);
    BOOST_REQUIRE_EQUAL(minutes, 35);
    BOOST_REQUIRE_EQUAL(seconds, 26);
}

BOOST_AUTO_TEST_CASE(localized_conversion)
{
    namespace vu = vle::utils;

    /* convert a C locale real */
    BOOST_REQUIRE_CLOSE(vu::convert < double >("123456789"), 123456789., 0.1);
    BOOST_REQUIRE_CLOSE(vu::convert < double >("12.3456789"), 12.3456789, 0.1);
    BOOST_REQUIRE_CLOSE(vu::convert < double >("12345678.9"), 12345678., 0.1);
    BOOST_REQUIRE_CLOSE(vu::convert < double >("-12345e5"), -12345e5, 0.1);
    BOOST_REQUIRE_CLOSE(vu::convert < double >("12345e5"), 12345e5, 0.1);
    BOOST_REQUIRE_CLOSE(vu::convert < double >("12345."), 12345., 0.1);

    /* convert a fr_FR locale real */
    if (vu::isLocaleAvailable("fr_FR")) {
        BOOST_REQUIRE_CLOSE(vu::convert < double >("123 456 789", true,
                                                   "fr_FR"), 123456789., 0.1);
        BOOST_REQUIRE_CLOSE(vu::convert < double >("12,3456789", true,
                                                   "fr_FR"), 12.3456789, 0.1);
        BOOST_REQUIRE_CLOSE(vu::convert < double >("12345678,9", true,
                                                   "fr_FR"), 12345678., 0.1);
        BOOST_REQUIRE_CLOSE(vu::convert < double >("-12345,0e5", true,
                                                   "fr_FR"), -12345e5, 0.1);
        BOOST_REQUIRE_CLOSE(vu::convert < double >("12345,0e5", true, "fr_FR"),
                            12345e5, 0.1);
        BOOST_REQUIRE_CLOSE(vu::convert < double >("12345,", true, "fr_FR"),
                            12345., 0.1);
    }
}

BOOST_AUTO_TEST_CASE(to_scientific_string_function)
{
    namespace vu = vle::utils;

    BOOST_REQUIRE_EQUAL(vu::toScientificString(0.0),"0");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(-1504),"-1504");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(3022),"3022");
#ifdef BOOST_WINDOWS
    BOOST_REQUIRE_EQUAL(vu::toScientificString(123456789123456789.0),
                        "1.23456789123457e+017");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(0.00000000000000000000982),
                        "9.82e-021");
#else
    BOOST_REQUIRE_EQUAL(vu::toScientificString(123456789123456789.0),
                        "1.23456789123457e+17");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(0.00000000000000000000982),
                        "9.82e-21");
#endif
    BOOST_REQUIRE_EQUAL(vu::toScientificString(-0.12345),
                        "-0.12345");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(0.12345),
                        "0.12345");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(0.0001),
                        "0.0001");
    BOOST_REQUIRE_EQUAL(vu::toScientificString(1000.0001),
                        "1000.0001");
}

BOOST_AUTO_TEST_CASE(test_format_copy)
{
    namespace vu = vle::utils;

    std::ostringstream out;
    int myints[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vu::formatCopy(myints, myints + 9, out, ", ", ";", "\"");

    BOOST_REQUIRE_EQUAL(out.str(),
                        "\"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", "
                        "\"8\", \"9\";");
}
