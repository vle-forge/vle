include(../../../../defaults.pri)

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

TARGET = DefaultVpzPanel

FORMS = filevpzclasses.ui filevpzdynamics.ui filevpzexpcond.ui filevpzexpview.ui filevpzobservables.ui filevpzproject.ui filevpzrtool.ui filevpzsim.ui filevpztooltip.ui filevpzview.ui

HEADERS = DefaultVpzPanel.h filevpzdynamics.h filevpzexpview.h filevpzproject.h filevpzsim.h vpzDiagScene.h filevpzclasses.h filevpzexpcond.h filevpzobservables.h filevpzrtool.h filevpzview.h widgetvpzproperty.h

SOURCES = DefaultVpzPanel.cpp filevpzdynamics.cpp filevpzexpview.cpp  filevpzproject.cpp filevpzsim.cpp vpzDiagScene.cpp filevpzclasses.cpp filevpzexpcond.cpp filevpzobservables.cpp filevpzrtool.cpp filevpzview.cpp widgetvpzproperty.cpp

target.path = $$LIBSDIR/pkgs/gvle.default/plugins/gvle/vpz

INSTALLS += target

macx {
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include

  LIBS += -L../../../../src -lvle-2.0
  LIBS += -L../../../../src/vle/gvle -lgvle-2.0
}