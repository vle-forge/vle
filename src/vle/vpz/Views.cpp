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
#include <iterator>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Views.hpp>

namespace vle {
namespace vpz {

void
Views::write(std::ostream& out) const
{
    out << "<views>\n";

    if (not m_outputs.empty()) {
        m_outputs.write(out);
    }

    if (not m_observables.empty()) {
        m_observables.write(out);
    }

    if (not m_list.empty()) {
        std::transform(begin(),
                       end(),
                       std::ostream_iterator<View>(out, "\n"),
                       utils::select2nd<ViewList::value_type>());
    }

    out << "</views>\n";
}

void
Views::delOutput(const std::string& name)
{
    m_outputs.del(name);
}

Observable&
Views::addObservable(const Observable& obs)
{
    return m_observables.add(obs);
}

Observable&
Views::addObservable(const std::string& name)
{
    return m_observables.add(name);
}

void
Views::delObservable(const std::string& name)
{
    m_observables.del(name);
}

void
Views::clear()
{
    m_list.clear();
    m_outputs.clear();
    m_observables.clear();
}

void
Views::add(const Views& views)
{
    std::for_each(views.viewlist().begin(),
                  views.viewlist().end(),
                  Views::AddViews(*this));

    m_outputs.add(views.outputs());
    m_observables.add(views.observables());
}

View&
Views::add(const View& view)
{
    std::pair<iterator, bool> x;
    x = m_list.insert(value_type(view.name(), view));

    if (not x.second) {
        throw utils::ArgError(
          (fmt(_("View '%1%' already exist")) % view.name()).str());
    }

    return x.first->second;
}

View&
Views::addEventView(const std::string& name,
                    View::Type type,
                    const std::string& output,
                    bool enable)
{
    if (isUsedOutput(output)) {
        throw utils::ArgError(
          (fmt(_("Output '%1%' of view '%2%' is already used")) % output %
           name)
            .str());
    }

    return add(View(name, type, output, 0.0, enable));
}

View&
Views::addTimedView(const std::string& name,
                    double timestep,
                    const std::string& output,
                    bool enable)
{
    if (isUsedOutput(output)) {
        throw utils::ArgError(
          (fmt(_("Output '%1%' of view '%2%' is already used")) % output %
           name)
            .str());
    }

    return add(View(name, View::TIMED, output, timestep, enable));
}

void
Views::del(const std::string& name)
{
    m_list.erase(name);
}

const View&
Views::get(const std::string& name) const
{
    auto it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError((fmt(_("Unknow view '%1%'\n")) % name).str());
    }

    return it->second;
}

View&
Views::get(const std::string& name)
{
    auto it = m_list.find(name);

    if (it == end()) {
        throw utils::ArgError((fmt(_("Unknow view '%1%'\n")) % name).str());
    }

    return it->second;
}

void
Views::renameOutput(const std::string& oldname, const std::string& newname)
{
    iterator prec;
    for (auto it = begin(); it != end(); ++it) {
        prec = it;
        if (it->second.output() == oldname) {
            vpz::View copy(get(it->first));
            del(it->first);
            it = prec;
            switch (copy.type()) {
            case vpz::View::TIMED:
                addTimedView(copy.name(), copy.timestep(), newname);
                break;
            default:
                addEventView(copy.name(), copy.type(), newname);
                break;
            }
        }
    }
}

void
Views::renameView(const std::string& oldname, const std::string& newname)
{
    vpz::View copy = get(oldname);
    m_outputs.rename(oldname, newname);
    del(oldname);

    switch (copy.type()) {
    case vpz::View::TIMED:
        addTimedView(newname, copy.timestep(), newname);
        break;
    default:
        addEventView(newname, copy.type(), newname);
        break;
    }
}

void
Views::copyOutput(const std::string& outputname, const std::string& copyname)
{
    const Output& o = m_outputs.get(outputname);
    Output& no =
      addStreamOutput(copyname, o.location(), o.plugin(), o.package());

    if (o.data())
        no.setData(o.data()->clone());
}

void
Views::copyView(const std::string& viewname, const std::string& copyname)
{
    vpz::View view = get(viewname);
    vpz::View copy = get(viewname);
    copy.setName(copyname);
    std::string copyoutputname;
    int number = 1;

    do {
        copyoutputname = view.output() + "_";
        copyoutputname += boost::lexical_cast<std::string>(number);
        ++number;
    } while (outputs().exist(copyoutputname));

    copyOutput(view.output(), copyoutputname);

    switch (copy.type()) {
    case vpz::View::TIMED:
        addTimedView(copy.name(), copy.timestep(), copyoutputname);
        break;
    default:
        addEventView(copy.name(), copy.type(), copyoutputname);
        break;
    }
}

bool
Views::isUsedOutput(const std::string& name) const
{
    auto it(std::find_if(begin(), end(), UseOutput(name)));
    return it != end();
}
}
} // namespace vle vpz
