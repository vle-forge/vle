/**
 * @file src/examples/dess/seir.hpp
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


#ifndef VLE_EXAMPLES_SEIR_HPP
#define VLE_EXAMPLES_SEIR_HPP

#include <vle/extension/CombinedQSS.hpp>

namespace vle { namespace examples { namespace dess {

class seir : public vle::extension::CombinedQss
{
private:
    double m;
    double g;
    double a;
    double b0;
    double b1;
    double N;
    
public:
    seir(const vle::graph::AtomicModel&,
	 const vle::devs::InitEventList&);    
    virtual ~seir(){}
    
    virtual double compute(unsigned int i, 
			   const vle::devs::Time& time) const;
};

}}} // namespace vle examples dess

#endif
