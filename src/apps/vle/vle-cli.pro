include(../../../defaults.pri)

CONFIG += c++14
CONFIG += console
CONFIG += thread
CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = app

TARGET = vle

SOURCES = main.cpp

mac {
  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  LIBS += -lxml2
  LIBS += -L../../../src -lvle-2.0
}

win32 {
}

unix {
  PKGCONFIG += libxml-2.0
  CONFIG += link_pkgconfig
  LIBS += -L../../../src -lvle-2.0
}

target.path = $$BINDIR

vleman.path = $$MANPATH
vleman.files += vle.1

INSTALLS += target vleman
