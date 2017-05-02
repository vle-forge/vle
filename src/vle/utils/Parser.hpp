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

#ifndef VLE_UTILS_PARSER_HPP
#define VLE_UTILS_PARSER_HPP 1

#include <istream>
#include <map>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/utils/Exception.hpp>

namespace vle {
namespace utils {

struct VLE_API Block
{
    typedef std::multimap<std::string, std::string> Strings;
    typedef std::multimap<std::string, double> Reals;
    typedef std::multimap<std::string, double> RelativeReals;
    typedef std::multimap<std::string, Block> Blocks;

    typedef std::pair<Reals::const_iterator, Reals::const_iterator>
      RealsResult;
    typedef std::pair<RelativeReals::const_iterator,
                      RelativeReals::const_iterator>
      RelativeRealsResult;
    typedef std::pair<Strings::const_iterator, Strings::const_iterator>
      StringsResult;
    typedef std::pair<Blocks::const_iterator, Blocks::const_iterator>
      BlocksResult;

    Block(const std::string& name)
      : name(name)
    {
    }

    void addString(const std::string& key, const std::string& str);

    void addReal(const std::string& key, const double& val);

    void addRelativeReal(const std::string& key, const double& val);

    Block& addBlock(const std::string& name);

    const Block& getBlock(const std::string& name) const;

    std::string name;
    Strings strings;
    RelativeReals relativeReals;
    Reals reals;
    Blocks blocks;
};

class VLE_API Parser
{
public:
    /**
     * Parser for a classical Unix file format.
     * @code
     * # the `#' character to build a comment.
     * test { # a block
     *    a = "a"; # a list of value
     *    b = 12.345;
     *    c = "a", "b", "c";
     *
     *    subblock { # a block may containt block
     *      x = "12.32";
     *      y = "65.12";
     *    }
     *    block {
     *    }
     * }
     * @code
     *
     * @endcode
     *
     * @param stream A standard input stream to read data.
     * @throw utils::ParseError if an error occured during parsing file.
     */
    Parser(std::istream& stream) throw(utils::ParseError);

    /**
     * @brief Access to the root of the data.
     *
     * @return A constant reference to the root.
     */
    inline const Block& root() const
    {
        return mRoot;
    }

private:
    inline char get()
    {
        mLast = mStream.get();

        if (mLast == '\n') {
            mOldLine = mLine++;
            mOldColumn = mColumn;
            mColumn = 0;
        } else {
            mOldColumn = mColumn++;
        }

        return mLast;
    }

    inline void unget()
    {
        if (mLast == '\n') {
            mLine = mOldLine;
            mColumn = mOldColumn;
        } else {
            mColumn = mOldColumn;
        }

        mStream.unget();
    }

    inline void forgetLine()
    {
        std::string tmp;
        std::getline(mStream, tmp);

        mOldColumn = mColumn;
        mColumn = 0;
        mOldLine = mLine++;
    }

    enum Token
    {
        QuoteStr,
        Str,
        Begin,
        End,
        Comma,
        Equal,
        Real,
        RelativeReal,
        Semicolon,
        Number,
        Error
    };

    void readBlock(Block& block);

    std::string readKeyword();

    std::string readString();

    double readReal();

    double readRelativeReal();

    std::string readQuotedString();

    void readChar(char r);
    void readBegin()
    {
        readChar('{');
    }
    void readEnd()
    {
        readChar('}');
    }
    void readComma()
    {
        readChar(',');
    }
    void readQuote()
    {
        readChar('"');
    }
    void readEqual()
    {
        readChar('=');
    }
    void readSemicolon()
    {
        readChar(';');
    }

    Token nextToken();

    //
    // attributes
    //

    Block mRoot;           ///< the main data.
    std::istream& mStream; ///< the input stream
    long mLine;            ///< number of line readed
    long mOldLine;         ///< previous number of line readed (when use unget)
    long mColumn;          ///< number of character readed
    long mOldColumn;       ///< previous number of column read (when use unged)
    char mLast;            ///< latest character readed from the stream
};
}
} // namespace vle utils

#endif
