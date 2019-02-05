/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/devs/Dynamics.hpp>
#include <vle/devs/ExternalEvent.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Tools.hpp>

#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>

#include "devs/DynamicsInit.hpp"
#include "utils/i18n.hpp"

#include <cstdarg>

namespace vle {
namespace devs {

Dynamics::Dynamics(const DynamicsInit& init, const InitEventList& /* events */)
  : m_context(init.context)
  , m_model(init.model)
  , m_packageid(init.packageid)
{}

std::string
Dynamics::getPackageDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageSimulatorDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getPluginSimulatorDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageSrcDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getSrcDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageDataDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDataDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageDocDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDocDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageExpDir() const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getExpDir(vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageLibFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getLibFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageSrcFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getSrcFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageDataFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDataFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageDocFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getDocFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

std::string
Dynamics::getPackageExpFile(const std::string& name) const
{
    vle::utils::Package pkg(m_context, *m_packageid);
    if (pkg.existsBinary()) {
        return pkg.getExpFile(name, vle::utils::PKG_BINARY);
    } else {
        throw vle::utils::FileError("Package '%s' is not installed",
                                    m_packageid->c_str());
    }
}

void
Dynamics::Trace(int priority, const char* format, ...) const
{
    if (m_context->get_log_priority() < priority)
        return;

    va_list ap;
    va_start(ap, format);
    try {
        m_context->log(priority, utils::vformat(format, ap));
    } catch (...) {
    }
    va_end(ap);
}

void
Dynamics::Trace(int priority, const std::string& str) const
{
    if (m_context->get_log_priority() < priority)
        return;

    m_context->log(priority, str);
}

void
Trace(utils::ContextPtr ctx, int priority, const char* format, ...)
{
    if (ctx->get_log_priority() < priority)
        return;

    va_list ap;
    va_start(ap, format);
    try {
        ctx->log(priority, utils::vformat(format, ap));
    } catch (...) {
    }
    va_end(ap);
}
}
} // namespace vle devs
