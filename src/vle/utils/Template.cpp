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

#include <fstream>
#include <regex>
#include <sstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Template.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace utils {

void
SymbolString::append(const std::string& key, const std::string& value)
{
    auto it = lst_.find(key);
    if (it != end()) {
        throw utils::ArgError(
          (fmt(_("Symbol '%1%' already exist")) % key).str());
    }
    lst_.insert(std::make_pair(key, value));
}

void
SymbolString::remove(const std::string& key)
{
    auto it = lst_.find(key);
    if (it != end()) {
        lst_.erase(it);
    }
}

const std::string&
SymbolString::get(const std::string& key) const
{
    auto it = lst_.find(key);
    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("Unknown single symbol '%1%'")) % key).str());
    }
    return it->second;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void
SymbolBool::append(const std::string& key, bool value)
{
    auto it = lst_.find(key);
    if (it != end()) {
        throw utils::ArgError(
          (fmt(_("Symbol '%1%' already exist")) % key).str());
    }
    lst_.insert(std::make_pair(key, value));
}

void
SymbolBool::remove(const std::string& key)
{
    auto it = lst_.find(key);
    if (it != end()) {
        lst_.erase(it);
    }
}

bool
SymbolBool::get(const std::string& key) const
{
    auto it = lst_.find(key);
    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("Unknown bool symbol '%1%'")) % key).str());
    }
    return it->second;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void
SymbolList::append(const std::string& key)
{
    auto it = lst_.find(key);
    if (it == end()) {
        lst_.insert(std::make_pair(key, value_t()));
    }
}

void
SymbolList::append(const std::string& key, const std::string& value)
{
    auto it = lst_.find(key);
    if (it != end()) {
        it->second.push_back(value);
    } else {
        std::pair<iterator, bool> r =
          lst_.insert(std::make_pair(key, value_t()));
        r.first->second.push_back(value);
    }
}

void
SymbolList::remove(const std::string& key)
{
    auto it = lst_.find(key);
    if (it != end()) {
        lst_.erase(it);
    }
}

const std::string&
SymbolList::get(const std::string& key, value_t::size_type i) const
{
    auto it = lst_.find(key);
    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("Unknown list symbol '%1%'")) % key).str());
    }

    if (it->second.size() < i) {
        throw utils::ArgError("Too short symbol list");
    }

    return it->second[i];
}

SymbolList::value_t::size_type
SymbolList::size(const std::string& key) const
{
    auto it = lst_.find(key);
    if (it == end()) {
        throw utils::ArgError(
          (fmt(_("Unknown list symbol '%1%'")) % key).str());
    }

    return it->second.size();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void
Template::open(const std::string& filename)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        std::ostringstream output;

        file.open(filename.c_str());
        output << file.rdbuf();

        buffer_ = output.str();
    } catch (const std::ios_base::failure& e) {
        throw utils::ArgError(
          (fmt(_("Template error, can not open file '%1%'")) % filename)
            .str());
    }
}

void
Template::process(std::ostream& result) const
{
    std::string buffer(buffer_);

    buffer = processIf(buffer);
    buffer = processIfnot(buffer);
    buffer = processFor(buffer);
    buffer = processName(buffer);

    result << buffer;
}

void
Template::tag(std::string& pluginname,
              std::string& packagename,
              std::string& conf)
{
    std::regex tagbegin("@@tag [[:alnum:]_]*@", std::regex::grep);
    std::regex tagmiddle("@[[:alnum:]_\\.\\-]* @@", std::regex::grep);
    std::regex tagend("@@end tag@@", std::regex::grep);

    std::sregex_iterator it(buffer_.begin(), buffer_.end(), tagbegin);
    std::sregex_iterator jt(buffer_.begin(), buffer_.end(), tagmiddle);
    std::sregex_iterator kt(buffer_.begin(), buffer_.end(), tagend);
    std::sregex_iterator end;

    if (it == end or jt == end or kt == end) {
        throw utils::ArgError(_("Template error, no begin or end tag"));
    }

    if ((*jt)[0].second >= (*kt)[0].first) {
        throw utils::ArgError(_("Template error, bad tag"));
    }

    pluginname.assign(it->str(), 6, it->str().size() - 7);
    packagename.assign(jt->str(), 1, jt->str().size() - 4);
    conf.assign((*jt)[0].second, (*kt)[0].first);
}

