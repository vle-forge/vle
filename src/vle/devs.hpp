/** 
 * @file devs.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-08-13
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_DEVS_DEVS_HPP
#define VLE_DEVS_DEVS_HPP

#include <vle/devs/Attribute.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/DevsTypes.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Event.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/EventView.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/InstantaneousEvent.hpp>
#include <vle/devs/InstantaneousEventList.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/LocalStreamWriter.hpp>
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/NetStreamWriter.hpp>
#include <vle/devs/Observable.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/StateEvent.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <vle/devs/TimedView.hpp>
#include <vle/devs/Time.hpp>
#include <vle/devs/View.hpp>



namespace vle {

    /** 
     * @brief The devs namespace represents the DEVS Kernel simulator with B.P.
     * Zeigler algorithm for Simulator, Coordinator, RootCoordinator. Some
     * classes are provides like Time, Event to simplify the development of new
     * models. To build new DEVS models, you must inherit of the devs::Dynamics
     * or the devs::Executive (the F.Barros Dynamics Structures DEVS). If you
     * want to use CellDEVS, QSS, QSS2, CellQSS and other, see the
     * vle::extension namespace.
     */
    namespace devs {

    } // namespace devs

} // namespace vle

#endif
