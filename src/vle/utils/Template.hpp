/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#ifndef VLE_UTILS_TEMPLATE_HPP
#define VLE_UTILS_TEMPLATE_HPP

#include <map>
#include <string>
#include <vector>
#include <vle/DllDefines.hpp>

namespace vle {
namespace utils {

class VLE_API SymbolString
{
public:
    typedef std::map<std::string, std::string> symbols_t;
    typedef symbols_t::const_iterator const_iterator;
    typedef symbols_t::iterator iterator;
    typedef symbols_t::size_type size_type;

    void append(const std::string& key, const std::string& value);
    void remove(const std::string& key);
    const std::string& get(const std::string& key) const;

    const_iterator begin() const
    {
        return lst_.begin();
    }
    const_iterator end() const
    {
        return lst_.end();
    }
    iterator begin()
    {
        return lst_.begin();
    }
    iterator end()
    {
        return lst_.end();
    }
    size_type size() const
    {
        return lst_.size();
    }

private:
    symbols_t lst_;
};

class VLE_API SymbolBool
{
public:
    typedef std::map<std::string, bool> symbols_t;
    typedef symbols_t::const_iterator const_iterator;
    typedef symbols_t::iterator iterator;
    typedef symbols_t::size_type size_type;

    void append(const std::string& key, bool value);
    void remove(const std::string& key);
    bool get(const std::string& key) const;

    const_iterator begin() const
    {
        return lst_.begin();
    }
    const_iterator end() const
    {
        return lst_.end();
    }
    iterator begin()
    {
        return lst_.begin();
    }
    iterator end()
    {
        return lst_.end();
    }
    size_type size() const
    {
        return lst_.size();
    }

private:
    symbols_t lst_;
};

class VLE_API SymbolList
{
public:
    typedef std::vector<std::string> value_t;
    typedef std::map<std::string, value_t> symbols_t;
    typedef symbols_t::const_iterator const_iterator;
    typedef symbols_t::iterator iterator;
    typedef symbols_t::size_type size_type;

    void append(const std::string& key);
    void append(const std::string& key, const std::string& value);
    void remove(const std::string& key);
    const std::string& get(const std::string& key, value_t::size_type i) const;
    value_t::size_type size(const std::string& key) const;

    const_iterator begin() const
    {
        return lst_.begin();
    }
    const_iterator end() const
    {
        return lst_.end();
    }
    iterator begin()
    {
        return lst_.begin();
    }
    iterator end()
    {
        return lst_.end();
    }
    size_type size() const
    {
        return lst_.size();
    }
    const symbols_t& list() const
    {
        return lst_;
    }

private:
    symbols_t lst_;
};

class VLE_API Template
{
public:
    /**
     * @brief Build a template object.
     *
     * @param buffer The template, can be empty.
     */
    Template(const std::string& buffer = std::string())
      : buffer_(buffer)
    {
    }

    /**
     * @brief Open the specified template file and read the entire file into
     * memory.
     * @code
     * vle::utils::Template tpl;
     * tpl.open("/usr/share/vle/template/diffequ.tpl");
     * tpl.stringSymbol(...)
     * tpl.process(std::cout);
     *
     * tpl.open("/usr/share/vle/template/diffequ.tpl");
     * std::string plugin, conf;
     * tpl.tag(plugin, package, conf);
     * @endcode
     *
     * @throw utils::ArgError If an error occurred when open or read the file.
     *
     * @param filename The file to open.
     */
    void open(const std::string& filename);

    /**
     * @brief Copy in the stream the result of the templating operation.
     *
     * @throw utils::ArgError if an error occurred when get symbols in
     * SymbolList, SymbolString or SymbolBool.
     *
     * @param result Output parameter to get the result of templating.
     */
    void process(std::ostream& result) const;

    /**
     * @brief Parse the buffer an retrieve tag:
     * @@tag pluginname@packagename @@
     *  ...
     *  ... // configuration.
     *  ...
     * @@end tag@@
     *
     * @param pluginname The name of the plug-in in tag (output
     * parameter).
     * @param packagename The name of the plug-in in tag (output
     * parameter).
     * @param conf The configuration of the plug-in in tag (output parameter).
     * @throw utils::ArgError if an error occurred when retrieving tag.
     */
    void tag(std::string& pluginname,
             std::string& packagename,
             std::string& conf);

    const SymbolString& stringSymbol() const
    {
        return single_;
    }
    SymbolString& stringSymbol()
    {
        return single_;
    }
    const SymbolBool& boolSymbol() const
    {
        return bool_;
    }
    SymbolBool& boolSymbol()
    {
        return bool_;
    }
    const SymbolList& listSymbol() const
    {
        return list_;
    }
    SymbolList& listSymbol()
    {
        return list_;
    }
    const std::string& buffer() const
    {
        return buffer_;
    }

private:
    std::string buffer_;
    SymbolString single_;
    SymbolBool bool_;
    SymbolList list_;

    std::string processIf(const std::string& in) const;
    std::string processIfnot(const std::string& in) const;
    std::string processFor(const std::string& in) const;
    std::string processName(const std::string& in) const;
};
}
} // namespace vle utils

#endif
