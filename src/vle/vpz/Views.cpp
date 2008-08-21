/**
 * @file vle/vpz/Views.cpp
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


#include <vle/vpz/Views.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Algo.hpp>

namespace vle { namespace vpz {

void Views::write(std::ostream& out) const
{
    out << "<views>\n";

    if (not m_outputs.empty()) {
        m_outputs.write(out);
    }

    if (not m_observables.empty()) {
        m_observables.write(out);
    }

    if (not m_list.empty()) {
        std::transform(begin(), end(),
                       std::ostream_iterator < View >(out, "\n"),
                       utils::select2nd < ViewList::value_type >());
    }

    out << "</views>\n";
}

Output& Views::addLocalStreamOutput(const std::string& name,
                                    const std::string& location,
                                    const std::string& plugin)
{
    return m_outputs.addLocalStream(name, location, plugin);
}

Output& Views::addDistantStreamOutput(const std::string& name,
                                      const std::string& location,
                                      const std::string& plugin)
{
    return m_outputs.addDistantStream(name, location, plugin);
}

void Views::delOutput(const std::string& name)
{
    m_outputs.del(name);
}

Observable& Views::addObservable(const Observable& obs)
{
    return m_observables.add(obs);
}

Observable& Views::addObservable(const std::string& name)
{
    return m_observables.add(name);
}

void Views::delObservable(const std::string& name)
{
    m_observables.del(name);
    m_list.clear();
}

void Views::clear()
{
    m_list.clear();
    m_outputs.clear();
    m_observables.clear();
}

void Views::add(const Views& views)
{
    std::for_each(views.viewlist().begin(), views.viewlist().end(),
                  Views::AddViews(*this));

    m_outputs.add(views.outputs());
    m_observables.add(views.observables());
}

View& Views::add(const View& view)
{
    std::pair < iterator, bool > x;
    x = m_list.insert(std::make_pair < std::string, View >(view.name(), view));

    Assert(utils::ArgError, x.second, boost::format(
            "View '%1%' already exist") % view.name());

    return x.first->second;
}

View& Views::addEventView(const std::string& name,
                          const std::string& output)
{
    Assert(utils::ArgError, not isUsedOutput(output),
           boost::format("Output '%1%' of view '%2%' is already used") %
           output % name);

    return add(View(name, View::EVENT, output));
}

View& Views::addTimedView(const std::string& name,
                          double timestep,
                          const std::string& output)
{
    Assert(utils::ArgError, not isUsedOutput(output),
           boost::format("Output '%1%' of view '%2%' is already used") %
           output % name);

    return add(View(name, View::TIMED, output, timestep));
}

View& Views::addFinishView(const std::string& name,
                           const std::string& output)
{
    Assert(utils::ArgError, not isUsedOutput(output),
           boost::format("Output '%1%' of view '%2%' is already used") %
           output % name);

    return add(View(name, View::FINISH, output));
}

void Views::del(const std::string& name)
{
    m_list.erase(name);
}

const View& Views::get(const std::string& name) const
{
    ViewList::const_iterator it = m_list.find(name);
    Assert(utils::ArgError, it != end(),
           boost::format("Unknow view '%1%'\n") % name);

    return it->second;
}

View& Views::get(const std::string& name)
{
    ViewList::iterator it = m_list.find(name);
    Assert(utils::ArgError, it != end(),
           boost::format("Unknow view '%1%'\n") % name);

    return it->second;
}

void Views::renameOutput(const std::string& oldname,
                         const std::string& newname)
{
    for (iterator it = begin(); it != end(); ++it) {
        if (it->second.output() == oldname) {
            vpz::View copy(get(it->first));
            del(it->first);

            switch (copy.type()) {
            case vpz::View::TIMED:
                addTimedView(copy.name(), copy.timestep(), newname);
                break;
            case vpz::View::EVENT:
                addEventView(copy.name(), newname);
                break;
            case vpz::View::FINISH:
                addFinishView(copy.name(), newname);
                break;
            }
        }
        ++it;
    }
}

bool Views::isUsedOutput(const std::string& name) const
{
    ViewList::const_iterator it(std::find_if(begin(), end(), UseOutput(name)));
    return it != end();
}

}} // namespace vle vpz
