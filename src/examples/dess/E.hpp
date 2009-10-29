/**
 * @file examples/dess/E.hpp
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


#ifndef VLE_EXAMPLES_E_HPP
#define VLE_EXAMPLES_E_HPP

#include <vle/extension/DESS.hpp>

namespace vle { namespace examples { namespace dess {

class E : public extension::DESS
{
public:
    E(const devs::DynamicsInit& model,
      const devs::InitEventList& events);
    virtual ~E()
        { }
    
    virtual double compute(const devs::Time& time) const;
    
private:
    double b0;
    double b1;
    double m;
    double a;
    
    Var _E;
    Ext S;
    Ext I;
};

}}} // namespace vle examples dess

#endif
