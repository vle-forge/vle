/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#include <vle/translator/GraphTranslator.hpp>
#include <vle/utils/Array.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {

class VLE_API GraphTranslator
{
public:
    typedef vle::utils::Array<bool> BoolArray;
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
        : mExecutive(exe)
        , mNodeNumber(0)
        , mPrefix("vertex")
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

void GraphTranslator::translate(const value::Map& buffer)
{
    const value::Map& init = toMapValue(buffer);

    mNodeNumber = toInteger(init.get("number"));
    if (mNodeNumber <= 0) {
        throw utils::ArgError("GraphTranslator: bad node number");
    } else {
        mGraph.resize(mNodeNumber, mNodeNumber, false);
    }

    if (init.exist("prefix")) {
        mPrefix = toString(init.get("prefix"));
        if (mPrefix.empty()) {
            throw utils::ArgError("GraphTranslator: bad prefix");
        }
    }

    if (init.exist("port")) {
        mPort = toString(init.get("port"));
    }

    std::string adjmat = toString(init.get("adjacency matrix"));
    std::string classes = toString(init.get("classes"));

    typedef boost::tokenizer < boost::char_separator < char > > tokenizer;
    boost::char_separator<char> sep(" \n\t\r");

    {
        tokenizer tok(adjmat, sep);

        size_type i = 0, j = 0;
        for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
            mGraph.set(j, i, (*it) == "1");

            ++i;
            if (i == mNodeNumber) {
                i = 0;
                ++j;
            }
        }

        if (j * mNodeNumber + i != mNodeNumber * mNodeNumber) {
            throw utils::ArgError("GraphTranslator: bad node number in matrix");
        }
    }

    {
        tokenizer tok(classes, sep);

        for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
            mClass.push_back(*it);
        }
    }

    if (mClass.size() != mNodeNumber) {
        throw utils::ArgError("GraphTranslator: bad node number in class");
    }

    makeBigBang();
}

void GraphTranslator::makeBigBang()
{
    for (size_type i = 0; i < mNodeNumber; ++i){
        std::string name = (fmt("%1%-%2%") % mPrefix % i).str();
        createNewNode(name, mClass[i]);
    }

    for (size_type i = 0; i < mNodeNumber; ++i) {
        for (size_type j = 0; j < mNodeNumber; ++j) {
            if (mGraph(j, i)) {
                connectNodes(j, i);
            }
        }
    }
}

void GraphTranslator::createNewNode(const std::string& name,
                                    std::string& classname)
{
    mExecutive.createModelFromClass(classname, name);
    mNode.push_back(name);
}

void GraphTranslator::connectNodes(unsigned int from, unsigned int to)
{
    if (mPort == "in-out") {
        mExecutive.addOutputPort(mNode[from], "out");
        mExecutive.addInputPort(mNode[to], "in");
        mExecutive.addConnection(mNode[from], "out", mNode[to], "in");
    } else if (mPort == "in") {
        mExecutive.addOutputPort(mNode[from], mNode[to]);
        mExecutive.addInputPort(mNode[to], "in");
        mExecutive.addConnection(mNode[from], mNode[to], mNode[to], "in");
    } else if (mPort == "out") {
        mExecutive.addOutputPort(mNode[from], "out");
        mExecutive.addInputPort(mNode[to], mNode[from]);
        mExecutive.addConnection(mNode[from], "out", mNode[to], mNode[from]);
    } else {
        mExecutive.addOutputPort(mNode[from], mNode[to]);
        mExecutive.addInputPort(mNode[to], mNode[from]);
        mExecutive.addConnection(mNode[from], mNode[to], mNode[to],
                                 mNode[from]);
    }
}

graph_result VLE_API make_graph(vle::devs::Executive& executive,
                                const vle::value::Map& map)
{
    GraphTranslator translator(executive);
    translator.translate(map);

    graph_result ret;
    ret.graph = translator.mGraph;
    ret.nodes = translator.mNode;
    ret.classes = translator.mClass;
    ret.node_number = translator.mNodeNumber;

    return ret;
}

}} //namespace vle translator
