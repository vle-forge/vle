/**
 * @file src/vle/extension/TimedPetriNet.hpp
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

#ifndef VLE_EXTENSION_TIMEDPETRINET_HPP
#define VLE_EXTENSION_TIMEDPETRINET_HPP

#include <vle/extension/PetriNet.hpp>

namespace vle { namespace extension {



    /**
     * @brief
     *
     * @code
     * <condition>
     *  <port name="dynamics">
     *   <string>timed</string>
     *  </port>
     *  <port name="places">
     *   <set>
     *    <set>
     *     <string>name</string>
     *     <string>input|output|internal</string>
     *     <string>portName</string>
     *     <integer>tokenNumber</string>
     *    </set>
     *   </set>
     *  </port>
     *  <port name="transitions">
     *   <set>
     *    <set>
     *     <string>name</string>
     *     <string>input|output|internal</string>
     *     <double>delay</double>
     *     <string>portName</string>
     *    </set>
     *   </set>
     *  </port>
     *  <port name="arcs">
     *   <set>
     *    <set>
     *     <string>placeName</string>
     *     <string>transitionName</string>
     *     <string>input|output</string>
     *     <integer>tokenNumber</integer>
     *    </set>
     *   <set>
     *  </port>
     *  <port name="initialMarking">
     *   <set>
     *    <set>
     *     <string>placeName</string>
     *     <integer>tokenNumber</integer>
     *    </set>
     *   </set>
     *  </port>
     * </condition>
     * @endcode
     */
    class TimedPetriNet : public extension::PetriNet
    {
    public:
        TimedPetriNet(const graph::AtomicModel& model,
                      const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        { }

        virtual ~TimedPetriNet()
        { }

    private:
        class TimedToken;
        class TimedTransition;

        virtual void initParameters()
        { }	

        virtual void initTransitions(const value::VectorValue& transitions);

        virtual devs::Time start(const devs::Time& time);

        virtual void run(const devs::Time& time);

        virtual void computeEnabledTransition(const devs::Time& time);

        devs::Time getNextValid(Marking* marking,
                                unsigned int tokenNumber);

        devs::Time isEnabled(TimedTransition* transition);

        virtual void putToken(Marking* marking, const devs::Time& time)
        { marking->addToken(new TimedToken(time)); }	

        virtual bool removeTokens(Marking* marking, unsigned int tokenNumber);

        virtual void updateSigma(const devs::Time& /* time */)
        { }

        class TimedToken : public Token
        {
        public:
            TimedToken(const devs::Time& time) : mTime(time)
            { }

            virtual ~TimedToken()
            { }

            const devs::Time& getTime() const
            { return mTime; }

        private:
            devs::Time mTime;
        };

        class TimedTransition : public Transition
        {
        public:
            TimedTransition(const std::string& name,
                            double delay) :
                Transition(name),
                mDelay(delay)
            { }

            virtual ~TimedTransition()
            { }

            double getDelay() const
            { return mDelay; }

        private:
            double mDelay;
        };

    };

}} // namespace vle extension

#endif
