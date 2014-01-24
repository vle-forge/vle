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
#define BOOST_TEST_MODULE atomicdynamics_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;
using namespace vpz;

BOOST_AUTO_TEST_CASE(test_rename_model)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top =
        dynamic_cast<CoupledModel*>(file.project().model().model());
    BOOST_REQUIRE(top);
    BOOST_REQUIRE_NO_THROW(vpz::BaseModel::rename(top, "new_top"));

    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    BOOST_REQUIRE_THROW(vpz::BaseModel::rename(d, "e"), utils::DevsGraphError);
    BOOST_REQUIRE_NO_THROW(vpz::BaseModel::rename(d, "new_d"));

    CoupledModel * top1 = dynamic_cast<CoupledModel*>(top->findModel("top1"));
    BOOST_REQUIRE_THROW(vpz::BaseModel::rename(top1, "top2"), utils::DevsGraphError);
    BOOST_REQUIRE_NO_THROW(vpz::BaseModel::rename(top1, "new_top1"));
}

BOOST_AUTO_TEST_CASE(test_findModelFromPath)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));
    BaseModel* d = file.project().model().model()->findModelFromPath("d");
    bool found = (d != 0);

    BOOST_REQUIRE_EQUAL(found, true);
    BaseModel* top1 = file.project().model().model()->findModelFromPath("top1");
    found = (top1 != 0);
    BOOST_REQUIRE_EQUAL(found, true);
    BaseModel* top2_g = file.project().model().model()->findModelFromPath("top2,g");
    found = (top2_g != 0);
    BOOST_REQUIRE_EQUAL(found, true);
    BaseModel* nomodel =
        file.project().model().model()->findModelFromPath("nomodel");
    found = (nomodel != 0);
    BOOST_REQUIRE_EQUAL(found, false);
    BaseModel* empty = file.project().model().model()->findModelFromPath("");
    found = (empty != 0);
    BOOST_REQUIRE_EQUAL(found, false);
    BaseModel* top1_a_nomodel =
        file.project().model().model()->findModelFromPath("top1,a,nomodel");
    found = (top1_a_nomodel != 0);
    BOOST_REQUIRE_EQUAL(found, false);
}

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

    delete top;
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

    delete top;
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

    delete top;
    delete newtop;
}

BOOST_AUTO_TEST_CASE(test_complex_displace)
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

    BOOST_REQUIRE_EQUAL(top->getModelList().size(),
                        (ModelList::size_type)3);

    CoupledModel* newtop = new CoupledModel("newtop", top);

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    BOOST_REQUIRE_NO_THROW(top->displace(lst, newtop));

    BOOST_REQUIRE_EQUAL(top->getModelList().size(),
                        (ModelList::size_type)2);
    BOOST_REQUIRE_EQUAL(newtop->getModelList().size(),
                        (ModelList::size_type)2);

    newtop->existInternalConnection("a", "out", "b", "in");
    newtop->existInternalConnection("b", "out", "a", "in");
    newtop->existOutputPort("out");
    top->existInternalConnection("newtop", "out", "c", "in");

    delete top;
}

BOOST_AUTO_TEST_CASE(test_delinput_port)
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

    BOOST_REQUIRE_EQUAL(top->existInternalConnection("a", "out", "b", "in"),
                        true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("b", "out", "a", "in"),
                        true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("a", "out", "c", "in"),
                        true);
    c->delInputPort("in");

    BOOST_REQUIRE_EQUAL(top->existInputPort("in"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("a", "out", "c", "in"),
                        false);

    delete top;
}

