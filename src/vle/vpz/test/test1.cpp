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
#include <vle/value/Matrix.hpp>
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

BOOST_AUTO_TEST_CASE(value_bool)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<boolean>true</boolean>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<boolean>false</boolean>";
    const char* t3 = "<?xml version=\"1.0\"?>\n<boolean>1</boolean>";
    const char* t4 = "<?xml version=\"1.0\"?>\n<boolean>0</boolean>";

    value::Value* v;

    v = vpz::Vpz::parseValue(t1);
    BOOST_CHECK(value::toBoolean(v) == true);
    delete v;

    v = vpz::Vpz::parseValue(t2);
    BOOST_CHECK(value::toBoolean(v) == false);
    delete v;

    v = vpz::Vpz::parseValue(t3);
    BOOST_CHECK(value::toBoolean(v) == true);
    delete v;

    v = vpz::Vpz::parseValue(t4);
    BOOST_CHECK(value::toBoolean(v) == false);

    v = vpz::Vpz::parseValue(v->writeToXml());
    BOOST_CHECK(value::toBoolean(v) == false);
    delete v;
}

BOOST_AUTO_TEST_CASE(value_integer)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<integer>100</integer>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<integer>-100</integer>";
    const size_t bufferSize = 1000;
    char t3[bufferSize];
    char t4[bufferSize];

    snprintf(t3, bufferSize, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
             utils::to < int32_t >(std::numeric_limits< int32_t >::max()).c_str());
    snprintf(t4, bufferSize, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
             utils::to < int32_t >(std::numeric_limits< int32_t >::min()).c_str());

    value::Value* v;

    v = vpz::Vpz::parseValue(t1);
    BOOST_CHECK(value::toInteger(v) == 100);
    delete v;

    v = vpz::Vpz::parseValue(t2);
    BOOST_CHECK(value::toInteger(v) == -100);
    delete v;

    v = vpz::Vpz::parseValue(t3);
    BOOST_CHECK_EQUAL(value::toInteger(v), std::numeric_limits < int32_t >::max());
    delete v;

    v = vpz::Vpz::parseValue(t4);

    BOOST_CHECK_EQUAL(value::toInteger(v), std::numeric_limits < int32_t >::min());
    std::string t5(v->writeToXml());
    delete v;

    v = vpz::Vpz::parseValue(t5);
    BOOST_CHECK_EQUAL(value::toInteger(v), std::numeric_limits < int32_t >::min());
    delete v;
}

BOOST_AUTO_TEST_CASE(value_double)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<double>100.5</double>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<double>-100.5</double>";

    value::Value* v;

    v = vpz::Vpz::parseValue(t1);
    BOOST_CHECK_CLOSE(value::toDouble(v), 100.5, 1);
    delete v;

    v = vpz::Vpz::parseValue(t2);
    BOOST_CHECK_CLOSE(value::toDouble(v), -100.5, 1);
    std::string t3(v->writeToXml());
    delete v;

    v = vpz::Vpz::parseValue(t3);
    BOOST_CHECK_CLOSE(value::toDouble(v), -100.5, 1);
    delete v;
}

BOOST_AUTO_TEST_CASE(value_string)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<string>a b c d e f g h i j</string>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<string>a\nb\tc\n</string>";
    const char* t4 = "<?xml version=\"1.0\"?>\n<string>é ç € â ô f Û « © ±</string>";

    value::Value* v;

    v = vpz::Vpz::parseValue(t1);
    BOOST_CHECK(value::toString(v) == "a b c d e f g h i j");
    delete v;

    v = vpz::Vpz::parseValue(t2);
    BOOST_CHECK(value::toString(v) == "a\nb\tc\n");
    std::string t3(v->writeToXml());
    v = vpz::Vpz::parseValue(t3);
    BOOST_CHECK(value::toString(v) == "a\nb\tc\n");
    delete v;

    v = vpz::Vpz::parseValue(t4);
    BOOST_CHECK(value::toString(v) == "é ç € â ô f Û « © ±");
    delete v;
}

