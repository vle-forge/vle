/** 
 * @file FinishView.hpp
 * @author The vle Development Team
 * @date 2008-01-11
 */

/*
 * Copyright (C) 2008 - The vle Development Team
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

#ifndef VLE_DEVS_FINISH_VIEW_HPP
#define VLE_DEVS_FINISH_VIEW_HPP

#include <vle/devs/View.hpp>
#include <vle/devs/ObservationEvent.hpp>

namespace vle { namespace devs {

    /** 
     * @brief Define a Finish view based on devs::View class. This class build
     * state event only at the end of the simulation.
     */
    class FinishView : public View
    {
    public:
        FinishView(const std::string& name, StreamWriter* stream);

        virtual ~FinishView() { }

        virtual bool isFinish() const
        { return true; }

        virtual devs::ObservationEvent*
            processObservationEvent(devs::ObservationEvent* event);
    };

    typedef std::map < std::string, devs::FinishView* > FinishViewList;

}} // namespace vle devs

#endif
