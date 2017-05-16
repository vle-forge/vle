include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = dummy

SOURCES = Dummy.cpp

target.path = $$LIBSDIR/pkgs/vle.output/plugins/output

INSTALLS += target

macx {
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
  LIBS += -L../../../src -lvle-2.0
}

