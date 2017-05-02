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

#include <cstdarg>
#include <cstdio>
#include <limits>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>

namespace {

int
default_buffer_size(std::size_t size) noexcept
{
    if (vle::utils::is_numeric_castable<int>(size))
        return vle::utils::numeric_cast<int>(size);

    return BUFSIZ;
}
} // namespace anonymous

namespace vle {
namespace utils {

FileError::FileError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

FileError::FileError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
FileError::what() const noexcept
{
    return m_message.c_str();
}

ParseError::ParseError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

ParseError::ParseError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }
        size = n + 1;
        m_message.resize(size);
    }
}

const char*
ParseError::what() const noexcept
{
    return m_message.c_str();
}

ArgError::ArgError(std::string message)
  : std::logic_error("")
  , m_message(message)
{
}

ArgError::ArgError(const char* format, ...)
  : std::logic_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
ArgError::what() const noexcept
{
    return m_message.c_str();
}

CastError::CastError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

CastError::CastError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
CastError::what() const noexcept
{
    return m_message.c_str();
}

InternalError::InternalError(std::string message)
  : std::logic_error("")
  , m_message(message)
{
}

InternalError::InternalError(const char* format, ...)
  : std::logic_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
InternalError::what() const noexcept
{
    return m_message.c_str();
}

ModellingError::ModellingError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

ModellingError::ModellingError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
ModellingError::what() const noexcept
{
    return m_message.c_str();
}

NotYetImplemented::NotYetImplemented(std::string message)
  : std::logic_error("")
  , m_message(message)
{
}

NotYetImplemented::NotYetImplemented(const char* format, ...)
  : std::logic_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
NotYetImplemented::what() const noexcept
{
    return m_message.c_str();
}

DevsGraphError::DevsGraphError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

DevsGraphError::DevsGraphError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
DevsGraphError::what() const noexcept
{
    return m_message.c_str();
}

SaxParserError::SaxParserError(std::string message)
  : std::runtime_error("")
  , m_message(message)
{
}

SaxParserError::SaxParserError(const char* format, ...)
  : std::runtime_error("")
  , m_message(256, '\0')
{
    int n;
    int size = ::default_buffer_size(m_message.size());
    std::string ret(size, '\0');

    va_list ap;
    for (;;) {
        va_start(ap, format);
        n = vsnprintf(&m_message[0], size, format, ap);
        va_end(ap);

        if (n < 0)
            return;

        if (n < size) {
            m_message.erase(n, std::string::npos);
            return;
        }

        size = n + 1;
        m_message.resize(size);
    }
}

const char*
SaxParserError::what() const noexcept
{
    return m_message.c_str();
}
}
} // namespace vle utils
