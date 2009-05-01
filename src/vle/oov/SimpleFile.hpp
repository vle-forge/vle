/**
 * @file vle/oov/SimpleFile.hpp
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


#ifndef VLE_OOV_SIMPLEFILE_HPP
#define VLE_OOV_SIMPLEFILE_HPP

#include <vle/oov/Plugin.hpp>
#include <fstream>
#include <map>
#include <vector>


namespace vle { namespace oov { namespace plugin {

    class SimpleFile : public Plugin
    {
    public:
        SimpleFile(const std::string& location);

        virtual ~SimpleFile();

        ///
        //// the interface provided by oov::Plugin.
        ///

        virtual void onParameter(const std::string& plugin,
                                 const std::string& location,
                                 const std::string& file,
                                 value::Value* parameters,
                                 const double& time);

        virtual void onNewObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
                                     const std::string& view,
                                     const double& time);

        virtual void onDelObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
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
        //// the new interface for derived oov::SimpleFile plug-ins.
        ///

        virtual std::string extension() const = 0;

        virtual void writeSeparator(std::ostream& out) = 0;

        virtual void writeHead(std::ostream& out,
                               const std::vector < std::string >& heads) = 0;

    private:
        /** Define a dictionary (model's name, index) */
        typedef std::map < std::string, int > Columns;

        /** Define the buffer of values. */
        typedef std::vector < value::Value* > Line;

        /** Define the buffer for valid values (model observed). */
        typedef std::vector < bool > ValidElement;

        Columns         m_columns;
        Line            m_buffer;
        ValidElement    m_valid;
        double          m_time;
        std::ofstream   m_file;
        std::string     m_filename;
        std::string     m_filenametmp;
        bool            m_isstart;

        void flush(double trame_time);

        void finalFlush(double trame_time);

        /**
         * @brief This function is use to build uniq name to each row of the
         * text output.
         * @param parent the hierarchy of coupled model.
         * @param simulator the name of the devs::Model.
         * @param port the name of the state port of the devs::Model.
         * @return a representation of the uniq name.
         */
        inline std::string buildname(const std::string& parent,
                                     const std::string& simulator,
                                     const std::string& port)
        {
            std::string r(parent);
            r += ':';
            r += simulator;
            r += '.';
            r += port;
            return r;
        }
    };

}}} // namespace vle oov plugin

#endif
