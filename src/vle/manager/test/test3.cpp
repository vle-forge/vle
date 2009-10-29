/**
 * @file vle/manager/test/test3.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#define BOOST_TEST_MODULE manager_common_seed_test

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/utils/Path.hpp>

struct F {
    F()
    {
        vle::manager::init();
    }

    ~F()
    {
        vle::manager::finalize();
    }

};
BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(common_seed_test){
    using namespace manager;

    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-meteo.vpz"));

    file.project().experiment().setCombination("linear");
    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(3);
    vle::vpz::Views& vle_views = file.project().experiment().views();
    vle::vpz::Output& view =
        vle_views.outputs().get(vle_views.get("view").output());
    if (view.plugin() != "storage") {
        view.setLocalStream("", "storage");
    }

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("petri").clearValueOfPort("day");
    cnds.get("petri").addValueToPort("day", value::Integer::create(3));
    cnds.get("petri").addValueToPort("day", value::Integer::create(3));

    typedef value::MatrixView::size_type tmp_type;

    //option common_seed activated
    bool writefile = false;
    bool storecomb = true;
    bool commonseed = true;
    ManagerRunMono r(std::cout, writefile, storecomb, commonseed);
    r.start(file);
    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
                        (OutputSimulationMatrix::size_type)3);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
                        (OutputSimulationMatrix::size_type)2);

    for (OutputSimulationMatrix::size_type x = 0; x < out.shape()[0]; ++x) {
        //combination 0 : common seed
        const oov::OutputMatrixViewList& outView0 =  out[x][0];
        BOOST_REQUIRE_EQUAL(outView0.size(),
                            (oov::OutputMatrixViewList::size_type)1);
        const oov::OutputMatrix& outMatrix0 = outView0.find("view")->second;
        const value::ConstVectorView& vec0 =
            outMatrix0.getValue("top:PetriNet", "token");
        BOOST_REQUIRE_EQUAL(vec0.size(), 31);
        //combination 1 : common seed
        const oov::OutputMatrixViewList& outView1 =  out[x][1];
        BOOST_REQUIRE_EQUAL(outView1.size(),
                            (oov::OutputMatrixViewList::size_type)1);
        const oov::OutputMatrix& outMatrix1 = outView1.find("view")->second;
        const value::ConstVectorView& vec1 =
            outMatrix1.getValue("top:PetriNet", "token");
        BOOST_REQUIRE_EQUAL(vec1.size(), 31);
        //comparison
        for (unsigned int v = 0; v < vec1.size(); v++) {
            vle::value::Value* val0 = vec0[v];
            vle::value::Value* val1 = vec1[v];
            BOOST_REQUIRE_EQUAL(val0->toInteger().intValue(),
                                val1->toInteger().intValue());
        }
    }
    //option common_seed not activated
    writefile = false;
    storecomb = true;
    commonseed = false;
    ManagerRunMono rb(std::cout, writefile, storecomb, commonseed);
    rb.start(file);
    const OutputSimulationMatrix& outb(rb.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(outb.shape()[0],
                        (OutputSimulationMatrix::size_type)3);
    BOOST_REQUIRE_EQUAL(outb.shape()[1],
                        (OutputSimulationMatrix::size_type)2);

    for (OutputSimulationMatrix::size_type x = 0; x < outb.shape()[0]; ++x) {
        //combination 0 : common seed
        const oov::OutputMatrixViewList& outView0 =  outb[x][0];
        BOOST_REQUIRE_EQUAL(outView0.size(),
                            (oov::OutputMatrixViewList::size_type)1);
        const oov::OutputMatrix& outMatrix0 = outView0.find("view")->second;
        const value::ConstVectorView& vec0 =
            outMatrix0.getValue("top:PetriNet", "token");
        BOOST_REQUIRE_EQUAL(vec0.size(), 31);
        //combination 1 : common seed
        const oov::OutputMatrixViewList& outView1 =  outb[x][1];
        BOOST_REQUIRE_EQUAL(outView1.size(),
                            (oov::OutputMatrixViewList::size_type)1);
        const oov::OutputMatrix& outMatrix1 = outView1.find("view")->second;
        const value::ConstVectorView& vec1 =
            outMatrix1.getValue("top:PetriNet", "token");
        BOOST_REQUIRE_EQUAL(vec1.size(), 31);
        //comparison
        bool equality = true;
        for (unsigned int v = 0; v < vec1.size(); v++) {
            vle::value::Value* val0 = vec0[v];
            vle::value::Value* val1 = vec1[v];
            equality = equality &&
                       (val0->toInteger().intValue() == val1->toInteger().intValue());
        }
        BOOST_REQUIRE_EQUAL(equality, false);
    }

    delete file.project().model().model();
}

