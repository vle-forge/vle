/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_VPZ_SAXPARSER_HPP
#define VLE_VPZ_SAXPARSER_HPP

#include <libxml/SAX2.h>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/SaxStackValue.hpp>
#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>
#include <map>

namespace vle { namespace vpz {

    class Vpz;
    class BaseModel;

    /**
     * @brief The SaxParser is a C/C++ interface between the libxml-2.0 and the
     * VLE classes vpz.
     */
    class VLE_API SaxParser
    {
    public:
        /**
         * @brief Build a new SaxParser to fill the specific Vpz class.
         * @param vpz The Vpz class to fill when reading.
         */
        SaxParser(Vpz& vpz);

        /**
         * @brief Nothing to delete.
         */
        virtual ~SaxParser() {}

        /**
         * @brief Open the VPZ file and parse it with libxml2 SAX2 interface.
         * @param filename The name of the file to parse.
         * @throw utils::SaxError if the file is not readable.
         */
        void parseFile(const std::string& filename);

        /**
         * @brief Open the VPZ file and parse it with libxml2 SAX2 interface.
         * @param buffer the buffer to process.
         * @throw utils::SaxError if the buffer is not readable.
         */
        void parseMemory(const std::string& buffer);

        /**
         * @brief Delete all information from Sax parser like stack, etc. Use it
         * before restart a new parse.
         */
        void clearParserState();

        /**
         * @brief Return true if the SaxParser have read a value.
         * @return true if the parser have read a value, false otherwise.
         */
        inline bool isValue() const
        { return m_isValue; }

