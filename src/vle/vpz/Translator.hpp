/** 
 * @file vpz/Translator.hpp
 * @brief Translator base class to develop translator plugin.
 * @author The vle Development Team
 * @date mar, 28 fév 2006 15:01:46 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#ifndef VLE_VPZ_TRANSLATOR_HPP
#define VLE_VPZ_TRANSLATOR_HPP

#include <libxml++/libxml++.h>
#include <string>
#include <vector>

#define DECLARE_TRANSLATOR(x) \
    extern "C" { vle::vpz::Translator* makeNewTranslator \
        ( xmlpp::Element* root) { return new x(root); } }

namespace vle { namespace vpz {

    /**
     * @brief Class to translate original XML application into VLE XML
     * application.  Translation use tag NO_VLE from VLE project XML and
     * translate it.
     */
    class Translator
    {
    public :
        /**
         * Make a new translator with an XML reference to no-vle node.
         *
         * @param root XML reference to no-vle node.
         */
        Translator(xmlpp::Element* root);

        /**
         * Delete all XML Document.
         */
        virtual ~Translator();

        /**
         * Call to translate original XML application into VLE XML application
         * into document Structure, Dynamics, Experiment. Call getStructure,
         * getDynamics, getExperiment and getGraphics to get all VLE XML
         * application.
         */
        virtual void translate() = 0;

        /** @return XML Document structures. */
        inline xmlpp::Document* getStructure() const
        { return m_structure; }

        /** @return XML Document dynamics. */
        xmlpp::Document* getDynamics() const
        { return m_dynamics; }

        /** @return XML Document Experiment. */
        xmlpp::Document* getExperiment() const
        { return m_experiment; }

        /** @return XML Document Graphics. */
        xmlpp::Document* getGraphics() const
        { return m_graphics; }

    protected:
        /**
         * Get a reference to the root node of translator XML.
         *
         * @return reference of translator XML.
         */
        xmlpp::Element* getRoot() const;

        void buildDocuments(const std::string& modelName,
                            const std::string& modelType);

        xmlpp::Element* addAtomicModel(xmlpp::Element* node,
                                       const std::string& modelName);

        xmlpp::Element* addCoupledModel(xmlpp::Element* node,
                                        const std::string& modelName);

        void addInternalConnection(xmlpp::Element* node,
                                   const std::string& originModelName,
                                   const std::string& originPortName,
                                   const std::string& destinationModelName,
                                   const std::string& destinationPortName);

	void addInputConnection(xmlpp::Element* node,
				const std::string& originModelName,
				const std::string& originPortName,
				const std::string& destinationModelName,
				const std::string& destinationPortName);
	
	void addOutputConnection(xmlpp::Element* node,
				 const std::string& originModelName,
				 const std::string& originPortName,
				 const std::string& destinationModelName,
				 const std::string& destinationPortName);

        void addPort(xmlpp::Element* node , const std::string& portName);

        xmlpp::Element* addInitPorts(xmlpp::Element* node);

        xmlpp::Element* addInPorts(xmlpp::Element* node);

        xmlpp::Element* addOutPorts(xmlpp::Element* node);

        xmlpp::Element* addStatePorts(xmlpp::Element* node);

        xmlpp::Element* addDoubleParameter(xmlpp::Element* node,
                                           const std::string& name,
                                           double value);

        xmlpp::Element* addIntegerParameter(xmlpp::Element* node,
                                            const std::string& name,
                                            long value);

        xmlpp::Element* addBooleanParameter(xmlpp::Element* node,
                                            const std::string& name,
                                            bool value);

        xmlpp::Element* addStringParameter(xmlpp::Element* node,
                                           const std::string& name,
                                           const std::string& value);

        xmlpp::Element* addModelDynamics(xmlpp::Element* node,
                                         const std::string& name,
                                         const std::string& formalismName,
                                         const std::string& type);

        void setExperiment(xmlpp::Element* node,
                           const std::string& name,
                           const std::string& duration,
                           const std::string& date,
                           const std::string& seed);

        xmlpp::Element* addOutput(xmlpp::Element* node,
                                  const std::string& name,
                                  const std::string& type,
                                  const std::string& format,
                                  const std::string& plugin,
                                  const std::string& location);

        xmlpp::Element* addConditions(xmlpp::Element* node);

        void addBooleanCondition(xmlpp::Element* node,
                                 const std::string& modelName,
                                 const std::string& portName,
                                 bool value);

        void addDoubleCondition(xmlpp::Element* node,
                                const std::string& modelName,
                                const std::string& portName,
                                double value);

        void addIntegerCondition(xmlpp::Element* node,
                                 const std::string& modelName,
                                 const std::string& portName,
                                 long value);

	void addStringCondition(xmlpp::Element* node,
			     const std::string& modelName,
			     const std::string& portName,
			     const std::string& value);

        xmlpp::Element* addMeasures(xmlpp::Element* node);

        xmlpp::Element* addOutputs(xmlpp::Element* node);

        xmlpp::Element* addTimedMeasure(xmlpp::Element* node,
                                        const std::string& measureName,
                                        const std::string& timeStep,
                                        const std::string& outputName);

        xmlpp::Element* addEventMeasure(xmlpp::Element* node,
                                        const std::string& measureName,
                                        const std::string& outputName);

        void addObservable(xmlpp::Element* node,
                           const std::string& modelName,
                           const std::string& portName,
                           const std::string& group = "",
                           int index = -1);

        xmlpp::Document* m_structure;
        xmlpp::Document* m_dynamics;
        xmlpp::Document* m_experiment;
        xmlpp::Document* m_graphics;

    private :
        xmlpp::Element* m_root;
    };

}} // namespace vle vpz

#endif
