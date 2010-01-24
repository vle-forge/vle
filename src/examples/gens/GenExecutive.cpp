/**
 * @file examples/gens/GenExecutive.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <GenExecutive.hpp>
#include <vle/utils.hpp>
#include <vle/devs.hpp>
#include <vle/vpz.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace examples { namespace gens {

GenExecutive::GenExecutive(const devs::ExecutiveInit& mdl,
                           const devs::InitEventList& events) :
    devs::Executive(mdl, events)
{
}

devs::Time GenExecutive::init(const devs::Time& /* time */)
{
    vpz::Dynamic dyn("gensbeep");
    dyn.setLibrary("gens");
    dyn.setModel("beep");
    dyn.setLocalDynamics();
    dynamics().add(dyn);

    m_state = INIT;

    return 0.0;
}

devs::Time GenExecutive::timeAdvance() const
{
    switch (m_state) {
    case INIT:
        return 0.0;
    case IDLE:
        return 0.0;
    case ADDMODEL:
        return 1.0;
    case DELMODEL:
        return 1.0;
    }
    throw utils::InternalError("GenExecutive ta");
}

void GenExecutive::internalTransition(const devs::Time& time)
{
    switch (m_state) {
    case INIT:
        m_state = IDLE;
        break;
    case IDLE:
        if (time < 50.0) {
            m_state = ADDMODEL;
        }
        else {
            m_state = DELMODEL;
        }
        break;
    case ADDMODEL:
        add_new_model();
        m_state = IDLE;
        break;
    case DELMODEL:
        del_first_model();
        m_state = IDLE;
        break;
    }
}

value::Value* GenExecutive::observation(const devs::ObservationEvent& ev) const
{
    if (ev.onPort("nbmodel")) {
        return value::Integer::create(get_nb_model());
    } else if (ev.onPort("structure")) {
        std::ostringstream out;
        coupledmodel().writeXML(out);
        return value::String::create(out.str());
    } else if (ev.onPort("adjacency_matrix")) {
        value::Set *set = value::Set::create();
        set->addCloneValue(*value::Integer::create(1));
        if(get_nb_model() > 0 and ev.getTime() < 50.0){
            set->addCloneValue(*value::String::create("add"));
            std::string name = (fmt("beep_%1%") % m_stacknames.size()).str();
            set->addCloneValue(*value::String::create(name));
            set->addCloneValue(*value::String::create("2"));
            std::string edge =  name + std::string(" counter ");
            set->addCloneValue(*value::String::create(edge));
        }
        else if(get_nb_model() > 0){
            set->addCloneValue(*value::String::create("delete"));
            std::string name = (fmt(
                    "beep_%1%") % (get_nb_model())).str();
            set->addCloneValue(*value::String::create(name));
        }

        return set;
    } else if (ev.onPort("value"))
        return value::Integer::create(1);

    return devs::Executive::observation(ev);
}

//
///
////
///
//

void GenExecutive::add_new_model()
{
    std::string name((fmt("beep_%1%") % m_stacknames.size()).str());

    std::vector < std::string > outputs;
    outputs.push_back("out");

    createModel(name, std::vector < std::string >(), outputs, "gensbeep",
                vpz::Strings(), "");
    addConnection(name, "out", "counter", "in");

    m_stacknames.push(name);
}

void GenExecutive::del_first_model()
{
    if (m_stacknames.empty()) {
        throw utils::InternalError(fmt(
                "Cannot delete any model, the executive have no "
                "element.").str());
    }

    delModel(m_stacknames.top());
    m_stacknames.pop();
}

int GenExecutive::get_nb_model() const
{
    return boost::numeric_cast < int >(coupledmodel().getModelList().size()) - 1;
}

}}} // namespace vle examples gens