BOOST_AUTO_TEST_CASE(test_del_port)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top =
        dynamic_cast<CoupledModel*>(file.project().model().model());
    BOOST_REQUIRE(top);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "top2",
                                                     "in"), true);

    //Atomic Model
    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    BOOST_REQUIRE(d);
    BOOST_REQUIRE_EQUAL(d->existInputPort("in"), true);
    BOOST_REQUIRE_EQUAL(d->existOutputPort("out"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "d",
                                                     "in"), true);

    //Atomic Model -- Input Port
    d->delInputPort("in");
    BOOST_REQUIRE_EQUAL(d->existInputPort("in"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "d",
                                                     "in"), false);

    //Atomic Model -- Output Port
    d->delOutputPort("out");
    BOOST_REQUIRE_EQUAL(d->existOutputPort("out"), false);

    //Coupled Model
    CoupledModel* top1 =
        dynamic_cast<CoupledModel*>(top->findModel("top1"));
    BOOST_REQUIRE(top1);

    //Coupled Model -- Input Port
    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), true);
    BOOST_REQUIRE_EQUAL(top1->existInputConnection("in", "x", "in"), true);

    top1->delInputPort("in");
    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), false);
    BOOST_REQUIRE_EQUAL(top1->existInputConnection("in", "x", "in"), false);

    //Coupled Model -- Output Port
    BOOST_REQUIRE_EQUAL(top1->existOutputPort("out"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in1"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in2"), true);
    BOOST_REQUIRE_EQUAL(top1->existOutputConnection("x", "out", "out"),
                        true);

    top1->delOutputPort("out");
    BOOST_REQUIRE_EQUAL(top1->existOutputPort("out"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in1"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in2"), false);
    BOOST_REQUIRE_EQUAL(top1->existOutputConnection("x", "out", "out"),
                        false);
}

BOOST_AUTO_TEST_CASE(test_clone1)
{
    CoupledModel* top = new CoupledModel("top", 0);
    top->addInputPort("in");
    top->addOutputPort("out");

    AtomicModel* a(top->addAtomicModel("top"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    top->addInputConnection("in", "top", "in");
    top->addInternalConnection("top", "out", "b", "in");
    top->addInternalConnection("b", "out", "top", "in");
    top->addOutputConnection("top", "out", "out");

    BOOST_REQUIRE(top->existInternalConnection("top", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "top", "in"));

    CoupledModel* newtop(dynamic_cast < CoupledModel* >(top->clone()));
    BOOST_REQUIRE(newtop != 0);
    BOOST_REQUIRE(newtop->getModelList().size() == 2);

    AtomicModel* newa = dynamic_cast < AtomicModel* >(newtop->findModel("top"));
    BOOST_REQUIRE(newa != a);
    AtomicModel* newb = dynamic_cast < AtomicModel* >(newtop->findModel("b"));
    BOOST_REQUIRE(newb != b);

    BOOST_REQUIRE(newtop->existInternalConnection("top", "out", "b", "in"));
    BOOST_REQUIRE(newtop->existInternalConnection("b", "out", "top", "in"));
    BOOST_REQUIRE(newtop->existInputConnection("in", "top", "in"));
    BOOST_REQUIRE(newtop->existOutputConnection("top", "out", "out"));
    newtop->delAllConnection();
    BOOST_REQUIRE(not newtop->existOutputConnection("top", "out", "out"));
    BOOST_REQUIRE(not newtop->existInternalConnection("top", "out", "b", "in"));
    BOOST_REQUIRE(not newtop->existInternalConnection("top", "out", "b", "in"));
    BOOST_REQUIRE(not newtop->existInternalConnection("b", "out", "top", "in"));
    BOOST_REQUIRE(top->existInternalConnection("top", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "top", "in"));
    BOOST_REQUIRE(top->existInputConnection("in", "top", "in"));
    BOOST_REQUIRE(top->existOutputConnection("top", "out", "out"));

    delete newtop;
    delete top;
}

BOOST_AUTO_TEST_CASE(test_clone2)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* oldtop = dynamic_cast < CoupledModel* >(
        file.project().model().model());
    BOOST_REQUIRE(oldtop);

    CoupledModel* top = dynamic_cast < CoupledModel* >(oldtop->clone());
    BOOST_REQUIRE(top);
    CoupledModel* top1(dynamic_cast < CoupledModel*
                       >(top->findModel("top1")));
    BOOST_REQUIRE(top1);
    CoupledModel* top2(dynamic_cast < CoupledModel*
                       >(top->findModel("top2")));
    BOOST_REQUIRE(top2);

    AtomicModel* f(dynamic_cast < AtomicModel* >(top2->findModel("f")));
    BOOST_REQUIRE(f);
    AtomicModel* g(dynamic_cast < AtomicModel* >(top2->findModel("g")));
    BOOST_REQUIRE(g);

    delete top;
    delete file.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_clone_different_atomic)
{
    vpz::Vpz file1(utils::Path::path().getTemplate("unittest.vpz"));

    vpz::Vpz file2(file1);

    CoupledModel* top1 = dynamic_cast < CoupledModel*
        >(file1.project().model().model());
    CoupledModel* top2 = dynamic_cast < CoupledModel*
        >(file2.project().model().model());

    BOOST_REQUIRE(top1 and top2);
    BOOST_REQUIRE(top1 != top2);

    AtomicModelVector lst1, lst2;

    BaseModel::getAtomicModelList(top1, lst1);
    BaseModel::getAtomicModelList(top2, lst2);

    AtomicModelVector intersection;

    std::sort(lst1.begin(), lst1.end());
    std::sort(lst2.begin(), lst2.end());

    AtomicModelVector::iterator it;

    it = std::search(lst1.begin(), lst1.end(), lst2.begin(), lst2.end());

    BOOST_REQUIRE(it == lst1.end());

    delete file1.project().model().model();
    delete file2.project().model().model();
    utils::Trace::kill();
    utils::Path::kill();
}

BOOST_AUTO_TEST_CASE(test_get_port_index)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top = dynamic_cast < CoupledModel* >(
        file.project().model().model());

    BOOST_REQUIRE(top);

    BOOST_REQUIRE_EQUAL(top->getInputPortList().size(),
                        (ConnectionList::size_type)0);
    BOOST_REQUIRE_EQUAL(top->getOutputPortList().size(),
                        (ConnectionList::size_type)0);

    AtomicModel* e = dynamic_cast < AtomicModel*> (top->getModelList()["e"]);

    BOOST_REQUIRE(e);

    BOOST_REQUIRE_EQUAL(e->getInputPortList().size(),
                        (ConnectionList::size_type)2);
    BOOST_REQUIRE_EQUAL(e->getInputPortIndex("in1"), 0);
    BOOST_REQUIRE_EQUAL(e->getInputPortIndex("in2"), 1);
    BOOST_REQUIRE_EQUAL(e->getOutputPortList().size(),
                        (ConnectionList::size_type)1);

    delete file.project().model().model();
    utils::Trace::kill();
    utils::Path::kill();
}

BOOST_AUTO_TEST_CASE(test_rename_port)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top =
        dynamic_cast<CoupledModel*>(file.project().model().model());
    BOOST_REQUIRE(top);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "top2",
                                                     "in"), true);

    //Atomic Model
    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    BOOST_REQUIRE(d);
    BOOST_REQUIRE_EQUAL(d->existInputPort("in"), true);
    BOOST_REQUIRE_EQUAL(d->existOutputPort("out"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "d",
                                                     "in"), true);

    //Atomic Model -- Input Port
    d->renameInputPort("in", "new_in");
    BOOST_REQUIRE_EQUAL(d->existInputPort("in"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "d",
                                                     "in"), false);
    BOOST_REQUIRE_EQUAL(d->existInputPort("new_in"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "d",
                                                     "new_in"), true);

    //Atomic Model -- Output Port
    d->renameOutputPort("out", "new_out");
    BOOST_REQUIRE_EQUAL(d->existOutputPort("out"), false);
    BOOST_REQUIRE_EQUAL(d->existOutputPort("new_out"), true);

    //Coupled Model
    CoupledModel* top1 =
        dynamic_cast<CoupledModel*>(top->findModel("top1"));
    BOOST_REQUIRE(top1);

    //Coupled Model -- Input Port
    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), true);
    BOOST_REQUIRE_EQUAL(top1->existInputConnection("in", "x", "in"), true);

    top1->renameInputPort("in", "new_in");
    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), false);
    BOOST_REQUIRE_EQUAL(top1->existInputConnection("in", "x", "in"), false);
    BOOST_REQUIRE_EQUAL(top1->existInputPort("new_in"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "new_in"), true);
    BOOST_REQUIRE_EQUAL(top1->existInputConnection("new_in", "x", "in"), true);
    BOOST_REQUIRE_EQUAL(top1->nbInputConnection("new_in", "x", "in"), 1);

    //Coupled Model -- Output Port
    BOOST_REQUIRE_EQUAL(top1->existOutputPort("out"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in1"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in2"), true);
    BOOST_REQUIRE_EQUAL(top1->existOutputConnection("x", "out", "out"),
                        true);

    top1->renameOutputPort("out", "new_out");
    BOOST_REQUIRE_EQUAL(top1->existOutputPort("out"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in1"), false);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "e",
                                                     "in2"), false);
    BOOST_REQUIRE_EQUAL(top1->existOutputConnection("x", "out", "out"),
                        false);
    BOOST_REQUIRE_EQUAL(top1->existOutputPort("new_out"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "new_out", "e",
                                                     "in1"), true);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "new_out", "e",
                                                     "in2"), true);
    BOOST_REQUIRE_EQUAL(top1->existOutputConnection("x", "out", "new_out"),
                        true);
    BOOST_REQUIRE_EQUAL(top1->nbOutputConnection("x", "out", "new_out"), 1);
}

BOOST_AUTO_TEST_CASE(test_bug_rename_port)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top =
        dynamic_cast<CoupledModel*>(file.project().model().model());
    BOOST_REQUIRE(top);
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top1", "out", "top2",
                                                     "in"), true);
    CoupledModel* top1 =
        dynamic_cast<CoupledModel*>(top->findModel("top1"));
    BOOST_REQUIRE(top1);

    CoupledModel* top2 =
        dynamic_cast<CoupledModel*>(top->findModel("top2"));
    BOOST_REQUIRE(top2);

    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), true);

    BOOST_REQUIRE_EQUAL(top2->existOutputPort("out"), true);
    top2->delOutputPort("out");
    BOOST_REQUIRE_EQUAL(top2->existOutputPort("out"), false);

    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), false);

    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), true);
    BOOST_REQUIRE_NO_THROW(top1->delInputPort("in"));
    BOOST_REQUIRE_EQUAL(top1->existInputPort("in"), false);

    BOOST_REQUIRE_EQUAL(top->existInternalConnection("top2", "out", "top1",
                                                     "in"), false);
}

BOOST_AUTO_TEST_CASE(test_bug_duplication_connections)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top =
        dynamic_cast<CoupledModel*>(file.project().model().model());
    BOOST_REQUIRE(top);

    AtomicModel* atom1(top->addAtomicModel("atom1"));
    AtomicModel* atom2(top->addAtomicModel("atom2"));
    CoupledModel* coupled = new CoupledModel("coupled", top);
    ModelList mSelectedModels;

    BOOST_REQUIRE(atom1);
    BOOST_REQUIRE(atom2);
    BOOST_REQUIRE(coupled);

    atom1->addOutputPort("out");
    atom2->addInputPort("in");

    BOOST_REQUIRE_EQUAL(atom1->existOutputPort("out"), true);
    BOOST_REQUIRE_EQUAL(atom2->existInputPort("in"), true);

    top->addInternalConnection("atom1", "out", "atom2", "in");
    BOOST_REQUIRE_EQUAL(top->existInternalConnection("atom1", "out", "atom2", "in"),
                                                     true);

    mSelectedModels[atom1->getName()] = atom1;
    mSelectedModels[atom2->getName()] = atom2;
    BOOST_REQUIRE_EQUAL(top->nbInternalConnection("atom1", "out", "atom2", "in"), 1);

    top->displace(mSelectedModels, coupled);
    BOOST_REQUIRE_EQUAL(coupled->nbInternalConnection("atom1", "out", "atom2", "in"), 1);
}

