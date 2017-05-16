include(../../../defaults.pri)

CONFIG += c++14
CONFIG += console
CONFIG += thread
CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = app

TARGET = vle-$$VERSION_ABI

VERSION = 0

SOURCES = main.cpp

macx {
  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
  LIBS += -lxml2
  LIBS += -L../../../src -lvle-2.0
}

win32 {
}

unix:!macx {
  PKGCONFIG += libxml-2.0
  CONFIG += link_pkgconfig
  LIBS += -L../../../src -lvle-2.0
}

target.path = $$BINDIR

vleman.path = $$MANPATH
vleman.files += vle.1

INSTALLS += target vleman
