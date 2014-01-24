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
#define BOOST_TEST_MODULE values_simple_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <functional>
#include <vle/value/Value.hpp>
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
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(check_simple_value)
{
    {
        value::Boolean* b = new value::Boolean(true);
        BOOST_REQUIRE_EQUAL(b->value(), true);
        b->set(false);
        BOOST_REQUIRE_EQUAL(b->value(), false);
        delete b;
    }

    {
        value::Double* d = value::Double::create(12.34);
        BOOST_REQUIRE_CLOSE(d->value(), 12.34, 1e-10);
        d->set(43.21);
        BOOST_REQUIRE_CLOSE(d->value(), 43.21, 1e-10);
        delete d;
    }

    {
        value::Integer* i = value::Integer::create(1234);
        BOOST_REQUIRE_EQUAL(i->value(), 1234);
        i->set(4321);
        BOOST_REQUIRE_EQUAL(i->value(), 4321);
        delete i;
    }

    {
        value::String* s = value::String::create("1234");
        BOOST_REQUIRE_EQUAL(s->value(), "1234");
        s->set("4321");
        BOOST_REQUIRE_EQUAL(s->value(), "4321");
        delete s;
    }

    {
        value::Xml* x = value::Xml::create("test");
        BOOST_REQUIRE_EQUAL(x->value(), "test");
        x->set("tset");
        BOOST_REQUIRE_EQUAL(x->value(), "tset");
        delete x;
    }
}

BOOST_AUTO_TEST_CASE(check_map_value)
{
    value::Map* mp = value::Map::create();

    BOOST_REQUIRE_EQUAL(mp->empty(), true);
    mp->addBoolean("boolean", true);
    mp->addInt("integer", 1234);
    mp->addDouble("double", 12.34);
    mp->addString("string", "test");
    mp->addXml("xml", "xml test");
    BOOST_REQUIRE_EQUAL(mp->empty(), false);

    BOOST_REQUIRE_EQUAL(mp->getBoolean("boolean"), true);
    BOOST_REQUIRE_EQUAL(mp->getInt("integer"), 1234);
    BOOST_REQUIRE_CLOSE(mp->getDouble("double"), 12.34, 1e-10);
    BOOST_REQUIRE_EQUAL(mp->getString("string"), "test");
    BOOST_REQUIRE_EQUAL(mp->getXml("xml"), "xml test");

    BOOST_REQUIRE_THROW(mp->getInt("boolean"), utils::CastError);
    BOOST_REQUIRE_NO_THROW(mp->getInt("integer"));
    BOOST_REQUIRE_THROW(mp->getInt("double"), utils::CastError);
    BOOST_REQUIRE_THROW(mp->getInt("string"), utils::CastError);
    BOOST_REQUIRE_THROW(mp->getInt("xml"), utils::CastError);

    delete(mp);
}

BOOST_AUTO_TEST_CASE(check_set_value)
{
    value::Set* st = value::Set::create();

    BOOST_REQUIRE_EQUAL(st->empty(), true);
    st->addBoolean(true);
    st->addInt(1234);
    st->addDouble(12.34);
    st->addString("test");
    st->addXml("xml test");
    BOOST_REQUIRE_EQUAL(st->empty(), false);

    BOOST_REQUIRE_EQUAL(st->getBoolean(0), true);
    BOOST_REQUIRE_EQUAL(st->getInt(1), 1234);
    BOOST_REQUIRE_CLOSE(st->getDouble(2), 12.34, 1e-10);
    BOOST_REQUIRE_EQUAL(st->getString(3), "test");
    BOOST_REQUIRE_EQUAL(st->getXml(4), "xml test");

    BOOST_REQUIRE_THROW(st->getInt(0), utils::CastError);
    BOOST_REQUIRE_NO_THROW(st->getInt(1));
    BOOST_REQUIRE_THROW(st->getInt(2), utils::CastError);
    BOOST_REQUIRE_THROW(st->getInt(3), utils::CastError);
    BOOST_REQUIRE_THROW(st->getInt(4), utils::CastError);

    delete(st);
}

BOOST_AUTO_TEST_CASE(check_clone)
{
    value::Map* mp = value::Map::create();
    mp->add("x1", value::String::create("toto"));
    mp->add("x2", value::String::create("toto"));
    mp->add("x3", value::String::create("toto"));

    value::Map* mpclone = dynamic_cast < value::Map* >(mp->clone());
    BOOST_REQUIRE(mp->get("x1") != mpclone->get("x1"));
    BOOST_REQUIRE(mp->get("x2") != mpclone->get("x2"));
    BOOST_REQUIRE(mp->get("x3") != mpclone->get("x3"));

    value::Set* st = value::Set::create();
    st->add(value::String::create("toto"));
    st->add(value::String::create("toto"));
    st->add(value::String::create("toto"));

    value::Set* stclone = dynamic_cast < value::Set* >(st->clone());

    BOOST_REQUIRE_EQUAL(st->size(), stclone->size());

    BOOST_REQUIRE(st->get(0) != stclone->get(0));
    BOOST_REQUIRE(st->get(1) != stclone->get(1));
    BOOST_REQUIRE(st->get(2) != stclone->get(2));

    delete(mp);
    delete(mpclone);
    delete(st);
    delete(stclone);
}

