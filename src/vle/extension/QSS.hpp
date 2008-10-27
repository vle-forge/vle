/**
 * @file src/vle/extension/QSS.hpp
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

#ifndef VLE_EXTENSION_QSS_HPP
#define VLE_EXTENSION_QSS_HPP

#include <vle/extension/DifferentialEquation.hpp>

namespace vle { namespace extension {

    class qss : public vle::extension::DifferentialEquation
    {
    public:
        qss(const vle::graph::AtomicModel& model,
            const vle::devs::InitEventList& events);
	virtual ~qss() { }

	virtual double getEstimatedValue(double e) const;

        virtual vle::devs::Time init(const devs::Time& time);

    private:
        virtual void reset(const vle::devs::Time& /* time */,
			   double /* value */);
	virtual void updateGradient(bool external, const vle::devs::Time& time);
        virtual void updateSigma(const vle::devs::Time& time);
        virtual void updateValue(bool external, const vle::devs::Time& time);

        unsigned long mIndex;
        /** Constantes */
        double mPrecision;
        double mEpsilon;
        double mThreshold;

        inline double d(unsigned long x)
        { return x * mPrecision; }
    };

}} // namespace vle extension

#endif
