/**
 * @file vle/oov/plugins/Storage.hpp
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


#ifndef VLE_OOV_PLUGINS_RPLUG_HPP
#define VLE_OOV_PLUGINS_RPLUG_HPP

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
        virtual value::Value serialize() const;

        virtual void deserialize(value::Value& vals);

        /*
         * override the oov::OutputMatrix function to get the result of a
         * simulation's view.
         */

        virtual bool haveOutputMatrix() const
        { return true; }

        virtual oov::OutputMatrix outputMatrix() const
        { return m_matrix; }

        virtual std::string name() const;

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);


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
        value::MatrixFactory::VectorView getTime();

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

    DECLARE_OOV_PLUGIN(vle::oov::plugin::Storage)

}}} // namespace vle oov plugin

#endif