BOOST_AUTO_TEST_CASE(check_null)
{
    value::Set* st = value::Set::create();
    st->addString("toto1");
    st->addNull();
    st->addString("toto2");
    st->addNull();
    st->addString("toto3");
    st->addNull();
    st->addString("toto4");
    st->addNull();
    st->addString("toto5");
    st->addNull();

    BOOST_REQUIRE_EQUAL(st->size(), (value::VectorValue::size_type) 10);

    value::VectorValue::iterator it = st->value().begin();
    while ((it = std::find_if(it, st->value().end(), value::IsNullValue()))
            != st->value().end()) {
        delete(*it);
        *it = 0;
    }
    it = std::remove_if(st->value().begin(), st->value().end(),
                        std::bind2nd(std::equal_to <value::Value*>(), 0));

    st->value().erase(it, st->value().end());

    BOOST_REQUIRE_EQUAL(st->size(), (value::VectorValue::size_type) 5);

    delete(st);
}

BOOST_AUTO_TEST_CASE(check_matrix)
{
    value::Matrix* mx = value::Matrix::create(100, 100, 10, 10);
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::Matrix::size_type)100);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::Matrix::size_type)100);

    mx->addColumn();
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::Matrix::size_type)100);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::Matrix::size_type)101);

    mx->addRow();
    BOOST_REQUIRE_EQUAL(mx->rows(), (value::Matrix::size_type)101);
    BOOST_REQUIRE_EQUAL(mx->columns(), (value::Matrix::size_type)101);

    mx->add(0, 0, value::Integer::create(10));
    BOOST_REQUIRE_EQUAL(mx->value()[0][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(value::toInteger(*mx->value()[0][0]), 10);

    value::Matrix* cpy = dynamic_cast < value::Matrix* >(mx->clone());
    BOOST_REQUIRE_EQUAL(cpy->value()[0][0]->isInteger(), true);
    value::toIntegerValue(*cpy->value()[0][0]).set(20);
    BOOST_REQUIRE_EQUAL(value::toInteger(*mx->value()[0][0]), 10);
    BOOST_REQUIRE_EQUAL(value::toInteger(*cpy->value()[0][0]), 20);

    delete(mx);
    delete(cpy);
}

namespace test {

    class MyData : public vle::value::User
    {
    public:
        double x, y, z;
        std::string name;

        MyData()
            : vle::value::User(), x(0.0), y(0.0), z(0.0)
        {}

        MyData(double x, double y, double z, const std::string &name)
            : vle::value::User(), x(x), y(y), z(z), name(name)
        {}

        MyData(const MyData &value)
            : vle::value::User(value), x(value.x), y(value.y), z(value.z),
            name(value.name)
        {}

        virtual ~MyData() {}

        virtual size_t id() const { return 15978462u; }

        virtual Value* clone() const { return new MyData(*this); }

        virtual void writeFile(std::ostream& out) const
        {
            out << "(" << name << ": " << x << ", " << y <<  ", " << z << ")";
        }

        virtual void writeString(std::ostream& out) const
        {
            writeFile(out);
        }

        virtual void writeXml(std::ostream& out) const
        {
            out << "<set><string>" << name << "</string><double>" << x <<
                "</double><double>" << y << "</double> <double>" << z <<
                "</double></set>";
        }
    };
}

void check_user_value(vle::value::Value *to_check)
{
    BOOST_REQUIRE(to_check);
    BOOST_REQUIRE_EQUAL(to_check->getType(), vle::value::Value::USER);

    vle::value::User *user = vle::value::toUserValue(to_check);
    BOOST_REQUIRE(user);
    BOOST_REQUIRE_EQUAL(user->getType(), vle::value::Value::USER);
    BOOST_REQUIRE_EQUAL(user->id(), 15978462u);

    test::MyData *mydata = dynamic_cast <test::MyData*>(user);
    BOOST_REQUIRE(mydata);

    BOOST_REQUIRE_EQUAL(mydata->x, 1.);
    BOOST_REQUIRE_EQUAL(mydata->y, 2.);
    BOOST_REQUIRE_EQUAL(mydata->z, 3.);
    BOOST_REQUIRE_EQUAL(mydata->name, "test-vle");
}

BOOST_AUTO_TEST_CASE(test_user_value)
{
    vle::value::Value *data = new test::MyData(1., 2., 3., "test-vle");
    check_user_value(data);

    vle::value::Value *cloned_data = data->clone();
    check_user_value(cloned_data);

    BOOST_REQUIRE(data != cloned_data);

    delete data;
    delete cloned_data;
}
