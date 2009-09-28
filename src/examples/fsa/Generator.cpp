/**
 * @file examples/fsa/Generator.cpp
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

#include <vle/devs/Dynamics.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace examples { namespace fsa {

namespace vd = vle::devs;

class generator : public vd::Dynamics
{
public:
    generator(const vd::DynamicsInit& init,
              const vd::InitEventList& events) :
        vd::Dynamics(init, events)
    {
        int size = toInteger(events.get("size"));

        for (int i = 0; i < size; ++i) {
            mSigmas.push_back(0);
            mDates.push_back(0);
        }
    }

    virtual ~generator()
    { }

    virtual vd::Time init(const vd::Time& /* time */)
    {
        {
            std::vector < double >::iterator jt = mDates.begin();

            for (std::vector < vd::Time >::iterator it = mSigmas.begin();
                 it != mSigmas.end(); ++it, ++jt) {
                (*it) = rand().getDouble(2, 5);
                (*jt) = *it;
            }
        }

        std::vector < vd::Time >::const_iterator it =
            std::min_element(mSigmas.begin(), mSigmas.end());
        return *it;
    }

    virtual vd::Time timeAdvance() const
    {
        std::vector < vd::Time >::const_iterator it =
            std::min_element(mSigmas.begin(), mSigmas.end());

        return *it;
    }

    virtual void output(const vd::Time& /* time */,
                        vd::ExternalEventList& outputs) const
    {
        std::vector < vd::Time >::const_iterator it =
            std::min_element(mSigmas.begin(), mSigmas.end());
        int index = boost::lexical_cast < int >(
            std::distance(mSigmas.begin(), it)) + 1;

        outputs.addEvent(
            buildEvent((boost::format("out%1%") % index).str()));
    }

    virtual void internalTransition(const vd::Time& time)
    {
        std::vector < vd::Time >::iterator it =
            std::min_element(mSigmas.begin(), mSigmas.end());

        (*it) = rand().getDouble(2, 5);
        mDates[std::distance(mSigmas.begin(), it)] = time + *it;
        for (std::vector < vd::Time >::iterator it2 = mSigmas.begin();
             it2 != mSigmas.end(); ++it2) {
            if (it2 != it) {
                (*it2) -= time - mLastTime;
            }
        }
        mLastTime = time;
    }

    virtual void externalTransition(
        const vd::ExternalEventList& /*event*/,
        const vd::Time& time)
    {
        for (std::vector < vd::Time >::iterator it = mSigmas.begin();
             it != mSigmas.end(); ++it) {
            (*it) -= time - mLastTime;
        }
        mLastTime = time;
    }

    virtual vle::value::Value* observation(
        const vd::ObservationEvent& event) const
    {
        std::string sub(event.getPortName(), 0, 4);

        if (sub == "date") {
            int index = boost::lexical_cast < int >(
                std::string(event.getPortName(), 4,
                            event.getPortName().size() - 4)) - 1;

            return vle::value::Double::create(mDates.at(index));
        } else {
            return 0;
        }
    }

private:
    std::vector < vd::Time > mSigmas;
    std::vector < double > mDates;
    vd::Time mLastTime;
};

DECLARE_NAMED_DYNAMICS(generator, generator)

}}} // namespace vle examples fsa
