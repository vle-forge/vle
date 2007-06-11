/**
 * @file extension/QSS.hpp
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

#ifndef VLE_EXTENSION_QSS_HPP
#define VLE_EXTENSION_QSS_HPP

#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension {
    
    class qss : public vle::devs::Dynamics
    {
    public:
      enum state { INIT, RUN };
      
      qss(const vle::graph::AtomicModel& model);
      virtual ~qss() { }

      // DEVS Methods
      virtual void finish();
      virtual vle::devs::Time init();
      virtual void getOutputFunction(const vle::devs::Time& /* time */,
				     vle::devs::ExternalEventList& /* output */);
      virtual vle::devs::Time getTimeAdvance();
      virtual vle::devs::Event::EventType processConflict(const vle::devs::InternalEvent& /* internal */,
					       const vle::devs::ExternalEventList& /* extEventlist */) const;
      //      virtual void processInitEvents(const vle::devs::InitEventList& /* event */);
      virtual void processInternalEvent(const vle::devs::InternalEvent& /* event */);
      virtual void processExternalEvents(const vle::devs::ExternalEventList& /* event */,
					 const vle::devs::Time& /* time */);
      virtual vle::value::Value processStateEvent(const vle::devs::StateEvent& /* event */) const;

    private:
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
      
      // Current model
      unsigned int m_currentModel;
      
      double m_precision;
      double m_epsilon;
      double m_threshold;
      
      inline double d(long x);
      virtual double compute(unsigned int i) =0;
      inline double getGradient(unsigned int i) const;
      inline long getIndex(unsigned int i) const;
      inline const devs::Time & getLastTime(unsigned int i) const;
      inline const devs::Time & getSigma(unsigned int i) const;
      inline state getState(unsigned int i) const;
      virtual void processPerturbation(std::string,double) { }
      inline void setIndex(unsigned int i,long p_index);
      inline void setGradient(unsigned int i,double p_gradient);
      inline void setLastTime(unsigned int i,const devs::Time & p_time);
      inline void setSigma(unsigned int i,const devs::Time & p_time);
      inline void setState(unsigned int i,state p_state);
      inline void setValue(unsigned int i,double p_value);
      void updateSigma(unsigned int i);
      void minSigma();

    protected:
      double getValue(unsigned int i) const { return m_value[i]; }
      //      double parseParameter(const std::string & p_name, 
      //			    xmlpp::Element* p_dynamicsNode);
    };

}} // namespace vle extension

#endif
