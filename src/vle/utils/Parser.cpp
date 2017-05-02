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

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace utils {

void
Block::addString(const std::string& key, const std::string& str)
{
    strings.insert(std::make_pair(key, str));
}

void
Block::addReal(const std::string& key, const double& val)
{
    reals.insert(std::make_pair(key, val));
}

void
Block::addRelativeReal(const std::string& key, const double& val)
{
    relativeReals.insert(std::make_pair(key, val));
}

Block&
Block::addBlock(const std::string& name)
{
    auto it = blocks.insert(std::make_pair(name, Block(name)));

    return it->second;
}

const Block&
Block::getBlock(const std::string& name) const
{
    auto it = blocks.find(name);
    if (it == blocks.end()) {
        throw utils::ParseError(
          (fmt(_("The block `%1%' does not exist")) % name).str());
    }
    return it->second;
}

Parser::Parser(std::istream& stream) throw(utils::ParseError)
  : mRoot(std::string())
  , mStream(stream)
  , mLine(0)
  , mOldLine(0)
  , mColumn(0)
  , mOldColumn(0)
  , mLast(0)
{
    try {
        readBlock(mRoot);
    } catch (const std::exception& e) {
        throw utils::ParseError(
          (fmt(_("Parser error before line %1% column %2%:\t%3%")) % mLine %
           mColumn % e.what())
            .str());
    }
}

std::string
Parser::readKeyword()
{
    do {
        char c = get();

        if (isspace(c)) {
            continue;
        } else if (c == '#') {
            forgetLine();
            continue;
        } else {
            unget();
            break;
        }
    } while (mStream);

    if (not mStream)
        throw utils::ParseError(_("End of file"));

    std::string result;

    do {
        char c = get();

        if (isspace(c)) {
            unget();
            break;
        } else if (c == '#') {
            forgetLine();
            break;
        } else {
            result += c;
        }
    } while (mStream);

    return result;
}

std::string
Parser::readString()
{
    do {
        char c = get();

        if (isspace(c)) {
            continue;
        } else if (c == '#') {
            forgetLine();
            continue;
        } else {
            unget();
            break;
        }
    } while (mStream);

    if (not mStream)
        throw utils::ParseError(_("End of file"));

    std::string result;

    do {
        char c = get();

        if (isalnum(c)) {
            result += c;
        } else {
            unget();
            return result;
        }
    } while (mStream);

    return result;
}

double
Parser::readReal()
{
    do {
        char c = get();

        if (isspace(c)) {
            continue;
        } else if (c == '#') {
            forgetLine();
            continue;
        } else {
            unget();
            break;
        }
    } while (mStream);

    if (not mStream)
        throw utils::ParseError(_("End of file"));

    std::string result;

    do {
        char c = get();

        if (isspace(c)) {
            unget();
            break;
        } else if (c == '#') {
            forgetLine();
            break;
        } else if (isdigit(c) or c == '-') {
            result += c;
        } else if (c == '.') {
            if (result.find(".") != std::string::npos)
                throw utils::ParseError(
                  _("Multiple '.' while parsing real `%s'"), result.c_str());

            result += c;
        } else {
            unget();
            break;
        }
    } while (mStream);

    return boost::lexical_cast<double>(result);
}

double
Parser::readRelativeReal()
{
    readChar('+');
    return readReal();
}

std::string
Parser::readQuotedString()
{
    readChar('"');
    std::string result;

    do {
        char c = get();

        if (c == '"') {
            return result;
        } else {
            result += c;
            continue;
        }
    } while (mStream);

    throw utils::ParseError(_("End of file"));
}

void
Parser::readChar(char r)
{
    do {
        char c = get();

        if (isspace(c)) {
            continue;
        } else if (c == '#') {
            forgetLine();
            continue;
        } else if (c == r) {
            return;
        } else {
            throw utils::ParseError(_("End of file"));
        }
    } while (mStream);

    throw utils::ParseError(_("End of file"));
}

Parser::Token
Parser::nextToken()
{
    while (mStream) {
        char c = get();

        if (not mStream) {
            return Parser::Error;
        } else if (isspace(c)) {
            continue;
        } else if (c == '#') {
            forgetLine();
            continue;
        } else if (std::isdigit(c) or c == '-') {
            unget();
            return Parser::Real;
        } else if (c == '+') {
            unget();
            return Parser::RelativeReal;
        } else if (c == '"') {
            unget();
            return Parser::QuoteStr;
        } else if (c == '{') {
            unget();
            return Parser::Begin;
        } else if (c == '}') {
            unget();
            return Parser::End;
        } else if (c == '=') {
            unget();
            return Parser::Equal;
        } else if (c == ',') {
            unget();
            return Parser::Comma;
        } else if (c == ';') {
            unget();
            return Parser::Semicolon;
        } else {
            unget();
            return Parser::Str;
        }
    }

    return Parser::Error;
}

void
Parser::readBlock(Block& block)
{
    do {
        std::string str = readKeyword();

        switch (nextToken()) {
        case Parser::Error:
            return;
        case Parser::Begin:
            readBegin();
            readBlock(block.addBlock(str));
            readEnd();
            break;
        case Parser::Equal:
            readEqual();
            if (nextToken() == QuoteStr) {
                block.addString(str, readQuotedString());
                while (nextToken() == Parser::Comma) {
                    readComma();
                    block.addString(str, readQuotedString());
                }
            } else if (nextToken() == Real) {
                block.addReal(str, readReal());
                while (nextToken() == Parser::Comma) {
                    readComma();
                    block.addReal(str, readReal());
                }
            } else if (nextToken() == RelativeReal) {
                block.addRelativeReal(str, readRelativeReal());
                while (nextToken() == Parser::Comma) {
                    readComma();
                    block.addRelativeReal(str, readRelativeReal());
                }
            } else if (nextToken() == Str) {
                block.addString(str, readString());
                while (nextToken() == Parser::Comma) {
                    readComma();
                    block.addString(str, readString());
                }
            } else {
                throw utils::ParseError(_("End of file"));
            }
            readSemicolon();
            break;
        default:
            throw utils::ParseError(_("End of file"));
        };
    } while (nextToken() != Parser::End and mStream);
}
}
} // namespace vle utils
