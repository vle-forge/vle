/**
 * @file devs/sModel.hpp
 * @author The VLE Development Team.
 * @brief
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_SMODEL_HPP
#define DEVS_SMODEL_HPP

#include <vle/devs/InitEvent.hpp>
#include <vle/devs/StateEvent.hpp>

namespace vle { namespace devs {

    class Simulator;
    class StateEvent;

    class sModel
    {
    public:
	sModel(Simulator* simulator);

	virtual ~sModel();

        Simulator* getSimulator() const;

	long getSimulatorIndex() const;

        virtual bool isAtomic() const =0;

	virtual bool isCoupled() const =0;

        virtual void processInitEvent(InitEvent* event)=0;

	virtual StateEvent* processStateEvent(StateEvent* event) const =0;

    private:
	Simulator* m_simulator;
    };

}} // namespace vle devs

#endif
