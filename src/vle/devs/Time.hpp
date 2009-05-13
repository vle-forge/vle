/**
 * @file vle/devs/Time.hpp
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


#ifndef VLE_DEVS_TIME_HPP
#define VLE_DEVS_TIME_HPP

#include <vle/utils/Exception.hpp>
#include <ostream>
#include <limits>

namespace vle { namespace devs {

/**
 * @brief The definition of time in DEVS simulators. This class encapsulates
 * a real type in a double simple type.
 * - The infinity is represented by std::numeric_limits < double >::max().
 * - The infinity constant static object is used to simplify the source code.
 */
class Time
{
public:
    /**
     * @brief Constructor with  a real value.
     * @param the double value to assign.
     */
    Time(const double& value = 0.0)
        : m_value(value)
    {}

    /**
     * @brief Constructor with an integer value.
     * @param the integer value to assign.
     */
    Time(const int& value)
        : m_value(value)
    {}

    /**
     * @brief Copy constructor.
     * @param time the Time to copy.
     */
    Time(const Time& time)
        : m_value(time.m_value)
    {}

    /**
     * @brief Destructor, nothing to delete.
     */
    ~Time() {}

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Test if the time represents the infinity.
     * @return true if time is infinity, otherwise false.
     */
    inline bool isInfinity() const
    { return m_value == std::numeric_limits < double >::max(); }

    /**
     * @brief Test if the time represents the infinity.
     * @param value the value to ckeck.
     * @return true if time is infinity, otherwise false.
     */
    inline static bool isInfinity(const double& value)
    { return value == std::numeric_limits < double >::max(); }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Swap the content of the devs::Time object.
     * @param other The other devs::Time of swap time.
     */
    inline void swap(Time& other)
    { std::swap(m_value, other.m_value); }

    /**
     * @brief Assigment operator.
     * @param other The devs::Time to assign.
     * @return The newly allocated devs::Time.
     */
    inline Time operator=(const Time& other)
    { Time tmp(other); swap(tmp); return *this; }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Cast operator for double. This operator automatically cast a
     * vle::devs::Time into a double value.
     * @code
     * vle::devs::Time t(1.234);
     * double y = 2.345;
     * double z = x + y;
     * @endcode
     * @return The double representation of the vle::devs::Time.
     */
    inline operator double() const
    { return m_value; }

    /**
     * @brief Get current value.
     * @return double representation of Time.
     */
    inline const double& getValue() const
    { return m_value; }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    inline bool operator<(const Time& time) const
    { return (m_value < time.m_value and !isInfinity()) or time.isInfinity(); }

    inline bool operator<(const double& time) const
    { return (m_value < time and !isInfinity()) or isInfinity(time); }

    inline bool operator<(const int& time) const
    { return (m_value < time and !isInfinity()) or isInfinity(time); }


    inline bool operator<=(const Time& time) const
    { return (m_value <= time.m_value and !isInfinity()) or time.isInfinity(); }

    inline bool operator<=(const double& time) const
    { return (m_value <= m_value and !isInfinity()) or isInfinity(time); }

    inline bool operator<=(const int& time) const
    { return (m_value <= m_value and !isInfinity()) or isInfinity(time); }


    inline bool operator>(const Time& time) const
    { return (m_value > time.m_value and !time.isInfinity()) or
        (isInfinity() and !time.isInfinity()); }

    inline bool operator>(const double& time) const
    { return (m_value > time and not isInfinity(time)) or
        (isInfinity() and not isInfinity(time)); }

    inline bool operator>(const int& time) const
    { return (m_value > time and not isInfinity(time)) or
        (isInfinity() and not isInfinity(time)); }


    inline bool operator>=(const Time& time) const
    { return (m_value >= time.m_value and !time.isInfinity()) or
        (isInfinity() and !time.isInfinity()); }

    inline bool operator>=(const double& time) const
    { return (m_value >= m_value and not isInfinity(time)) or
        (isInfinity() and not isInfinity(time)); }

    inline bool operator>=(const int& time) const
    { return ( m_value >= time and not isInfinity(time)) or
        (isInfinity() and not isInfinity(time)); }


    inline bool operator==(const Time& time) const
    { return m_value == time.m_value; }

    inline bool operator==(const double& time) const
    { return m_value == time; }

    inline bool operator==(const int& time) const
    { return m_value == static_cast < double >(time); }


    inline bool operator!=(const Time& time) const
    { return m_value != time.m_value; }

    inline bool operator!=(const double& time) const
    { return m_value != time; }

    inline bool operator!=(const int& time) const
    { return m_value != static_cast < double >(time); }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    inline void operator++()
    {
        if (not isInfinity()) {
            ++m_value;
        }
    }

    inline void operator+=(const Time& step)
    {
        if (not isInfinity() and not step.isInfinity()) {
            m_value += step.m_value;
        } else {
            m_value = infinity;
        }
    }

    inline void operator+=(const double& step)
    {
        if (not isInfinity()) {
            m_value += step;
        }
    }

    inline void operator+=(const int& step)
    {
        if (not isInfinity()) {
            m_value += step;
        }
    }


    inline Time operator+(const Time& step) const
    {
        if (not isInfinity() and not step.isInfinity()) {
            return Time(m_value + step.m_value);
        } else {
            return infinity;
        }
    }

    inline Time operator+(const double& step) const
    {
        if (not isInfinity()) {
            return Time(m_value + step);
        } else {
            return infinity;
        }
    }

    inline Time operator+(const int& step) const
    {
        if (not isInfinity()) {
            return Time(m_value + step);
        } else {
            return infinity;
        }
    }


    inline void operator--()
    {
        if (not isInfinity()) {
            --m_value;
        }
    }


    inline void operator-=(const Time& step)
    {
        if (not isInfinity()) {
            if (not step.isInfinity()) {
                m_value -= step;
            } else {
                throw utils::ArgError(_(
                        "Time: -infinity is not representable"));
            }
        } else {
            if (step.isInfinity()) {
                throw utils::ArgError(_(
                        "Time: infinity -= infinity"));
            }
        }
    }

    inline void operator-=(const double& step)
    {
        if (not isInfinity()) {
            m_value -= step;
        }
    }

    inline void operator-=(const int& step)
    {
        if (not isInfinity()) {
            m_value -= step;
        }
    }


    inline Time operator-(const Time& step) const
    {
        if (not isInfinity()) {
            if (not step.isInfinity()) {
                return Time(m_value - step.m_value);
            } else {
                throw utils::ArgError(_(
                        "Time: -infinity is not representable"));
            }
        } else {
            if (not step.isInfinity()) {
                return infinity;
            } else {
                throw utils::ArgError(_(
                        "Time: infinity -= infinity"));
            }
        }
    }

    inline Time operator-(const double& step) const
    {
        if (not isInfinity()) {
            return Time(m_value - step);
        } else {
            return infinity;
        }
    }

    inline Time operator-(const int& step) const
    {
        if (not isInfinity()) {
            return Time(m_value - step);
        } else {
            return infinity;
        }
    }


    static const Time infinity;

    inline friend std::ostream& operator<<(std::ostream& out, const Time& t)
    {
        return out << t.m_value;
    }

private:
    double m_value;
};


/**
 * @brief Functor to check if the Time is infinity.
 * @code
 * it = std::find_if(c.begin(), c.end(), IsInfinity());
 * if (it != c.end()) {
 *     std::cout << "Infinity time found";
 * }
 * @endcode
 */
struct IsInfinity
{
    inline bool operator()(const Time& time) const
    { return time.isInfinity(); }
};

}} // namespace vle devs

#endif
