/** 
 * @file test5.cpp
 * @brief Test unit of the sax value parser.
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE value_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/SaxVPZ.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <limits>
#include <fstream>

using namespace vle;

BOOST_AUTO_TEST_CASE(trame_parameter)
{
    const char* xml = 
        "<?xml version=\"1.0\"?>\n"
        "<vle_trame>\n"
        "<trame type=\"parameter\" date=\"0.33\" plugin=\"text\" location=\"tutu\" >\n"
        "<![CDATA["
        "empty"
        "]]>\n"
        "</trame>"
        "</vle_trame>";

    vpz::TrameList tr = vpz::Vpz::parse_trame(xml);
    BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);

    vpz::ParameterTrame* ptr = dynamic_cast < vpz::ParameterTrame* >(tr.front());
    BOOST_CHECK(ptr);
    BOOST_REQUIRE_EQUAL(ptr->time(), "0.33");
    BOOST_REQUIRE_EQUAL(ptr->data(), "empty");
    BOOST_REQUIRE_EQUAL(ptr->plugin(), "text");
    BOOST_REQUIRE_EQUAL(ptr->location(), "tutu");
}

BOOST_AUTO_TEST_CASE(trame_addobservable)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_trame>\n"
        "<trame type=\"new\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
        " view=\"view1\" />"
        "</vle_trame>";

    vpz::TrameList tr = vpz::Vpz::parse_trame(xml);
    BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);

    vpz::NewObservableTrame* ptr = dynamic_cast < vpz::NewObservableTrame*>(tr.front());
    BOOST_CHECK(ptr);
    BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    BOOST_REQUIRE_EQUAL(ptr->name(), "mdl");
    BOOST_REQUIRE_EQUAL(ptr->parent(), "");
    BOOST_REQUIRE_EQUAL(ptr->port(), "x");
    BOOST_REQUIRE_EQUAL(ptr->view(), "view1");
}

BOOST_AUTO_TEST_CASE(trame_delobservable)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_trame>"
        "<trame type=\"del\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
        " view=\"view1\" />"
        "</vle_trame>";

    vpz::TrameList tr = vpz::Vpz::parse_trame(xml);
    BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);

    vpz::DelObservableTrame* ptr = dynamic_cast < vpz::DelObservableTrame* >(tr.front());
    BOOST_CHECK(ptr);
    BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    BOOST_REQUIRE_EQUAL(ptr->name(), "mdl");
    BOOST_REQUIRE_EQUAL(ptr->parent(), "");
    BOOST_REQUIRE_EQUAL(ptr->port(), "x");
    BOOST_REQUIRE_EQUAL(ptr->view(), "view1");
}

BOOST_AUTO_TEST_CASE(trame_value)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_trame>\n"
        "<trame type=\"value\" date=\".33\" >"
        " <modeltrame name=\"n1\" parent=\"p1\" port=\"port\" view=\"view1\" >"
        "  <set>"
        "   <integer>1</integer>"
        "   <set>"
        "    <integer>2</integer>"
        "    <integer>3</integer>"
        "    <integer>4</integer>"
        "   </set>"
        "  </set>"
        " </modeltrame>"
        "</trame>"
        "</vle_trame>";

    vpz::TrameList tr = vpz::Vpz::parse_trame(xml);
    BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);

    vpz::ValueTrame* ptr = dynamic_cast < vpz::ValueTrame* >(tr.front());
    BOOST_CHECK(ptr);
    BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    BOOST_REQUIRE_EQUAL(ptr->trames().size(), (unsigned int)1);

    const vpz::ModelTrame& r(ptr->trames().front());
    BOOST_REQUIRE_EQUAL(r.simulator(), "n1");
    BOOST_REQUIRE_EQUAL(r.parent(), "p1");
    BOOST_REQUIRE_EQUAL(r.port(), "port");
    BOOST_REQUIRE_EQUAL(r.view(), "view1");

    value::Value v = r.value();
    BOOST_CHECK(v->isSet());

    value::Set s1 = value::toSetValue(v);
    BOOST_REQUIRE_EQUAL(s1->size(), (unsigned int)2);
}
