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

#ifndef VLE_UTILS_EXCEPTION_HPP
#define VLE_UTILS_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <vle/DllDefines.hpp>

#if defined(__GNUC__)
#define GCC_ATTRIBUTE_FORMAT(idformat, idarg)                                 \
    __attribute__((format(printf, (idformat), (idarg))))
#else
#define GCC_ATTRIBUTE_FORMAT(idformat, idarg)
#endif

namespace vle {
namespace utils {

/**
 * @brief Throw to report a bad file access (read, write, open).
 *
 */
class VLE_API FileError : public std::runtime_error
{
    std::string m_message;

public:
    FileError(std::string message);

    FileError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report a bad XML file parsing.
 *
 */
class VLE_API ParseError : public std::runtime_error
{
    std::string m_message;

public:
    ParseError(std::string message);

    ParseError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report a bad parameter.
 *
 */
class VLE_API ArgError : public std::logic_error
{
    std::string m_message;

public:
    ArgError(std::string message);

    ArgError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report a bad parameter.
 *
 */
class VLE_API CastError : public std::runtime_error
{
    std::string m_message;

public:
    CastError(std::string message);

    CastError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report an internal error.
 *
 */
class VLE_API InternalError : public std::logic_error
{
    std::string m_message;

public:
    InternalError(std::string message);

    InternalError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report an modelling error.
 */
class VLE_API ModellingError : public std::runtime_error
{
    std::string m_message;

public:
    ModellingError(std::string message);

    ModellingError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report an unimplemted feature.
 */
class VLE_API NotYetImplemented : public std::logic_error
{
    std::string m_message;

public:
    NotYetImplemented(std::string message);

    NotYetImplemented(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report a DEVS graph library error.
 */
class VLE_API DevsGraphError : public std::runtime_error
{
    std::string m_message;

public:
    DevsGraphError(std::string message);

    DevsGraphError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};

/**
 * @brief Throw to report an error in SaxParser.
 */
class VLE_API SaxParserError : public std::runtime_error
{
    std::string m_message;

public:
    SaxParserError(std::string message);

    SaxParserError(const char* format, ...) GCC_ATTRIBUTE_FORMAT(2, 3);

    virtual const char* what() const noexcept;
};
}
} // namespace vle utils

#endif
