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

#include <sstream>
#include <QFileInfo>
#include <vle/utils/Template.hpp>
#include "DefaultCppPanel.h"

namespace vle {
namespace gvle {





DefaultCppPanel::DefaultCppPanel():
            PluginMainPanel(), m_edit(0), m_file("")
{

    m_edit = new QTextEdit();
    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

}

DefaultCppPanel::~DefaultCppPanel()
{
    delete m_edit;
}


void
DefaultCppPanel::initCpp(QString pkg, QString classname, QString filePath)
{
    std::string tpl =
    "/*\n"                                                                     \
    "* @@tagdynamic@@\n"                                                       \
    "* @@tagdepends:@@endtagdepends\n"                                         \
    "*/\n"                                                                     \
    "#include <vle/value/Value.hpp>\n"                                         \
    "#include <vle/devs/Dynamics.hpp>\n"                                       \
    "namespace vd = vle::devs;\n"                                              \
    "namespace vv = vle::value;\n"                                             \
    "namespace {{pkg}} {\n"                                                    \
    "class {{classname}} : public vd::Dynamics\n"                              \
    "{\n"                                                                      \
    " public:\n"                                                               \
    "  {{classname}}(const vd::DynamicsInit& init,\n"                          \
    "                const vd::InitEventList& events)\n"                       \
    "        : vd::Dynamics(init, events)\n"                                   \
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    "  virtual ~{{classname}}()\n"                                             \
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    "  virtual vd::Time init(vle::devs::Time /*time*/) override\n"             \
    "  {\n"                                                                    \
    "    return vd::infinity;\n"                                               \
    "  }\n"                                                                    \
    "  virtual void output(vle::devs::Time /*time*/,\n"                        \
    "                      vd::ExternalEventList& /*output*/) const override\n"\
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    "  virtual vd::Time timeAdvance() const override\n"                        \
    "  {\n"                                                                    \
    "    return vd::infinity;\n"                                               \
    "  }\n"                                                                    \
    "  virtual void internalTransition(vle::devs::Time /*time*/) override\n"   \
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    "  virtual void externalTransition(\n"                                     \
    "            const vd::ExternalEventList& /*event*/,\n"                    \
    "            vle::devs::Time /*time*/) override\n"                         \
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    "  virtual void confluentTransitions(\n"                                   \
    "            vle::devs::Time time,\n"                                      \
    "            const vd::ExternalEventList& events) override\n"              \
    "  {\n"                                                                    \
    "    internalTransition(time);\n"                                          \
    "    externalTransition(events, time);\n"                                  \
    "  }\n"                                                                    \
    "  virtual std::unique_ptr<vle::value::Value> observation(\n"              \
    "            const vd::ObservationEvent& /*event*/) const override\n"      \
    "  {\n"                                                                    \
    "    return 0;\n"                                                          \
    "  }\n"                                                                    \
    "  virtual void finish()\n"                                                \
    "  {\n"                                                                    \
    "  }\n"                                                                    \
    " };\n"                                                                    \
    "} // namespace\n"                                                         \
    "DECLARE_DYNAMICS({{pkg}}::{{classname}})\n";

    vle::utils::Template vleTpl(tpl);
    vleTpl.stringSymbol().append("classname", classname.toStdString());
    vleTpl.stringSymbol().append("pkg", pkg.toStdString());

    std::ostringstream out;
    vleTpl.process(out);

    QFile cppfile(filePath);
    cppfile.open(QIODevice::WriteOnly | QIODevice::Text);

    if(!cppfile.isOpen()){
        qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
    }
    QTextStream outStream(&cppfile);
    outStream << out.str().c_str();
    cppfile.close();
}


void
DefaultCppPanel::init(const gvle_file& gf, utils::Package* pkg,
        Logger* /*log*/, gvle_plugins* /*plugs*/,
        const utils::ContextPtr& /*ctx*/)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    m_file = gf.source_file;

    if (not QFile(m_file).exists()){
        initCpp(QString(pkg->name().c_str()),QFileInfo(m_file).baseName(),
                m_file);
    }

    QFile cppFile (m_file);

    if (!cppFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultCppPanel::init ";
    }

    QTextStream in(&cppFile);
    m_edit->setText(in.readAll());
}

QString
DefaultCppPanel::canBeClosed()
{
    return "";
}

void
DefaultCppPanel::save()
{
    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str()) ;
            file.flush();
            file.close();
        }
    }
    emit undoAvailable(false);
}

QString
DefaultCppPanel::getname()
{
    return "Default";
}

QWidget*
DefaultCppPanel::leftWidget()
{
    return m_edit;
}

QWidget*
DefaultCppPanel::rightWidget()
{
    return 0;
}

void
DefaultCppPanel::undo()
{
}

void
DefaultCppPanel::redo()
{

}

void
DefaultCppPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces
