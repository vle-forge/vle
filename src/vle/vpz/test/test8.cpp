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
#include <limits>
#include <stdexcept>
#include <vle/utils/Context.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Value.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;
using namespace vpz;

void
test_rename_model()
{
    auto ctx = vle::utils::make_context();

    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(top);

    if (not top)
        return;

    EnsuresNotThrow(vpz::BaseModel::rename(top, "new_top"), std::exception);

    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    Ensures(d);

    if (not d)
        return;

    EnsuresThrow(vpz::BaseModel::rename(d, "e"), utils::DevsGraphError);
    EnsuresNotThrow(vpz::BaseModel::rename(d, "new_d"), std::exception);

    CoupledModel* top1 = dynamic_cast<CoupledModel*>(top->findModel("top1"));
    Ensures(top1);

    if (not top1)
        return;

    EnsuresThrow(vpz::BaseModel::rename(top1, "top2"), utils::DevsGraphError);
    EnsuresNotThrow(vpz::BaseModel::rename(top1, "new_top1"), std::exception);
}

void
test_findModelFromPath()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");
    BaseModel* d = file.project().model().node()->findModelFromPath("d");
    bool found = (d != nullptr);

    EnsuresEqual(found, true);
    BaseModel* top1 = file.project().model().node()->findModelFromPath("top1");
    found = (top1 != nullptr);
    EnsuresEqual(found, true);
    BaseModel* top2_g =
      file.project().model().node()->findModelFromPath("top2,g");
    found = (top2_g != nullptr);
    EnsuresEqual(found, true);
    BaseModel* nomodel =
      file.project().model().node()->findModelFromPath("nomodel");
    found = (nomodel != nullptr);
    EnsuresEqual(found, false);
    BaseModel* empty = file.project().model().node()->findModelFromPath("");
    found = (empty != nullptr);
    EnsuresEqual(found, false);
    BaseModel* top1_a_nomodel =
      file.project().model().node()->findModelFromPath("top1,a,nomodel");
    found = (top1_a_nomodel != nullptr);
    EnsuresEqual(found, false);
}

void
test_del_all_connection()
{
    CoupledModel* top = new CoupledModel("top", nullptr);

    AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    Ensures(top->existInternalConnection("a", "out", "b", "in"));
    Ensures(top->existInternalConnection("b", "out", "a", "in"));
    top->delAllConnection();
    Ensures(not top->existInternalConnection("a", "out", "b", "in"));
    Ensures(not top->existInternalConnection("b", "out", "a", "in"));

    delete top;
}

void
test_have_connection()
{
    CoupledModel* top = new CoupledModel("top", nullptr);

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

    Ensures(not top->hasConnectionProblem(lst));

    top->addInternalConnection("c", "out", "a", "in");

    Ensures(top->hasConnectionProblem(lst));

    delete top;
}

void
test_displace()
{
    CoupledModel* top = new CoupledModel("top", nullptr);

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

    CoupledModel* newtop = new CoupledModel("newtop", nullptr);

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    top->displace(lst, newtop);

    EnsuresEqual(top->getModelList().size(), (ModelList::size_type)1);

    EnsuresEqual(newtop->getModelList().size(), (ModelList::size_type)2);

    Ensures(newtop->existInternalConnection("a", "out", "b", "in"));
    Ensures(newtop->existInternalConnection("b", "out", "a", "in"));

    delete top;
    delete newtop;
}

void
test_complex_displace()
{
    CoupledModel* top = new CoupledModel("top", nullptr);

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

    EnsuresEqual(top->getModelList().size(), (ModelList::size_type)3);

    CoupledModel* newtop = new CoupledModel("newtop", top);

    ModelList lst;
    lst["a"] = a;
    lst["b"] = b;

    EnsuresNotThrow(top->displace(lst, newtop), std::exception);

    EnsuresEqual(top->getModelList().size(), (ModelList::size_type)2);
    EnsuresEqual(newtop->getModelList().size(), (ModelList::size_type)2);

    newtop->existInternalConnection("a", "out", "b", "in");
    newtop->existInternalConnection("b", "out", "a", "in");
    newtop->existOutputPort("out");
    top->existInternalConnection("newtop", "out", "c", "in");

    delete top;
}

void
test_delinput_port()
{
    CoupledModel* top = new CoupledModel("top", nullptr);

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

    EnsuresEqual(top->existInternalConnection("a", "out", "b", "in"), true);
    EnsuresEqual(top->existInternalConnection("b", "out", "a", "in"), true);
    EnsuresEqual(top->existInternalConnection("a", "out", "c", "in"), true);
    c->delInputPort("in");

    EnsuresEqual(top->existInputPort("in"), false);
    EnsuresEqual(top->existInternalConnection("a", "out", "c", "in"), false);

    delete top;
}

