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

#include "vle/gvle/aboutbox.h"
#include "ui_aboutbox.h"
#include <QString>
#include <QTextStream>
#include <vle/vle.hpp>

AboutBox::AboutBox(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::AboutBox)
{
    ui->setupUi(this);

    auto vers = vle::version();

    QString str;
    QTextStream(&str) << "VLE " << std::get<0>(vers) << '.'
                      << std::get<1>(vers) << '.' << std::get<2>(vers);

    if (not std::get<3>(vers).empty())
        QTextStream(&str) << '-' << std::get<3>(vers).c_str();

    ui->label_2->setText(str);
}

AboutBox::~AboutBox()
{
    delete ui;
}
