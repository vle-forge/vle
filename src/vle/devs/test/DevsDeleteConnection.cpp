/*
 * @file vle/devs/test/DevsDeleteConnection.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <vle/devs/Executive.hpp>

namespace vle { namespace unittest {

class DeleteConnection : public devs::Executive
{
public:
    DeleteConnection(const devs::ExecutiveInit& init,
                     const devs::InitEventList& events)
        : devs::Executive(init, events)
    {
       mAlarm = events.getDouble("alarm");
       mModelSource = events.getString("model source");
       mPortSource = events.getString("port source");
       mModelDest = events.getString("model dest");
       mPortDest = events.getString("port dest");

       mRemaining = mAlarm;
    }

    virtual ~DeleteConnection()
    {}

    virtual devs::Time init(const devs::Time& /*time*/)
    {
        return timeAdvance();
    }

    virtual devs::Time timeAdvance() const
    {
        return mRemaining;
    }

    virtual void internalTransition(const devs::Time& time)
    {
        devs::Time tmp = std::abs(std::min(time, mAlarm) -
                                  std::max(time, mAlarm));

       if (tmp < 0.1) {
            mRemaining = devs::Time::infinity;
            removeConnection(mModelSource, mPortSource, mModelDest, mPortDest);
        } else {
            mRemaining = tmp;
        }
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
                                    const devs::Time& time)
    {
        mRemaining = std::abs(std::min(time, mAlarm) - std::max(time, mAlarm));
    }

    devs::Time mAlarm;
    devs::Time mRemaining;
    devs::Time mLast;
    std::string mModelSource;
    std::string mPortSource;
    std::string mModelDest;
    std::string mPortDest;
};

}}

DECLARE_EXECUTIVE(vle::unittest::DeleteConnection)