        /**
         * @brief Return true if the SaxParser have read a vpz file.
         * @return true if the parser have read a vpz file, false otherwise.
         */
        inline bool isVpz() const
        { return m_isVPZ; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the content of the SaxStackValue.
         * @return A constant reference to the vector of value::Value.
         */
        const std::vector < value::Value* >& getValues() const;

        /**
         * @brief Get the content of the SaxParserError.
         * @return A reference to the vector of value::Value.
         */
        std::vector < value::Value* >& getValues();

        /**
         * @brief Get the content of a specific cells of the SaxStackValue.
         * @param pos The index of the value::Value.
         * @throw utils::SaxParserError if i is greater that size of the vector.
         * @return A constant reference to the value.
         */
        value::Value* getValue(const size_t pos) const;

        /**
         * @brief Get a constant reference to the Vpz.
         * @return A constant reference to the Vpz.
         */
        inline const vpz::Vpz& vpz() const
        { return m_vpz; }

    private:
        static void onStartDocument(void* ctx);

        static void onEndDocument(void* ctx);

        static void onStartElement(void* ctx, const xmlChar* name,
                                   const xmlChar** atts);

        static void onEndElement(void* ctx, const xmlChar* att);

        static void onCharacters(void* ctx, const xmlChar* ch, int len);

        static void onCDataBlock(void* ctx, const xmlChar* value, int len);

        static void onWarning(void *user_data, const char *msg, ...);

        static void onError(void *user_data, const char *msg, ...);

        static void onFatalError(void *user_data, const char *msg, ...);

        /**
         * @brief Get the last characters from internal buffer.
         * @return A characters buffer.
         */
        const std::string& lastCharactersStored() const
        { return m_lastCharacters; }

        /**
         * @brief Clear the internal buffer for last characters.
         */
        void clearLastCharactersStored()
        { m_lastCharacters.clear(); }

        /**
         * @brief Append characters to the last characters readed.
         * @param characters The characters to append.
         */
        void addToCharacters(const std::string& characters)
        { m_lastCharacters.append(characters); }

        /**
         * @brief Stop the parsing of the XML file.
         * @param error The message to store into the error's buffer string.
         */
        void stopParser(const std::string& error);

        /**
         * @brief Return true if the parsing of the XML file has been stopped.
         * @return true if the parser was stopped, false otherwise.
         */
        bool isStopped() const { return m_stop; }

        xmlSAXHandler m_sax;
        bool          m_stop;
        std::string   m_error;
        SaxStackVpz   m_vpzstack;
        ValueStackSax m_valuestack;
        std::string   m_lastCharacters;
        std::string   m_cdata;
        Vpz&          m_vpz;

        bool          m_isValue;
        bool          m_isVPZ;

        struct XmlCompare
        {
            inline bool operator()(const xmlChar* s1, const xmlChar* s2) const
            { return xmlStrcmp(s1, s2) < 0; }
        };

        typedef void (SaxParser::* startfunc)(const xmlChar**);
        typedef void (SaxParser::* endfunc)();
        typedef std::map < const xmlChar*, startfunc, XmlCompare > StartFuncList;
        typedef std::map < const xmlChar*, endfunc, XmlCompare > EndFuncList;

        StartFuncList   m_starts;
        EndFuncList     m_ends;

        void onBoolean(const xmlChar** att);
        void onInteger(const xmlChar** att);
        void onDouble(const xmlChar** att);
        void onString(const xmlChar** att);
        void onSet(const xmlChar** att);
        void onMatrix(const xmlChar** att);
        void onMap(const xmlChar** att);
        void onKey(const xmlChar** att);
        void onTuple(const xmlChar** att);
        void onTable(const xmlChar** att);
        void onXML(const xmlChar** att);
        void onNull(const xmlChar** att);
        void onVLEProject(const xmlChar** att);
        void onStructures(const xmlChar** att);
        void onModel(const xmlChar** att);
        void onIn(const xmlChar** att);
        void onOut(const xmlChar** att);
        void onPort(const xmlChar** att);
        void onSubModels(const xmlChar** att);
        void onConnections(const xmlChar** att);
        void onConnection(const xmlChar** att);
        void onOrigin(const xmlChar** att);
        void onDestination(const xmlChar** att);
        void onDynamics(const xmlChar** att);
        void onDynamic(const xmlChar** att);
        void onExperiment(const xmlChar** att);
        void onReplicas(const xmlChar** att);
        void onConditions(const xmlChar** att);
        void onCondition(const xmlChar** att);
        void onViews(const xmlChar** att);
        void onOutputs(const xmlChar** att);
        void onOutput(const xmlChar** att);
        void onView(const xmlChar** att);
        void onObservables(const xmlChar** att);
        void onObservable(const xmlChar** att);
        void onAttachedView(const xmlChar** att);
        void onClasses(const xmlChar** att);
        void onClass(const xmlChar** att);

        void onEndBoolean();
        void onEndInteger();
        void onEndDouble();
        void onEndString();
        void onEndSet();
        void onEndMatrix();
        void onEndMap();
        void onEndKey();
        void onEndTuple();
        void onEndTable();
        void onEndXML();
        void onEndNull();
        void onEndVLEProject();
        void onEndStructures();
        void onEndModel();
        void onEndIn();
        void onEndOut();
        void onEndPort();
        void onEndSubModels();
        void onEndConnections();
        void onEndConnection();
        void onEndOrigin();
        void onEndDestination();
        void onEndDynamics();
        void onEndDynamic();
        void onEndExperiment();
        void onEndReplicas();
        void onEndConditions();
        void onEndCondition();
        void onEndViews();
        void onEndOutputs();
        void onEndOutput();
        void onEndView();
        void onEndObservables();
        void onEndObservable();
        void onEndAttachedView();
        void onEndClasses();
        void onEndClass();

        void add(const char* name, startfunc fct, endfunc efct)
        {
            m_starts[(const xmlChar*)name] = fct;
            m_ends[(const xmlChar*)name] = efct;
        }

        void fillTagList()
        {
            add("boolean", &SaxParser::onBoolean, &SaxParser::onEndBoolean);
            add("integer", &SaxParser::onInteger, &SaxParser::onEndInteger);
            add("double", &SaxParser::onDouble, &SaxParser::onEndDouble);
            add("string", &SaxParser::onString, &SaxParser::onEndString);
            add("set", &SaxParser::onSet, &SaxParser::onEndSet);
            add("matrix", &SaxParser::onMatrix, &SaxParser::onEndMatrix);
            add("map", &SaxParser::onMap, &SaxParser::onEndMap);
            add("key", &SaxParser::onKey, &SaxParser::onEndKey);
            add("tuple", &SaxParser::onTuple, &SaxParser::onEndTuple);
            add("table", &SaxParser::onTable, &SaxParser::onEndTable);
            add("xml", &SaxParser::onXML, &SaxParser::onEndXML);
            add("null", &SaxParser::onNull, &SaxParser::onEndNull);
            add("vle_project", &SaxParser::onVLEProject,
                &SaxParser::onEndVLEProject);
            add("structures", &SaxParser::onStructures,
                &SaxParser::onEndStructures);
            add("model", &SaxParser::onModel, &SaxParser::onEndModel);
            add("in", &SaxParser::onIn, &SaxParser::onEndIn);
            add("out", &SaxParser::onOut, &SaxParser::onEndOut);
            add("port", &SaxParser::onPort, &SaxParser::onEndPort);
            add("submodels", &SaxParser::onSubModels,
                &SaxParser::onEndSubModels);
            add("connections", &SaxParser::onConnections,
                &SaxParser::onEndConnections);
            add("connection", &SaxParser::onConnection,
                &SaxParser::onEndConnection);
            add("origin", &SaxParser::onOrigin, &SaxParser::onEndOrigin);
            add("destination", &SaxParser::onDestination,
                &SaxParser::onEndDestination);
            add("dynamics", &SaxParser::onDynamics, &SaxParser::onEndDynamics);
            add("dynamic", &SaxParser::onDynamic, &SaxParser::onEndDynamic);
            add("experiment", &SaxParser::onExperiment,
                &SaxParser::onEndExperiment);
            add("conditions", &SaxParser::onConditions,
                &SaxParser::onEndConditions);
            add("condition", &SaxParser::onCondition, &SaxParser::onEndCondition);
            add("views", &SaxParser::onViews, &SaxParser::onEndViews);
            add("outputs", &SaxParser::onOutputs, &SaxParser::onEndOutputs);
            add("output", &SaxParser::onOutput, &SaxParser::onEndOutput);
            add("view", &SaxParser::onView, &SaxParser::onEndView);
            add("observables", &SaxParser::onObservables,
                &SaxParser::onEndObservables);
            add("observable", &SaxParser::onObservable,
                &SaxParser::onEndObservable);
            add("attachedview", &SaxParser::onAttachedView,
                &SaxParser::onEndAttachedView);
            add("classes", &SaxParser::onClasses, &SaxParser::onEndClasses);
            add("class", &SaxParser::onClass, &SaxParser::onEndClass);
        }
    };

