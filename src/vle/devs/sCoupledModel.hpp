/**
 * @file sCoupledModel.hpp
 * @author The VLE Development Team.
 * @brief
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
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

#ifndef DEVS_SCOUPLED_MODEL_HPP
#define DEVS_SCOUPLED_MODEL_HPP

#include <vle/devs/sModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <vector>

namespace vle { namespace devs {

    class Simulator;

    class sCoupledModel : public sModel
    {
    public:
	sCoupledModel(graph::CoupledModel* c, Simulator* s);

	virtual ~sCoupledModel();

	graph::Model * getModel(const std::string& modelName);

        std::vector < graph::Model * > getModelList() const;
        
        graph::CoupledModel* getStructure() const;

	virtual bool isAtomic() const;
        
        virtual bool isCoupled() const;
        
        virtual graph::Model* findModel(const std::string& name) const;
        
        virtual bool parseXML(xmlNodePtr modelNode,
			      Simulator* simulator,
			      graph::CoupledModel* parent);

	virtual void processInitEvent(InitEvent* event);

	virtual StateEvent* processStateEvent(StateEvent* event) const;

    private:
	graph::CoupledModel * m_coupledModel;
    };

}} // namespace vle devs

#endif
