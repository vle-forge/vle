﻿/*
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

#include "gvle_file.h"


namespace vle {
namespace gvle {

gvle_file::gvle_file():
        relPath(""),source_file(""), metadata_file(""),
        plug_type(gvleplug::GVLE_PLUG_NONE), plug_name("")
{
}
gvle_file::gvle_file(const gvle_file& f):
            relPath(f.relPath), source_file(f.source_file),
            metadata_file(f.metadata_file), plug_type(f.plug_type),
            plug_name(f.plug_name)
{
}

gvle_file::gvle_file(const utils::Package& pkg, QString l_relPath):
        relPath(l_relPath), source_file(""), metadata_file(""),
        plug_type(gvleplug::GVLE_PLUG_NONE),  plug_name("")
{
    QString pkgPath = QString(pkg.getDir(utils::PKG_SOURCE).c_str());
    source_file = pkgPath + "/" + relPath;
    QFileInfo fileInfo = QFileInfo(source_file);

    QString suffix = fileInfo.suffix();         // vpz, cpp or dat
    QString baseName = fileInfo.baseName();     // eg NewCpp
    QString dir = fileInfo.dir().dirName();     //src, exp or output
    QString suffix_metadata = "";               // vpm, sm or om

    //get suffix_metadata and set plugin informations
    if (suffix == "vpz" and dir == "exp") {
        suffix_metadata = "vpm";
        plug_type = gvleplug::GVLE_PLUG_MAIN_VPZ;
        plug_name = "Default";
    } else if(suffix == "cpp" and dir == "src") {
        suffix_metadata = "sm";
        plug_type = gvleplug::GVLE_PLUG_MAIN;
        plug_name = "Default";
    } else if (suffix  == "dat" and dir =="output") {
        suffix_metadata = "om";
        plug_type = gvleplug::GVLE_PLUG_MAIN_OUT;
        plug_name = "Default";
    }
    //set metadata file
    if (suffix_metadata != "") {
        metadata_file = pkgPath + "/metadata/" + dir + "/" + baseName + "."
                            + suffix_metadata;
    }
    //set plug type
    if (metadata_file != "") {
        QFile file(metadata_file);
        if (file.exists()) {
            QDomDocument dom("vle_project_metadata");
            QXmlInputSource source(&file);
            QXmlSimpleReader reader;
            dom.setContent(&source, &reader);
            QDomElement docElem = dom.documentElement();
            QDomNode pluginNode;
            if (suffix_metadata == "vpm") {
                pluginNode = dom.elementsByTagName("vpzPlugin").item(0);
            } else if (suffix_metadata == "sm") {
                pluginNode = dom.elementsByTagName("srcPlugin").item(0);
            } else if (suffix_metadata == "dat") {
                pluginNode = dom.elementsByTagName("outPlugin").item(0);
            }
            if (not pluginNode.isNull()) {
                plug_name =
                        pluginNode.attributes().namedItem("name").nodeValue();
            }
        }
    }
}


QString
gvle_file::baseName() const
{
    QFileInfo fileInfo = QFileInfo(source_file);
    return fileInfo.baseName();
}

PluginMainPanel*
gvle_file::newInstanceMainPanel(gvle_plugins& plugins)
{
    switch(plug_type) {
    case gvleplug::GVLE_PLUG_OUTPUT:
    case gvleplug::GVLE_PLUG_COND:
    case gvleplug::GVLE_PLUG_SIM:
        return 0;
        break;
    case gvleplug::GVLE_PLUG_MAIN:
        return plugins.newInstanceMainPanelPlugin(plug_name);
        break;
    case gvleplug::GVLE_PLUG_MAIN_OUT:
        return plugins.newInstanceMainPanelOutPlugin(plug_name);
        break;
    case gvleplug::GVLE_PLUG_MAIN_VPZ:
        return plugins.newInstanceMainPanelVpzPlugin(plug_name);
        break;
    case gvleplug::GVLE_PLUG_NONE:
        if (relPath == "Description.txt") {
            return plugins.newInstanceMainPanelPlugin("Default");
        } else {
            return 0;
        }
        break;
    }
    return 0;
}

/*************** Static functions ***************/

gvle_file
gvle_file::getNewCpp(const utils::Package& pkg)
{
    QString pkgPath = QString(pkg.getDir(utils::PKG_SOURCE).c_str());
    QString baseName = "NewCpp";
    bool found = false;
    int i = 0;
    while (not found) {
        if (i > 0) {
            baseName += "_" + QVariant(i).toString();
            baseName += "_" + QVariant(i).toString();
        }
        found= not QFile(pkgPath + "/src/" + baseName+ ".cpp").exists() and
               not QFile(pkgPath + "/metadata/src/" + baseName+ ".sm").exists();
        i++;
    }
    QString relPath = "src/"+baseName+".cpp";
    return gvle_file(pkg, relPath);
}

gvle_file
gvle_file::getNewVpz(const utils::Package& pkg)
{
    QString pkgPath = QString(pkg.getDir(utils::PKG_SOURCE).c_str());
    QString baseName = "NewVpz";
    bool found = false;
    int i = 0;
    while (not found) {
        if (i > 0) {
            baseName += "_" + QVariant(i).toString();
            baseName += "_" + QVariant(i).toString();
        }
        found= not QFile(pkgPath + "/exp/" + baseName+ ".vpz").exists() and
               not QFile(pkgPath + "/metadata/exp/" + baseName+ ".vpm").exists();
        i++;
    }
    QString relPath = "exp/"+baseName+".vpz";
    return gvle_file(pkg, relPath);
}

gvle_file
gvle_file::getCopy(const utils::Package& pkg, gvle_file gf)
{
    QString pkgPath = QString(pkg.getDir(utils::PKG_SOURCE).c_str());
    QFileInfo fileInfo = QFileInfo(gf.source_file);

    QString suffix = fileInfo.suffix();         // vpz, cpp or dat
    QString baseName = fileInfo.baseName();     // eg NewCpp
    QString dir = fileInfo.dir().dirName();     //src, exp or output
    QString suffix_metadata = "";               // vpm, sm or om

    //get suffix_metadata
    if (suffix == "vpz" and dir == "exp") {
        suffix_metadata = "vpm";
    } else if(suffix == "cpp" and dir == "src") {
        suffix_metadata = "sm";
    } else if (suffix  == "dat" and dir =="output") {
        suffix_metadata = "om";
    }

    QString currName;
    bool found = false;
    int i = 0;
    while (not found) {
        currName = baseName;
        if (i > 0) {
            currName += "_" + QVariant(i).toString();
        }
        found= not QFile(pkgPath+"/"+dir+"/"+currName+ "."+suffix).exists() and
               not QFile(pkgPath+"/metadata/"+dir
                       +"/"+currName+"."+suffix_metadata).exists();
        i++;
    }
    gvle_file gfcopy(pkg, dir+"/"+currName+"."+suffix);
    gfcopy.plug_name = gf.plug_name;
    gfcopy.plug_type = gf.plug_type;
    return gfcopy;
}

}} //namespaces
