include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = storage

SOURCES = Storage.cpp

target.path = $$LIBDIR/pkgs/vle.output/plugins/output

INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../src/release/vle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../src/debug/vle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

DEPENDPATH += $$PWD/../../../src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/release/libvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/debug/libvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

macx {
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
}

win32 {
  QMAKE_INCDIR += d:\msys64\mingw32\include
}
