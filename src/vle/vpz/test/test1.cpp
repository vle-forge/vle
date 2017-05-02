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
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vle/utils/Tools.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;

void
value_bool()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<boolean>true</boolean>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<boolean>false</boolean>";
    const char* t3 = "<?xml version=\"1.0\"?>\n<boolean>1</boolean>";
    const char* t4 = "<?xml version=\"1.0\"?>\n<boolean>0</boolean>";

    std::shared_ptr<value::Value> v;

    v = vpz::Vpz::parseValue(t1);
    Ensures(v->toBoolean().value() == true);

    v = vpz::Vpz::parseValue(t2);
    Ensures(v->toBoolean().value() == false);

    v = vpz::Vpz::parseValue(t3);
    Ensures(v->toBoolean().value() == true);

    v = vpz::Vpz::parseValue(t4);
    Ensures(v->toBoolean().value() == false);

    v = vpz::Vpz::parseValue(v->writeToXml());
    Ensures(v->toBoolean().value() == false);
}

void
value_integer()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<integer>100</integer>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<integer>-100</integer>";
    const size_t bufferSize = 1000;
    char t3[bufferSize];
    char t4[bufferSize];

    snprintf(t3,
             bufferSize,
             "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
             utils::to<int32_t>(std::numeric_limits<int32_t>::max()).c_str());
    snprintf(t4,
             bufferSize,
             "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
             utils::to<int32_t>(std::numeric_limits<int32_t>::min()).c_str());

    std::shared_ptr<value::Value> v;

    v = vpz::Vpz::parseValue(t1);
    Ensures(v->toInteger().value() == 100);

    v = vpz::Vpz::parseValue(t2);
    Ensures(v->toInteger().value() == -100);

    v = vpz::Vpz::parseValue(t3);
    EnsuresEqual(v->toInteger().value(), std::numeric_limits<int32_t>::max());

    v = vpz::Vpz::parseValue(t4);

    EnsuresEqual(v->toInteger().value(), std::numeric_limits<int32_t>::min());
    std::string t5(v->writeToXml());

    v = vpz::Vpz::parseValue(t5);
    EnsuresEqual(v->toInteger().value(), std::numeric_limits<int32_t>::min());
}

void
value_double()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<double>100.5</double>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<double>-100.5</double>";

    std::shared_ptr<value::Value> v;

    v = vpz::Vpz::parseValue(t1);
    EnsuresApproximatelyEqual(v->toDouble().value(), 100.5, 1);

    v = vpz::Vpz::parseValue(t2);
    EnsuresApproximatelyEqual(v->toDouble().value(), -100.5, 1);
    std::string t3(v->writeToXml());

    v = vpz::Vpz::parseValue(t3);
    EnsuresApproximatelyEqual(v->toDouble().value(), -100.5, 1);
}

void
value_string()
{
    const char* t1 =
      "<?xml version=\"1.0\"?>\n<string>a b c d e f g h i j</string>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<string>a\nb\tc\n</string>";
    const char* t4 =
      "<?xml version=\"1.0\"?>\n<string>é ç € â ô f Û « © ±</string>";

    std::shared_ptr<value::Value> v;

    v = vpz::Vpz::parseValue(t1);
    Ensures(v->toString().value() == "a b c d e f g h i j");

    v = vpz::Vpz::parseValue(t2);
    Ensures(v->toString().value() == "a\nb\tc\n");
    std::string t3(v->writeToXml());
    v = vpz::Vpz::parseValue(t3);
    Ensures(v->toString().value() == "a\nb\tc\n");

    v = vpz::Vpz::parseValue(t4);
    Ensures(v->toString().value() == "é ç € â ô f Û « © ±");
}

