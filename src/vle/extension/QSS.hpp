/**
 * @file extension/QSS.hpp
 * @author The VLE Development Team.
 * @brief
 */

/*
 * Copyright (C) 2006, 07 - The vle Development Team
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

#ifndef VLE_EXTENSION_QSS_HPP
#define VLE_EXTENSION_QSS_HPP

#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension {
    
    class qss : public devs::Dynamics
    {
    public:
        enum state { INIT, RUN, POST, POST2, POST3 };

        qss(const graph::AtomicModel& model);

        virtual ~qss() { }

        virtual void finish();

        virtual devs::Time init();

        virtual void getOutputFunction(
            const devs::Time& time,
            devs::ExternalEventList& output);

        virtual devs::Time getTimeAdvance();

        virtual devs::Event::EventType processConflict(
            const devs::InternalEvent& internal,
            const devs::ExternalEventList& extEventlist) const;

        virtual void processInitEvents(
            const devs::InitEventList& event);

        virtual void processInternalEvent(
            const devs::InternalEvent& event);

        virtual void processExternalEvents(
            const devs::ExternalEventList& event,
            const devs::Time& time);

        virtual value::Value processStateEvent(
            const devs::StateEvent& event) const;

    protected:
        /**
         * @brief The function to develop mathemacial expression like:
         * @code
         * if (i == 0) { // the first variable
         *   return a * getValue(0) - b * getValue(0) * getValue(1);
         * } else {
         *   return c * getValue(1) - d * getValue(1) * getValue(0);
         * }
         * @endcode
         * @param i the index of the variable to compute.
         */
        virtual double compute(unsigned int i) = 0;

        /**
         * @brief Get the index of the specified variable name.
         * @param name the name of the variable to find.
         * @return the index of the specified variable name.
         * @throw utils::InternalError if name does not exist.
         */
        int getVariable(const std::string& name) const;

        /**
         * @brief Get the name of the specified index.
         * @param index the index of the varaible to find.
         * @return a string representation of the variable name.
         * @throw utils::InternalError if name does not exist.
         */
        const std::string& getVariable(unsigned int index) const;

        /**
         * @brief Called when a event occur on the 'parameter' port with two
         * keys name and value.
         * @code
         * { ('name', 'variablename'), ('value', value::Double) }
         * @endcode
         * @param variable the name of the variable to perturb.
         * @param value a value to the perturbation.
         */
        virtual void processPerturbation(const std::string& /* variable */,
                                         double /* value */)
        { }

        /**
         * @brief Set the value of the variable specified by index. Be carefull,
         * no check on the variable i: 0 <= i < m_functionNumber.
         * @param i index of the variable.
         */
        inline void setValue(unsigned int i, double value)
        { m_value[i] = value; }

        /**
         * @brief Get the value of the variable specified by index. Be carefull,
         * no check on the variable i: 0 <= i < m_functionNumber.
         * @param i index of the variable.
         */
        inline double getValue(unsigned int i) const
        { return m_value[i]; }

        bool m_active;
        /** Number of functions. */
        unsigned int m_functionNumber;

        /** Initial values. */
        std::vector < std::pair < unsigned int , double > > m_initialValueList;

        std::map < unsigned int , std::string > m_variableName;
        std::map < std::string , unsigned int > m_variableIndex;

        /** State. */
        double* m_gradient;
        long* m_index;
        double* m_value;
        devs::Time* m_sigma;
        devs::Time* m_lastTime;
        state* m_state;
        double m_precision;
        double m_epsilon;

    private:
        // Current model
        unsigned int m_currentModel;

        double m_threshold;

        inline double d(long x)
        { return x * m_precision; }

        inline double getGradient(unsigned int i) const
        { return m_gradient[i]; }

        inline long getIndex(unsigned int i) const
        { return m_index[i]; }

        inline const devs::Time& getLastTime(unsigned int i) const
        { return m_lastTime[i]; }

        inline const devs::Time& getSigma(unsigned int i) const
        { return m_sigma[i]; }

        inline state getState(unsigned int i) const
        { return m_state[i]; }

        inline void setIndex(unsigned int i, long index)
        { m_index[i] = index; }

        inline void setGradient(unsigned int i,double gradient)
        { m_gradient[i] = gradient; }

        inline void setLastTime(unsigned int i,const devs::Time & p_time)
        { m_lastTime[i] = p_time; }

        inline void setSigma(unsigned int i,const devs::Time & p_time)
        { m_sigma[i] = p_time; }

        inline void setState(unsigned int i,state p_state)
        { m_state[i] = p_state; }

        void updateSigma(unsigned int i);

        void minSigma();
    };

}} // namespace vle extension

#endif
