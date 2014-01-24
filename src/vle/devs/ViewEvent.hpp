/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#ifndef VLE_DEVS_VIEWEVENT_HPP
#define VLE_DEVS_VIEWEVENT_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/devs/View.hpp>
#include <vector>

namespace vle { namespace devs {

/**
 * ViewEvent is used in scheduller to store the date to launch observation of
 * atomic models.
 */
class VLE_API ViewEvent
{
public:
    /**
     * @brief Build a new ViewEvent to be store into the scheduller.
     *
     * @param view The View to store.
     * @param time When wake up the View.
     */
    ViewEvent(View* view, const Time& time)
        : mView(*view), mTime(time)
    {
        assert(view);
    }

    /**
     * @brief Build a new ViewEvent to be store into the scheduller.
     *
     * @param view The View to store.
     * @param time When wake up the View.
     */
    ViewEvent(View& view, const Time& time)
        : mView(view), mTime(time)
    {}

    //
    //

    /**
     * @brief Call for each atomic models (devs::Dynamics) attached to this
     * view, the observation function.
     *
     * @return The next date of the futur
     */
    void run(const Time& time) { mView.run(time); }

    /**
     * @brief Update the next wake-up time.
     */
    void update(const Time& current) { mTime = mView.getNextTime(current); }

    //
    //

    /**
     * @brief Get a reference to the View.
     *
     * @return A valid View.
     */
    View& getView() const { return mView; }

    /**
     * @brief Get the next time to wake up the View.
     *
     * @return An absolute date.
     */
    const Time& getTime() const { return mTime; }

    /**
     * Inferior comparator use Time as key.
     * @param event Event to test, no test on validity.
     * @return true if this Event is inferior to event.
     */
    inline bool operator<(const ViewEvent* event) const
    { return mTime < event->mTime; }

    /**
     * Superior comparator use Time as key.
     * @param event Event to test, no test on validity.
     * @return true if this Event is superior to event.
     */
    inline bool operator>(const ViewEvent* event) const
    { return mTime > event->mTime; }

    /**
     * Equality comparator use Time as key.
     * @param event Event to test, no test on validity.
     * @return true if this Event is equal to  event.
     */
    inline bool operator==(const ViewEvent * event) const
    { return mTime == event->mTime; }

private:
    View& mView;
    Time  mTime;
};

class ViewEventList
{
public:
    typedef std::vector < ViewEvent* > value_type;
    typedef value_type::iterator iterator;
    typedef value_type::const_iterator const_iterator;
    typedef value_type::size_type size_type;

    void add(ViewEvent* event)
    {
        mElems.push_back(event);
    }

    void pop()
    {
        mElems.pop_back();
    }

    ViewEvent* front()
    {
        checkRange(0);
        return mElems[0];
    }

    devs::Time time()
    {
        return front()->getTime();
    }

    void clear()
    {
        mElems.clear();
    }

    void erase()
    {
        std::for_each(mElems.begin(), mElems.end(),
                      boost::checked_deleter < ViewEvent >());

        mElems.clear();
    }

    void insert(iterator first, iterator last)
    {
        mElems.insert(begin(), first, last);
    }

    void remove(Simulator* sim)
    {
        std::for_each(mElems.begin(), mElems.end(),
                      ViewEventList::RemoveSimulator(sim));
    }

    iterator begin() { return mElems.begin(); }
    const_iterator begin() const { return mElems.begin(); }
    iterator end() { return mElems.end(); }
    const_iterator end() const { return mElems.end(); }
    bool empty() const { return mElems.empty(); }
    size_type size() const { return mElems.size(); }

    struct RemoveSimulator
    {
        RemoveSimulator(Simulator* sim) : mSim(sim) {}

        void operator()(ViewEvent* view)
        {
            view->getView().removeObservable(mSim);
        }

        Simulator* mSim;
    };

private:
    value_type mElems;

    void checkRange(size_type i)
    {
        if (i >= size()) {
            throw utils::InternalError("ViewEvent: index out of range");
        }
    }
};


}} // namespace vle devs

#endif

