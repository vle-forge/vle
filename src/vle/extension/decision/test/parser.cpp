/*
 * @file vle/extension/decision/test/parser.cpp
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
#define BOOST_TEST_MODULE test_parser
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <boost/assign/list_of.hpp>
#include <boost/assign.hpp>
#include <vle/version.hpp>
#include <vle/value/Double.hpp>
#include <vle/extension/Decision.hpp>

#ifdef VLE_HAVE_BOOST_SPIRIT2

namespace vmd = vle::extension::decision;
namespace vd = vle::devs;
using vle::fmt;
using namespace boost::assign;

namespace vle { namespace extension { namespace decision { namespace ex {

class KnowledgeBase : public vmd::KnowledgeBase
{
public:
    KnowledgeBase()
        : vmd::KnowledgeBase(), mNbUpdate(0), mNbAck(0), mNbOut(0)
    {
        addFacts(this) +=
            F("fact 1", &KnowledgeBase::updateFact1),
            F("fact 2", &KnowledgeBase::updateFact2);

        addPredicates(this) +=
            P("pred 1", &KnowledgeBase::isAlwaysTrue),
            P("pred 2", &KnowledgeBase::isAlwaysTrue),
            P("pred 3", &KnowledgeBase::isAlwaysTrue),
            P("pred 4", &KnowledgeBase::isAlwaysTrue),
            P("pred 5", &KnowledgeBase::isAlwaysTrue),
            P("pr ed 5", &KnowledgeBase::isAlwaysTrue),
            P("pred 6", &KnowledgeBase::isAlwaysTrue);

        addOutputFunctions(this) +=
            O("output function", &KnowledgeBase::out);

        addAcknowledgeFunctions(this) +=
            A("ack function", &KnowledgeBase::ack);

        addUpdateFunctions(this) +=
            U("update function", &KnowledgeBase::update);
    }

    virtual ~KnowledgeBase() {}

    void updateFact1(const value::Value&)
    {
    }

    void updateFact2(const value::Value&)
    {
    }

    void ack(const std::string&, const Activity&)
    {
        mNbAck++;
    }

    void out(const std::string&, const Activity&,
             vle::devs::ExternalEventList&)
    {
        mNbOut++;
    }

    void update(const std::string&, const Activity&)
    {
        mNbUpdate++;
    }

    bool isAlwaysTrue() const
    {
        return true;
    }

    int getNumberOfUpdate() const
    {
        return mNbUpdate;
    }

    int getNumberOfAck() const
    {
        return mNbAck;
    }

    int getNumberOfOut() const
    {
        return mNbOut;
    }

    int mNbUpdate, mNbAck, mNbOut;
};


const char* Plan1 = \
"# This file is a part of the VLE environment # http://www.vle-project.org\n"
"# Copyright (C) 2010 INRA http://www.inra.fr\n"
"#\n"
"# This program is free software: you can redistribute it and/or modify\n"
"# it under the terms of the GNU General Public License as published by\n"
"# the Free Software Foundation, either version 3 of the License, or\n"
"# (at your option) any later version.\n"
"#\n"
"# This program is distributed in the hope that it will be useful,\n"
"# but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"# GNU General Public License for more details.\n"
"#\n"
"# You should have received a copy of the GNU General Public License\n"
"# along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
"\n"
"\n"
"rules { # listes des règles.\n"
"    rule { # définition de la rèle `rule 1'.\n"
"        id = \"rule 1\";\n"
"        predicates = \"pred 1\" \"pred 2\" \"pred 3\"\n"
"            \"pred 4\" \"pred 5\" \"pred 6\"; # sa liste de prédicats.\n"
"    }\n"
"\n"
"    rule {\n"
"        id = \"rule 2\";\n"
"        predicates = \"pr ed 5\" \"pred 6\";\n"
"    }\n"
"\n"
"    rule {\n"
"        id = \"rule 3\";\n"
"    }\n"
"}\n"
"\n"
"activities {\n"
"    activity {\n"
"        id = \"activity1\";\n"
"        rules = \"rule 1\" \"rule 2\";\n"
"        temporal {\n"
"            start = 0;\n"
"            finish = 101;\n"
"        }\n"
"        ack = \"ack function\";\n"
"        output = \"output function\";\n"
"    }\n"
"    activity {\n"
"        id = \"activity2\";\n"
"        rules = \"rule 1\" \"rule 2\";\n"
"        temporal {\n"
"            start = 0;\n"
"            minfinish = 99;\n"
"            maxfinish = 101;\n"
"        }\n"
"        ack = \"ack function\";\n"
"        output = \"output function\";\n"
"    }\n"
"    activity {\n"
"        id = \"activity3\";\n"
"        rules = \"rule 1\" \"rule 2\";\n"
"        temporal {\n"
"            minstart = 0;\n"
"            maxstart = 10;\n"
"            finish = 1000;\n"
"        }\n"
"        ack = \"ack function\";\n"
"        output = \"output function\";\n"
"    }\n"
"    activity {\n"
"        id = \"activity4\";\n"
"        rules = \"rule 1\" \"rule 2\";\n"
"        temporal {\n"
"            minstart = 0;\n"
"            maxstart = 10;\n"
"            minfinish = 100;\n"
"            maxfinish = 1000;\n"
"        }\n"
"        ack = \"ack function\";\n"
"        output = \"output function\";\n"
"    }\n"
"}\n"
"\n"
"precedences {\n"
"    precedence {\n"
"        type = SS;\n"
"        first = \"activity1\";\n"
"        second = \"activity2\";\n"
"        mintimelag = 10;\n"
"        maxtimelag = 100;\n"
"    }\n"
"    precedence {\n"
"        type = FF;\n"
"        first = \"activity2\";\n"
"        second = \"activity3\";\n"
"        maxtimelag = 100;\n"
"    }\n"
"    precedence {\n"
"        type = FS;\n"
"        first = \"activity2\";\n"
"        second = \"activity3\";\n"
"        mintimelag = 100;\n"
"    }\n"
"    precedence {\n"
"        type = FS;\n"
"        first = \"activity3\";\n"
"        second = \"activity4\";\n"
"    }\n"
"}\n";

}}}} // namespace vle extension decision ex

BOOST_AUTO_TEST_CASE(parser)
{
    vle::value::init();

    vmd::ex::KnowledgeBase b;
    vmd::Parser parser(b, vmd::ex::Plan1);

    BOOST_REQUIRE_EQUAL(b.activities().size(), (vmd::Activities::size_type)4);
}
#else
BOOST_AUTO_TEST_CASE(parser)
{
}
#endif