BOOST_AUTO_TEST_CASE(value_set)
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

    value::Set* v;

    v = value::toSetValue(vpz::Vpz::parseValue(t1));
    BOOST_CHECK(value::toString(v->get(1)) == "test");
    BOOST_CHECK(value::toInteger(v->get(0)) == 1);
    delete v;

    v = value::toSetValue(vpz::Vpz::parseValue(t2));
    BOOST_CHECK(value::toInteger(v->get(0)) == 1);
    {
        value::Set& v2 = value::toSetValue(*v->get(1));
        BOOST_CHECK(value::toString(v2.get(0)) == "test");
    }
    std::string t3(v->writeToXml());
    delete v;

    v = value::toSetValue(vpz::Vpz::parseValue(t3));
    BOOST_CHECK(value::toInteger(v->get(0)) == 1);
    {
        value::Set& v2 = value::toSetValue(*v->get(1));
        BOOST_CHECK(value::toString(v2.get(0)) == "test");
    }
    delete v;
}

BOOST_AUTO_TEST_CASE(value_map)
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

    value::Map* v;

    v = value::toMapValue(vpz::Vpz::parseValue(t1));
    BOOST_CHECK(value::toInteger(v->get("a")) == 10);
    delete v;

    v = value::toMapValue(vpz::Vpz::parseValue(t2));
    std::string t3(v->writeToXml());
    {
        value::Set& s = v->getSet("a");
        BOOST_CHECK(value::toInteger(s.get(0)) == 1);
        BOOST_CHECK(value::toString(s.get(1)) == "test");
    }
    delete v;

    v = value::toMapValue(vpz::Vpz::parseValue(t3));
    {
        value::Set& s = v->getSet("a");
        BOOST_CHECK(value::toInteger(s.get(0)) == 1);
        BOOST_CHECK(value::toString(s.get(1)) == "test");
    }
    delete v;
}

BOOST_AUTO_TEST_CASE(value_tuple)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<tuple>1 2 3</tuple>\n";

    value::Tuple* v;

    v = value::toTupleValue(vpz::Vpz::parseValue(t1));
    BOOST_REQUIRE_EQUAL(v->size(), (size_t)3);
    BOOST_CHECK_CLOSE(v->operator[](0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](1), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](2), 3.0, 0.1);
    delete v;

    const char* t2 = "<?xml version=\"1.0\"?>\n"
        "<map>\n"
        "   <key name=\"testtest\">\n"
        "      <tuple>100 200 300</tuple>\n"
        "   </key>\n"
        "</map>\n";

    value::Map* m;
    m = value::toMapValue(vpz::Vpz::parseValue(t2));

    value::Value& t = value::reference(m->get("testtest"));
    value::Tuple& v2(toTupleValue(t));
    BOOST_CHECK_CLOSE(v2.operator[](0), 100.0, 0.1);
    BOOST_CHECK_CLOSE(v2.operator[](1), 200.0, 0.1);
    BOOST_CHECK_CLOSE(v2.operator[](2), 300.0, 0.1);
    std::string t3 = v2.writeToXml();
    delete m;

    value::Tuple* v3 = value::toTupleValue(vpz::Vpz::parseValue(t3));
    BOOST_CHECK_CLOSE(v3->operator[](0), 100.0, 0.1);
    BOOST_CHECK_CLOSE(v3->operator[](1), 200.0, 0.1);
    BOOST_CHECK_CLOSE(v3->operator[](2), 300.0, 0.1);
    delete v3;
}

