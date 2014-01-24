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


#ifndef VLE_TRANSLATOR_GRAPHTRANSLATOR_HPP
#define VLE_TRANSLATOR_GRAPHTRANSLATOR_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Executive.hpp>
#include <boost/multi_array.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {

/**
 * @brief A translator to build a DEVS graph where nodes are vpz::Class. The
 * graph uses an adjacency matrix to build connections between nodes.
 * @code
 * <map>
 *  <key name="prefix">
 *   <string>node</string> <!-- build node-0, node-1 etc. Default is vertex -->
 *  </key>
 *  <key name="number">
 *   <integer>10</integer> <!-- number of node in the graph. -->
 *  </key>
 *  <key name="adjacency matrix">
 *   <string>
 *    <!-- build a input, output ports and a connection. For example, the first
 *    operation is:
 *      * node-0 add an output port node-1.
 *      * node-1 add an input port node-0.
 *      * add connection (node-0, node-1) to (node-1 port node-0).
 *      -->
 *    0 1 0 1 1 1 1
 *    0 0 0 0 1 1 1
 *    0 0 0 0 0 1 1
 *    0 0 0 0 0 0 1
 *    0 0 0 0 0 0 0
 *    0 0 1 0 1 0 0
 *    0 0 0 1 0 0 0
 *   </string>
 *  </key>
 *  <key name="classes">
 *   <string>
 *   <!-- one class per node -->
 *   class1 class2 class3 class4 class5
 *   class6 class7
 *   </string>
 *  </key>
 *  <key name="port">
 *   <string>
 *   <!-- Type of connection:
 *     * in-out: add an output port `out' for source model and an input port
 *     `in' for destination model.
 *     * in: add an output port with same name as the destination model, and an
 *     input port `in'.
 *     * out: add an output port `out' for source model and an input port with
 *     the same name as the source model.
 *     * other: add an output port with same name as the destination model and
 *     an input port with the same name as the source model.
 *   </string>
 *  </key>
 * </map>
 * @endcode
 */
class VLE_API GraphTranslator
{
public:
    typedef boost::multi_array < bool, 2 > BoolArray;
    typedef BoolArray::iterator iterator;
    typedef BoolArray::const_iterator const_iterator;
    typedef BoolArray::size_type size_type;

    typedef std::vector < std::string > Strings;
    typedef Strings::size_type index;


    /**
     * @brief Build an empty GraphTranslator, zero node and the prefix of node
     * is `vertex'.
     * @param exe The executive to manipulate coupled model
     * (graph::CoupledModel) and coordinator (devs::Coordinator)..
     */
    GraphTranslator(devs::Executive& exe)
        : mExecutive(exe), mNodeNumber(0), mPrefix("vertex")
    {}

    virtual ~GraphTranslator()
    {}

    /**
     * @brief Build the graph translator.
     * @param buffer A value::Map which contains the adjacency matrix, classes
     * etc.
     * @throw utils::ArgError if the value::Map contains bad parameters,
     * utils::Exception if error in value.
     */
    void translate(const value::Map& buffer);

    inline int getNodeNumber() const { return mNodeNumber; }
    inline const std::string& getNode(index i) const { return mNode[i]; }
    inline const std::string& getClass(index i) const { return mClass[i]; }

    inline iterator begin() { return mGraph.begin(); }
    inline iterator end() { return mGraph.end(); }
    inline const_iterator begin() const { return mGraph.begin(); }
    inline const_iterator end() const { return mGraph.end(); }
    inline size_type size() const { return mGraph.size(); }

private:
    devs::Executive& mExecutive;
    unsigned int mNodeNumber;
    BoolArray mGraph;
    std::vector < std::string > mNode;
    std::vector < std::string > mClass;
    std::string mPrefix;
    std::string mPort;

    void makeBigBang();
    void createNewNode(const std::string& name, std::string& classname);
    void connectNodes(unsigned int from, unsigned int to);
};

}} // namesapce vle translator

#endif
