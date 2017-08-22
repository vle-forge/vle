/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <QDebug>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
//#include <QtCore/qnamespace.h>
#include "FilePluginGUItab.h"
#include "ui_FilePluginGvle.h"
#include <iostream>

/**
 * @brief FilePluginGUItab::FilePluginGUItab
 *        Default constructor
 */
FilePluginGUItab::FilePluginGUItab(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::FilePluginGvle)
  , mvleVpz(0)
  , mViewName("")
  , outputNodeConfig(nullptr)
{
    ui->setupUi(this);

    QObject::connect(ui->flushByBag,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(flushByBagChanged(int)));
    QObject::connect(ui->julianDay,
                     SIGNAL(stateChanged(int)),
                     this,
                     SLOT(julianDayChanged(int)));
    QObject::connect(ui->locale,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(localeChanged(const QString&)));
    QObject::connect(ui->destination,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(destinationChanged(const QString&)));
    QObject::connect(ui->fileType,
                     SIGNAL(currentIndexChanged(const QString&)),
                     this,
                     SLOT(fileTypeChanged(const QString&)));
}

FilePluginGUItab::~FilePluginGUItab()
{
    delete ui;
}

void
FilePluginGUItab::init(vle::gvle::vleVpz* vpz, const QString& viewName)
{
    mvleVpz = vpz;
    mViewName = viewName;

    outputNodeConfig = mvleVpz->buildOutputConfigMap(mViewName);

    if (not wellFormed()) {
        buildDefaultConfig();
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }

    bool oldBlock = ui->flushByBag->blockSignals(true);
    ui->flushByBag->setCheckState(Qt::CheckState(
      getCheckState(outputNodeConfig->getBoolean("flush-by-bag"))));
    ui->flushByBag->blockSignals(oldBlock);
    oldBlock = ui->julianDay->blockSignals(true);
    ui->julianDay->setCheckState(Qt::CheckState(
      getCheckState(outputNodeConfig->getBoolean("julian-day"))));
    ui->julianDay->blockSignals(oldBlock);
    oldBlock = ui->locale->blockSignals(true);
    ui->locale->setCurrentIndex(ui->locale->findText(
      QString(outputNodeConfig->getString("locale").c_str())));
    ui->locale->blockSignals(oldBlock);
    oldBlock = ui->destination->blockSignals(true);
    ui->destination->setCurrentIndex(
      ui->destination->findText(QString("File")));
    ui->destination->setEnabled(false);
    ui->destination->blockSignals(oldBlock);
    oldBlock = ui->fileType->blockSignals(true);
    ui->fileType->setCurrentIndex(ui->fileType->findText(
      QString(outputNodeConfig->getString("type").c_str())));
    ui->fileType->blockSignals(oldBlock);
}

void
FilePluginGUItab::flushByBagChanged(int val)
{
    bool& b = outputNodeConfig->getBoolean("flush-by-bag");
    if (b != (bool)val) {
        b = (bool)val;
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }
}

void
FilePluginGUItab::julianDayChanged(int val)
{
    bool& b = outputNodeConfig->getBoolean("julian-day");
    if (b != (bool)val) {
        b = (bool)val;
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }
}

void
FilePluginGUItab::localeChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("locale");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }
}

void
FilePluginGUItab::destinationChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("output");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }
}

void
FilePluginGUItab::fileTypeChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("type");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }
}

int
FilePluginGUItab::getCheckState(const vle::value::Boolean& v)
{
    if (v.value()) {
        return 2; /*Qt::CheckState::Checked*/
        ;
    } else {
        return 0; /*Qt::CheckState::Unchecked;*/
    }
    return 0;
}

bool
FilePluginGUItab::wellFormed()
{
    if (not outputNodeConfig) {
        return false;
    }
    if (outputNodeConfig->size() != 5) {
        return false;
    }
    vle::value::Map::const_iterator itb = outputNodeConfig->begin();
    vle::value::Map::const_iterator ite = outputNodeConfig->end();
    for (; itb != ite; itb++) {
        const std::string& key = itb->first;
        const std::unique_ptr<vle::value::Value>& v = itb->second;
        if (key == "flush-by-bag") {
            if (not v->isBoolean()) {
                return false;
            }
        } else if (key == "julian-day") {
            if (not v->isBoolean()) {
                return false;
            }
        } else if (key == "locale") {
            if (not v->isString()) {
                return false;
            }
            if ((v->toString().value() != "C") and
                (v->toString().value() != "user")) {
                return false;
            }
        } else if (key == "output") {
            if (not v->isString()) {
                return false;
            }
        } else if (key == "type") {
            if (not v->isString()) {
                return false;
            }
            if ((v->toString().value() != "csv") and
                (v->toString().value() != "rdata") and
                (v->toString().value() != "text")) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void
FilePluginGUItab::buildDefaultConfig()
{
    if (not outputNodeConfig) {
        qDebug() << " Internal Error FilePluginGUItab::buildDefaultConfig() ";
        return;
    }
    outputNodeConfig.reset(new vle::value::Map());
    outputNodeConfig->addBoolean("flush-by-bag", false);
    outputNodeConfig->addBoolean("julian-day", false);
    outputNodeConfig->addString("locale", "C");
    outputNodeConfig->addString("output", "file");
    outputNodeConfig->addString("type", "text");
}
