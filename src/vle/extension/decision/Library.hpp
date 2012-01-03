/**
 * @file vle/extension/decision/Library.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2012 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_DECISION_LIBRARY_HPP
#define VLE_EXTENSION_DECISION_LIBRARY_HPP 1

#include <vle/extension/DllDefines.hpp>
#include <vle/extension/decision/Plan.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <string>
#include <map>

namespace vle { namespace extension { namespace decision {

class KnowledgeBase;

class VLE_EXTENSION_EXPORT Library
{
public:
    typedef std::map < std::string, Plan > LibraryT;
    typedef LibraryT::value_type value_type;
    typedef LibraryT::const_iterator const_iterator;
    typedef LibraryT::iterator iterator;
    typedef LibraryT::size_type size_type;

    Library(KnowledgeBase& kb)
        : mKb(kb)
    {}

    void add(const std::string& name, const std::string& content);

    void add(const std::string& name, std::istream& stream);

    void del(const std::string& name)
    {
        del(get(name));
    }

    void del(iterator it)
    {
        mLst.erase(it);
    }

    const_iterator get(const std::string& name) const
    {
        const_iterator it = find(name);

        if (it == end()) {
            throw utils::ArgError(
                fmt(_("Decision: plan library, plan `%1%' is unknown")) % name);
        }

        return it;
    }

    iterator get(const std::string& name)
    {
        iterator it = find(name);

        if (it == end()) {
            throw utils::ArgError(
                fmt(_("Decision: plan library, plan `%1%' is unknonw")) % name);
        }

        return it;
    }

    bool exist(const std::string& name)
    {
        return find(name) != end();
    }

    bool empty() const
    {
        return mLst.empty();
    }

    const Plan& operator[](const std::string& name) const
    {
        return get(name)->second;
    }

    Plan& operator[](const std::string& name)
    {
        return get(name)->second;
    }

    iterator find(const std::string& name)
    {
        return mLst.find(name);
    }

    const_iterator find(const std::string& name) const
    {
        return mLst.find(name);
    }

    iterator begin() { return mLst.begin(); }
    const_iterator begin() const { return mLst.begin(); }
    iterator end() { return mLst.end(); }
    const_iterator end() const { return mLst.end(); }
    size_type size() const { return mLst.size(); }

private:
    LibraryT mLst;
    KnowledgeBase& mKb;
};

}}} // namespace vle model decision

#endif

