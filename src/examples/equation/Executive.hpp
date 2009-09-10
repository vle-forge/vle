/**
 * @file examples/equation/Executive.hpp
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


#ifndef EXAMPLES_EQUATION_EXECUTIVE_HPP
#define EXAMPLES_EQUATION_EXECUTIVE_HPP

#include <vle/devs.hpp>

namespace vle { namespace examples { namespace equation {

    class Executive : public devs::Executive
    {
    public:
        Executive(const devs::ExecutiveInit& model,
                  const devs::InitEventList& events);

        virtual ~Executive() { }

        virtual devs::Time init(const devs::Time& /* time */);

        virtual devs::Time timeAdvance() const;

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual void internalTransition(const devs::Time& ev);

        virtual void externalTransition(
            const devs::ExternalEventList& event,
            const devs::Time& time);

    private:

        class model
        {
        public:
            model(const std::string& name, const std::string& var,
                  const std::string& dyn,
                  value::Set in, value::Set out, value::Set generic) :
                name(name), var(var), dyn(dyn),
                in(in), out(out), generic(generic)
            { }

            std::string name;
            std::string var;
            std::string dyn;
            value::Set in;
            value::Set out;
            value::Set generic;
        };

        void buildInputConnections();
        void buildOutputConnections(model* mdl);
        void clearModels();
        void createModel(model* mdl);
        void removeInputConnections(model* mdl);
        void removeModel(model* mdl);
        void removeOutputConnections(model* mdl);
        void removeTemporaryConnections(model* mdl);

        enum state { ADD, POST_ADD, IDLE, REMOVE, POST_REMOVE };

        state mState;
        devs::Time mSigma;
        double mTimeStep;
        std::string mView;
        bool mReRun;
        unsigned int mReceives;
        bool mSent;

        std::map < std::pair < std::string, std::string >, double > mValues;
        std::map < std::string, std::pair < std::string, bool > > mVariables;
        std::vector < model* > mModels;
        std::vector < std::string > mActiveModels;
    };

}}} // namespace vle examples equation

DECLARE_NAMED_EXECUTIVE(Executive, vle::examples::equation::Executive)

#endif
