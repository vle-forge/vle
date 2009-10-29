/**
 * @file vle/gvle/ConditionPlugin.cpp
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


#include <vle/gvle/ConditionPlugin.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <cassert>

namespace vle { namespace gvle {

ConditionPlugin::ConditionPlugin(const std::string& name)
    : m_name(name)
{
    assert(not name.empty());
}

Glib::RefPtr < Gdk::Pixbuf > ConditionPlugin::icon() const
{
    if (not m_icon) {
        std::string file = utils::Path::path().getConditionPixmapFile(m_name);
        try {
            m_icon = Gdk::Pixbuf::create_from_file(file);
        } catch(const Glib::FileError& e) {
            throw utils::FileError(fmt(_(
                    "ConditionPlugin '%1%': FileError, %2%")) % m_name %
                e.what());
        } catch(const Gdk::PixbufError& e) {
            throw utils::FileError(fmt(_(
                    "ConditionPlugin '%1%': PixbufError, %2%")) % m_name %
                e.what());
        } catch(...) {
            throw utils::FileError(fmt(_(
                    "ConditionPlugin '%1%': Unknow error")) % m_name);
        }
    }
    return m_icon;
}

}} // namespace vle gvle
