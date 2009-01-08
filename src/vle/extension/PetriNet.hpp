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
     * @brief PetriNet The class PetriNet encapsules the dynamics
     * of hybrid Petri net : arc with weight, timed transition,
     * inhibitor arc, priority, input transition  and output
     * transition and place.
     * The structure of Petri net can be specify with C++ code or
     * vpz condition.
     *
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
    public:
        /**
         * @brief Constructor of the dynamics of an
         * PetriNet model. Process the
         * init events: these events occurs only at the beginning of the
         * simulation and initialize the state of the model.
         * @param model the atomic model to which belongs the dynamics.
         * @param event the init event list.
         */
        PetriNet(const graph::AtomicModel& model,
                 const devs::InitEventList& events);

        /**
         * @brief Destructor
         */
        virtual ~PetriNet();

        /**
         * @brief Build the structure of PetriNet if it is not overlapped,
         * the structure is build with the vpz conditions.
         */
        virtual void build();

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /**
         * @brief Add a new arc between a transition and a place
         * or inversely.
         * @param src the name of the origin of arc (a
         * transition or a place).
         * @param dst the name of the destination of arc (a
         * transition or a place).
         * @param tokenNumber the weight of arc, the number of
         * produced or consumed tokens when the firing of transition.
         */
        void addArc(const std::string& src,
                    const std::string& dst,
                    unsigned int tokenNumber = 1);

        /**
         * @brief Specify the initial marking of a place.
         * @param placeName the name of place.
         * @param tokenNumber the initial number of tokens.
         */
        void addInitialMarking(const std::string& placeName,
                               unsigned int tokenNumber);

        /**
         * @brief Add a new input transition. An input transition
         * is a transition linked to a input DEVS port. When an
         * event occurs on the specified port, the transition is
         * fired.
         * @param transitionName the name of transition.
         * @param portName the name of input DEVS port.
         * @param delay the delay before the firing of
         * transition. The delay is null by default.
         */
        void addInputTransition(const std::string& transitionName,
                                const std::string& portName,
                                double delay = 0.0);

        /**
         * @brief Add a new output place. An output place
         * is a place linked to a output DEVS port. When a token
         * arrives in the place, an output event is created.
         * @param placeName the name of place.
         * @param portName the name of output DEVS port.
         */
        void addOutputPlace(const std::string& placeName,
                            const std::string& portName);

        /**
         * @brief Add a new output transition. An output transition
         * is a transition linked to a output DEVS port. When the
         * transition is fired, an output event is created.
         * @param transitionName the name of transition.
         * @param portName the name of output DEVS port.
         * @param priority the priority of transition. The high
         * value is 0, the default value.
         */
        void addOutputTransition(const std::string& transitionName,
                                 const std::string& portName,
                                 unsigned int priority = 0);

        /**
         * @brief Add a new place. A delay can be specified to a
         * place. When a token arrive to a kind of place, the
         * token is available after the delay.
         * @param placeName the name of place.
         * @param delay the delay after the availability of tokens.
         * The delay is null by default.
         */
        void addPlace(const std::string& placeName,
                      double delay = 0.0);

        /**
         * @brief Add a new transition. When an
         * event occurs on the specified port, the transition is
         * fired.
         * @param transitionName the name of transition.
         * @param delay the delay before the firing of
         * transition. The delay is null by default.
         * @param priority the priority of transition. The high
         * value is 0, the default value.
         */
        void addTransition(const std::string& transitionName,
                           double delay = 0.0,
                           unsigned int priority = 0);

    private:
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
        typedef std::pair < devs::Time, Transition* > pairTimeTransition;

        typedef enum { IDLE, WAITING, OUT, OUT2, RUN } phase;

        class Token
        {
        public:
            Token(const devs::Time& time) : mTime(time), mSent(false)
            { }

            virtual ~Token()
            { }

            const devs::Time& getTime() const
            { return mTime; }

            bool sent() const { return mSent; }

            void send() { mSent = true; }

        private:
            devs::Time mTime;
            bool mSent;
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

            const TokenList& getTokens() const
            { return mTokens; }

        private:
            Place* mPlace;
            TokenList mTokens;
        };

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        void initArcs(const value::VectorValue& arcs);
        void initInitialMarking(
            const value::VectorValue& initialMarkings);
        void initPlaces(const value::VectorValue& places);
        void initTransitions(const value::VectorValue& transitions);

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        void addEnabledTransition(Transition* transition);
        void buildInitialMarking(const devs::Time& time);
        void computeEnabledTransition(const devs::Time& time);
        void disableOutTransition();
        void disableOutPlace(const devs::Time& time);

        inline bool existPlace(const std::string& name) const
        { return mPlaces.find(name) != mPlaces.end(); }

        inline bool existTransition(const std::string& name) const
        { return mTransitions.find(name) != mTransitions.end(); }

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

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual devs::Time init(
            const devs::Time& time);

        virtual void output(
            const devs::Time& time,
            devs::ExternalEventList& output) const;

        virtual devs::Time timeAdvance() const;

        virtual devs::Event::EventType confluentTransitions(
            const devs::Time& /* time */,
            const devs::ExternalEventList& /* events */) const
        { return devs::Event::EXTERNAL; }

        virtual void internalTransition(
            const devs::Time& event);

        virtual void externalTransition(
            const devs::ExternalEventList& event,
            const devs::Time& time);

        virtual value::Value* observation(
            const devs::ObservationEvent& event) const;

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

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

