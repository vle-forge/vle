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

#include "help.h"
#include "ui_help.h"
#include <QDebug>
#include <QResource>

/**
 * @brief help::help Default constructor
 *        Load the index page into the help tab (according to language)
 */
help::help(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::help)
{
    mhelpEngine = 0;

    ui->setupUi(this);
    loadHelp("index");
}

/**
 * @brief help::~help Default destructor
 *        Clean memory
 */
help::~help()
{
    delete ui;

    if (mhelpEngine)
        delete mhelpEngine;
}

/**
 * @brief help::loadResource
 *        Load help page from internal resource (old help support)
 *
 */
void
help::loadResource()
{
    mLocaleName = QLocale::system().name();
    QString lang = mLocaleName.left(mLocaleName.indexOf("_"));

    QString rc = ":/help/help/index";

    QResource r(rc + "-" + lang);
    if (!r.isValid())
        r.setFileName(rc);

    if (r.isValid()) {
        QString pageContent((char*)r.data());
        ui->textBrowser->setHtml(pageContent);
    }
}

/**
 * @brief help::loadHelp
 *        Load help page using QHelp framework
 *
 */
void
help::loadHelp(QString topic)
{
    if (mhelpEngine == 0) {
        QString colFile;
        colFile = qApp->property("manPath").toString();
        colFile += "/gvle.qhc";

        // ToDo : select translated QHC file accoding to locales

        mhelpEngine = new QHelpEngineCore(colFile, this);
        if (!mhelpEngine->setupData()) {
            delete mhelpEngine;
            mhelpEngine = 0;
            qDebug() << tr("Fail to load help file : ") << colFile;
            return;
        }
        ui->textBrowser->setHelp(mhelpEngine);
    }

    ui->textBrowser->showHelpForKeyword(topic);
}
