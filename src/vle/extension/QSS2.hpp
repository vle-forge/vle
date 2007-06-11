/**
 * @file extension/QSS2.hpp
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

#ifndef VLE_EXTENSION_QSS2_HPP
#define VLE_EXTENSION_QSS2_HPP

#include <vle/devs/Dynamics.hpp>
#include <vector>
#include <map>

namespace vle { namespace extension {
    
    struct Couple {
      double value;
      double derivative;
    };
    
    class qss2;

    class mfi
    {
    public:
      mfi(qss2*,unsigned int,unsigned int);
      virtual ~mfi();
      
      Couple init();
      Couple ext(const vle::devs::Time& p_time, unsigned int i,const Couple & couple);

    private:
      qss2* m_qss;
      unsigned int index;
      unsigned int m_functionNumber;
      vle::devs::Time m_lastTime;
      double fiz;
      double* c;
      double* z;
      double* mz;
      double* xt;
      double* yt;
    };
    
    typedef mfi* pmfi;
    
    class qss2:public devs::Dynamics
    {
      friend class mfi;
      
    public:
      qss2(const vle::graph::AtomicModel& model);
      virtual ~qss2() { }
      
      // DEVS Methods
      virtual void finish();
      virtual vle::devs::Time init();
      virtual void getOutputFunction(const vle::devs::Time& /* time */,
				     vle::devs::ExternalEventList& /* output */);
      virtual vle::devs::Time getTimeAdvance();
      virtual vle::devs::Event::EventType processConflict(const vle::devs::InternalEvent& /* internal */,
					       const vle::devs::ExternalEventList& /* extEventlist */) const;
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
      double* m_value;
      double* m_derivative;
      double* m_derivative2;
      double* m_index;
      double* m_index2;
      vle::devs::Time* m_sigma;
      vle::devs::Time* m_lastTime;
      pmfi* m_mfi;
      
      // Current model
      unsigned int m_currentModel;
      
      double m_precision;
      double m_epsilon;
      double m_threshold;
      
      bool delta(double a,double b,double c,double& s);
      bool intermediaire(double a,double b,double c,double cp,double& s);
      virtual double compute(unsigned int i) =0;
      inline double getDerivative(unsigned int i) const;
      inline double getDerivative2(unsigned int i) const;
      inline double getIndex(unsigned int i) const;
      inline double getIndex2(unsigned int i) const;
      inline const vle::devs::Time & getLastTime(unsigned int i) const;
      inline const vle::devs::Time & getSigma(unsigned int i) const;
      void init2();
      virtual void processPerturbation(std::string,double) { }
      inline void setIndex(unsigned int i,double p_index);
      inline void setIndex2(unsigned int i,double p_index);
      inline void setDerivative(unsigned int i,double p_derivative);
      inline void setDerivative2(unsigned int i,double p_derivative);
      inline void setLastTime(unsigned int i,const vle::devs::Time & p_time);
      inline void setSigma(unsigned int i,const vle::devs::Time & p_time);
      inline void setValue(unsigned int i,double p_value);
      void updateSigma(unsigned int i);
      void minSigma();
      
    protected:
      double getValue(unsigned int i) const { return m_value[i]; }
    };
    
  }} // namespace vle extension

#endif
