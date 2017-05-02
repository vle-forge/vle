/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vle/utils/Exception.hpp>
#include <vle/utils/unit-test.hpp>
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
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>
#include <vle/vle.hpp>

using namespace vle;

void
check_simple_value()
{
    {
        auto b = std::unique_ptr<value::Boolean>(new value::Boolean(true));
        EnsuresEqual(b->value(), true);
        b->set(false);
        EnsuresEqual(b->value(), false);
    }

    {
        auto d = std::unique_ptr<value::Double>(new value::Double(12.34));
        EnsuresApproximatelyEqual(d->value(), 12.34, 1e-10);
        d->set(43.21);
        EnsuresApproximatelyEqual(d->value(), 43.21, 1e-10);
    }

    {
        auto i = std::unique_ptr<value::Integer>(new value::Integer(1234));
        EnsuresEqual(i->value(), 1234);
        i->set(4321);
        EnsuresEqual(i->value(), 4321);
    }

    {
        auto s = std::unique_ptr<value::String>(new value::String("1234"));
        EnsuresEqual(s->value(), "1234");
        s->set("4321");
        EnsuresEqual(s->value(), "4321");
    }

    {
        auto x = std::unique_ptr<value::Xml>(new value::Xml("test"));
        EnsuresEqual(x->value(), "test");
        x->set("tset");
        EnsuresEqual(x->value(), "tset");
    }
}

void
check_map_value()
{
    auto mp = std::unique_ptr<value::Map>(new value::Map());

    EnsuresEqual(mp->empty(), true);
    mp->addBoolean("boolean", true);
    mp->addInt("integer", 1234);
    mp->addDouble("double", 12.34);
    mp->addString("string", "test");
    mp->addXml("xml", "xml test");
    EnsuresEqual(mp->empty(), false);

    EnsuresEqual(mp->getBoolean("boolean"), true);
    EnsuresEqual(mp->getInt("integer"), 1234);
    EnsuresApproximatelyEqual(mp->getDouble("double"), 12.34, 1e-10);
    EnsuresEqual(mp->getString("string"), "test");
    EnsuresEqual(mp->getXml("xml"), "xml test");

    EnsuresThrow(mp->getInt("boolean"), utils::CastError);
    EnsuresNotThrow(mp->getInt("integer"), std::exception);
    EnsuresThrow(mp->getInt("double"), utils::CastError);
    EnsuresThrow(mp->getInt("string"), utils::CastError);
    EnsuresThrow(mp->getInt("xml"), utils::CastError);
}

void
check_set_value()
{
    auto st = std::unique_ptr<value::Set>(new value::Set());

    EnsuresEqual(st->empty(), true);
    st->addBoolean(true);
    st->addInt(1234);
    st->addDouble(12.34);
    st->addString("test");
    st->addXml("xml test");
    EnsuresEqual(st->empty(), false);

    EnsuresEqual(st->getBoolean(0), true);
    EnsuresEqual(st->getInt(1), 1234);
    EnsuresApproximatelyEqual(st->getDouble(2), 12.34, 1e-10);
    EnsuresEqual(st->getString(3), "test");
    EnsuresEqual(st->getXml(4), "xml test");

    EnsuresThrow(st->getInt(0), utils::CastError);
    EnsuresNotThrow(st->getInt(1), std::exception);
    EnsuresThrow(st->getInt(2), utils::CastError);
    EnsuresThrow(st->getInt(3), utils::CastError);
    EnsuresThrow(st->getInt(4), utils::CastError);
}

void
check_clone()
{
    std::unique_ptr<value::Value> clone;

    {
        auto mp = std::unique_ptr<value::Map>(new value::Map());
        mp->add("x1", value::String::create("toto"));
        mp->add("x2", value::String::create("toto"));
        mp->add("x3", value::String::create("toto"));

        clone = mp->clone();
        auto mpclone = clone->toMap();

        Ensures(mp->get("x1") != mpclone.get("x1"));
        Ensures(mp->get("x2") != mpclone.get("x2"));
        Ensures(mp->get("x3") != mpclone.get("x3"));
    }

    {
        auto st = std::unique_ptr<value::Set>(new value::Set());
        st->add(value::String::create("toto"));
        st->add(value::String::create("toto"));
        st->add(value::String::create("toto"));

        clone = st->clone();
        auto stclone = clone->toSet();

        EnsuresEqual(st->size(), stclone.size());
        Ensures(st->get(0) != stclone.get(0));
        Ensures(st->get(1) != stclone.get(1));
        Ensures(st->get(2) != stclone.get(2));
    }
}

void
check_null()
{
    auto st = std::unique_ptr<value::Set>(new value::Set());
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

    EnsuresEqual(st->size(), (value::VectorValue::size_type)10);

    auto it = st->value().begin();

    while ((it = std::find_if(it, st->value().end(), value::IsNullValue())) !=
           st->value().end()) {
        *it = std::unique_ptr<value::Value>();
    }

    it = std::remove_if(st->value().begin(),
                        st->value().end(),
                        [](const std::unique_ptr<value::Value>& value) {
                            return not value.get();
                        });

    st->value().erase(it, st->value().end());

    EnsuresEqual(st->size(), (value::VectorValue::size_type)5);
}

