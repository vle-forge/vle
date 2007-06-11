/** 
 * @file extension/CellQSS.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:07:16 +0200
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

#ifndef __CELL_QSS_HPP
#define __CELL_QSS_HPP

#include <vle/extension/CellDevs.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension {
    class CellQSS:public CellDevs
    {
	enum state
	{
	    INIT, INIT2, RUN
	};

    private:
// Number of functions
	unsigned int m_functionNumber;

// Initial values
	std::vector < std::pair < unsigned int ,
				  double > > m_initialValueList;

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

    protected:
	const devs::Time & getCurrentTime(unsigned int i) const;
	const devs::Time & getLastTime(unsigned int i) const;
	void setGradient(unsigned int i,double p_gradient);
	void updateSigma(unsigned int i);
	double getValue(unsigned int i) const;
	void setValue(unsigned int i,double p_value);

    public:
      CellQSS(const vle::graph::AtomicModel& p_model);
      virtual ~CellQSS() { }

      // DEVS Methods
      virtual void finish();
      virtual vle::devs::Time init();
      virtual void processInternalEvent(const vle::devs::InternalEvent& /* event */);
      virtual void processExternalEvents(const vle::devs::ExternalEventList& /* event */,
					 const vle::devs::Time& /* time */);
      virtual vle::value::Value processStateEvent(const vle::devs::StateEvent& /* event */) const;

      virtual void processPerturbation(const vle::devs::ExternalEvent& event);
    };
}} // namespace vle extension

#endif
