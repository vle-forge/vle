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


#ifndef VLE_UTILS_I18N_HPP
#define VLE_UTILS_I18N_HPP

#include <vle/version.hpp>
#include <boost/format.hpp>

namespace vle {

    typedef boost::format fmt;

}


#ifdef VLE_HAVE_NLS
# ifndef ENABLE_NLS
#  define ENABLE_NLS
# endif
#  include <libintl.h>
#  include <locale.h>
#  define _(x) gettext(x)
#  define gettext_noop(x) x
#  define N_(x) gettext_noop(x)
#else
#  define _(x) x
#  define N_(x) x
#endif

#endif