void
value_set()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<set>\n"
                     "<integer>1</integer>\n"
                     "<string>test</string>\n"
                     "</set>";

    const char* t2 = "<?xml version=\"1.0\"?>\n"
                     "<set>\n"
                     "  <integer>1</integer>\n"
                     "  <set>\n"
                     "    <string>test</string>\n"
                     "  </set>\n"
                     "</set>";

    {
        auto ptr = vpz::Vpz::parseValue(t1);
        const auto& v = ptr->toSet();

        Ensures(v.getString(1) == "test");
        Ensures(v.getInt(0) == 1);
    }

    std::string t3;

    {
        auto ptr = vpz::Vpz::parseValue(t2);

        std::cout << ptr->writeToXml() << '\n';

        const auto& v = ptr->toSet();
        Ensures(value::toInteger(v.get(0)) == 1);

        {
            const auto& v2 = v.get(1)->toSet();
            Ensures(v2.size() == 1);
            Ensures(v2.get(0)->toString().value() == "test");
        }

        t3 = v.writeToXml();
    }

    auto ptr = vpz::Vpz::parseValue(t3);
    const auto& v = ptr->toSet();

    Ensures(v.get(0)->toInteger().value() == 1);
    {
        const value::Set& v2 = v.get(1)->toSet();
        Ensures(value::toString(v2.get(0)) == "test");
    }
}

void
value_map()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<map>\n"
                     " <key name=\"a\">\n"
                     "  <integer>10</integer>\n"
                     " </key>\n"
                     "</map>\n";

    const char* t2 = "<?xml version=\"1.0\"?>\n"
                     "<map>\n"
                     " <key name=\"a\">\n"
                     "  <set>\n"
                     "   <integer>1</integer>\n"
                     "   <string>test</string>\n"
                     "  </set>\n"
                     " </key>\n"
                     "</map>\n";

    {
        auto v = vpz::Vpz::parseValue(t1)->toMap();
        Ensures(v.getInt("a") == 10);
    }

    std::string t3;

    {
        auto v = vpz::Vpz::parseValue(t2)->toMap();
        t3 = (v.writeToXml());
        {
            auto s = v.getSet("a");
            Ensures(s.get(0)->toInteger().value() == 1);
            Ensures(s.get(1)->toString().value() == "test");
        }
    }

    {
        auto v = vpz::Vpz::parseValue(t3)->toMap();
        {
            auto s = v.getSet("a");
            Ensures(s.get(0)->toInteger().value() == 1);
            Ensures(s.get(1)->toString().value() == "test");
        }
    }
}

void
value_tuple()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<tuple>1 2 3</tuple>\n";

    auto ptr = vpz::Vpz::parseValue(t1);
    auto v = ptr->toTuple();
    EnsuresEqual(v.size(), (size_t)3);
    EnsuresApproximatelyEqual(v[0], 1.0, 0.1);
    EnsuresApproximatelyEqual(v[1], 2.0, 0.1);
    EnsuresApproximatelyEqual(v[2], 3.0, 0.1);

    const char* t2 = "<?xml version=\"1.0\"?>\n"
                     "<map>\n"
                     "   <key name=\"testtest\">\n"
                     "      <tuple>100 200 300</tuple>\n"
                     "   </key>\n"
                     "</map>\n";

    auto ptr_m = vpz::Vpz::parseValue(t2);
    auto m = ptr_m->toMap();

    auto t = m.getTuple("testtest");
    value::Tuple& v2(toTupleValue(t));
    EnsuresApproximatelyEqual(v2.operator[](0), 100.0, 0.1);
    EnsuresApproximatelyEqual(v2.operator[](1), 200.0, 0.1);
    EnsuresApproximatelyEqual(v2.operator[](2), 300.0, 0.1);
    std::string t3 = v2.writeToXml();

    auto ptr_v3 = vpz::Vpz::parseValue(t3);
    auto v3 = ptr_v3->toTuple();
    EnsuresApproximatelyEqual(v3.operator[](0), 100.0, 0.1);
    EnsuresApproximatelyEqual(v3.operator[](1), 200.0, 0.1);
    EnsuresApproximatelyEqual(v3.operator[](2), 300.0, 0.1);
}

