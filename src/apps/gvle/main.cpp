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
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>
#include <QTranslator>
#include <QtDebug>
#include <QtGlobal>
#include <iostream>
#include <iostream>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vle.hpp>

int
main(int argc, char* argv[])
{

#ifdef _WIN32
    {
        QDir qt5plugins_dir =
          QCoreApplication::applicationDirPath() + "/../share/qt5/plugins";
        if (not qt5plugins_dir.exists()) {
            qDebug() << " building lib path error :  "
                     << qt5plugins_dir.absolutePath();
        } else {
            QCoreApplication::addLibraryPath(qt5plugins_dir.absolutePath());
        }
    }
    {
        bool found = false;
        for (unsigned int i = 0; (i < 2) and not found; i++) {
            QString reg_str = "";
            if (i == 0) {
                reg_str = vle::utils::format("HKEY_LOCAL_"
                                             "MACHINE\\SOFTWARE\\Wow6432Node\\"
                                             "VLE Development Team\\VLE %s.0",
                                             vle::string_version_abi().c_str())
                            .c_str();
            } else {
                reg_str =
                  vle::utils::format("HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE "
                                     "Development Team\\VLE %s.0",
                                     vle::string_version_abi().c_str())
                    .c_str();
            }
            QSettings reg_vle(reg_str, QSettings::NativeFormat);

            QVariant reg_path;
            {
                reg_path = reg_vle.value(".");
                if (reg_path.isValid()) {
                    QDir tmp(
                      QDir(reg_path.toString()).filePath("share/qt5/plugins"));
                    if (tmp.exists()) {
                        QCoreApplication::addLibraryPath(tmp.absolutePath());
                        found = true;
                    }
                }
            }
            {
                reg_path = reg_vle.value("Default");
                if (reg_path.isValid()) {
                    QDir tmp(QDir(reg_path.toString())
                               .filePath("/share/qt5/plugins"));
                    if (tmp.exists()) {
                        QCoreApplication::addLibraryPath(tmp.absolutePath());
                        found = true;
                    }
                }
            }
            {
                reg_path = reg_vle.value("path");
                if (reg_path.isValid()) {
                    QDir tmp(QDir(reg_path.toString())
                               .filePath("/share/qt5/plugins"));
                    if (tmp.exists()) {
                        QCoreApplication::addLibraryPath(tmp.absolutePath());
                        found = true;
                    }
                }
            }
        }
        if (not found) {
            qDebug() << " building lib path error : qt plugins not found ";
        }
    }
#endif

    qunsetenv("QT_HASH_SEED");
    qputenv("QT_HASH_SEED","0");

    QApplication a(argc, argv);
    vle::Init m_app;

    bool result;

    auto ctx = vle::utils::make_context();

    vle::utils::Path prefix(ctx->getPrefixDir());
    vle::utils::Path localesPath(prefix);
    localesPath /= "share";
    localesPath /=
      vle::utils::format("vle-%s", vle::string_version_abi().c_str());
    localesPath /= "translations";

    vle::utils::Path manPath(prefix);
    manPath /= "share";
    manPath /= vle::utils::format("vle-%s", vle::string_version_abi().c_str());
    manPath /= "man";

    a.setProperty("localesPath", QString(localesPath.string().c_str()));
    a.setProperty("manPath", QString(manPath.string().c_str()));

    QTranslator qtTranslator;
    result = qtTranslator.load("gvle_" + QLocale::system().name() + ".qm",
                               localesPath.string().c_str());
    if (result == false) {
        qDebug() << "Load Translator : " << QLocale::system().name()
                 << " not found "
                 << "here : " << localesPath.string().c_str();
    }
    a.installTranslator(&qtTranslator);

    vle::gvle::gvle_win w(ctx);

    w.show();

    result = a.exec();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