BOOST_AUTO_TEST_CASE(test_atomic_model_source)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top((file.project().model().model())->toCoupled());
    BOOST_REQUIRE(top);
    AtomicModel* e(top->findModel("e")->toAtomic());
    BOOST_REQUIRE(e);
    AtomicModel* d(top->findModel("d")->toAtomic());
    BOOST_REQUIRE(d);

    vpz::ModelPortList result;
    e->getAtomicModelsSource("in1", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    e->getAtomicModelsSource("in2", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    d->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();

    CoupledModel* top1(top->findModel("top1")->toCoupled());
    BOOST_REQUIRE(top1);

    AtomicModel* a(top1->findModel("a")->toAtomic());
    BOOST_REQUIRE(a);
    a->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    AtomicModel* b(top1->findModel("b")->toAtomic());
    BOOST_REQUIRE(b);
    b->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    AtomicModel* c(top1->findModel("c")->toAtomic());
    BOOST_REQUIRE(c);
    c->getAtomicModelsSource("in1", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in2", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();

    AtomicModel* x(top1->findModel("x")->toAtomic());
    BOOST_REQUIRE(x);
    x->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();

    CoupledModel* top2(top->findModel("top2")->toCoupled());
    BOOST_REQUIRE(top2);

    AtomicModel* f(top2->findModel("f")->toAtomic());
    BOOST_REQUIRE(f);
    AtomicModel* g(top2->findModel("g")->toAtomic());
    BOOST_REQUIRE(g);

    f->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    g->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
}

BOOST_AUTO_TEST_CASE(test_atomic_model_source_2)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top((file.project().model().model())->toCoupled());
    CoupledModel* top1(top->findModel("top1")->toCoupled());
    CoupledModel* top2(top->findModel("top2")->toCoupled());

    AtomicModel* e(top->findModel("e")->toAtomic());
    AtomicModel* d(top->findModel("d")->toAtomic());

    AtomicModel* a(top1->findModel("a")->toAtomic());
    AtomicModel* b(top1->findModel("b")->toAtomic());
    AtomicModel* c(top1->findModel("c")->toAtomic());
    AtomicModel* x(top1->findModel("x")->toAtomic());

    AtomicModel* f(top2->findModel("f")->toAtomic());
    AtomicModel* g(top2->findModel("g")->toAtomic());

    top2->addOutputConnection(f, "out", "out");
    top2->addOutputConnection(g, "out", "out");

    vpz::ModelPortList result;
    e->getAtomicModelsSource("in1", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    e->getAtomicModelsSource("in2", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    d->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();

    a->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    b->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in1", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in2", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();

    x->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)3);
    result.clear();

    f->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
    g->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
    result.clear();
}

BOOST_AUTO_TEST_CASE(test_atomic_model_source_3)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));

    CoupledModel* top((file.project().model().model())->toCoupled());
    CoupledModel* top2(top->findModel("top2")->toCoupled());
    vpz::ModelPortList result;

    top2->getAtomicModelsSource("in", result);
    BOOST_REQUIRE_EQUAL(result.size(), (size_t)1);
}

BOOST_AUTO_TEST_CASE(test_name)
{
    vpz::Vpz file(utils::Path::path().getTemplate("unittest.vpz"));
    BaseModel *a, *b;

    a = file.project().model().model()->findModelFromPath("top2,g");
    b = file.project().model().model()->findModelFromPath("top1,x");

    BOOST_REQUIRE(a);
    BOOST_REQUIRE(b);

    BOOST_REQUIRE_EQUAL(a->getCompleteName(), "top,top2,g");
    BOOST_REQUIRE_EQUAL(b->getCompleteName(), "top,top1,x");
}
