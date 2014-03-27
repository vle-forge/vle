#! /usr/bin/python3
#
# Copyright (C) 2014 INRA
# Gauthier Quesnel <gauthier.quesnel@toulouse.inra.fr>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys
from xml.dom import minidom, Node
from xml.dom.minidom import DOMImplementation

def remove_blanks(node):
    for x in node.childNodes:
        if x.nodeType == Node.TEXT_NODE:
            if x.nodeValue:
                x.nodeValue = x.nodeValue.strip()
        elif x.nodeType == Node.ELEMENT_NODE:
            remove_blanks(x)

def run(arg):
    xmldoc = minidom.parse(arg)
    remove_blanks(xmldoc)
    xmldoc.normalize()

    vle_project = xmldoc.getElementsByTagName('vle_project')
    experiment = vle_project[0].getElementsByTagName('experiment')
    conditions = vle_project[0].getElementsByTagName('conditions')

    if not vle_project.length > 0 or not experiment.length > 0:
        print(' -> Not a VLE VPZ file?')
        return

    if not experiment[0].hasAttribute('begin') or \
            not experiment[0].hasAttribute('duration'):
        print(' -> Already a VLE 1.2 file?')
        return

    # get begin and duration tags data and remove them from the experiment
    # element
    begintag = experiment[0].getAttribute('begin')
    durationtag = experiment[0].getAttribute('duration')
    experiment[0].removeAttribute('begin')
    experiment[0].removeAttribute('duration')

    # build the new simulation_engine' condition.
    newcondition = xmldoc.createElement('condition')
    newcondition.setAttribute('name', 'simulation_engine')
    conditions[0].appendChild(newcondition)

    # build the condition's port begin.
    port = xmldoc.createElement('port')
    port.setAttribute('name', 'begin');
    newcondition.appendChild(port)
    double = xmldoc.createElement('double')
    textnode = xmldoc.createTextNode(begintag)
    double.appendChild(textnode)
    port.appendChild(double)

    # build the conditions's port duration.
    port = xmldoc.createElement('port')
    port.setAttribute('name', 'duration');
    newcondition.appendChild(port)
    double = xmldoc.createElement('double')
    textnode = xmldoc.createTextNode(durationtag)
    double.appendChild(textnode)
    port.appendChild(double)

    imp = DOMImplementation()
    doctype = imp.createDocumentType(  
            qualifiedName='vle_project',  
            publicId='-//VLE TEAM//DTD Strict//EN',  
            systemId='http://www.vle-project.org/vle-1.2.0.dtd')

    xmldoc.replaceChild(doctype, xmldoc.doctype)

    with file(arg, 'w') as result:
        result.write(xmldoc.toprettyxml(indent = ' '))

def main():
    args = sys.argv[1:]
    if len(args) == 0:
        print('Argument are missing')
        print('\n   Usage: convert-vpz11-vpz12.py [[file.vpz] ...]')
        print('          convert-vpz11-vpz12.py \'**/*\'.vpz\n')
    else:
        for arg in args:
            try:
                print('Convert [%s]') % arg
                run(arg)
            except Exception as e:
                print(e)

if __name__ == '__main__':
    main()
