/**
 * @file src/examples/lifegame/Cell.hpp
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


#ifndef MODEL_LIFEGAME_CELL_HPP
#define MODEL_LIFEGAME_CELL_HPP

#include <vle/devs.hpp>
#include <vle/extension.hpp>

namespace model { namespace lifegame {

class Cell : public vle::extension::CellDevs
{
public:
    Cell(const vle::graph::AtomicModel& model,
         const vle::devs::InitEventList& events) :
        vle::extension::CellDevs(model, events)
    { }

    virtual ~Cell() { }
    
    // DEVS Methods
    virtual vle::devs::Time init(const vle::devs::Time& /* time */);
    virtual void internalTransition(const vle::devs::Time& /* event */);
    virtual void processPerturbation(const vle::devs::ExternalEvent& /* event */) { }
    
private:
    enum state { INIT, IDLE, NEWSTATE };
    
    double mTimeStep;
    state mState;      
    
    virtual void updateSigma(vle::devs::Event*) { setSigma(0); };
};

}}

DECLARE_NAMED_DYNAMICS(cell, model::lifegame::Cell);

#endif
