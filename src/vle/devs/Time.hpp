/**
 * @file devs/Time.hpp
 * @author The VLE Development Team.
 * @brief The definition of time in VLE simulators. This class encapsulate a
 * double simple type to define time.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef DEVS_TIME_HPP
#define DEVS_TIME_HPP

#include <limits>

namespace vle { namespace devs {

    /**
     * @brief The definition of time in VLE simulators. This class encapsulate
     * a double simple type to define time.
     *
     * - The infinity static variable is equal to -1.
     * - All function are inline.
     *
     */
    class Time
    {
    public:
	/**
	 * Constructor with specified value.
	 *
	 * @param value specified time.
	 */
	Time(double value = 0.0) :
	    m_value(value)
        { }

	/**
	 * Copy constructor.
	 *
	 * @param time Time to copy.
	 */
	Time(const Time& time) :
	    m_value(time.m_value)
        { }

	/**
	 * Destructor, nothing to delete.
	 */
	virtual ~Time()
        { }

	/**
	 * Test if time is infinity.
	 *
	 * @return true if time is infinity, otherwise false.
	 */
	inline bool isInfinity() const
        { return m_value == -1; }

	/**
	 * Get current value.
	 *
	 * @return double representation of Time.
	 */
	inline double getValue() const {
	    return isInfinity() ?
		std::numeric_limits<double>::max() :
		m_value; }

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline virtual bool operator<(const Time& time) const {
	    return (m_value < time.m_value and !isInfinity()) or
		time.isInfinity(); }

	inline virtual bool operator<=(const Time& time) const {
	    return (m_value <= time.m_value and !isInfinity()) or
		time.isInfinity(); }

	inline virtual bool operator>(const Time& time) const {
	    return (m_value > time.m_value and !time.isInfinity()) or
		(isInfinity() and !time.isInfinity()); }

	inline virtual bool operator>=(const Time& time) const {
	    return (m_value >= time.m_value and !time.isInfinity()) or
		isInfinity(); }

	inline virtual bool operator==(const Time& time) const {
	    return m_value == time.m_value; }

	inline virtual bool operator!=(const Time& time) const {
	    return m_value != time.m_value; }

	inline virtual void operator++() {
	    if (!isInfinity()) m_value++; }

	inline virtual void operator+=(double step) {
	    if (!isInfinity()) m_value += step; }

	inline virtual void operator+=(const Time& step) {
	    if (!isInfinity() and !step.isInfinity())
		m_value += step.m_value;
	    else
		m_value = -1;
	}

	inline virtual Time operator+(const Time& step) const {
	    return (!isInfinity() and !step.isInfinity()) ?
		Time(m_value + step.m_value) :
		infinity;
	}

	inline virtual Time operator+(double step) const {
	    return (!isInfinity()) ?
		Time(m_value + step) :
		infinity; }

	inline virtual void operator--() {
	    if (!isInfinity()) m_value--; }

	inline virtual void operator-=(double step) {
	    if (!isInfinity()) m_value -= step; }

	inline virtual void operator-=(const Time& step) {
	    if (!isInfinity()) m_value -= step.m_value; }

	inline virtual Time operator-(double step) const {
	    return (!isInfinity()) ? Time(m_value - step) : infinity; }

	inline virtual Time operator-(const Time& step) const {
	    return (!isInfinity() and !step.isInfinity()) ?
		Time(m_value - step.m_value) :
		(isInfinity()?infinity:Time(0));
	}

	static const Time infinity;

    private:
	double m_value;
    };

}} // namespace vle devs

#endif
