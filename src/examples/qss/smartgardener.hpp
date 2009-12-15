/**
 * @file examples/qss/smartgardener.hpp
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


#ifndef EXAMPLES_QSS_SMARTGARDENER_HPP
#define EXAMPLES_QSS_SMARTGARDENER_HPP

#include <vle/devs/Dynamics.hpp>

namespace vle { namespace examples { namespace qss {

    class Smartgardener : public devs::Dynamics
    {
    public:
        enum State { INIT, IDLE, PEST };

        Smartgardener(const devs::DynamicsInit& model,
                      const devs::InitEventList& events);

        virtual ~Smartgardener();

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual devs::Time timeAdvance() const;

        virtual devs::Time init(const vle::devs::Time& /* time */);

        virtual void internalTransition(const devs::Time& event);

        void externalTransition(const devs::ExternalEventList& event,
                                   const devs::Time& time);

        virtual value::Value* observation(const devs::ObservationEvent& event)
            const;

    private:
        State state;

        double s;
        double p_p;
        double p_l;
        double d;
        double plantlouseamount;
        double ladybirdamount;
    };

}}} // namespace vle examples qss

#endif