void
test_del_port()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(top);
    if (not top)
        return;
    EnsuresEqual(top->existInternalConnection("top1", "out", "top2", "in"),
                 true);

    // Atomic Model
    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    Ensures(d);
    if (not d)
        return;

    EnsuresEqual(d->existInputPort("in"), true);
    EnsuresEqual(d->existOutputPort("out"), true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "d", "in"), true);

    // Atomic Model -- Input Port
    d->delInputPort("in");
    EnsuresEqual(d->existInputPort("in"), false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "d", "in"),
                 false);

    // Atomic Model -- Output Port
    d->delOutputPort("out");
    EnsuresEqual(d->existOutputPort("out"), false);

    // Coupled Model
    CoupledModel* top1 = dynamic_cast<CoupledModel*>(top->findModel("top1"));
    Ensures(top1);
    if (not top1)
        return;

    // Coupled Model -- Input Port
    EnsuresEqual(top1->existInputPort("in"), true);
    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 true);
    EnsuresEqual(top1->existInputConnection("in", "x", "in"), true);

    top1->delInputPort("in");
    EnsuresEqual(top1->existInputPort("in"), false);
    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 false);
    EnsuresEqual(top1->existInputConnection("in", "x", "in"), false);

    // Coupled Model -- Output Port
    EnsuresEqual(top1->existOutputPort("out"), true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in1"),
                 true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in2"),
                 true);
    EnsuresEqual(top1->existOutputConnection("x", "out", "out"), true);

    top1->delOutputPort("out");
    EnsuresEqual(top1->existOutputPort("out"), false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in1"),
                 false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in2"),
                 false);
    EnsuresEqual(top1->existOutputConnection("x", "out", "out"), false);
}

void
test_clone1()
{
    auto top = std::make_unique<CoupledModel>("top", nullptr);
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

    Ensures(top->existInternalConnection("top", "out", "b", "in"));
    Ensures(top->existInternalConnection("b", "out", "top", "in"));

    auto newtop =
      std::unique_ptr<CoupledModel>(dynamic_cast<CoupledModel*>(top->clone()));

    Ensures(newtop != nullptr);
    if (not newtop)
        return;

    Ensures(newtop->getModelList().size() == 2);

    AtomicModel* newa = dynamic_cast<AtomicModel*>(newtop->findModel("top"));
    Ensures(newa != a);
    AtomicModel* newb = dynamic_cast<AtomicModel*>(newtop->findModel("b"));
    Ensures(newb != b);

    Ensures(newtop->existInternalConnection("top", "out", "b", "in"));
    Ensures(newtop->existInternalConnection("b", "out", "top", "in"));
    Ensures(newtop->existInputConnection("in", "top", "in"));
    Ensures(newtop->existOutputConnection("top", "out", "out"));
    newtop->delAllConnection();
    Ensures(not newtop->existOutputConnection("top", "out", "out"));
    Ensures(not newtop->existInternalConnection("top", "out", "b", "in"));
    Ensures(not newtop->existInternalConnection("top", "out", "b", "in"));
    Ensures(not newtop->existInternalConnection("b", "out", "top", "in"));
    Ensures(top->existInternalConnection("top", "out", "b", "in"));
    Ensures(top->existInternalConnection("b", "out", "top", "in"));
    Ensures(top->existInputConnection("in", "top", "in"));
    Ensures(top->existOutputConnection("top", "out", "out"));
}

void
test_clone2()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* oldtop =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(oldtop);

    auto top = std::unique_ptr<CoupledModel>(
      dynamic_cast<CoupledModel*>(oldtop->clone()));

    Ensures(top);
    if (not top)
        return;

    CoupledModel* top1(dynamic_cast<CoupledModel*>(top->findModel("top1")));
    Ensures(top1);
    if (not top1)
        return;

    CoupledModel* top2(dynamic_cast<CoupledModel*>(top->findModel("top2")));
    Ensures(top2);
    if (not top2)
        return;

    AtomicModel* f(dynamic_cast<AtomicModel*>(top2->findModel("f")));
    Ensures(f);
    AtomicModel* g(dynamic_cast<AtomicModel*>(top2->findModel("g")));
    Ensures(g);
}

void
test_clone_different_atomic()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file1(VPZ_TEST_DIR "/unittest.vpz");

    vpz::Vpz file2(file1);

    CoupledModel* top1 =
      dynamic_cast<CoupledModel*>(file1.project().model().node());
    CoupledModel* top2 =
      dynamic_cast<CoupledModel*>(file2.project().model().node());

    Ensures(top1 and top2);
    Ensures(top1 != top2);

    if (not top1 or not top2)
        return;

    AtomicModelVector list1, list2;

    BaseModel::getAtomicModelList(top1, list1);
    BaseModel::getAtomicModelList(top2, list2);

    AtomicModelVector intersection;

    std::sort(list1.begin(), list1.end());
    std::sort(list2.begin(), list2.end());

    AtomicModelVector::iterator it;

    it = std::search(list1.begin(), list1.end(), list2.begin(), list2.end());

    Ensures(it == list1.end());
}

