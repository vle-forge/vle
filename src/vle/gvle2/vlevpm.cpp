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

#include "vlevpm.h"

#include <QDebug>

namespace vle {
namespace gvle2 {

vpzVpm::vpzVpm(): mDoc(0), mFileName("")
{

}

vpzVpm::vpzVpm(const QString& filename): mDoc(0), mFileName(filename)
{
}

QString
vpzVpm::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vpzVpm::xCreateDom()
{
    if (not mDoc) {
        mDoc = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDoc->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDoc->appendChild(pi);

        QDomElement vpmRoot = mDoc->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "me");
        QDomElement xCondPlug = mDoc->createElement("condPlugins");
        vpmRoot.appendChild(xCondPlug);
        mDoc->appendChild(vpmRoot);
    }
}

void
vpzVpm::xReadDom()
{
    if (mDoc) {
        QFile file(mFileName);
        mDoc->setContent(&file);
    }
}

void
vpzVpm::xSaveDom()
{
    if (mDoc) {
        QFile file(mFileName);
        file.open(QIODevice::Truncate | QIODevice::WriteOnly);
        QByteArray xml = mDoc->toByteArray();
        file.write(xml);
        file.close();
    }
}


void
vpzVpm::setCondPlugin(const QString& condName, const QString& name)
{
    xCreateDom();

    QDomElement docElem = mDoc->documentElement();

    QDomNode condsPlugins =
            mDoc->elementsByTagName("condPlugins").item(0);
    QDomNodeList plugins =
            condsPlugins.toElement().elementsByTagName("condPlugin");
    for (unsigned int i =0; i< plugins.length(); i++) {
        QDomNode plug = plugins.at(i);
        for (int j=0; j< plug.attributes().size(); j++) {
            if ((plug.attributes().item(j).nodeName() == "cond") and
                    (plug.attributes().item(j).nodeValue() == condName))  {
                plug.toElement().setAttribute("plugin", name);
                return;
            }
        }
    }
    QDomElement el = mDoc->createElement("condPlugin");
    el.setAttribute("cond", condName);
    el.setAttribute("plugin", name);
    condsPlugins.appendChild(el);
}

QString
vpzVpm::getCondPlugin(const QString& condName)
{

    QStringList res;
    if (mDoc) {
        QDomElement docElem = mDoc->documentElement();
        QDomNode condsPlugins =
                mDoc->elementsByTagName("condPlugins").item(0);
        QDomNodeList plugins =
                condsPlugins.toElement().elementsByTagName("condPlugin");
        for (unsigned int i =0; i< plugins.length(); i++) {
            QDomNode plug = plugins.item(i);
            if (plug.attributes().contains("cond") and
                (plug.attributes().namedItem("cond").nodeValue() == condName)) {
                return plug.attributes().namedItem("plugin").nodeValue();
            }
        }
    }
    return "";
}


}}//namespaces
