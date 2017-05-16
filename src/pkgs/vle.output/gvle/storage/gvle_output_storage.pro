include(../../../../../defaults.pri)

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

TEMPLATE = lib

TARGET = gvleOutputStorage

FORMS = StoragePluginGvle.ui

SOURCES = StoragePluginGUI.cpp  StoragePluginGUItab.cpp

HEADERS = StoragePluginGUI.h  StoragePluginGUItab.h

target.path = $$LIBSDIR/pkgs/vle.output/plugins/gvle/output

INSTALLS += target

macx {
  LIBS += -L../../../../../src -lvle-2.0
  LIBS += -L../../../../../src/vle/gvle -lgvle-2.0
}