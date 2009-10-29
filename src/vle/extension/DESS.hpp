/**
 * @file vle/extension/DESS.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_EXTENSION_DESS_HPP
#define VLE_EXTENSION_DESS_HPP

#include <vle/extension/DifferentialEquation.hpp>
#include <vle/extension/DllDefines.hpp>

namespace vle { namespace extension {

    class DESS;

    class VLE_EXTENSION_EXPORT Method
    {
    public:
        Method(DESS& dess):dess(dess) {}

        virtual ~Method() {}

        virtual void operator()(const vle::devs::Time& time) = 0;

    protected:
        DESS& dess;
    };

    class VLE_EXTENSION_EXPORT DESS : public vle::extension::DifferentialEquation
    {
    public:
        DESS(const vle::devs::DynamicsInit& model,
             const vle::devs::InitEventList& events);

        virtual ~DESS() {}

        virtual double getEstimatedValue(double e) const;

    private:
        virtual void reset(const vle::devs::Time& time,
                           double value);
        virtual void updateGradient(bool external,
                                    const vle::devs::Time& time);
        virtual void updateSigma(const vle::devs::Time& time);
        virtual void updateValue(bool external, const vle::devs::Time& time);

        double mTimeStep;
        Method* mMethod;

        friend struct RK4;
        friend struct Euler;
    };

    class VLE_EXTENSION_EXPORT RK4 : public Method
    {
    public:
        RK4(DESS& dess) : Method(dess) {}

        virtual ~RK4() {}

        virtual void operator()(const vle::devs::Time& time);
    };

    class VLE_EXTENSION_EXPORT Euler : public Method
    {
    public:
        Euler(DESS& dess):Method(dess) {}

        virtual ~Euler() {}

        virtual void operator()(const vle::devs::Time& time);
    };

}} // namespace vle extension

#endif
