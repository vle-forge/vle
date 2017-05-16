include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = Builder

SOURCES = src/Builder.cpp

target.path = $$LIBSDIR/pkgs/vle.generic.builder/plugins/simulator

other.path = $$LIBSDIR/pkgs/vle.generic.builder
other.files = Authors.txt Description.txt License.txt News.txt Readme.txt

INSTALLS += target other

macx {
  LIBS += -L../../../src -lvle-2.0
}