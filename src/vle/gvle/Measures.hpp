/**
 * @file vle/gvle/Measures.hpp
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


#ifndef GUI_MEASURES_HPP
#define GUI_MEASURES_HPP

#include <map>
#include <list>
#include <string>

namespace vle
{
namespace gvle {

/**
 * @brief A class to store the VLE project measure information. It store
 * output, measure, observable an the links between us.
 */
class Measures
{
public:
    /** define an Output like a couple Name, xml tree */
    typedef std::map < std::string, std::string > Outputs_t;

    /** define a Display like a couple Name, xml tree */
    typedef std::map < std::string, std::string > Eovs_t;

    /** define an Observable like couple Model name, port
     * Name */
    class Observable_t
    {
    public:
        std::string model;
        std::string port;
        std::string group;
        std::string index;

        Observable_t(const std::string& m, const std::string& p,
                     const std::string& g, const std::string& i):
                model(m),port(p),group(g),index(i) { }
    };

    /** define a Measure like a couple XML tree, list Observable */
    typedef std::pair < std::string, std::list < Observable_t > > Measure_t;

    /** define Measures like map of Measure name and Measure */
    typedef std::map < std::string, Measure_t > Measures_t;

public:
    /**
     * Create a class Measures for manage experiment measures
     */
    Measures() {}

    /**
     * Nothing, all is automatic
     */
    ~Measures() {}

    /**
     * Add an output type to Measures
     *
     * @param name output name
     * @param xml representation of output
     */
    void addOutputs(const std::string& name, const std::string& xml) {
        mOutputs[name] = xml;
    }

    const std::string& getOutputs(const std::string& name) {
        return mOutputs[name];
    }

    /**
     * Delete output type with name name
     *
     * @param name output name to delete
     */
    void delOutputs(const std::string& name);

    /**
     * Return all current outputs
     *
     * @return all outputs define
     */
    const Measures::Outputs_t& getOutputs() const {
        return mOutputs;
    }


    /**
     * Add an display type to Measures
     *
     * @param name display name
     * @param xml representation of display
     */
    void addEovs(const std::string& name, const std::string& xml) {
        mEovs[name] = xml;
    }

    const std::string& getEovs(const std::string& name) {
        return mEovs[name];
    }

    /**
     * Delete output type with name name
     *
     * @param name output name to delete
     */
    void delEovs(const std::string& name);

    /**
     * Return all current displays
     *
     * @return all displays define
     */
    const Measures::Eovs_t& getEovs() const {
        return mEovs;
    }

    /**
     * Add a Measure to Experience
     *
     * @param name new Measure name to add
     */
    void addMeasure(const std::string& name);

    /**
     * Delete a Measure from Experience with all Data linked
     *
     * @param name Measure name to delete
     */
    void delMeasure(const std::string& name);

    /**
     * Add an XML expression to the Measure. XML describe TYPE, TIME_STEP
     * data information.
     *
     * @param name Measure name to add XML
     * @param xml expression to add to Measure
     * @throw Exception::Internal if name don't exist
     */
    void addXMLToMeasure(const std::string& name, const std::string& xml);

    /**
     * Add an Obserbable to Measure name
     *
     * @param name Measure name to add XML
     * @param modelName model name to observe
     * @param portName port name of model to observe
     * @throw Exception::Internal if name don't exist
     */
    void addObservableToMeasure(const std::string& name,
                                const std::string& modelName,
                                const std::string& portName,
                                const std::string& groupName,
                                const std::string& index);

    /**
     * Delete an Obserbable to Measure name
     *
     * @param name Measure name to delete
     * @param modelName model name to observe
     * @param portName port name of model to observe
     * @throw Exception::Internal if name don't exist
     */
    void delObservableToMeasure(const std::string& name,
                                const std::string& modelName,
                                const std::string& portName);

    /**
     * Return all current measures
     *
     * @return all current measures define
     */
    const Measures::Measures_t& getMeasures() const {
        return mMeasures;
    }

    /**
     * Return measure with name name from all current measure
     *
     * @param name measure name to get
     * @return measure find
     * @throw Exception::Internal if measure name not found
     */
    const Measures::Measure_t& getMeasure(const std::string& name) const;

    /**
     * Return duration of experiment
     *
     * @return double > 0
     */
    double duration() const {
        return mDuration;
    }

    /**
     * Set a new duration to Experiment
     *
     * @param duration new duration. If duration < 0, then do nothing
     */
    void setDuration(double duration) {
        mDuration = (duration > 0) ? duration : mDuration;
    }

    /**
     * Return name of experiment
     *
     * @return name of experiment
     */
    const std::string & experimentName() const {
        return mExperimentName;
    }

    /**
     * Set a new name of Experiment
     *
     * @param duration new name of experiment.
     */
    void setExperimentName(const std::string & name) {
        mExperimentName = name;
    }

private:
    Measures_t  mMeasures;
    Outputs_t   mOutputs;

    // kiwamu
    Eovs_t mEovs;

    double      mDuration;
    std::string mExperimentName;
};

}
} // namespace vle gvle

#endif
