/**
 * @file extension/DifferenceEquation.hpp
 * @author The VLE Development Team.
 * @brief
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef VLE_EXTENSION_DIFFERENCE_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENCE_EQUATION_HPP

#include <vle/devs/Dynamics.hpp>
#include <vle/graph/Port.hpp>
#include <vle/devs/sAtomicModel.hpp>
#include <vle/utils/Tools.hpp>

#include <list>
#include <map>

namespace vle { namespace extension {

    class DifferenceEquation:public devs::Dynamics
    {
    private:
        enum state { _S_PARAMS, _S_CALCUL, _S_PAUSE, _S_PARAMS2 };
        typedef std::map < std::string, std::list < double > > ParameterList;

        //  protected:
        double m_delay;
        state m_state;
        //    unsigned int nb_params;
        double m_result;

        // List of parameters
        ParameterList m_parameters;
        std::map < std::string, unsigned int > m_listMaxSize;
        std::map < std::string, double > m_lastValue;

        //File des paramètres
        //std::list < double > * * params;

        //Profondeur sur t des paramètres:
        //  X(t) = f( Yi( t-taille_file[i]+1) )
        //En fait, le décalage temporel est proportionnel au dt m_delay:
        //t-A <=> t-A*m_delay
        //    unsigned int* taille_file;
        //    double* last_param;

        //Le nom du port d'init, donne le nom de la clé!
        //    Map init_v;

        bool master;

    public:
        DifferenceEquation( devs::sAtomicModel * p_model, bool m = false):
            devs::Dynamics(p_model), m_delay(1.), m_state(_S_PARAMS), m_result(0.0), 
            master(m) { }
        virtual ~DifferenceEquation() { }

        double getValue(const char* p_name)  
        { return m_parameters[p_name].front(); }

        double getDelay() const { return m_delay; }
        double getValue(const char* p_name,unsigned int p_delta);

        // XML loading method
        virtual bool parseXML(xmlpp::Element* p_dynamicsNode);

        // DEVS Methods
        virtual devs::Time init();
        devs::Time getTimeAdvance();
        virtual void processInternalEvent( devs::InternalEvent * event);
        virtual devs::ExternalEventList* getOutputFunction(devs::Time const &time);
        virtual void processExternalEvent( devs::ExternalEvent * event);
        virtual value::Value processStateEvent( devs::StateEvent * event) const;
        virtual void processInitEvent( devs::InitEvent * event);
        //		virtual bool processConflict(const devs::InternalEvent& , const devs::ExternalEventList& ) { std::cout<<"++++ ProcessConflict2"<<std::endl; return false; }
        virtual double compute() = 0;
    };

}} //namespace extension vle

#endif
