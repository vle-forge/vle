/**
 * @file vle/translator/MatrixTranslator.hpp
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


#ifndef VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP
#define VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP

#include <vle/devs/Executive.hpp>
#include <vle/utils/XML.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {

    class MatrixTranslator
    {
    public:
        MatrixTranslator();
        virtual ~MatrixTranslator();

        virtual void translate(const std::string& /* buffer */);

        typedef enum { VON_NEUMANN, MOORE, LINEAR } connectivity_type;

	graph::CoupledModel* mCoupledModel;
	devs::Coordinator* mCoordinator;

        bool existModel(unsigned int i, unsigned int j = 0);
	std::string getDynamics(unsigned int i, unsigned int j = 0);
        std::string getName(unsigned int i, unsigned int j = 0) const;

        unsigned int m_dimension;
        std::map < unsigned int, unsigned int > m_size;
        connectivity_type m_connectivity;
        bool m_multipleLibrary;
        std::string m_library;
        std::string m_model;
        std::map < unsigned int , std::pair < std::string, std::string > > m_libraries;
        std::string m_prefix;
        unsigned int* m_init;
        std::map < std::string , graph::AtomicModel* > m_models;
        bool        m_symmetricport;
        xmlpp::Element* m_root;

    private:
        xmlpp::DomParser m_parser;

        virtual void parseXML(const std::string& buffer);
	void translateStructures();
	void translateDynamics();
	void translateConditions();
	
    };

}} // namespace vle translator

#endif