BOOST_AUTO_TEST_CASE(value_table)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<table width=\"2\" height=\"3\">\n"
        "1 2 3 4 5 6"
        "</table>\n";

    value::Table* v = value::toTableValue(vpz::Vpz::parseValue(t1));
    BOOST_REQUIRE_EQUAL(v->width(), (value::Table::index)2);
    BOOST_REQUIRE_EQUAL(v->height(), (value::Table::index)3);
    BOOST_CHECK_CLOSE(v->get(0, 0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 1), 3.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 2), 5.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 0), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 1), 4.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 2), 6.0, 0.1);

    BOOST_REQUIRE_EQUAL(v->writeToString(), "((1,2),(3,4),(5,6))");

    std::string result("<?xml version=\"1.0\"?>\n");
    result += v->writeToXml();
    value::Table* w = value::toTableValue(vpz::Vpz::parseValue(result));
    BOOST_REQUIRE_EQUAL(w->width(), (value::Table::index)2);
    BOOST_REQUIRE_EQUAL(w->height(), (value::Table::index)3);
    BOOST_CHECK_CLOSE(w->get(0, 0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(w->get(0, 1), 3.0, 0.1);
    BOOST_CHECK_CLOSE(w->get(0, 2), 5.0, 0.1);
    BOOST_CHECK_CLOSE(w->get(1, 0), 2.0, 0.1);
    BOOST_CHECK_CLOSE(w->get(1, 1), 4.0, 0.1);
    BOOST_CHECK_CLOSE(w->get(1, 2), 6.0, 0.1);

    delete v;
    delete w;
}


BOOST_AUTO_TEST_CASE(value_table_map)
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
        value::Map* m = value::toMapValue(vpz::Vpz::parseValue(t1));
        const value::Table& w(m->getTable("tr"));
        BOOST_REQUIRE_EQUAL(w.width(), (value::Table::index)2);
        BOOST_REQUIRE_EQUAL(w.height(), (value::Table::index)3);
        BOOST_CHECK_CLOSE(w.get(0, 0), 1.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 1), 3.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 2), 5.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 0), 2.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 1), 4.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 2), 6.0, 0.1);
        delete m;
    }
    {
        value::Map* m = value::toMapValue(vpz::Vpz::parseValue(t2));
        const value::Table& w(m->getTable("tr"));
        BOOST_REQUIRE_EQUAL(w.width(), (value::Table::index)2);
        BOOST_REQUIRE_EQUAL(w.height(), (value::Table::index)3);
        BOOST_CHECK_CLOSE(w.get(0, 0), 1.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 1), 3.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 2), 5.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 0), 2.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 1), 4.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 2), 6.0, 0.1);
        delete m;
    }
    {
        value::Map* m = value::toMapValue(vpz::Vpz::parseValue(t3));
        const value::Table& w(m->getTable("tr"));
        BOOST_REQUIRE_EQUAL(w.width(), (value::Table::index)2);
        BOOST_REQUIRE_EQUAL(w.height(), (value::Table::index)3);
        BOOST_CHECK_CLOSE(w.get(0, 0), 1.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 1), 3.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(0, 2), 5.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 0), 2.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 1), 4.0, 0.1);
        BOOST_CHECK_CLOSE(w.get(1, 2), 6.0, 0.1);
        delete m;
    }

}

BOOST_AUTO_TEST_CASE(value_xml)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<xml>"
        "<![CDATA[test 1 2 1 2]]>\n"
        "</xml>";

    value::Xml* str(value::toXmlValue(vpz::Vpz::parseValue(t1)));
    std::string t2(str->writeToXml());
    BOOST_REQUIRE_EQUAL(str->value(), "test 1 2 1 2");
    delete str;

    value::Xml* str2(value::toXmlValue(vpz::Vpz::parseValue(t2)));
    BOOST_REQUIRE_EQUAL(str2->value(), "test 1 2 1 2");
    delete str2;
}

BOOST_AUTO_TEST_CASE(value_matrix)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<matrix rows=\"3\" columns=\"2\" columnmax=\"15\" "
        "        rowmax=\"25\" columnstep=\"100\" rowstep=\"200\" >"
        "<integer>1</integer>"
        "<integer>2</integer>"
        "<integer>3</integer>"
        "<integer>4</integer>"
        "<integer>5</integer>"
        "<integer>6</integer>"
        "</matrix>";

    value::Value* v = vpz::Vpz::parseValue(t1);
    BOOST_REQUIRE_EQUAL(v->isMatrix(), true);

    value::Matrix* m = value::toMatrixValue(v);
    value::MatrixView mv = value::toMatrix(v);

    BOOST_REQUIRE_EQUAL(m->rows(), (value::Matrix::size_type)3);
    BOOST_REQUIRE_EQUAL(m->columns(), (value::Matrix::size_type)2);
    BOOST_REQUIRE_EQUAL(m->matrix().shape()[0], (value::Matrix::size_type)15);
    BOOST_REQUIRE_EQUAL(m->matrix().shape()[1], (value::Matrix::size_type)25);
    BOOST_REQUIRE_EQUAL(m->resizeRow(), (value::Matrix::size_type)200);
    BOOST_REQUIRE_EQUAL(m->resizeColumn(), (value::Matrix::size_type)100);

    BOOST_REQUIRE_EQUAL(mv[0][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv[0][1]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv[0][2]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv[1][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv[1][1]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv[1][2]->isInteger(), true);

    BOOST_REQUIRE_EQUAL(value::toInteger(mv[0][0]), 1);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv[1][0]), 2);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv[0][1]), 3);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv[1][1]), 4);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv[0][2]), 5);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv[1][2]), 6);

    std::string result("<?xml version=\"1.0\"?>\n");
    result += v->writeToXml();
    value::Value* v2 = vpz::Vpz::parseValue(result);
    BOOST_REQUIRE_EQUAL(v2->isMatrix(), true);
    value::Matrix* m2 = value::toMatrixValue(v2);
    value::MatrixView mv2 = value::toMatrix(v2);

    BOOST_REQUIRE_EQUAL(m2->rows(), (value::Matrix::size_type)3);
    BOOST_REQUIRE_EQUAL(m2->columns(), (value::Matrix::size_type)2);

    BOOST_REQUIRE_EQUAL(mv2[0][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv2[0][1]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv2[0][2]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv2[1][0]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv2[1][1]->isInteger(), true);
    BOOST_REQUIRE_EQUAL(mv2[1][2]->isInteger(), true);

    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[0][0]), 1);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[1][0]), 2);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[0][1]), 3);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[1][1]), 4);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[0][2]), 5);
    BOOST_REQUIRE_EQUAL(value::toInteger(mv2[1][2]), 6);

    delete v;
    delete v2;
}


