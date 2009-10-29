/**
 * @file vle/oov/plugins/cairo/plot/RealCurve.hpp
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


#ifndef VLE_OOV_PLUGINS_CAIRO_PLOT_REALCURVE_HPP
#define VLE_OOV_PLUGINS_CAIRO_PLOT_REALCURVE_HPP

#include <vle/oov/plugins/cairo/plot/Parameter.hpp>
#include <vle/oov/plugins/cairo/plot/Curve.hpp>
#include <vector>
#include <cairomm/cairomm.h>

namespace vle { namespace oov { namespace plugin {

class RealCurve: public Curve
{
private :
    std::vector < std::pair < double, double > > m_valueList;
    std::string const m_type;

public :
    RealCurve(const std::string & name,
	      unsigned int r,
	      unsigned int g,
	      unsigned int b) : Curve(name, r, g, b)
	{ }

    virtual ~RealCurve() { }

    void add(double date, double value);

    void draw(Cairo::RefPtr < Cairo::Context > ctx, Parameter & m_parameter);

    double get_last_value() const;

    bool is_double() { return true; }

    int get_size() { return m_valueList.size(); }

    void get_min_max_value(int number_value, double & min, double &
				 max, Parameter & m_parameter);

    double get_min_value(int number_value, Parameter & m_parameter);

    double get_max_value(int number_value, Parameter & m_parameter);

    int get_number_value();

    double get_date(int index);

    double get_value(int index);

    void get_min_max_value_prec_index(int indice, int number_value,
					    double & min, double & max);

    double get_min_value_prec_index(int indice, int number_value);

    double get_max_value_prec_index(int indice, int number_value);

};

}}} // namespace vle oov plugin

#endif
