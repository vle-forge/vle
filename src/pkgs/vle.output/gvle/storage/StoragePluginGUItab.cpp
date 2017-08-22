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

#include "StoragePluginGUItab.h"
#include "ui_StoragePluginGvle.h"
#include <QDebug>

/**
 * @brief StoragePluginGUItab::StoragePluginGUItab
 *        Default constructor
 */
StoragePluginGUItab::StoragePluginGUItab(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::StoragePluginGvle)
  , mvleVpz(0)
  , mViewName("")
  , outputNodeConfig(nullptr)
{
    //
    ui->setupUi(this);
    QObject::connect(ui->spinBoxRows,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(rowsChanged(int)));
    QObject::connect(ui->spinBoxColumns,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(columnsChanged(int)));
    QObject::connect(ui->spinBoxUpdateRows,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(incRowsChanged(int)));
    QObject::connect(ui->spinBoxUpdateColumns,
                     SIGNAL(valueChanged(int)),
                     this,
                     SLOT(incColumnsChanged(int)));
    QObject::connect(ui->checkBoxHeader,
                     SIGNAL(clicked(bool)),
                     this,
                     SLOT(headerOnTopChanged(bool)));
}

StoragePluginGUItab::~StoragePluginGUItab()
{
    delete ui;
}

void
StoragePluginGUItab::init(vle::gvle::vleVpz* vpz, const QString& viewName)
{
    mvleVpz = vpz;
    mViewName = viewName;
    outputNodeConfig = mvleVpz->buildOutputConfigMap(mViewName);
    if (not wellFormed()) {
        buildDefaultConfig();
        mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
    }

    bool oldBlock = ui->spinBoxRows->blockSignals(true);
    ui->spinBoxRows->setValue(outputNodeConfig->getInt("rows"));
    ui->spinBoxRows->blockSignals(oldBlock);

    oldBlock = ui->spinBoxColumns->blockSignals(true);
    ui->spinBoxColumns->setValue(outputNodeConfig->getInt("columns"));
    ui->spinBoxColumns->blockSignals(oldBlock);

    oldBlock = ui->spinBoxUpdateRows->blockSignals(true);
    ui->spinBoxUpdateRows->setValue(outputNodeConfig->getInt("inc_rows"));
    ui->spinBoxUpdateRows->blockSignals(oldBlock);

    oldBlock = ui->spinBoxUpdateColumns->blockSignals(true);
    ui->spinBoxUpdateColumns->setValue(
      outputNodeConfig->getInt("inc_columns"));
    ui->spinBoxUpdateColumns->blockSignals(oldBlock);

    oldBlock = ui->checkBoxHeader->blockSignals(true);
    if (outputNodeConfig->getString("header") == "top") {
        ui->checkBoxHeader->setCheckState(Qt::Checked);
    } else {
        ui->checkBoxHeader->setCheckState(Qt::Unchecked);
    }
    ui->checkBoxHeader->blockSignals(oldBlock);
}

void
StoragePluginGUItab::rowsChanged(int v)
{
    int& toUp = outputNodeConfig->getInt("rows");
    toUp = v;
    mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
}

void
StoragePluginGUItab::columnsChanged(int v)
{
    int& toUp = outputNodeConfig->getInt("columns");
    toUp = v;
    mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
}

void
StoragePluginGUItab::incRowsChanged(int v)
{
    int& toUp = outputNodeConfig->getInt("inc_rows");
    toUp = v;
    mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
}

void
StoragePluginGUItab::incColumnsChanged(int v)
{
    int& toUp = outputNodeConfig->getInt("inc_columns");
    toUp = v;
    mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
}

void
StoragePluginGUItab::headerOnTopChanged(bool v)
{
    std::string& toUp = outputNodeConfig->getString("header");
    if (v) {
        toUp = "top";
    } else {
        toUp = "none";
    }
    mvleVpz->fillOutputConfigMap(mViewName, *outputNodeConfig);
}

bool
StoragePluginGUItab::wellFormed()
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
        if (key == "rows") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "columns") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "inc_rows") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "inc_columns") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "header") {
            if (not v->isString()) {
                return false;
            } else if (outputNodeConfig->getString("header") != "top" &&
                       outputNodeConfig->getString("header") != "none") {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void
StoragePluginGUItab::buildDefaultConfig()
{
    outputNodeConfig.reset(new vle::value::Map());
    outputNodeConfig->addInt("rows", 15);
    outputNodeConfig->addInt("columns", 15);
    outputNodeConfig->addInt("inc_rows", 10);
    outputNodeConfig->addInt("inc_columns", 10);
    outputNodeConfig->addString("header", "none");
}