void
check_matrix()
{
    auto mx =
      std::unique_ptr<value::Matrix>(new value::Matrix(100, 100, 10, 10));
    EnsuresEqual(mx->rows(), (value::Matrix::size_type)100);
    EnsuresEqual(mx->columns(), (value::Matrix::size_type)100);

    mx->addColumn();
    EnsuresEqual(mx->rows(), (value::Matrix::size_type)100);
    EnsuresEqual(mx->columns(), (value::Matrix::size_type)101);

    mx->addRow();
    EnsuresEqual(mx->rows(), (value::Matrix::size_type)101);
    EnsuresEqual(mx->columns(), (value::Matrix::size_type)101);

    mx->add(0, 0, value::Integer::create(10));

    EnsuresEqual(mx->get(0, 0)->isInteger(), true);
    EnsuresEqual(mx->get(0, 0)->toInteger().value(), 10);

    auto cpy = std::unique_ptr<value::Matrix>(new value::Matrix(*mx));

    EnsuresEqual(cpy->get(0, 0)->isInteger(), true);

    cpy->get(0, 0)->toInteger().set(20);
    std::cout << cpy->writeToString() << '\n';

    EnsuresEqual(mx->get(0, 0)->toInteger().value(), 10);
    EnsuresEqual(cpy->get(0, 0)->toInteger().value(), 20);

    EnsuresEqual(cpy->rows(), 101);
    EnsuresEqual(cpy->columns(), 101);

    cpy->resize(3, 4);
    std::cout << cpy->writeToString() << '\n';

    EnsuresEqual(cpy->rows(), 4);
    EnsuresEqual(cpy->columns(), 3);

    cpy->resize(5, 3, value::Boolean::create(true));
    std::cout << cpy->writeToString() << '\n';

    EnsuresEqual(cpy->rows(), 3);
    EnsuresEqual(cpy->columns(), 5);
    Ensures(cpy->getBoolean(4, 2));

    cpy->resize(10, 10, value::Boolean::create(false));
    std::cout << cpy->writeToString() << '\n';
}

namespace test {

class MyData : public vle::value::User
{
public:
    double x, y, z;
    std::string name;

    MyData()
      : vle::value::User()
      , x(0.0)
      , y(0.0)
      , z(0.0)
    {
    }

    MyData(double x, double y, double z, std::string name)
      : vle::value::User()
      , x(x)
      , y(y)
      , z(z)
      , name(std::move(name))
    {
    }

    MyData(const MyData& value)
      : vle::value::User(value)
      , x(value.x)
      , y(value.y)
      , z(value.z)
      , name(value.name)
    {
    }

    virtual ~MyData()
    {
    }

    virtual size_t id() const override
    {
        return 15978462u;
    }

    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new MyData(*this));
    }

    virtual void writeFile(std::ostream& out) const override
    {
        out << "(" << name << ": " << x << ", " << y << ", " << z << ")";
    }

    virtual void writeString(std::ostream& out) const override
    {
        writeFile(out);
    }

    virtual void writeXml(std::ostream& out) const override
    {
        out << "<set><string>" << name << "</string><double>" << x
            << "</double><double>" << y << "</double> <double>" << z
            << "</double></set>";
    }
};
}

void
check_user_value(vle::value::Value& to_check)
{
    EnsuresEqual(to_check.getType(), vle::value::Value::USER);

    auto& user = to_check.toUser();
    EnsuresEqual(user.getType(), vle::value::Value::USER);
    EnsuresEqual(user.id(), 15978462u);

    test::MyData* mydata = dynamic_cast<test::MyData*>(&user);
    Ensures(mydata);

    if (not mydata)
        return;

    EnsuresEqual(mydata->x, 1.);
    EnsuresEqual(mydata->y, 2.);
    EnsuresEqual(mydata->z, 3.);
    EnsuresEqual(mydata->name, "test-vle");
}

void
test_user_value()
{
    auto data =
      std::unique_ptr<value::Value>(new test::MyData(1., 2., 3., "test-vle"));
    check_user_value(*data.get());

    std::unique_ptr<vle::value::Value> cloned_data = data->clone();
    check_user_value(*cloned_data.get());

    Ensures(data.get() != cloned_data.get());
}

void
test_tuple()
{
    value::Tuple t(4, 1.);

    for (auto& v : t.value())
        Ensures(v == 1.);

    t(0) = 2;
    Ensures(t.at(0) == t(0));
}

void
test_table()
{
    value::Table t(2, 3);

    {
        double value = 0;
        for (auto& v : t.value())
            v = value++;
    }

    Ensures(t(0, 0) == 0.);
    Ensures(t(1, 0) == 1.);
    Ensures(t(0, 1) == 2.);
    Ensures(t(1, 1) == 3.);
    Ensures(t(0, 2) == 4.);
    Ensures(t(1, 2) == 5.);

    t.resize(3, 4);

    Ensures(t(0, 0) == 0.);
    Ensures(t(1, 0) == 1.);
    Ensures(t(0, 1) == 2.);
    Ensures(t(1, 1) == 3.);
    Ensures(t(0, 2) == 4.);
    Ensures(t(1, 2) == 5.);

    t.resize(1, 3);

    Ensures(t(0, 0) == 0.);
    Ensures(t(0, 1) == 2.);
    Ensures(t(0, 2) == 4.);
}

int
main()
{
    vle::Init app;

    check_simple_value();
    check_map_value();
    check_set_value();
    check_clone();
    check_null();
    check_matrix();
    test_user_value();
    test_tuple();
    test_table();

    return unit_test::report_errors();
}