void
value_table()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<table width=\"2\" height=\"3\">\n"
                     "1 2 3 4 5 6"
                     "</table>\n";

    auto ptr = vpz::Vpz::parseValue(t1);
    auto v = ptr->toTable();
    EnsuresEqual(v.width(), (value::Table::index)2);
    EnsuresEqual(v.height(), (value::Table::index)3);
    EnsuresApproximatelyEqual(v.get(0, 0), 1.0, 0.1);
    EnsuresApproximatelyEqual(v.get(0, 1), 3.0, 0.1);
    EnsuresApproximatelyEqual(v.get(0, 2), 5.0, 0.1);
    EnsuresApproximatelyEqual(v.get(1, 0), 2.0, 0.1);
    EnsuresApproximatelyEqual(v.get(1, 1), 4.0, 0.1);
    EnsuresApproximatelyEqual(v.get(1, 2), 6.0, 0.1);

    EnsuresEqual(v.writeToString(), "((1,2),(3,4),(5,6))");

    std::string result("<?xml version=\"1.0\"?>\n");
    result += v.writeToXml();

    auto ptrw = vpz::Vpz::parseValue(result);
    auto w = ptrw->toTable();
    EnsuresEqual(w.width(), (value::Table::index)2);
    EnsuresEqual(w.height(), (value::Table::index)3);
    EnsuresApproximatelyEqual(w.get(0, 0), 1.0, 0.1);
    EnsuresApproximatelyEqual(w.get(0, 1), 3.0, 0.1);
    EnsuresApproximatelyEqual(w.get(0, 2), 5.0, 0.1);
    EnsuresApproximatelyEqual(w.get(1, 0), 2.0, 0.1);
    EnsuresApproximatelyEqual(w.get(1, 1), 4.0, 0.1);
    EnsuresApproximatelyEqual(w.get(1, 2), 6.0, 0.1);
}

void
value_table_map()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<map>"
                     "<key name=\"a\"><double>0.1</double></key>"
                     "<key name=\"tr\">"
                     "<table width=\"2\" height=\"3\">\n"
                     "1 2 3 4 5 6"
                     "</table>\n"
                     "</key>"
                     "<key name=\"c\"><double>0.1</double></key>"
                     "</map>";

    const char* t2 = "<?xml version=\"1.0\"?>\n"
                     "<map>"
                     "<key name=\"a\"><double>0.1</double></key>"
                     "<key name=\"tr\">"
                     "<table width=\"2\" height=\"3\">\n"
                     "1 2 3 4 5 6"
                     "</table>\n"
                     "</key>"
                     "</map>";

    const char* t3 = "<?xml version=\"1.0\"?>\n"
                     "<map>"
                     "<key name=\"tr\">"
                     "<table width=\"2\" height=\"3\">\n"
                     "1 2 3 4 5 6"
                     "</table>\n"
                     "</key>"
                     "<key name=\"c\"><double>0.1</double></key>"
                     "</map>";

    {
        auto ptr = vpz::Vpz::parseValue(t1);
        auto m = ptr->toMap();
        auto w = m.getTable("tr");
        EnsuresEqual(w.width(), (value::Table::index)2);
        EnsuresEqual(w.height(), (value::Table::index)3);
        EnsuresApproximatelyEqual(w.get(0, 0), 1.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 1), 3.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 2), 5.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 0), 2.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 1), 4.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 2), 6.0, 0.1);
    }

    {
        auto ptr = vpz::Vpz::parseValue(t2);
        auto m = ptr->toMap();
        auto w = m.getTable("tr");
        EnsuresEqual(w.width(), (value::Table::index)2);
        EnsuresEqual(w.height(), (value::Table::index)3);
        EnsuresApproximatelyEqual(w.get(0, 0), 1.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 1), 3.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 2), 5.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 0), 2.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 1), 4.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 2), 6.0, 0.1);
    }

    {
        auto ptr = vpz::Vpz::parseValue(t3);
        auto m = ptr->toMap();
        auto w = m.getTable("tr");
        EnsuresEqual(w.width(), (value::Table::index)2);
        EnsuresEqual(w.height(), (value::Table::index)3);
        EnsuresApproximatelyEqual(w.get(0, 0), 1.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 1), 3.0, 0.1);
        EnsuresApproximatelyEqual(w.get(0, 2), 5.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 0), 2.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 1), 4.0, 0.1);
        EnsuresApproximatelyEqual(w.get(1, 2), 6.0, 0.1);
    }
}

