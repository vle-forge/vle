include(../../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG += qt
CONFIG += object_parallel_to_source

QT += core
QT += gui
QT += widgets
QT += help
QT += xml
QT += printsupport

TEMPLATE = lib

TARGET = DefaultSimSubpanel

FORMS = simpanelleft.ui simpanelright.ui

HEADERS = DefaultSimSubpanel.h qcustomplot/qcustomplot.h

SOURCES = DefaultSimSubpanel.cpp qcustomplot/qcustomplot.cpp

target.path = $$LIBSDIR/pkgs/gvle.default/plugins/gvle/simulating

INSTALLS += target

macx {
  LIBS += -L../../../../src -lvle-2.0
  LIBS += -L../../../../src/vle/gvle -lgvle-2.0
}