std::string
Template::processIf(const std::string& buffer) const
{
    std::regex expif("{{if [[:alnum:]_]*}}", std::regex::grep);
    std::regex expendif("{{end if}}", std::regex::grep);

    std::sregex_iterator it(buffer.begin(), buffer.end(), expif);
    std::sregex_iterator jt(buffer.begin(), buffer.end(), expendif);
    std::sregex_iterator end, begin = it;
    std::string temp;

    while (it != end) {
        if (it == begin) {
            temp.append(it->prefix());
        }
        std::string sub(it->str(), 5, it->str().size() - 7);

        if (boolSymbol().get(sub)) {
            std::string sub((*it)[0].second, (*jt)[0].first);
            temp.append(sub);
        }

        ++it;

        if (it == end) {
            temp.append(jt->suffix());
        } else {
            std::string sub((*jt)[0].second, (*it)[0].first);
            temp.append(sub);
        }
        ++jt;
    }

    if (not temp.empty()) {
        return temp;
    } else {
        return buffer;
    }
}

std::string
Template::processIfnot(const std::string& buffer) const
{
    std::regex expifnot("{{ifnot [[:alnum:]_]*}}", std::regex::grep);
    std::regex expendifnot("{{end ifnot}}", std::regex::grep);

    std::sregex_iterator it(buffer.begin(), buffer.end(), expifnot);
    std::sregex_iterator jt(buffer.begin(), buffer.end(), expendifnot);
    std::sregex_iterator end, begin = it;
    std::string temp;

    while (it != end) {
        if (it == begin) {
            temp.append(it->prefix());
        }
        std::string sub(it->str(), 8, it->str().size() - 10);

        if (not boolSymbol().get(sub)) {
            std::string sub((*it)[0].second, (*jt)[0].first);
            temp.append(sub);
        }

        ++it;

        if (it == end) {
            temp.append(jt->suffix());
        } else {
            std::string sub((*jt)[0].second, (*it)[0].first);
            temp.append(sub);
        }
        ++jt;
    }

    if (not temp.empty()) {
        return temp;
    } else {
        return buffer;
    }
}

std::string
Template::processFor(const std::string& buffer) const
{
    std::regex expression("{{[[:alnum:]_]*}}", std::regex::grep);

    std::sregex_iterator it(buffer.begin(), buffer.end(), expression);
    std::sregex_iterator end, current;
    std::string temp;

    while (it != end) {
        temp.append(it->prefix());
        std::string sub(it->str(), 2, it->str().size() - 4);
        temp.append(stringSymbol().get(sub));

        current = it;
        ++it;

        if (it == end) {
            temp.append(current->suffix());
        }
    }
    if (not temp.empty()) {
        return temp;
    } else {
        return buffer;
    }
}

std::string
Template::processName(const std::string& buffer) const
{
    std::regex expfor("{{for [[:alnum:]_] in [[:alnum:]_]*}}",
                      std::regex::grep);
    std::regex expendfor("{{end for}}", std::regex::grep);
    std::regex var("{{[[:alnum:]_]*\\^[[:alnum:]_]}}", std::regex::grep);

    std::sregex_iterator it(buffer.begin(), buffer.end(), expfor);
    std::sregex_iterator jt(buffer.begin(), buffer.end(), expendfor);
    std::sregex_iterator end, current;
    std::sregex_iterator begin = it;
    std::string temp;

    while (it != end) {
        if (it == begin) {
            temp.append(it->prefix());
        }

        std::string sub(it->str(), 6, it->str().size() - 8);
        std::string list(sub, 5, sub.size() - 5);

        std::string str((*it)[0].second, (*jt)[0].first);

        for (uint32_t i = 0; i < listSymbol().list().find(list)->second.size();
             ++i) {
            std::sregex_iterator vt(str.begin(), str.end(), var);

            while (vt != end) {
                temp.append(vt->prefix());
                std::string sub(vt->str(), 2, vt->str().size() - 4);
                std::string listname(sub, 0, sub.size() - 2);

                temp.append(listSymbol().get(listname, i));

                current = vt;
                ++vt;

                if (vt == end) {
                    temp.append(current->suffix());
                }
            }
        }

        std::sregex_iterator nextJt = jt;
        std::sregex_iterator nextIt = it;
        nextJt++;
        nextIt++;

        if (nextJt == end) {
            temp.append(jt->suffix());
        } else {
            std::string buffer((*jt)[0].second, (*nextIt)[0].first);
            temp.append(buffer);
        }

        ++it;
        ++jt;
    }
    if (not temp.empty()) {
        return temp;
    } else {
        return buffer;
    }
}
}
} // namespace vle utils