void
value_xml()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
                     "<xml>"
                     "<![CDATA[test 1 2 1 2]]>\n"
                     "</xml>";

    auto ptr = vpz::Vpz::parseValue(t1);
    auto str = ptr->toXml();

    std::string t2(str.writeToXml());
    EnsuresEqual(str.value(), "test 1 2 1 2");

    auto ptr2 = vpz::Vpz::parseValue(t2);
    auto str2 = ptr2->toXml();
    EnsuresEqual(str2.value(), "test 1 2 1 2");
}

void
value_matrix()
{
    const char* t1 =
      "<?xml version=\"1.0\"?>\n"
      "<matrix rows=\"3\" columns=\"2\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>1</integer>"
      "<integer>2</integer>"
      "<integer>3</integer>"
      "<integer>4</integer>"
      "<integer>5</integer>"
      "<integer>6</integer>"
      "</matrix>";

    auto ptr = vpz::Vpz::parseValue(t1);
    EnsuresEqual(ptr->isMatrix(), true);

    auto& m = ptr->toMatrix();

    EnsuresEqual(m.rows(), (value::Matrix::size_type)3);
    EnsuresEqual(m.columns(), (value::Matrix::size_type)2);
    EnsuresEqual(m.rows_max(), (value::Matrix::size_type)25);
    EnsuresEqual(m.columns_max(), (value::Matrix::size_type)15);
    EnsuresEqual(m.resizeRow(), (value::Matrix::size_type)200);
    EnsuresEqual(m.resizeColumn(), (value::Matrix::size_type)100);

    std::cout << "value_matrix:\n" << m.writeToString() << '\n';

    EnsuresEqual(m(0, 0)->isInteger(), true);
    EnsuresEqual(m(0, 1)->isInteger(), true);
    EnsuresEqual(m(0, 2)->isInteger(), true);
    EnsuresEqual(m(1, 0)->isInteger(), true);
    EnsuresEqual(m(1, 1)->isInteger(), true);
    EnsuresEqual(m(1, 2)->isInteger(), true);

    EnsuresEqual(value::toInteger(m(0, 0)), 1);
    EnsuresEqual(value::toInteger(m(1, 0)), 2);
    EnsuresEqual(value::toInteger(m(0, 1)), 3);
    EnsuresEqual(value::toInteger(m(1, 1)), 4);
    EnsuresEqual(value::toInteger(m(0, 2)), 5);
    EnsuresEqual(value::toInteger(m(1, 2)), 6);

    std::string result("<?xml version=\"1.0\"?>\n");
    result += m.writeToXml();

    auto ptr2 = vpz::Vpz::parseValue(result);
    EnsuresEqual(ptr2->isMatrix(), true);

    auto m2 = ptr2->toMatrix();

    EnsuresEqual(m2.rows(), (value::Matrix::size_type)3);
    EnsuresEqual(m2.columns(), (value::Matrix::size_type)2);

    EnsuresEqual(m2(0, 0)->isInteger(), true);
    EnsuresEqual(m2(0, 1)->isInteger(), true);
    EnsuresEqual(m2(0, 2)->isInteger(), true);
    EnsuresEqual(m2(1, 0)->isInteger(), true);
    EnsuresEqual(m2(1, 1)->isInteger(), true);
    EnsuresEqual(m2(1, 2)->isInteger(), true);

    EnsuresEqual(value::toInteger(m2(0, 0)), 1);
    EnsuresEqual(value::toInteger(m2(1, 0)), 2);
    EnsuresEqual(value::toInteger(m2(0, 1)), 3);
    EnsuresEqual(value::toInteger(m2(1, 1)), 4);
    EnsuresEqual(value::toInteger(m2(0, 2)), 5);
    EnsuresEqual(value::toInteger(m2(1, 2)), 6);
}

