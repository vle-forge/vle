/**
 * @file vle/graph/test/test1.cpp
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
#include <vle/graph.hpp>
#include <vle/utils.hpp>
#include <vle/vpz.hpp>

using namespace vle;
using namespace graph;


BOOST_AUTO_TEST_CASE(test_del_all_connection)
{
    CoupledModel* top = new CoupledModel("top", 0);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    BOOST_REQUIRE(top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "a", "in"));
    top->delAllConnection();
    BOOST_REQUIRE(not top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(not top->existInternalConnection("b", "out", "a", "in"));
}

BOOST_AUTO_TEST_CASE(test_have_connection)
{
    CoupledModel* top = new CoupledModel("top", 0);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    AtomicModel* c(top->addAtomicModel("c"));
    c->addInputPort("in");
    c->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    BOOST_REQUIRE(not top->hasConnectionProblem(lst));

    top->addInternalConnection("c", "out", "a", "in");

    BOOST_REQUIRE(top->hasConnectionProblem(lst)); 
}

BOOST_AUTO_TEST_CASE(test_displace)
{
    CoupledModel* top = new CoupledModel("top", 0);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    AtomicModel* c(top->addAtomicModel("c"));
    c->addInputPort("in");
    c->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    CoupledModel* newtop = new CoupledModel("newtop", 0);

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    top->displace(lst, newtop);

    BOOST_REQUIRE_EQUAL(top->getModelList().size(),
                        (ModelList::size_type)1);
    
    BOOST_REQUIRE_EQUAL(newtop->getModelList().size(),
                        (ModelList::size_type)2);

    BOOST_REQUIRE(newtop->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(newtop->existInternalConnection("b", "out", "a", "in"));
}

BOOST_AUTO_TEST_CASE(test_prohibited_displace)
{
    CoupledModel* top = new CoupledModel("top", 0);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    AtomicModel* c(top->addAtomicModel("c"));
    c->addInputPort("in");
    c->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");
    top->addInternalConnection("a", "out", "c", "in");

    CoupledModel* newtop = new CoupledModel("newtop", 0);

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    BOOST_REQUIRE_THROW(top->displace(lst, newtop), utils::DevsGraphError);
}

BOOST_AUTO_TEST_CASE(test_delinput_port)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
}

BOOST_AUTO_TEST_CASE(test_clone1)
{
    CoupledModel* top = new CoupledModel("top", 0);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    BOOST_REQUIRE(top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "a", "in"));

    CoupledModel* newtop(
        dynamic_cast < CoupledModel* >(top->clone()));
    BOOST_REQUIRE(newtop != 0);
    BOOST_REQUIRE(newtop->getModelList().size() == 2);

    AtomicModel* newa = dynamic_cast < AtomicModel* >(newtop->findModel("a"));
    BOOST_REQUIRE(newa != a);
    AtomicModel* newb = dynamic_cast < AtomicModel* >(newtop->findModel("b"));
    BOOST_REQUIRE(newb != b);
    
    BOOST_REQUIRE(newtop->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(newtop->existInternalConnection("b", "out", "a", "in"));
    newtop->delAllConnection();
    BOOST_REQUIRE(not newtop->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(not newtop->existInternalConnection("b", "out", "a", "in"));
    BOOST_REQUIRE(top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "a", "in"));
}

BOOST_AUTO_TEST_CASE(test_clone2)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    CoupledModel* oldtop = dynamic_cast < CoupledModel*
        >(file.project().model().model());
    BOOST_REQUIRE(oldtop);

    CoupledModel* top = dynamic_cast < CoupledModel* >(oldtop->clone());
    BOOST_REQUIRE(top);
    CoupledModel* top1(dynamic_cast < CoupledModel* >(top->findModel("top1")));
    BOOST_REQUIRE(top1);
    CoupledModel* top2(dynamic_cast < CoupledModel* >(top->findModel("top2")));
    BOOST_REQUIRE(top2);

    AtomicModel* f(dynamic_cast < AtomicModel* >(top2->findModel("f")));
    BOOST_REQUIRE(f);
    AtomicModel* g(dynamic_cast < AtomicModel* >(top2->findModel("g")));
    BOOST_REQUIRE(g);
}

BOOST_AUTO_TEST_CASE(test_clone_different_atomic)
{
    vpz::Vpz file1(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    vpz::Vpz file2(file1);

    CoupledModel* top1 = dynamic_cast < CoupledModel*
        >(file1.project().model().model());
    CoupledModel* top2 = dynamic_cast < CoupledModel*
        >(file2.project().model().model());

    BOOST_REQUIRE(top1 and top2);
    
    AtomicModelVector lst1, lst2;

    Model::getAtomicModelList(top1, lst1);
    Model::getAtomicModelList(top2, lst2);

    graph::AtomicModelVector intersection;

    std::sort(lst1.begin(), lst1.end());
    std::sort(lst2.begin(), lst2.end());

    graph::AtomicModelVector::iterator it;

    it = std::search(lst1.begin(), lst1.end(), lst2.begin(), lst2.end());

    BOOST_REQUIRE(it == lst1.end());
}