void
test_get_port_index()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());

    Ensures(top);
    if (not top)
        return;

    EnsuresEqual(top->getInputPortList().size(), (ConnectionList::size_type)0);
    EnsuresEqual(top->getOutputPortList().size(),
                 (ConnectionList::size_type)0);

    AtomicModel* e = dynamic_cast<AtomicModel*>(top->getModelList()["e"]);
    Ensures(e);
    if (not e)
        return;

    EnsuresEqual(e->getInputPortList().size(), (ConnectionList::size_type)2);
    EnsuresEqual(e->getInputPortIndex("in1"), 0);
    EnsuresEqual(e->getInputPortIndex("in2"), 1);
    EnsuresEqual(e->getOutputPortList().size(), (ConnectionList::size_type)1);
}

void
test_rename_port()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(top);
    if (not top)
        return;

    EnsuresEqual(top->existInternalConnection("top1", "out", "top2", "in"),
                 true);

    // Atomic Model
    AtomicModel* d = dynamic_cast<AtomicModel*>(top->findModel("d"));
    Ensures(d);
    if (not d)
        return;

    EnsuresEqual(d->existInputPort("in"), true);
    EnsuresEqual(d->existOutputPort("out"), true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "d", "in"), true);

    // Atomic Model -- Input Port
    d->renameInputPort("in", "new_in");
    EnsuresEqual(d->existInputPort("in"), false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "d", "in"),
                 false);
    EnsuresEqual(d->existInputPort("new_in"), true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "d", "new_in"),
                 true);

    // Atomic Model -- Output Port
    d->renameOutputPort("out", "new_out");
    EnsuresEqual(d->existOutputPort("out"), false);
    EnsuresEqual(d->existOutputPort("new_out"), true);

    // Coupled Model
    CoupledModel* top1 = dynamic_cast<CoupledModel*>(top->findModel("top1"));
    Ensures(top1);
    if (not top1)
        return;

    // Coupled Model -- Input Port
    EnsuresEqual(top1->existInputPort("in"), true);
    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 true);
    EnsuresEqual(top1->existInputConnection("in", "x", "in"), true);

    top1->renameInputPort("in", "new_in");
    EnsuresEqual(top1->existInputPort("in"), false);
    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 false);
    EnsuresEqual(top1->existInputConnection("in", "x", "in"), false);
    EnsuresEqual(top1->existInputPort("new_in"), true);
    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "new_in"),
                 true);
    EnsuresEqual(top1->existInputConnection("new_in", "x", "in"), true);
    EnsuresEqual(top1->nbInputConnection("new_in", "x", "in"), 1);

    // Coupled Model -- Output Port
    EnsuresEqual(top1->existOutputPort("out"), true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in1"),
                 true);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in2"),
                 true);
    EnsuresEqual(top1->existOutputConnection("x", "out", "out"), true);

    top1->renameOutputPort("out", "new_out");
    EnsuresEqual(top1->existOutputPort("out"), false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in1"),
                 false);
    EnsuresEqual(top->existInternalConnection("top1", "out", "e", "in2"),
                 false);
    EnsuresEqual(top1->existOutputConnection("x", "out", "out"), false);
    EnsuresEqual(top1->existOutputPort("new_out"), true);
    EnsuresEqual(top->existInternalConnection("top1", "new_out", "e", "in1"),
                 true);
    EnsuresEqual(top->existInternalConnection("top1", "new_out", "e", "in2"),
                 true);
    EnsuresEqual(top1->existOutputConnection("x", "out", "new_out"), true);
    EnsuresEqual(top1->nbOutputConnection("x", "out", "new_out"), 1);
}

void
test_bug_rename_port()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(top);
    if (not top)
        return;

    EnsuresEqual(top->existInternalConnection("top1", "out", "top2", "in"),
                 true);
    CoupledModel* top1 = dynamic_cast<CoupledModel*>(top->findModel("top1"));
    Ensures(top1);
    if (not top1)
        return;

    CoupledModel* top2 = dynamic_cast<CoupledModel*>(top->findModel("top2"));
    Ensures(top2);
    if (not top2)
        return;

    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 true);

    EnsuresEqual(top2->existOutputPort("out"), true);
    top2->delOutputPort("out");
    EnsuresEqual(top2->existOutputPort("out"), false);

    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 false);

    EnsuresEqual(top1->existInputPort("in"), true);
    EnsuresNotThrow(top1->delInputPort("in"), std::exception);
    EnsuresEqual(top1->existInputPort("in"), false);

    EnsuresEqual(top->existInternalConnection("top2", "out", "top1", "in"),
                 false);
}

