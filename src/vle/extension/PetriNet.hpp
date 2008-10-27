/**
 * @file vle/extension/PetriNet.hpp
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


#ifndef VLE_EXTENSION_PETRINET_HPP
#define VLE_EXTENSION_PETRINET_HPP

#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension {

    /**
     * @brief
     * @code
     * <condition>
     *  <port name="places">
     *   <set>
     *    <set>
     *     <string>name</string>
     *     [<string>portName</string>]
     *     [<double>delay</double>]
     *    </set>
     *   </set>
     *  </port>
     *  <port name="transitions">
     *   <set>
     *    <set>
     *     <string>name</string>
     *     [<string>input|output</string>]
     *     [<string>portName</string>]
     *     [<double>delay</double>]
     *     [<integer>priority</integer>]
     *    </set>
     *   </set>
     *  </port>
     *  <port name="arcs">
     *   <set>
     *    <set>
     *     <string>placeName or transitionName</string>
     *     <string>transitionName or placeName</string>
     *     [<integer>tokenNumber</integer>]
     *    </set>
     *   <set>
     * </port>
     * <port name="initialMarkings">
     *  <set>
     *   <set>
     *    <string>placeName</string>
     *    <integer>tokenNumber</integer>
     *   </set>
     *  </set>
     * </port>
     * @endcode
     */
    class PetriNet : public devs::Dynamics
    {
    protected:
        class Transition;
        class Input;
        class Output;
        class Marking;
        class Place;
        class Token;

        typedef std::vector < Input* > InputList;
        typedef std::vector < Output* > OutputList;
        typedef std::map < std::string, Transition* > TransitionList;
        typedef std::vector < Transition* > enabledTransitionList;
        typedef std::map < std::string, Marking* > MarkingList;
        typedef std::map < std::string, std::pair < std::string,
                unsigned int > > devsPlaceMarkingList;
        typedef std::map < std::string,
                std::string > devsInTransitionMarkingList;
        typedef std::map < std::string,
                std::pair < std::string,
                bool > > devsOutTransitionMarkingList;
        typedef std::map < std::string, unsigned int >
            initialMarkingList;
        typedef std::map < std::string, Place* > PlaceList;
        typedef std::vector < Token* > TokenList;

        typedef enum { WAITING, OUT, RUN } phase;

        class Token
        {
        public:
            Token(const devs::Time& time) : mTime(time)
            { }

            virtual ~Token()
            { }

            const devs::Time& getTime() const
            { return mTime; }

        private:
            devs::Time mTime;
        };

        class Transition
        {
        public:
            Transition(const std::string& name,
                       double delay = 0.0,
                       unsigned int priority = 0) : mName(name),
            mDelay(delay),
            mPriority(priority)
            { }

            virtual ~Transition()
            { }

            void addInput(Input* input)
            { mInputs.push_back(input); }

            void addOutput(Output* output)
            { mOutputs.push_back(output); }

            double getDelay() const
            { return mDelay; }

            const std::string& getName() const
            { return mName; }

            unsigned int getPriority() const
            { return mPriority; }

            const InputList& inputs() const
            { return mInputs; }

            const OutputList& outputs() const
            { return mOutputs; }

        private:
            std::string mName;
            InputList mInputs;
            OutputList mOutputs;
            double mDelay;
            unsigned int mPriority;
        };

        class Input
        {
        public:
            Input(Place* place, Transition* transition,
                  unsigned int consumedTokenNumber) :
                mPlace(place),
                mTransition(transition),
                mConsumedTokenNumber(consumedTokenNumber)
            { }

            virtual ~Input()
            { }

            unsigned int getConsumedTokenNumber() const
            { return mConsumedTokenNumber; }

            Place* getPlace() const
            { return mPlace; }

            Transition* getTransition() const
            { return mTransition; }

        private:
            Place* mPlace;
            Transition* mTransition;
            unsigned int mConsumedTokenNumber;
        };

        class Output
        {
        public:
            Output(Transition* transition,
                   Place* place,
                   unsigned int producedTokenNumber) :
                mTransition(transition),
                mPlace(place),
                mProducedTokenNumber(producedTokenNumber)
            { }
            virtual ~Output()
            { }

            Place* getPlace() const
            { return mPlace; }

            unsigned int getProducedTokenNumber() const
            { return mProducedTokenNumber; }

            Transition* getTransition() const
            { return mTransition; }

        private:
            Transition* mTransition;
            Place* mPlace;
            unsigned int mProducedTokenNumber;
        };

        class Place
        {
        public:
            Place(const std::string& name,
                  double delay) : mName(name), mDelay(delay)
            { }
            virtual ~Place()
            { }

            void addInput(Input* input)
            { mInputs.push_back(input); }

            void addOutput(Output* output)
            { mOutputs.push_back(output); }

            double getDelay() const
            { return mDelay; }

            const std::string& getName() const
            { return mName; }

        private:
            std::string mName;
            double mDelay;
            InputList mInputs;
            OutputList mOutputs;
        };

        class Marking
        {
        public:
            Marking(Place* place) : mPlace(place) { }
            virtual ~Marking();

            void addToken(Token* token)
            { mTokens.push_back(token); }

            unsigned int getTokenNumber() const
            { return mTokens.size(); }

            Place* getPlace() const
            { return mPlace; }

            const std::string& getPlaceName() const
            { return mPlace->getName(); }

            TokenList& getTokens()
            { return mTokens; }

        private:
            Place* mPlace;
            TokenList mTokens;
        };

    public:
        typedef std::pair < devs::Time, Transition* > pairTimeTransition;

        PetriNet(const graph::AtomicModel& model,
                 const devs::InitEventList& events);
        virtual ~PetriNet();

        virtual void build();

        virtual void finish()
        { }

        virtual devs::Time init(
            const devs::Time& time);

        virtual void output(
            const devs::Time& time,
            devs::ExternalEventList& output) const;

        virtual devs::Time timeAdvance() const;

        virtual void internalTransition(
            const devs::Time& event);

        virtual void externalTransition(
            const devs::ExternalEventList& event,
            const devs::Time& time);

        virtual value::Value* observation(
            const devs::ObservationEvent& event) const;

        // petrinet construction methods
        void addArc(const std::string first,
                    const std::string second,
                    unsigned int tokenNumber = 1);
        void addInitialMarking(const std::string placeName,
                               unsigned int tokenNumber);
        void addInputTransition(const std::string transitionName,
                                const std::string portName,
                                double delay = 0.0);
        void addOutputPlace(const std::string placeName,
                            const std::string portName);
        void addOutputTransition(const std::string transitionName,
                                 const std::string portName,
                                 unsigned int priority = 0);
        void addPlace(const std::string placeName,
                      double delay = 0.0);
        void addTransition(const std::string transitionName,
                           double delay = 0.0,
                           unsigned int priority = 0);

    private:
        // parsing methods
        void initArcs(const value::VectorValue& arcs);
        void initInitialMarking(
            const value::VectorValue& initialMarkings);
        void initPlaces(const value::VectorValue& places);
        void initTransitions(const value::VectorValue& transitions);

        void addEnabledTransition(Transition* transition);
        void buildInitialMarking(const devs::Time& time);
        void computeEnabledTransition(const devs::Time& time);
        void fire(const devs::Time& time);
        bool goInTransition(Transition* transition);
        void goOutTransition(Transition* transition, const devs::Time& time);
        devs::Time getNextValid(Marking* marking,
                                unsigned int tokenNumber);
        devs::Time getValidTime(Transition* transition);
        void putTokens(Transition* transition, const devs::Time& time);
        void putToken(Marking* marking, const devs::Time& time);
        bool removeTokens(Marking* marking, unsigned int tokenNumber);
        void run(const devs::Time& time);
        Transition* selectTransition();

        inline bool existPlace(const std::string& name) const
        { return mPlaces.find(name) != mPlaces.end(); }

        inline bool existTransition(const std::string& name) const
        { return mTransitions.find(name) != mTransitions.end(); }

        // initial values
        const devs::InitEventList& mInitEvents;

        // structure
        PlaceList mPlaces;
        TransitionList mTransitions;
        devsPlaceMarkingList mOutPlaceMarkings;
        devsPlaceMarkingList mInPlaceMarkings;
        devsOutTransitionMarkingList mOutTransitionMarkings;
        devsInTransitionMarkingList mInTransitionMarkings;
        InputList mInputs;
        OutputList mOutputs;

        // marking
        initialMarkingList mInitialMarking;
        MarkingList mMarkings;
        enabledTransitionList mEnabledTransitions;

        // transition
        std::list < pairTimeTransition > mWaitingTransitions;

        int mTokenNumber;

        phase mPhase;
        devs::Time mSigma;

        friend bool operator<(const pairTimeTransition& x,
                              const pairTimeTransition& y);
    };

    bool operator<(const PetriNet::pairTimeTransition& x,
                   const PetriNet::pairTimeTransition& y);

}} // namespace vle extension

#endif

