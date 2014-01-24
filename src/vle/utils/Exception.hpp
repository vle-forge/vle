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


#ifndef UTILS_EXCEPTION_HPP
#define UTILS_EXCEPTION_HPP

#include <string>
#include <exception>
#include <stdexcept>
#include <vle/utils/i18n.hpp>
#include <vle/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * @brief Base class to manage exception in VLE application.
     *
     */
    class VLE_API BaseError : public std::runtime_error
    {
    public:
        explicit BaseError(const std::string& argv = std::string())
            : std::runtime_error(argv) {}

        explicit BaseError(const boost::format& argv)
            : std::runtime_error(argv.str()) {}
    };

    /**
     * @brief Throw to report a bad file access (read, write, open).
     *
     */
    class VLE_API FileError : public BaseError
    {
    public:
        explicit FileError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit FileError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report a bad XML file parsing.
     *
     */
    class VLE_API ParseError : public BaseError
    {
    public:
        explicit ParseError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit ParseError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report a bad parameter.
     *
     */
    class VLE_API ArgError : public BaseError
    {
    public:
        explicit ArgError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit ArgError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report a bad parameter.
     *
     */
    class VLE_API CastError : public BaseError
    {
    public:
        explicit CastError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit CastError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report an internal error.
     *
     */
    class VLE_API InternalError : public BaseError
    {
    public:
        explicit InternalError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit InternalError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report an modelling error.
     */
    class VLE_API ModellingError : public BaseError
    {
    public:
        explicit ModellingError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit ModellingError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report an unimplemted feature.
     */
    class VLE_API NotYetImplemented : public BaseError
    {
    public:
        explicit NotYetImplemented(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit NotYetImplemented(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report a DEVS graph library error.
     */
    class VLE_API DevsGraphError : public BaseError
    {
    public:
        explicit DevsGraphError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit DevsGraphError(const boost::format& argv)
            : BaseError(argv) {}
    };

    class VLE_API VpzError : public BaseError
    {
    public:
        explicit VpzError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit VpzError(const boost::format& argv)
            : BaseError(argv) {}
    };

    /**
     * @brief Throw to report an error in SaxParser.
     */
    class VLE_API SaxParserError : public BaseError
    {
    public:
        explicit SaxParserError(const std::string& argv = std::string())
            : BaseError(argv) {}

        explicit SaxParserError(const boost::format& argv)
            : BaseError(argv) {}
    };

}} // namespace vle utils

#endif
