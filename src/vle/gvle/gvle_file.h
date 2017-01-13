/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
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

#ifndef gvle_gvle_file_H
#define gvle_gvle_file_H

#include <QtCore>
#include <QtXml>

#include <vle/utils/Package.hpp>

#include "gvle_plugins.h"
#include "plugin_mainpanel.h"

namespace vle {
namespace gvle {

/***
 * @brief the couple of a source file (either cpp or vpz) with the metadata
 * file
 */
struct gvle_file {
    gvle_file();
    gvle_file(const gvle_file& f);
    gvle_file(const utils::Package& pkg, QString relPath);

    QString baseName() const; //eg NewCpp
    PluginMainPanel* newInstanceMainPanel(gvle_plugins& plugins);
    QString suffix(); //eg cpp or vpz or dat

    static gvle_file getNewCpp(const utils::Package& pkg);
    static gvle_file getNewVpz(const utils::Package& pkg);
    static gvle_file getNewData(const utils::Package& pkg);
    static gvle_file getCopy(const utils::Package& pkg, gvle_file gf);

    QString                    relPath;
    QString                    source_file;
    QString                    metadata_file;
    gvleplug::GVLE_PLUGIN_TYPE plug_type;
    QString                    plug_name;

};

}}//namespaces

#endif // gvle_gvle_file_H