BOOST_AUTO_TEST_CASE(value_matrix_of_matrix)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
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

    value::Value* v = vpz::Vpz::parseValue(t1);
    BOOST_REQUIRE_EQUAL(v->isMatrix(), true);

    value::Matrix* m = value::toMatrixValue(v);
    value::MatrixView mv = value::toMatrix(v);

    BOOST_REQUIRE_EQUAL(m->rows(), (value::Matrix::size_type)1);
    BOOST_REQUIRE_EQUAL(m->columns(), (value::Matrix::size_type)3);

    BOOST_REQUIRE_EQUAL(mv[0][0]->isMatrix(), true);
    BOOST_REQUIRE_EQUAL(mv[1][0]->isMatrix(), true);
    BOOST_REQUIRE_EQUAL(mv[2][0]->isMatrix(), true);

    value::MatrixView m1 = value::toMatrix(mv[0][0]);
    value::MatrixView m2 = value::toMatrix(mv[1][0]);
    value::MatrixView m3 = value::toMatrix(mv[2][0]);
    BOOST_REQUIRE_EQUAL(value::toInteger(m1[0][0]), 1);
    BOOST_REQUIRE_EQUAL(value::toInteger(m1[1][0]), 2);
    BOOST_REQUIRE_EQUAL(value::toInteger(m1[2][0]), 3);
    BOOST_REQUIRE_EQUAL(value::toInteger(m2[0][0]), 4);
    BOOST_REQUIRE_EQUAL(value::toInteger(m2[1][0]), 5);
    BOOST_REQUIRE_EQUAL(value::toInteger(m2[2][0]), 6);
    BOOST_REQUIRE_EQUAL(value::toInteger(m3[0][0]), 7);
    BOOST_REQUIRE_EQUAL(value::toInteger(m3[1][0]), 8);
    BOOST_REQUIRE_EQUAL(value::toInteger(m3[2][0]), 9);

    delete v;
}

BOOST_AUTO_TEST_CASE(value_matrix_of_matrix_io)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
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

    value::Value* v = vpz::Vpz::parseValue(t1);

    std::string str(v->writeToXml());
    str = "<?xml version=\"1.0\"?>\n" + str;

    value::Value* v2 = vpz::Vpz::parseValue(str);
    BOOST_REQUIRE_EQUAL(v2->isMatrix(), true);

    value::Matrix* m = value::toMatrixValue(v2);
    value::MatrixView mv = value::toMatrix(v2);

    BOOST_REQUIRE_EQUAL(m->rows(), (value::Matrix::size_type)1);
    BOOST_REQUIRE_EQUAL(m->columns(), (value::Matrix::size_type)3);

    BOOST_REQUIRE_EQUAL(mv[0][0]->isMatrix(), true);
    BOOST_REQUIRE_EQUAL(mv[1][0]->isMatrix(), true);
    BOOST_REQUIRE_EQUAL(mv[2][0]->isMatrix(), true);

    value::MatrixView m1 = value::toMatrix(mv[0][0]);
    value::MatrixView m2 = value::toMatrix(mv[1][0]);
    value::MatrixView m3 = value::toMatrix(mv[2][0]);
    BOOST_REQUIRE_EQUAL(m1[0][0], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(m1[1][0]), 2);
    BOOST_REQUIRE_EQUAL(value::toInteger(m1[2][0]), 3);
    BOOST_REQUIRE_EQUAL(value::toInteger(m2[0][0]), 4);
    BOOST_REQUIRE_EQUAL(value::toInteger(m2[1][0]), 5);
    BOOST_REQUIRE_EQUAL(m2[2][0], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(m3[0][0]), 7);
    BOOST_REQUIRE_EQUAL(m3[1][0], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(m3[2][0]), 9);

    delete v;
    delete v2;
}
