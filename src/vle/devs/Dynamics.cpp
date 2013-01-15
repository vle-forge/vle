/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#include <vle/devs/ExternalEvent.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>


namespace vle { namespace devs {

ExternalEvent* Dynamics::buildEvent(const std::string& portName) const
{
  return new ExternalEvent(portName);
}

ExternalEvent* Dynamics::buildEventWithADouble(
    const std::string & portName,
    const std::string & attributeName,
    double attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);
    event->putAttribute(attributeName,
                        value::Double::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithAInteger(
    const std::string & portName,
    const std::string & attributeName,
    long attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);

    event->putAttribute(attributeName,
                          value::Integer::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithABoolean(
    const std::string & portName,
    const std::string & attributeName,
    bool attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);

    event->putAttribute(attributeName,
                        value::Boolean::create(attributeValue));
    return event;
}

ExternalEvent* Dynamics::buildEventWithAString(
    const std::string & portName,
    const std::string & attributeName,
    const std::string & attributeValue) const
{
    ExternalEvent* event = new ExternalEvent(portName);

    event->putAttribute(attributeName,
                        value::String::create(attributeValue));
    return event;
}

std::string Dynamics::getPackageDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDir();
    } else {
        return utils::Path::path().getExternalPackageDir(*m_packageid);
    }
}

std::string Dynamics::getPackageSimulatorDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackagePluginSimulatorDir();
    } else {
        return utils::Path::path().getExternalPackagePluginSimulatorDir(
            *m_packageid);
    }
}

std::string Dynamics::getPackageSrcDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageSrcDir();
    } else {
        return utils::Path::path().getExternalPackageSrcDir(*m_packageid);
    }
}

std::string Dynamics::getPackageDataDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDataDir();
    } else {
        return utils::Path::path().getExternalPackageDataDir(*m_packageid);
    }
}

std::string Dynamics::getPackageDocDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDocDir();
    } else {
        return utils::Path::path().getExternalPackageDocDir(*m_packageid);
    }
}

std::string Dynamics::getPackageExpDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageExpDir();
    } else {
        return utils::Path::path().getExternalPackageExpDir(*m_packageid);
    }
}

std::string Dynamics::getPackageBuildDir() const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDir();
    } else {
        return utils::Path::path().getExternalPackageExpDir(*m_packageid);
    }
}

std::string Dynamics::getPackageFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageFile(name);
    } else {
        return utils::Path::path().getExternalPackageFile(*m_packageid, name);
    }
}

std::string Dynamics::getPackageLibFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageLibFile(name);
    } else {
        return utils::Path::path().getExternalPackageLibFile(*m_packageid,
                                                             name);
    }
}

std::string Dynamics::getPackageSrcFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageSrcFile(name);
    } else {
        return utils::Path::path().getExternalPackageSrcFile(*m_packageid,
                                                             name);
    }
}

std::string Dynamics::getPackageDataFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDataFile(name);
    } else {
        return utils::Path::path().getExternalPackageDataFile(*m_packageid,
                                                              name);
    }
}

std::string Dynamics::getPackageDocFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageDocFile(name);
    } else {
        return utils::Path::path().getExternalPackageDocFile(*m_packageid,
                                                             name);
    }
}

std::string Dynamics::getPackageExpFile(const std::string& name) const
{
    if (utils::Package::package().name() == *m_packageid) {
        return utils::Path::path().getPackageExpFile(name);
    } else {
        return utils::Path::path().getExternalPackageExpFile(*m_packageid,
                                                             name);
    }
}

}} // namespace vle devs
