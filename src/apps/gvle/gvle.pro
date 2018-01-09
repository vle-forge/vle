include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG -= app_bundle
CONFIG += qt

QT += core
QT += gui
QT += widgets
QT += help
QT += xml

win32:VERSION = $$VERSION_MAJOR.$$VERSION_MINOR.$$VERSION_PATCH.0
else:VERSION = $$VERSION_MAJOR.$$VERSION_MINOR.$$VERSION_PATCH

TEMPLATE = app

TARGET = gvle-$$VERSION_ABI

SOURCES = main.cpp

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../src/release/vle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../src/debug/vle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

win32:CONFIG(release, debug|release): LIBS += $$OUT_PWD/../../../src/vle/gvle/release/gvle-2.0.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$OUT_PWD/../../../src/vle/gvle/debug/gvle-2.0.dll
else:unix: LIBS += -L$$OUT_PWD/../../../src/ -lvle-2.0

DEPENDPATH += $$PWD/../../../src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/release/libvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/debug/libvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/vle/gvle/release/libgvle-2.0.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../src/vle/gvle/debug/libgvle-2.0.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../src/libvle-2.0.a

macx {
  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  LIBS += -lxml2
  LIBS += -L../../../src -lvle-2.0
  LIBS += -L../../../src/vle/gvle -lgvle-2.0
}

win32 {
}

unix:!macx {
  PKGCONFIG += libxml-2.0
  CONFIG += link_pkgconfig
  LIBS += -L../../../src -lvle-2.0
  LIBS += -L../../../src/vle/gvle -lgvle-2.0
}

target.path = $$BINDIR

INSTALLS += target