void
value_matrix_of_matrix()
{
    const char* t1 =
      "<?xml version=\"1.0\"?>\n"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>1</integer>"
      "<integer>2</integer>"
      "<integer>3</integer>"
      "</matrix>"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>4</integer>"
      "<integer>5</integer>"
      "<integer>6</integer>"
      "</matrix>"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>7</integer>"
      "<integer>8</integer>"
      "<integer>9</integer>"
      "</matrix>"
      "</matrix>";

    auto ptr = vpz::Vpz::parseValue(t1);
    EnsuresEqual(ptr->isMatrix(), true);

    const auto& m = ptr->toMatrix();
    std::cout << "matrix_of_matrix\n" << m.writeToXml() << '\n';

    EnsuresEqual(m.rows(), (value::Matrix::size_type)1);
    EnsuresEqual(m.columns(), (value::Matrix::size_type)3);

    EnsuresEqual(m(0, 0)->isMatrix(), true);
    EnsuresEqual(m(1, 0)->isMatrix(), true);
    EnsuresEqual(m(2, 0)->isMatrix(), true);

    auto m1 = m(0, 0)->toMatrix();
    auto m2 = m(1, 0)->toMatrix();
    auto m3 = m(2, 0)->toMatrix();

    EnsuresEqual(m1(0, 0)->toInteger().value(), 1);
    EnsuresEqual(m1(1, 0)->toInteger().value(), 2);
    EnsuresEqual(m1(2, 0)->toInteger().value(), 3);
    EnsuresEqual(m2(0, 0)->toInteger().value(), 4);
    EnsuresEqual(m2(1, 0)->toInteger().value(), 5);
    EnsuresEqual(m2(2, 0)->toInteger().value(), 6);
    EnsuresEqual(m3(0, 0)->toInteger().value(), 7);
    EnsuresEqual(m3(1, 0)->toInteger().value(), 8);
    EnsuresEqual(m3(2, 0)->toInteger().value(), 9);
}

void
value_matrix_of_matrix_io()
{
    const char* t1 =
      "<?xml version=\"1.0\"?>\n"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<null />"
      "<integer>2</integer>"
      "<integer>3</integer>"
      "</matrix>"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>4</integer>"
      "<integer>5</integer>"
      "<null />"
      "</matrix>"
      "<matrix rows=\"1\" columns=\"3\" columnmax=\"15\" "
      "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
      "<integer>7</integer>"
      "<null />"
      "<integer>9</integer>"
      "</matrix>"
      "</matrix>";

    auto ptr = vpz::Vpz::parseValue(t1);

    std::string str(ptr->writeToXml());
    str = "<?xml version=\"1.0\"?>\n" + str;

    auto ptr2 = vpz::Vpz::parseValue(str);
    EnsuresEqual(ptr2->isMatrix(), true);

    auto m = ptr->toMatrix();

    EnsuresEqual(m.rows(), (value::Matrix::size_type)1);
    EnsuresEqual(m.columns(), (value::Matrix::size_type)3);

    EnsuresEqual(m(0, 0)->isMatrix(), true);
    EnsuresEqual(m(1, 0)->isMatrix(), true);
    EnsuresEqual(m(2, 0)->isMatrix(), true);

    auto m1 = value::toMatrixValue(m(0, 0));
    auto m2 = value::toMatrixValue(m(1, 0));
    auto m3 = value::toMatrixValue(m(2, 0));
    Ensures(not m1(0, 0).get());
    EnsuresEqual(value::toInteger(m1(1, 0)), 2);
    EnsuresEqual(value::toInteger(m1(2, 0)), 3);
    EnsuresEqual(value::toInteger(m2(0, 0)), 4);
    EnsuresEqual(value::toInteger(m2(1, 0)), 5);
    Ensures(not m2(2, 0).get());
    EnsuresEqual(value::toInteger(m3(0, 0)), 7);
    Ensures(not m3(1, 0).get());
    EnsuresEqual(value::toInteger(m3(2, 0)), 9);
}

int
main()
{
    vle::Init app;

    value_bool();
    value_integer();
    value_double();
    value_string();
    value_set();
    value_map();
    value_tuple();
    value_table();
    value_table_map();
    value_xml();
    value_matrix();
    value_matrix_of_matrix();
    value_matrix_of_matrix_io();

    return unit_test::report_errors();
}
