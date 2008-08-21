/**
 * @file vle/gvle/Measures.cpp
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


#include <vle/gvle/Measures.hpp>
#include <vle/utils/Debug.hpp>

using std::string;
using std::list;
using std::map;
using namespace vle;

namespace vle
{
namespace gvle {

void Measures::delOutputs(const string& name)
{
    Outputs_t::iterator it = mOutputs.find(name);
    if (it != mOutputs.end()) {
        mOutputs.erase(it);
    }
}

void Measures::delEovs(const std::string& name)
{
    Eovs_t::iterator it = mEovs.find(name);
    if (it != mEovs.end()) {
        mEovs.erase(it);
    }
}

void Measures::addMeasure(const string& name)
{
    mMeasures[name] = Measure_t(string(), list < Observable_t >());
}

void Measures::delMeasure(const string& name)
{
    Measures_t::iterator it = mMeasures.find(name);
    if (it != mMeasures.end()) {
        mMeasures.erase(it);
    }
}

void Measures::addXMLToMeasure(const string& name, const string& xml)
{
    Measures_t::iterator it = mMeasures.find(name);
    AssertI(it != mMeasures.end());

    (*it).second.first.assign(xml);
}

void Measures::addObservableToMeasure(const string& name,
                                      const string& model,
                                      const string& port,
                                      const string& group,
                                      const string& index)
{
    Measures_t::iterator it = mMeasures.find(name);
    AssertI(it != mMeasures.end());

    (*it).second.second.push_back(Observable_t(model, port, group, index));
}

void Measures::delObservableToMeasure(const string& name, const string&
                                      model, const string& port)
{
    Measures_t::iterator it = mMeasures.find(name);
    AssertI(it != mMeasures.end());

    std::list < Observable_t >::iterator jt = (*it).second.second.begin();
    while (jt != (*it).second.second.end()) {
        if (jt->model == model and jt->port == port) {
            (*it).second.second.erase(jt);
            break;
        }
        ++jt;
    }
}

const Measures::Measure_t& Measures::getMeasure(const std::string& name) const
{
    Measures_t::const_iterator it = mMeasures.find(name);
    AssertI(it != mMeasures.end());
    return (*it).second;
}

}
} // namespace vle gvle
