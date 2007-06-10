/** 
 * @file extension/Equation.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:07:37 +0200
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef __EQUATION_HPP
#define __EQUATION_HPP

#include <libxml++/libxml++.h>

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace extension {


    class Equation:public devs::Dynamics
    {
        enum state { INIT, NEXT, RUN };

    public:
        Equation(devs::Simulator* p_model):devs::Dynamics(p_model) { }
        virtual ~Equation() { }

        // XML loading method
        virtual bool parseXML(xmlpp::Element* p_dynamicsNode);

        // DEVS Methods
        virtual devs::Time getTimeAdvance();
        virtual devs::ExternalEventList* getOutputFunction(devs::Time const &
                                                            time); 
        virtual devs::Time init();
        virtual void processInternalEvent(devs::InternalEvent* event);
        virtual void processExternalEvent(devs::ExternalEvent* event);
        virtual void processInitEvent(devs::InitEvent* event);
        virtual bool processConflict(const devs::InternalEvent& internal,
                                     const devs::ExternalEventList&
                                     extEventlist);
        virtual value::Value processStateEvent(devs::StateEvent* event) const;

    private:
        state m_state;
        unsigned int m_var;
        // Equation
        unsigned int m_equationNumber;
        std::string* m_nameList;
        double* m_valueList;
        std::map < std::string, unsigned int > m_indexList;
        // Variable
        unsigned int m_variableNumber;
        int m_number;
        std::vector < std::string > m_variableNameList;
        std::map < std::string, double > m_variable;

    protected:
        virtual void compute() =0;

        std::string getName(unsigned int p_index) const 
        { return m_nameList[p_index]; }
        void initEquation(unsigned int p_index,double p_value);
        void initVariable(std::string const & p_name,double p_value);
        bool existEquation(std::string const & p_name) const;
        bool existVariable(std::string const & p_name) const;
        double getVariable(std::string const & p_name) const;
        unsigned int getVariableNumber() const { return m_variableNumber; }
        std::string getVariableName(unsigned int p_index) const;
        void setVariable(std::string const & p_name,double p_value);
        double getEquation(unsigned int p_index) const 
        { return m_valueList[p_index]; }
        void setEquation(unsigned int p_index, double p_value);
    };

}} // namespace vle extension

#endif
