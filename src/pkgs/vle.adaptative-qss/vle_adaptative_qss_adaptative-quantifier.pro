include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = AdaptativeQuantifier

SOURCES = src/AdaptativeQuantifier.cpp

target.path = $$LIBSDIR/pkgs/vle.adaptative-qss/plugins/simulator

other.path = $$LIBSDIR/pkgs/vle.adaptative-qss
other.files = Authors.txt Description.txt License.txt News.txt Readme.txt

INSTALLS += target other

macx {
  LIBS += -L../../../src -lvle-2.0
}