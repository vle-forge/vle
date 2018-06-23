include(../../../defaults.pri)

CONFIG += c++14
CONFIG += console
CONFIG += thread
CONFIG -= app_bundle
CONFIG -= qt

win32:VERSION = $$VERSION_MAJOR.$$VERSION_MINOR.$$VERSION_PATCH.0
else:VERSION = $$VERSION_MAJOR.$$VERSION_MINOR.$$VERSION_PATCH

TEMPLATE = app

TARGET = vle-$$VERSION_ABI

SOURCES = main.cpp

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../src/release/vle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../src/debug/vle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

DEPENDPATH += $$PWD/../../../src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/release/libvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/debug/libvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

macx {
  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
  LIBS += -lxml2
  LIBS += -L../../../src -lvle-2.0
}

win32 {
  QMAKE_INCDIR += d:\msys64\mingw32\include
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

HEADERS += \
    conditionupdater.hpp
