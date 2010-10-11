/*
 * @file vle/value/test/test_serialization.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE values_serialization_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <sstream>
#include <functional>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>


struct F
{
    F() { vle::value::init(); }
    ~F() { vle::value::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(check_tuple_serialization)
{
    std::string save;

    {
        value::Tuple mp;

        for (double i = 0.; i < 100.0; ++i) {
            mp.add(i);
        }

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Tuple&)mp;
        save = out.str();
    }

    {
        value::Tuple mp;

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Tuple&)mp;

        BOOST_REQUIRE_EQUAL(mp.size(), (value::Tuple::size_type)100);

        for (double i = 0.; i < 100.; ++i) {
            BOOST_REQUIRE_EQUAL(mp[(int)i], (double)i);
        }
    }
}

BOOST_AUTO_TEST_CASE(check_table_serialization)
{
    std::string save;

    {
        value::Table mp(10, 5);
        double val = 0.;

        for (value::Table::index j = 0; j < mp.height(); ++j) {
            for (value::Table::index i = 0; i < mp.width(); ++i) {
                mp.get(i, j) = val++;
            }
        }

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Table&)mp;
        save = out.str();
    }

    {
        value::Table mp;

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Table&)mp;

        BOOST_REQUIRE_EQUAL(mp.width(), (value::Table::index)10);
        BOOST_REQUIRE_EQUAL(mp.height(), (value::Table::index)5);

        double val = 0.;

        for (value::Table::index j = 0; j < mp.height(); ++j) {
            for (value::Table::index i = 0; i < mp.width(); ++i) {
                BOOST_REQUIRE_EQUAL(mp.get(i, j), val);
                val++;
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(check_map_serialization)
{
    std::string save;

    {
        value::Map* mp = value::Map::create();
        mp->addBoolean("boolean", true);
        mp->addInt("integer", 1234);
        mp->addDouble("double", 12.34);
        mp->addString("string", "test");
        mp->addXml("xml", "xml test");

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Map&)*mp;
        save = out.str();

        delete mp;
    }

    {
        value::Map* mp = value::Map::create();

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Map&)*mp;

        BOOST_REQUIRE_EQUAL(mp->getBoolean("boolean"), true);
        BOOST_REQUIRE_EQUAL(mp->getInt("integer"), 1234);
        BOOST_REQUIRE_CLOSE(mp->getDouble("double"), 12.34, 1e-10);
        BOOST_REQUIRE_EQUAL(mp->getString("string"), "test");
        BOOST_REQUIRE_EQUAL(mp->getXml("xml"), "xml test");

        delete mp;
    }
}

BOOST_AUTO_TEST_CASE(check_set_serialization)
{
    std::string save;

    {
        value::Set* st = value::Set::create();
        st->addBoolean(true);
        st->addInt(1234);
        st->addDouble(12.34);
        st->addString("test");
        st->addXml("xml test");
        st->addNull();

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Set&)*st;
        save = out.str();

        delete st;
    }

    {
        value::Set* st = value::Set::create();

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Set&)*st;

        BOOST_REQUIRE_EQUAL(st->size(), (value::Set::size_type)6);
        BOOST_REQUIRE_EQUAL(st->getBoolean(0), true);
        BOOST_REQUIRE_EQUAL(st->getInt(1), 1234);
        BOOST_REQUIRE_CLOSE(st->getDouble(2), 12.34, 1e-10);
        BOOST_REQUIRE_EQUAL(st->getString(3), "test");
        BOOST_REQUIRE_EQUAL(st->getXml(4), "xml test");
        BOOST_REQUIRE_EQUAL(st->get(5)->getType(), value::Value::NIL);

        BOOST_REQUIRE_THROW(st->getInt(0), utils::CastError);
        BOOST_REQUIRE_NO_THROW(st->getInt(1));
        BOOST_REQUIRE_THROW(st->getInt(2), utils::CastError);
        BOOST_REQUIRE_THROW(st->getInt(3), utils::CastError);
        BOOST_REQUIRE_THROW(st->getInt(4), utils::CastError);

        delete st;
    }
}

BOOST_AUTO_TEST_CASE(check_matrix_serialization)
{
    std::string save;

    {
        value::Matrix* mx = value::Matrix::create(100, 100, 10, 10);
        mx->addColumn();
        mx->addRow();
        mx->add(0, 0, value::Integer::create(10));

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Matrix&)*mx;
        save = out.str();

        delete mx;
    }

    {
        value::Matrix* mx = value::Matrix::create();

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Matrix&)*mx;

        BOOST_REQUIRE_EQUAL(mx->rows(), (value::Matrix::size_type)101);
        BOOST_REQUIRE_EQUAL(mx->columns(), (value::Matrix::size_type)101);

        BOOST_REQUIRE_EQUAL(mx->value()[0][0]->isInteger(), true);
        BOOST_REQUIRE_EQUAL(value::toInteger(*mx->value()[0][0]), 10);

        delete mx;
    }
}

BOOST_AUTO_TEST_CASE(check_composite1_serialization)
{
    std::string save;

    {
        value::Map mp;

        mp.addString("test-1", "test 1");
        mp.addString("test-2", "test 2");
        mp.addString("test-3", "test 3");
        mp.addString("test-4", "test 4");

        value::Set* st = new value::Set();
        st->addString("test 5");
        st->addString("test 6");
        st->addString("test 7");
        st->addString("test 8");

        mp.add("copain", st);

        std::ostringstream out;
        boost::archive::text_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Map&)mp;
        save = out.str();
    }

    {
        value::Map mp;

        std::istringstream in(save);
        boost::archive::text_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Map&)mp;

        BOOST_REQUIRE_EQUAL(mp.getString("test-1"), "test 1");
        BOOST_REQUIRE_EQUAL(mp.getString("test-2"), "test 2");
        BOOST_REQUIRE_EQUAL(mp.getString("test-3"), "test 3");
        BOOST_REQUIRE_EQUAL(mp.getString("test-4"), "test 4");

        const value::Set& st(mp.getSet("copain"));

        BOOST_REQUIRE_EQUAL(st.size(), (value::Set::size_type)4);
        BOOST_REQUIRE_EQUAL(st.getString(0), "test 5");
        BOOST_REQUIRE_EQUAL(st.getString(1), "test 6");
        BOOST_REQUIRE_EQUAL(st.getString(2), "test 7");
        BOOST_REQUIRE_EQUAL(st.getString(3), "test 8");
    }
}

BOOST_AUTO_TEST_CASE(check_composite2_serialization)
{
    std::string save;

    {
        value::Set* st = value::Set::create();
        st->addBoolean(true);
        st->addInt(1234);
        st->addDouble(12.34);
        st->addString("test");
        st->addXml("xml test");
        st->addNull();

        std::ostringstream out(std::ostringstream::binary);
        boost::archive::binary_oarchive oa(out);
        value::Value::registerValues(oa);
        oa << (const value::Set&)*st;
        save = out.str();

        delete st;
    }

    {
        value::Set* st = value::Set::create();

        std::istringstream in(save, std::istringstream::binary);
        boost::archive::binary_iarchive ia(in);
        value::Value::registerValues(ia);
        ia >> (value::Set&)*st;

        BOOST_REQUIRE_EQUAL(st->size(), (value::Set::size_type)6);
        BOOST_REQUIRE_EQUAL(st->getBoolean(0), true);
        BOOST_REQUIRE_EQUAL(st->getInt(1), 1234);
        BOOST_REQUIRE_CLOSE(st->getDouble(2), 12.34, 1e-10);
        BOOST_REQUIRE_EQUAL(st->getString(3), "test");
        BOOST_REQUIRE_EQUAL(st->getXml(4), "xml test");
        BOOST_REQUIRE_EQUAL(st->get(5)->getType(), value::Value::NIL);

        BOOST_REQUIRE_THROW(st->getInt(0), utils::CastError);
        BOOST_REQUIRE_NO_THROW(st->getInt(1));
        BOOST_REQUIRE_THROW(st->getInt(2), utils::CastError);
        BOOST_REQUIRE_THROW(st->getInt(3), utils::CastError);
        BOOST_REQUIRE_THROW(st->getInt(4), utils::CastError);

        delete st;
    }
}

BOOST_AUTO_TEST_CASE(check_serialize_deserialize)
{
    std::string buffer;

    for (int i = 0; i < 10; ++i) {
        {
            value::Set* st = new value::Set;
            value::Map* mp = new value::Map;
            mp->addString("test", "test");
            mp->addDouble("toto", 123.321);
            mp->addBoolean("tutu", true);

            st->add(mp);
            st->add(new value::String("test"));

            value::Set::serializeBinaryBuffer(*st, buffer);
            delete st;
        }

        {
            value::Set* st = new value::Set;
            value::Set::deserializeBinaryBuffer(*st, buffer);

            BOOST_REQUIRE_EQUAL(st->size(), (value::Set::size_type)2);
            delete st;
        }
    }
}

BOOST_AUTO_TEST_CASE(check_serialize_deserialize2)
{
    std::string buffer;

    for (int i = 0; i < 10; ++i) {
        {
            value::Set* st = new value::Set;
            value::Map* mp = new value::Map;
            mp->addString("test", "test");
            mp->addDouble("toto", 123.321);
            mp->addBoolean("tutu", true);

            st->add(mp);
            st->add(new value::String("test"));

            value::Set::serializeBinaryBuffer(*st, buffer);
            delete st;
        }

        {
            value::Set* st = new value::Set;
            value::Set::deserializeBinaryBuffer(*st, buffer);

            BOOST_REQUIRE_EQUAL(st->size(), (value::Set::size_type)2);
            delete st;
        }
    }
}
