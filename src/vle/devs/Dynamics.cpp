/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageSimulatorDir() const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getPluginSimulatorDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageSrcDir() const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getSrcDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageDataDir() const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDataDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageDocDir() const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDocDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageExpDir() const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getExpDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageLibFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getLibFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageSrcFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getSrcFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageDataFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDataFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageDocFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDocFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

std::string Dynamics::getPackageExpFile(const std::string& name) const
{
    vle::utils::Package pkg(*m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getExpFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError(vle::fmt(_("Package '%1%' is not "
                "installed")) % *m_packageid);
    }
}

}} // namespace vle devs
