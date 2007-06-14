/** 
 * @file extension/CellDevs.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:07:06 +0200
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

#ifndef __CELL_DEVS_HPP
#define __CELL_DEVS_HPP

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace extension {
   
    class CellDevs:public devs::Dynamics
    {
    private:
      // variable utilisée par ta() pour connaître la durée de
      // l'état courant
      devs::Time m_sigma;
      // date du dernier événement
      devs::Time m_lastTime;
      // délai de transmission de l'état courant aux cellules voisines
      double m_delay;
      // Si une variable d'etat non cachee est modifiee alors m_modified
      // est affectee ; la fonction de sortie la reinitialise
      bool m_modified;
      bool m_neighbourModified;
      // Liste des noms de variable
      std::vector < std::string > m_stateNameList;
      // L'etat d'une cellule est un map de Value
      std::map < std::string ,  std::pair < value::Value , bool > > m_state;
      // Liste des etats des voisins
      std::map < std::string , std::map < std::string ,
					  value::Value > > m_neighbourState;
      // Liste des ports lies aux voisins
      std::vector < std::string > m_neighbourPortList;
      
    public:
      CellDevs(const vle::graph::AtomicModel& p_model):devs::Dynamics(p_model),
						       m_modified(false) { }
      virtual ~CellDevs() { }
      
      // get and set sigma
      devs::Time const & getSigma() const;
      void setSigma(const devs::Time & p_sigma);
      virtual void updateSigma(devs::Event*) { };
      
      // get and set last time
      devs::Time const & getLastTime() const;
      void setLastTime(const devs::Time & p_lastTime);
      
      // Init state
      
      void initState(std::string const & p_name,
		     value::Value p_value,
		     bool p_visible=true);
      void initDoubleState(std::string const & p_name,
			   double p_value,
			   bool p_visible=true);
      void initIntegerState(std::string const & p_name,
			    long p_value,
			    bool p_visible=true);
      void initBooleanState(std::string const & p_name,
			    bool p_value,
			    bool p_visible=true);
      void initStringState(std::string const & p_name,
			   std::string const & p_value,
			   bool p_visible=true);
      
      void initNeighbourhood(std::string const & p_stateName,
			     value::Value p_value);
      void initDoubleNeighbourhood(std::string const & p_stateName,
				   double p_value);
      void initIntegerNeighbourhood(std::string const & p_stateName,
				    long p_value);
      void initBooleanNeighbourhood(std::string const & p_stateName,
				    bool p_value);
      void initStringNeighbourhood(std::string const & p_stateName,
				   std::string const & p_value);
      void initNeighbourState(std::string const & p_neighbourName,
			      std::string const & p_stateName,
			      value::Value p_value);
      void initDoubleNeighbourState(std::string const & p_neighbourName,
				    std::string const & p_stateName,
				    double p_value);
      void initIntegerNeighbourState(std::string const & p_neighbourName,
				     std::string const & p_stateName,
				     long p_value);
      void initBooleanNeighbourState(std::string const & p_neighbourName,
				     std::string const & p_stateName,
				     bool p_value);
      void initStringNeighbourState(std::string const & p_neighbourName,
				    std::string const & p_stateName,
				    std::string const & p_value);
      
      bool existState(std::string const & p_name) const;
      
      bool isNeighbourEvent(devs::ExternalEvent* p_event) const;
      
      bool existNeighbourState(std::string const & p_name) const;
      value::Value getNeighbourState(std::string const & p_neighbourName,
				     std::string const & p_stateName) const;
      double getDoubleNeighbourState(std::string const & p_neighbourName,
				     std::string const & p_stateName) const;
      long getIntegerNeighbourState(std::string const & p_neighbourName,
				    std::string const & p_stateName) const;
      bool getBooleanNeighbourState(std::string const & p_neighbourName,
				    std::string const & p_stateName) const;
      std::string getStringNeighbourState(std::string const & p_neighbourName,
					  std::string const & p_stateName) const;
      unsigned int getNeighbourStateNumber() const;
      unsigned int getBooleanNeighbourStateNumber(
						  std::string const & p_stateName,
						  bool p_value) const;
      unsigned int getIntegerNeighbourStateNumber(
						  std::string const & p_stateName,
						  long p_value) const;
      
      // Hidden state
      void hiddenState(std::string const & p_name);
      
      // Get and set delay
      double getDelay() const;
      void setDelay(double p_delay) { m_delay = p_delay; }
      
      // Get and set state
      value::Value getState(std::string const & p_name) const;
      double getDoubleState(std::string const & p_name) const;
      long getIntegerState(std::string const & p_name) const;
      bool getBooleanState(std::string const & p_name) const;
      std::string getStringState(std::string const & p_name) const;
      
      void setState(std::string const & p_name,value::Value p_value);
      void setDoubleState(std::string const & p_name,double p_value);
      void setIntegerState(std::string const & p_name,long p_value);
      void setBooleanState(std::string const & p_name,bool p_value);
      void setStringState(std::string const & p_name,
			  std::string const & p_value);
      
      void modify() { m_modified = true; }
      
      void setNeighbourState(std::string const & p_neighbourName,
			     std::string const & p_stateName,
			     value::Value p_value);
      
      inline bool isNeighbourModified() const 
      { return m_neighbourModified; }
      
      inline void neighbourModify()
      { m_neighbourModified = true; }
      
      inline void resetNeighbourModified()
      { m_neighbourModified = false; }
      
      // DEVS Methods
      virtual vle::devs::Time init();
      virtual void getOutputFunction(const vle::devs::Time& /* time */,
				     vle::devs::ExternalEventList& /* output */);
      virtual vle::devs::Time getTimeAdvance();
      virtual void processInitEvents(const vle::devs::InitEventList& /* event */);
      virtual void processExternalEvents(const vle::devs::ExternalEventList& /* event */,
					 const vle::devs::Time& /* time */);
      virtual vle::value::Value processStateEvent(const vle::devs::StateEvent& /* event */) const;
      
      virtual void processPerturbation(const vle::devs::ExternalEvent& /* event */) =0;
      virtual void processParameters(const std::string& /* name */, const vle::value::Value& /* value */) { }
    };
    
}} // namespace vle extension

#endif
