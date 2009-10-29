/**
 * @file vle/devs.hpp
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


#ifndef VLE_DEVS_DEVS_HPP
#define VLE_DEVS_DEVS_HPP

#include <vle/devs/Attribute.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Event.hpp>
#include <vle/devs/EventList.hpp>
#include <vle/devs/EventTable.hpp>
#include <vle/devs/EventView.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/FinishView.hpp>
#include <vle/devs/InternalEvent.hpp>
#include <vle/devs/LocalStreamWriter.hpp>
#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/NetStreamWriter.hpp>
#include <vle/devs/Observable.hpp>
#include <vle/devs/ObservationEvent.hpp>
#include <vle/devs/RequestEvent.hpp>
#include <vle/devs/RequestEventList.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Simulator.hpp>
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
