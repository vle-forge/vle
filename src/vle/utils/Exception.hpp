/**
 * @file Exception.hpp
 * @author The VLE Development Team.
 * @brief A namespace that define all exception use in VLE projects. All
 * classes inherit of the std::runtime_error of the standard C++ exceptions.
 */

/*
 * Copyright (c) 2004, 2005, 2006 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef UTILS_EXCEPTION_HPP
#define UTILS_EXCEPTION_HPP

#include <string>
#include <exception>
#include <stdexcept>



/**
 * @brief Define exception use into VLE framework. All exception use the base
 * class std::runtime_error.
 *
 */
namespace vle { namespace utils {

    /**
     * @brief Base class to manage exception in VLE application.
     *
     */
    class BaseError : public std::runtime_error
    {
    public:
        explicit BaseError(const std::string& argv) :
            std::runtime_error(argv)
        { }
    };

    /**
     * @brief Throw to report a bad file access (read, write, open).
     *
     */
    class FileError : public BaseError
    {
    public:
        explicit FileError(const std::string& argv) :
            BaseError(argv)
        { }
    };

    /**
     * @brief Throw to report a bad XML file parsing.
     *
     */
    class ParseError : public BaseError
    {
    public:
        explicit ParseError(const std::string& argv) :
            BaseError(argv)
        { }
    };

    /**
     * @brief Throw to report a bad parameter.
     *
     */
    class ArgError : public BaseError
    {
    public:
        explicit ArgError(const std::string& argv) :
            BaseError(argv)
        { }
    };

    /**
     * @brief Throw to report an internal error.
     *
     */
    class InternalError : public BaseError
    {
    public:
        explicit InternalError(const std::string& argv) :
            BaseError(argv)
        { }
    };

    /** 
     * @brief Throw to report an modelling error.
     */
    class ModellingError : public BaseError
    {
    public:
        explicit ModellingError(const std::string& argv) :
            BaseError(argv)
        { }
    };

}} // namespace vle utils

#endif
