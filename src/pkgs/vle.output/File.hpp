/*
 * @file vle/oov/plugins/File.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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
#ifndef VLE_OOV_PLUGINS_FILE_HPP
#define VLE_OOV_PLUGINS_FILE_HPP 1

#include <fstream>
#include <map>
#include <vector>
#include <vle/oov/Plugin.hpp>
#include <vle/value/Set.hpp>

namespace vle {
namespace oov {
namespace plugin {

/**
 * @brief File is a virtual class for the csv, text, and rdata
 * plug-in.
 * When simulation is running, File writes information into a
 * tempory file localized into the local directory or in the directory
 * specified in the parameter trame.
 * The File accepts a value::Map in parameter with two keys:
 * - out: define the type of output. By default, it uses and file. But if
 *   the value equal 'out', it copy result into the standard output and if
 *   the value equal 'error', it copy result into the stardard error.
 * - locale: define the locale of the stream. By default, the locale 'C' is
 *   used, ie. the C Ansi locale. If the value equal 'user' then the locale
 *   is attached to the locale of the user (run locale). Otherwise, the user
 *   can use all locale defines in the environment. Use the 'locale -a'
 *   command to show all locale of your system.
 * - flush-by-bag: If the value is true, an output is provided for
 * each bag.
 * <map>
 *  <key name="output">
 *   <string>out</string> <!-- or 'error' -->
 *  </key>
 *  <key name="type">
 *   <string>csv, text, rdata</string>
 *  </key>
 *  <key name="locale">
 *   <string>fr_FR.UTF-8</string> <!-- 'user', 'C', '' etc.
 *  </key>
 * </map>
 */
class File : public Plugin
{
public:
    /**
     * @brief Defines the names of the columns.
     */
    typedef std::vector<std::string> Strings;

    File(const std::string& location);

    virtual ~File();

    ///
    //// the interface provided by oov::Plugin.
    ///

    virtual void onParameter(const std::string& plugin,
                             const std::string& location,
                             const std::string& file,
                             std::unique_ptr<value::Value> parameters,
                             const double& time) override;

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time) override;

    virtual void onDelObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time) override;

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time,
                         std::unique_ptr<value::Value> value) override;

    virtual std::unique_ptr<value::Matrix> finish(const double& time) override;

    class FileType
    {
    public:
        virtual ~FileType()
        {
        }

        virtual std::string extension() const = 0;

        virtual void writeSeparator(std::ostream& out) = 0;

        virtual void writeHead(std::ostream& out, const Strings& heads) = 0;
    };

private:
    /** Define a dictionary (model's name, index) */
    typedef std::map<std::string, int> Columns;

    /** Define the buffer of values. */
    typedef std::vector<value::Value*> Line;

    /** Define the buffer for valid values (model observed). */
    typedef std::vector<bool> ValidElement;

    /** Define a new bag indicator*/
    typedef std::map<std::string, double> NewBagWatcher;

    enum OutputType
    {
        FILE,          /*!< classical file stream (std::ofstream). */
        STANDARD_OUT,  /*!< use the standard output (std::cout). */
        STANDARD_ERROR /*!< use the error output (std::cerr). */
    };

    FileType* m_filetype;
    Columns m_columns;
    value::Set m_buffer;
    ValidElement m_valid;
    NewBagWatcher m_newbagwatcher;
    double m_time;
    std::ofstream m_file;
    std::string m_filename;
    std::string m_filenametmp;
    bool m_isstart;
    bool m_havefirstevent;
    bool m_julian;
    OutputType m_type;
    bool m_flushbybag;

    void flush();

    void finalFlush(double trame_time);

    void copyToFile(const std::string& filename,
                    const std::vector<std::string>& array);

    void copyToStream(std::ostream& out,
                      const std::vector<std::string>& array);

    /**
     * @brief This function is use to build uniq name to each row of the
     * text output.
     * @param parent the hierarchy of coupled model.
     * @param simulator the name of the devs::Model.
     * @param port the name of the state port of the devs::Model.
     * @return a representation of the uniq name.
     */
    std::string buildname(const std::string& parent,
                          const std::string& simulator,
                          const std::string& port);
};
}
}
} // namespace vle oov plugin

#endif
