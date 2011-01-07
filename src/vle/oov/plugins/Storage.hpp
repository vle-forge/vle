/*
 * @file vle/oov/plugins/Storage.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_OOV_PLUGINS_STORAGE_HPP
#define VLE_OOV_PLUGINS_STORAGE_HPP 1

#include <vle/oov/Plugin.hpp>
#include <vle/oov/OutputMatrix.hpp>
#include <fstream>
#include <map>
#include <vector>

namespace vle { namespace oov { namespace plugin {

class Storage : public Plugin
{
public:
    Storage(const std::string& location);

    virtual ~Storage();

    virtual bool isSerializable() const;

    /**
     * @brief Serialize the storage plugin data's to and output values. The
     * serialization is defined like:
     * @code
     * <set>
     *  <integer>nbcolumns</integer> <!-- x number of columns -->
     *  <integer>nbrow</integer>     <!-- y number of rows -->
     *  <double>time</double>        <!-- double time -->
     *  <set>                        <!-- x set to represents -->
     *   <string>model</string>      <!-- model name -->
     *   <string>port</string>       <!-- port name -->
     *   <integer>index</integer>    <!-- index in the data array -->
     *  </set>
     *  [...] <!-- x number -->
     *  <set>                        <!-- and now, the big array -->
     *   <set>                       <!-- y row -->
     *    [...]                      <!-- x columns -->
     *  </set>
     * </set>
     * @endcode
     * @param pluginname Name of this plugins
     * @return The serialized Storage plugin data's.
     */
    virtual value::Value* serialize() const;

    virtual void deserialize(const value::Value& vals);

    /*
     * override the oov::OutputMatrix function to get the result of a
     * simulation's view.
     */

    virtual bool haveOutputMatrix() const
    { return true; }

    virtual oov::OutputMatrix outputMatrix() const
    { return m_matrix; }

    virtual std::string name() const;

    ///
    ////
    ///

    virtual void onParameter(const std::string& plugin,
                             const std::string& location,
                             const std::string& file,
                             value::Value* parameters,
                             const double& time);

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time);

    virtual void onDelObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time);

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time,
                         value::Value* value);

    virtual void close(const double& time);

    ///
    ////
    ///

    /**
     * @brief Return a constant reference to the oov::OutputMatrix.
     * @return A constant reference.
     */
    inline const oov::OutputMatrix& outputmatrix() const
    { return m_matrix; }

    /**
     * @brief Return a view on the Time vector..
     * @return A view on the Time vector.
     */
    value::VectorView getTime();

    /**
     * @brief Return the Matrix of the values.
     * @return A constant reference to the Matrix.
     */
    inline const OutputMatrix& values() const
    { return m_matrix; }

    /**
     * @brief Return the Matrix of the values.
     * @return A constant reference to the Matrix.
     */
    inline OutputMatrix& values()
    { return m_matrix; }

private:
    OutputMatrix       m_matrix;
    double             m_time;
    bool               m_isstart;


    void nextTime(double trame_time);
};

}}} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::Storage)

#endif
