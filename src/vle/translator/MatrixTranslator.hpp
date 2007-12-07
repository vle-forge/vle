/** 
 * @file MatrixTranslator.hpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 05 jui 2007 17:12:53 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