    /**
     * @brief Convert the xmlChar pointer to a char pointer.
     * @param str The constant xmlChar pointer to cast.
     * @return A constant char pointer to cast.
     */
    inline const char* xmlCharToString(const xmlChar* str)
    { return (const char*)str; }

    /**
     * @brief Convert the xmlChar pointer to a boolean.
     * @param str The constant xmlChar pointer to translator.
     * @throw utils::SaxParserError if the xmlChar can not be translated into a
     * boolean
     * @return The boolean.
     */
    VLE_API bool xmlCharToBoolean(const xmlChar* str);

    /**
     * @brief Convert the xmlChar pointer to a long integer.
     * @param str The constant xmlChar pointer to translate.
     * @throw utils::SaxParserError if the xmlChar can not be translated into a
     * long integer.
     * @return The long integer.
     */
    VLE_API long int xmlCharToInt(const xmlChar* str);

    /**
     * @brief Convert the xmlChar pointer to an unsigned long integer.
     * @param str The constant xmlChar pointer to translate.
     * @throw utils::SaxParserError if the xmlChar can not be translated into an
     * unsigned long integer.
     * @return The unsigned long integer.
     */
    VLE_API unsigned long int xmlCharToUnsignedInt(const xmlChar* str);

    /**
     * @brief Convert the xmlChar pointer to a double.
     * @param str The constant xmlChar pointer to translate.
     * @throw utils::SaxParserError if the xmlChar can not be translated into a
     * double
     * @return The double.
     */
    VLE_API double xmlCharToDouble(const xmlChar* str);

}} // namespace vle vpz

#endif
