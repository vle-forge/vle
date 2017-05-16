include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = console

SOURCES = Console.cpp

target.path = $$LIBSDIR/pkgs/vle.output/plugins/output

other.path = $$LIBSDIR/pkgs/vle.output
other.files = Authors.txt Description.txt License.txt News.txt Readme.txt

INSTALLS += target other

macx {
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
  LIBS += -L../../../src -lvle-2.0
}