void
test_bug_duplication_connections()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top =
      dynamic_cast<CoupledModel*>(file.project().model().node());
    Ensures(top);
    if (not top)
        return;

    AtomicModel* atom1(top->addAtomicModel("atom1"));
    AtomicModel* atom2(top->addAtomicModel("atom2"));
    CoupledModel* coupled = new CoupledModel("coupled", top);
    ModelList mSelectedModels;

    Ensures(atom1);
    Ensures(atom2);
    Ensures(coupled);

    if (not atom1 or not atom2 or not coupled)
        return;

    atom1->addOutputPort("out");
    atom2->addInputPort("in");

    EnsuresEqual(atom1->existOutputPort("out"), true);
    EnsuresEqual(atom2->existInputPort("in"), true);

    top->addInternalConnection("atom1", "out", "atom2", "in");
    EnsuresEqual(top->existInternalConnection("atom1", "out", "atom2", "in"),
                 true);

    mSelectedModels[atom1->getName()] = atom1;
    mSelectedModels[atom2->getName()] = atom2;
    EnsuresEqual(top->nbInternalConnection("atom1", "out", "atom2", "in"), 1);

    top->displace(mSelectedModels, coupled);
    EnsuresEqual(coupled->nbInternalConnection("atom1", "out", "atom2", "in"),
                 1);
}

void
test_atomic_model_source()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top((file.project().model().node())->toCoupled());
    Ensures(top);
    if (not top)
        return;

    AtomicModel* e(top->findModel("e")->toAtomic());
    Ensures(e);
    AtomicModel* d(top->findModel("d")->toAtomic());
    Ensures(d);

    if (not e or not d)
        return;

    vpz::ModelPortList result;
    e->getAtomicModelsSource("in1", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    e->getAtomicModelsSource("in2", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    d->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();

    CoupledModel* top1(top->findModel("top1")->toCoupled());
    Ensures(top1);
    if (not top1)
        return;

    AtomicModel* a(top1->findModel("a")->toAtomic());
    Ensures(a);
    if (not a)
        return;

    a->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    AtomicModel* b(top1->findModel("b")->toAtomic());
    Ensures(b);
    if (not b)
        return;

    b->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    AtomicModel* c(top1->findModel("c")->toAtomic());
    Ensures(c);
    if (not c)
        return;

    c->getAtomicModelsSource("in1", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in2", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();

    AtomicModel* x(top1->findModel("x")->toAtomic());
    Ensures(x);
    if (not x)
        return;

    x->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();

    CoupledModel* top2(top->findModel("top2")->toCoupled());
    Ensures(top2);
    if (not top2)
        return;

    AtomicModel* f(top2->findModel("f")->toAtomic());
    Ensures(f);
    AtomicModel* g(top2->findModel("g")->toAtomic());
    Ensures(g);

    if (not f or not g)
        return;

    f->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    g->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
}

void
test_atomic_model_source_2()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top((file.project().model().node())->toCoupled());
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
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    e->getAtomicModelsSource("in2", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    d->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();

    a->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    b->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in1", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    c->getAtomicModelsSource("in2", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();

    x->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)3);
    result.clear();

    f->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
    g->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
    result.clear();
}

void
test_atomic_model_source_3()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");

    CoupledModel* top((file.project().model().node())->toCoupled());
    CoupledModel* top2(top->findModel("top2")->toCoupled());
    vpz::ModelPortList result;

    top2->getAtomicModelsSource("in", result);
    EnsuresEqual(result.size(), (size_t)1);
}

void
test_name()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz file(VPZ_TEST_DIR "/unittest.vpz");
    BaseModel *a, *b;

    a = file.project().model().node()->findModelFromPath("top2,g");
    b = file.project().model().node()->findModelFromPath("top1,x");

    Ensures(a);
    Ensures(b);

    if (not a or not b)
        return;

    EnsuresEqual(a->getCompleteName(), "top,top2,g");
    EnsuresEqual(b->getCompleteName(), "top,top1,x");
}

int
main()
{
    vle::Init app;

    test_rename_model();
    test_findModelFromPath();
    test_del_all_connection();
    test_have_connection();
    test_displace();
    test_complex_displace();
    test_delinput_port();
    test_del_port();
    test_clone1();
    test_clone2();
    test_clone_different_atomic();
    test_get_port_index();
    test_rename_port();
    test_bug_rename_port();
    test_bug_duplication_connections();
    test_atomic_model_source();
    test_atomic_model_source_2();
    test_atomic_model_source_3();
    test_name();

    return unit_test::report_errors();
}
