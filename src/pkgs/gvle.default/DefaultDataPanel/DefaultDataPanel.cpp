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

#include "DefaultDataPanel.h"
#include <QFileInfo>
#include <sstream>
#include <vle/utils/Template.hpp>

namespace vle {
namespace gvle {

DefaultDataPanel::DefaultDataPanel()
  : PluginMainPanel()
  , m_edit(0)
  , m_file("")
{

    m_edit = new QTextEdit();
    QObject::connect(
      m_edit, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
}

DefaultDataPanel::~DefaultDataPanel()
{
}

void
DefaultDataPanel::initData(QString pkg, QString classname, QString filePath)
{
    std::string tpl = "# data file";

    vle::utils::Template vleTpl(tpl);
    vleTpl.stringSymbol().append("classname", classname.toStdString());
    vleTpl.stringSymbol().append("pkg", pkg.toStdString());

    std::ostringstream out;
    vleTpl.process(out);

    QFile datafile(filePath);
    datafile.open(QIODevice::WriteOnly | QIODevice::Text);

    if (!datafile.isOpen()) {
        qDebug() << "- Error, unable to open"
                 << "dataFilename"
                 << "for data";
    }
    QTextStream outStream(&datafile);
    outStream << out.str().c_str();
    datafile.close();
}

void
DefaultDataPanel::init(const gvle_file& gf,
                       utils::Package* pkg,
                       Logger* /*log*/,
                       gvle_plugins* /*plugs*/,
                       const utils::ContextPtr& /*ctx*/)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    m_file = gf.source_file;

    if (not QFile(m_file).exists()) {
        initData(
          QString(pkg->name().c_str()), QFileInfo(m_file).baseName(), m_file);
    }

    QFile dataFile(m_file);

    if (!dataFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultDataPanel::init ";
    }

    QTextStream in(&dataFile);
    m_edit->setText(in.readAll());
}

QString
DefaultDataPanel::canBeClosed()
{
    return "";
}

void
DefaultDataPanel::save()
{
    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str());
            file.flush();
            file.close();
        }
    }
    emit undoAvailable(false);
}

QString
DefaultDataPanel::getname()
{
    return "Default";
}

QWidget*
DefaultDataPanel::leftWidget()
{
    return m_edit;
}

QWidget*
DefaultDataPanel::rightWidget()
{
    return 0;
}

void
DefaultDataPanel::undo()
{
}

void
DefaultDataPanel::redo()
{
}

void
DefaultDataPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}
}
} // namespaces
