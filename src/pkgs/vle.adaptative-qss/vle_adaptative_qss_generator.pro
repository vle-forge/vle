include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = Generator

SOURCES = src/Generator.cpp

target.path = $$LIBSDIR/pkgs/vle.adaptative-qss/plugins/simulator

INSTALLS += target

macx {
  LIBS += -L../../../src -lvle-2.0
}