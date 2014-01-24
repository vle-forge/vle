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
#define BOOST_TEST_MODULE value_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/SaxParser.hpp>
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
#include <vle/vle.hpp>
#include <limits>
#include <fstream>


struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(trame_parameter)
{
    //const char* xml =
    //"<?xml version=\"1.0\"?>\n"
    //"<vle_trame>\n"
    //"<trame type=\"parameter\" date=\"0.33\" plugin=\"text\" location=\"tutu\" >\n"
    //"<![CDATA["
    //"empty"
    //"]]>\n"
    //"</trame>"
    //"</vle_trame>";
    //
    //vpz::TrameList tr = vpz::Vpz::parseTrame(xml);
    //BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);
    //
    //vpz::ParameterTrame* ptr = dynamic_cast < vpz::ParameterTrame* >(tr.front());
    //BOOST_CHECK(ptr);
    //BOOST_REQUIRE_EQUAL(ptr->time(), "0.33");
    //BOOST_REQUIRE_EQUAL(ptr->data(), "empty");
    //BOOST_REQUIRE_EQUAL(ptr->plugin(), "text");
    //BOOST_REQUIRE_EQUAL(ptr->location(), "tutu");
}

BOOST_AUTO_TEST_CASE(trame_addobservable)
{
    //const char* xml =
    //"<?xml version=\"1.0\"?>\n"
    //"<vle_trame>\n"
    //"<trame type=\"new\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
    //" view=\"view1\" />"
    //"</vle_trame>";
    //
    //vpz::TrameList tr = vpz::Vpz::parseTrame(xml);
    //BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);
    //
    //vpz::NewObservableTrame* ptr = dynamic_cast < vpz::NewObservableTrame*>(tr.front());
    //BOOST_CHECK(ptr);
    //BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    //BOOST_REQUIRE_EQUAL(ptr->name(), "mdl");
    //BOOST_REQUIRE_EQUAL(ptr->parent(), "");
    //BOOST_REQUIRE_EQUAL(ptr->port(), "x");
    //BOOST_REQUIRE_EQUAL(ptr->view(), "view1");
}

BOOST_AUTO_TEST_CASE(trame_delobservable)
{
    //const char* xml =
    //"<?xml version=\"1.0\"?>\n"
    //"<vle_trame>"
    //"<trame type=\"del\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
    //" view=\"view1\" />"
    //"</vle_trame>";
    //
    //vpz::TrameList tr = vpz::Vpz::parseTrame(xml);
    //BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);
    //
    //vpz::DelObservableTrame* ptr = dynamic_cast < vpz::DelObservableTrame* >(tr.front());
    //BOOST_CHECK(ptr);
    //BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    //BOOST_REQUIRE_EQUAL(ptr->name(), "mdl");
    //BOOST_REQUIRE_EQUAL(ptr->parent(), "");
    //BOOST_REQUIRE_EQUAL(ptr->port(), "x");
    //BOOST_REQUIRE_EQUAL(ptr->view(), "view1");
}

BOOST_AUTO_TEST_CASE(trame_value)
{
    //const char* xml =
    //"<?xml version=\"1.0\"?>\n"
    //"<vle_trame>\n"
    //"<trame type=\"value\" date=\".33\" >"
    //" <modeltrame name=\"n1\" parent=\"p1\" port=\"port\" view=\"view1\" >"
    //"  <set>"
    //"   <integer>1</integer>"
    //"   <set>"
    //"    <integer>2</integer>"
    //"    <integer>3</integer>"
    //"    <integer>4</integer>"
    //"   </set>"
    //"  </set>"
    //" </modeltrame>"
    //" <modeltrame name=\"n2\" parent=\"p1\" port=\"port\" view=\"view1\" >"
    //"  <set>"
    //"   <integer>5</integer>"
    //"   <set>"
    //"    <integer>6</integer>"
    //"    <integer>7</integer>"
    //"    <integer>8</integer>"
    //"   </set>"
    //"  </set>"
    //" </modeltrame>"
    //"</trame>"
    //"</vle_trame>";
    //
    //vpz::TrameList tr = vpz::Vpz::parseTrame(xml);
    //BOOST_REQUIRE_EQUAL(tr.size(), (vpz::TrameList::size_type)1);
    //
    //vpz::ValueTrame* ptr = dynamic_cast < vpz::ValueTrame* >(tr.front());
    //BOOST_CHECK(ptr);
    //BOOST_REQUIRE_EQUAL(ptr->time(), ".33");
    //BOOST_REQUIRE_EQUAL(ptr->trames().size(),
    //(vpz::ModelTrameList::size_type)2);
    //
    //vpz::ModelTrameList::const_iterator it = ptr->trames().begin();
    //{
    //const vpz::ModelTrame& r(*it);
    //BOOST_REQUIRE_EQUAL(r.simulator(), "n1");
    //BOOST_REQUIRE_EQUAL(r.parent(), "p1");
    //BOOST_REQUIRE_EQUAL(r.port(), "port");
    //BOOST_REQUIRE_EQUAL(r.view(), "view1");
    //
    //const value::Value* v = r.value();
    //BOOST_CHECK(v->isSet());
    //const value::Set* s1 = value::toSetValue(v);
    //BOOST_REQUIRE_EQUAL(s1->size(), (unsigned int)2);
    //const value::Value& v1(s1->get(0));
    //BOOST_CHECK(v1.isInteger());
    //BOOST_REQUIRE_EQUAL(value::toInteger(v1), 1);
    //const value::Set& s2 = value::toSetValue(s1->get(1));
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(0)), 2);
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(1)), 3);
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(2)), 4);
    //}
    //it++;
    //{
    //const vpz::ModelTrame& r(*it);
    //BOOST_REQUIRE_EQUAL(r.simulator(), "n2");
    //BOOST_REQUIRE_EQUAL(r.parent(), "p1");
    //BOOST_REQUIRE_EQUAL(r.port(), "port");
    //BOOST_REQUIRE_EQUAL(r.view(), "view1");
    //
    //const value::Value* v = r.value();
    //BOOST_CHECK(v->isSet());
    //const value::Set* s1(value::toSetValue(v));
    //BOOST_REQUIRE_EQUAL(s1->size(), (unsigned int)2);
    //const value::Value& v1(s1->get(0));
    //BOOST_CHECK(v1.isInteger());
    //BOOST_REQUIRE_EQUAL(value::toInteger(v1), 5);
    //const value::Set& s2(value::toSetValue(s1->get(1)));
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(0)), 6);
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(1)), 7);
    //BOOST_REQUIRE_EQUAL(value::toInteger(s2.get(2)), 8);
    //}
}
