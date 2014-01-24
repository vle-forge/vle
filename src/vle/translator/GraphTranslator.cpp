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


#include <vle/translator/GraphTranslator.hpp>
#include <boost/tokenizer.hpp>

namespace vle { namespace translator {

void GraphTranslator::translate(const value::Map& buffer)
{
    const value::Map& init = toMapValue(buffer);

    mNodeNumber = toInteger(init.get("number"));
    if (mNodeNumber <= 0) {
        throw utils::ArgError("GraphTranslator: bad node number");
    } else {
        BoolArray::extent_gen extents;

        mGraph.resize(extents[mNodeNumber][mNodeNumber]);
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
            mGraph[j][i] = ((*it) == "1");

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
        std::string name = (boost::format("%1%-%2%") % mPrefix % i).str();
        createNewNode(name, mClass[i]);
    }

    for (size_type i = 0; i < mNodeNumber; ++i) {
        for (size_type j = 0; j < mNodeNumber; ++j) {
            if (mGraph[j][i]) {
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

}} //namespace vle translator
