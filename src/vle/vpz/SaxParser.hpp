/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Base.hpp>

#include "vpz/SaxStackValue.hpp"
#include "vpz/SaxStackVpz.hpp"

#include <map>

namespace vle {
namespace vpz {

class Vpz;
class BaseModel;

/**
 * @brief The SaxParser is a C/C++ interface between the libexpat and the VLE
 *     classes vpz.
 */
class SaxParser
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
    virtual ~SaxParser() = default;

    /**
     * @brief Open the VPZ file and parse it with @c parse function.
     * @param filename The name of the file to parse.
     * @throw utils::SaxError if the file is not readable.
     */
    void parseFile(const std::string& filename);

    /**
     * @brief Open the VPZ file and parse it with @c parse function.
     * @param buffer the buffer to process.
     * @throw utils::SaxError if the buffer is not readable.
     */
    void parseMemory(const std::string& buffer);

    /**
     * @brief Read vpz xml from the input stream.
     * @details Use the libexpat to parse the input stream using chunk of @c
     *     size bytes.
     *
     * @param is the input stream.
     * @param size the size in byte of the chunk.
     *
     * @thow utils::SaxParserError if read or parse operation fails.
     */
    void parse(std::istream& is, std::size_t size);

    /**
     * @brief Return true if the SaxParser have read a value.
     * @return true if the parser have read a value, false otherwise.
     */
    inline bool isValue() const
    {
        return m_isValue;
    }

    /**
     * @brief Return true if the SaxParser have read a vpz file.
     * @return true if the parser have read a vpz file, false otherwise.
     */
    inline bool isVpz() const
    {
        return m_isVPZ;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set functions.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get the content of the SaxStackValue.
     * @return A constant reference to the vector of value::Value.
     */
    const std::vector<std::shared_ptr<value::Value>>& getValues() const;

    /**
     * @brief Get the content of the SaxParserError.
     * @return A reference to the vector of value::Value.
     */
    std::vector<std::shared_ptr<value::Value>>& getValues();

    /**
     * @brief Get the content of a specific cells of the SaxStackValue.
     * @param pos The index of the value::Value.
     * @throw utils::SaxParserError if i is greater that size of the vector.
     * @return A constant reference to the value.
     */
    const std::shared_ptr<value::Value>& getValue(const size_t pos) const;

    /**
     * @brief Get a constant reference to the Vpz.
     * @return A constant reference to the Vpz.
     */
    inline const vpz::Vpz& vpz() const
    {
        return m_vpz;
    }

    /**
     * @brief Get the last characters from internal buffer.
     * @return A characters buffer.
     */
    const std::string& lastCharactersStored() const
    {
        return m_lastCharacters;
    }

    /**
     * @brief Clear the internal buffer for last characters.
     */
    void clearLastCharactersStored()
    {
        m_lastCharacters.clear();
    }

    /**
     * @brief Append characters to the last characters readed.
     * @param characters The characters to append.
     */
    void addToCharacters(const std::string& characters)
    {
        m_lastCharacters.append(characters);
    }

    /**
     * @brief Get the last characters from internal buffer.
     * @return A characters buffer.
     */
    const std::string& lastCdataStored() const
    {
        return m_cdata;
    }

    /**
     * @brief Clear the internal buffer for last characters.
     */
    void clearLastCdataStored()
    {
        m_cdata.clear();
    }

    /**
     * @brief Append characters to the last characters readed.
     * @param characters The characters to append.
     */
    void addToCdata(const std::string& characters)
    {
        m_cdata.append(characters);
    }

    /**
     * @brief Stop the parsing of the XML file.
     * @param error The message to store into the error's buffer string.
     */
    void stopParser(const std::string& error);

    void onBoolean(const char** att);
    void onInteger(const char** att);
    void onDouble(const char** att);
    void onString(const char** att);
    void onSet(const char** att);
    void onMatrix(const char** att);
    void onMap(const char** att);
    void onKey(const char** att);
    void onTuple(const char** att);
    void onTable(const char** att);
    void onXML(const char** att);
    void onNull(const char** att);
    void onVLEProject(const char** att);
    void onStructures(const char** att);
    void onModel(const char** att);
    void onIn(const char** att);
    void onOut(const char** att);
    void onPort(const char** att);
    void onSubModels(const char** att);
    void onConnections(const char** att);
    void onConnection(const char** att);
    void onOrigin(const char** att);
    void onDestination(const char** att);
    void onDynamics(const char** att);
    void onDynamic(const char** att);
    void onExperiment(const char** att);
    void onReplicas(const char** att);
    void onConditions(const char** att);
    void onCondition(const char** att);
    void onViews(const char** att);
    void onOutputs(const char** att);
    void onOutput(const char** att);
    void onView(const char** att);
    void onObservables(const char** att);
    void onObservable(const char** att);
    void onAttachedView(const char** att);
    void onClasses(const char** att);
    void onClass(const char** att);

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

    void clear();
    void error(std::string error);
    std::string error() const;

private:
    void* m_ctxt;
    std::string m_error;
    SaxStackVpz m_vpzstack;
    ValueStackSax m_valuestack;
    std::string m_lastCharacters;
    std::string m_cdata;
    Vpz& m_vpz;

    bool m_isValue;
    bool m_isVPZ;
};

/**
 * @brief Convert the char pointer to a boolean.
 * @param str The constant char pointer to translator.
 * @throw utils::SaxParserError if the char can not be translated into a
 * boolean
 * @return The boolean.
 */
bool
charToBoolean(const char* str);

/**
 * @brief Convert the char pointer to a long integer.
 * @param str The constant char pointer to translate.
 * @throw utils::SaxParserError if the char can not be translated into a
 * long integer.
 * @return The long integer.
 */
long int
charToInt(const char* str);

/**
 * @brief Convert the char pointer to an unsigned long integer.
 * @param str The constant char pointer to translate.
 * @throw utils::SaxParserError if the char can not be translated into an
 * unsigned long integer.
 * @return The unsigned long integer.
 */
unsigned long int
charToUnsignedInt(const char* str);

/**
 * @brief Convert the char pointer to double.
 * @param str The constant char pointer to translate.
 * @throw utils::SaxParserError if the char can not be translated into an
 * double.
 * @return The double.
 */
double
charToDouble(const char* str);
}
} // namespace vle vpz

#endif
