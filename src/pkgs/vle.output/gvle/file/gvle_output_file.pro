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

TARGET = gvleOutputFile

FORMS = FilePluginGvle.ui

SOURCES = FilePluginGUI.cpp FilePluginGUItab.cpp

HEADERS = FilePluginGUI.h  FilePluginGUItab.h

target.path = $$LIBDIR/pkgs/vle.output/plugins/gvle/output

INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../../../src/release/vle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../../../src/debug/vle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../../../src/vle/gvle/release/gvle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../../../src/vle/gvle/debug/gvle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

DEPENDPATH += $$PWD/../../../src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../src/release/libvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../src/debug/libvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../src/vle/gvle/release/libgvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../src/vle/gvle/debug/libgvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

macx {
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include
}

win32 {
  QMAKE_INCDIR += d:\msys64\mingw32\include
}
