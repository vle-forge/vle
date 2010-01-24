/**
 * @file vle/extension/CellQSS.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_EXTENSION_CELLQSS_HPP
#define VLE_EXTENSION_CELLQSS_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/CellDevs.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {

    class VLE_EXTENSION_EXPORT CellQSS : public CellDevs
    {
    public:
        CellQSS(const devs::DynamicsInit& model,
                const devs::InitEventList& event);

        virtual ~CellQSS() { }

        virtual void finish();
        virtual devs::Time init(const devs::Time& /* time */);
        virtual void internalTransition(const devs::Time& event);
        virtual void externalTransition(const devs::ExternalEventList& event,
                                        const devs::Time& time);
        virtual value::Value* observation(const devs::ObservationEvent& event)
            const;

        virtual void processPerturbation(const devs::ExternalEvent& event);

    protected:
        const devs::Time & getCurrentTime(unsigned int i) const;
        const devs::Time & getLastTime(unsigned int i) const;
        void setGradient(unsigned int i,double p_gradient);
        void updateSigma(unsigned int i);
        double getValue(unsigned int i) const;
        void setValue(unsigned int i,double p_value);

    private:
        enum state {
            INIT, INIT2, RUN
        };

        // Number of functions
        unsigned int m_functionNumber;

        // Initial values
        std::vector < std::pair < unsigned int , double > > m_initialValueList;

        // State
        double* m_gradient;
        long* m_index;
        std::map < unsigned int , std::string > m_variableName;
        std::map < std::string , unsigned int > m_variableIndex;
        devs::Time* m_sigma;
        devs::Time* m_lastTime;
        devs::Time* m_currentTime;
        state* m_state;

        // Current model
        unsigned int m_currentModel;

        //
        double m_precision;
        double m_epsilon;
        bool m_active;

        inline double d(long x);
        virtual double compute(unsigned int i) =0;
        inline double getGradient(unsigned int i) const;
        inline long getIndex(unsigned int i) const;
        inline const devs::Time & getSigma(unsigned int i) const;
        inline state getState(unsigned int i) const;
        inline void setIndex(unsigned int i,long p_index);
        inline void setCurrentTime(unsigned int i,const devs::Time & p_time);
        inline void setLastTime(unsigned int i,const devs::Time & p_time);
        inline void setSigma(unsigned int i,const devs::Time & p_time);
        inline void setState(unsigned int i,state p_state);

    };

}} // namespace vle extension

#endif
