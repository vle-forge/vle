/**
 * @file src/vle/value/test/test1.cpp
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
#include <vle/value.hpp>
#include <vle/utils.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(check_simple_value)
{
    value::Boolean b = value::BooleanFactory::create(true);
    BOOST_REQUIRE_EQUAL(b->boolValue(), true);
    b->set(false);
    BOOST_REQUIRE_EQUAL(b->boolValue(), false);

    value::Double d = value::DoubleFactory::create(12.34);
    BOOST_REQUIRE_CLOSE(d->doubleValue(), 12.34, 1e-10);
    d->set(43.21);
    BOOST_REQUIRE_CLOSE(d->doubleValue(), 43.21, 1e-10);

    value::Integer i = value::IntegerFactory::create(1234);
    BOOST_REQUIRE_EQUAL(i->intValue(), 1234);
    i->set(4321);
    BOOST_REQUIRE_EQUAL(i->intValue(), 4321);

    value::String s = value::StringFactory::create("1234");
    BOOST_REQUIRE_EQUAL(s->stringValue(), "1234");
    s->set("4321");
    BOOST_REQUIRE_EQUAL(s->stringValue(), "4321");

    value::XML x = value::XMLFactory::create("test");
    BOOST_REQUIRE_EQUAL(x->stringValue(), "test");
    x->set("tset");
    BOOST_REQUIRE_EQUAL(x->stringValue(), "tset");
}

BOOST_AUTO_TEST_CASE(check_map_value)
{
    value::Map mp = value::MapFactory::create();
    mp->setBooleanValue("boolean", true);
    mp->setIntValue("integer", 1234);
    mp->setDoubleValue("double", 12.34);
    mp->setStringValue("string", "test");
    mp->setXMLValue("xml", "xml test");

    BOOST_REQUIRE_EQUAL(mp->getBooleanValue("boolean"), true);
    BOOST_REQUIRE_EQUAL(mp->getIntValue("integer"), 1234);
    BOOST_REQUIRE_CLOSE(mp->getDoubleValue("double"), 12.34, 1e-10);
    BOOST_REQUIRE_EQUAL(mp->getStringValue("string"), "test");
    BOOST_REQUIRE_EQUAL(mp->getXMLValue("xml"), "xml test");

    BOOST_REQUIRE_THROW(mp->getIntValue("boolean"), utils::ArgError);
    BOOST_REQUIRE_NO_THROW(mp->getIntValue("integer"));
    BOOST_REQUIRE_THROW(mp->getIntValue("double"), utils::ArgError);
    BOOST_REQUIRE_THROW(mp->getIntValue("string"), utils::ArgError);
    BOOST_REQUIRE_THROW(mp->getIntValue("xml"), utils::ArgError);
}

BOOST_AUTO_TEST_CASE(check_set_value)
{
    value::Set st = value::SetFactory::create();
    st->addBooleanValue(true);
    st->addIntValue(1234);
    st->addDoubleValue(12.34);
    st->addStringValue("test");
    st->addXMLValue("xml test");

    BOOST_REQUIRE_EQUAL(st->getBooleanValue(0), true);
    BOOST_REQUIRE_EQUAL(st->getIntValue(1), 1234);
    BOOST_REQUIRE_CLOSE(st->getDoubleValue(2), 12.34, 1e-10);
    BOOST_REQUIRE_EQUAL(st->getStringValue(3), "test");
    BOOST_REQUIRE_EQUAL(st->getXMLValue(4), "xml test");

    BOOST_REQUIRE_THROW(st->getIntValue(0), utils::ArgError);
    BOOST_REQUIRE_NO_THROW(st->getIntValue(1));
    BOOST_REQUIRE_THROW(st->getIntValue(2), utils::ArgError);
    BOOST_REQUIRE_THROW(st->getIntValue(3), utils::ArgError);
    BOOST_REQUIRE_THROW(st->getIntValue(4), utils::ArgError);
}

BOOST_AUTO_TEST_CASE(check_clone)
{
    value::Map mp = value::MapFactory::create();
    mp->addValue("x1", value::StringFactory::create("toto"));
    mp->addValue("x2", value::StringFactory::create("toto"));
    mp->addValue("x3", value::StringFactory::create("toto"));

    value::Map mpclone = value::toMapValue(mp->clone());
    BOOST_REQUIRE(mp->getValue("x1").get() != mpclone->getValue("x1").get());
    BOOST_REQUIRE(mp->getValue("x2").get() != mpclone->getValue("x2").get());
    BOOST_REQUIRE(mp->getValue("x3").get() != mpclone->getValue("x3").get());

    value::Set st = value::SetFactory::create();
    st->addValue(value::StringFactory::create("toto"));
    st->addValue(value::StringFactory::create("toto"));
    st->addValue(value::StringFactory::create("toto"));

    value::Set stclone = value::toSetValue(st->clone());

    BOOST_REQUIRE_EQUAL(st->size(), stclone->size());

    BOOST_REQUIRE(st->getValue(0).get() != stclone->getValue(0).get());
    BOOST_REQUIRE(st->getValue(1).get() != stclone->getValue(1).get());
    BOOST_REQUIRE(st->getValue(2).get() != stclone->getValue(2).get());
}

BOOST_AUTO_TEST_CASE(check_null)
{
    value::Set st = value::SetFactory::create();
    st->addStringValue("toto1");
    st->addNullValue();
    st->addStringValue("toto2");
    st->addNullValue();
    st->addStringValue("toto3");
    st->addNullValue();
    st->addStringValue("toto4");
    st->addNullValue();
    st->addStringValue("toto5");
    st->addNullValue();

    BOOST_REQUIRE_EQUAL(st->size(), (value::VectorValue::size_type) 10);

    st->getValue().erase(
        std::remove_if(st->getValue().begin(),
                       st->getValue().end(),
                       value::IsNullValue()),
        st->getValue().end());

    BOOST_REQUIRE_EQUAL(st->size(), (value::VectorValue::size_type) 5);
}

BOOST_AUTO_TEST_CASE(check_matrix)
{
    value::Matrix mx = value::MatrixFactory::create(100, 100, 10, 10);
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::MatrixFactory::size_type)0);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::MatrixFactory::size_type)0);

    mx->addColumn();
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::MatrixFactory::size_type)0);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::MatrixFactory::size_type)1);

    mx->addRow();
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::MatrixFactory::size_type)1);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::MatrixFactory::size_type)1);

    mx->addValue(0, 0, value::IntegerFactory::create(10));
    BOOST_REQUIRE_EQUAL(mx->getValue()[0][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(value::toInteger(mx->getValue()[0][0]), 10);

    value::Matrix cpy = value::toMatrixValue(mx->clone());
    BOOST_REQUIRE_EQUAL(cpy->getValue()[0][0]->isInteger(), true);
    value::toIntegerValue(cpy->getValue()[0][0])->set(20);
    BOOST_REQUIRE_EQUAL(value::toInteger(mx->getValue()[0][0]), 10);
    BOOST_REQUIRE_EQUAL(value::toInteger(cpy->getValue()[0][0]), 20);



}
