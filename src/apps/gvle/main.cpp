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

#include "vle/gvle/gvle_win.h"
#include <vle/utils/Filesystem.hpp>
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtDebug>
#include <iostream>
#include <vle/version.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool result;

    vle::utils::Path prefix(vle::utils::Path::path().getPrefixDir());
    vle::utils::Path localesPath(prefix);
    localesPath /= VLE_SHARE_DIRS;
    localesPath /= "translations";

    vle::utils::Path manPath(prefix);
    manPath /= VLE_SHARE_DIRS;
    manPath /= "man";

    a.setProperty("localesPath", QString(localesPath.string().c_str()));
    a.setProperty("manPath",     QString(manPath.string().c_str()));

    QTranslator qtTranslator;
    result = qtTranslator.load("gvle_" + QLocale::system().name() + ".qm", localesPath.string().c_str());
    if (result == false)
    {
        qDebug() << "Load Translator : " <<
            QLocale::system().name() << " not found " <<
            "here : " << localesPath.string().c_str();
    }
    a.installTranslator(&qtTranslator);

    vle::gvle::gvle_win w;

    w.show();

    result = a.exec();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
