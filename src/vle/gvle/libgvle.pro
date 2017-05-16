include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG -= app_bundle
CONFIG += qt
CONFIG += object_parallel_to_source
CONFIG += create_pc create_prl no_install_prl

QT += core
QT += gui
QT += widgets
QT += help
QT += xml

VERSION = 0

TEMPLATE = lib

macx {
  LIBS += -L../../../src -lvle-2.0
}

TARGET = gvle-$$VERSION_ABI

FORMS = aboutbox.ui gvle_win.ui help.ui

HEADERS = aboutbox.h gvle_file.h gvle_plugins.h gvle_widgets.h gvle_win.h \
    helpbrowser.h help.h logger.h plugin_cond.h plugin_mainpanel.h \
    plugin_output.h plugin_simpanel.h dom_tools.hpp vle_dom.hpp vlevpz.hpp

SOURCES = aboutbox.cpp  dom_tools.cpp  gvle_file.cpp  gvle_plugins.cpp \
    gvle_widgets.cpp  gvle_win.cpp  helpbrowser.cpp  help.cpp  logger.cpp \
    vle_dom.cpp vlevpz.cpp

target.path = $$LIBDIR

header_files_gvle.path = $$INCLUDEDIR/vle/gvle
header_files_gvle.files = gvle_widgets.h logger.h plugin_cond.h plugin_mainpanel.h plugin_simpanel.h plugin_output.h dom_tools.hpp vle_dom.hpp vlevpz.hpp gvle_plugins.h gvle_file.h

RESOURCES = gvle.qrc

QMAKE_PKGCONFIG_FILE = gvle-$${VERSION_ABI}
QMAKE_PKGCONFIG_NAME = gvle-$${VERSION_ABI}
QMAKE_PKGCONFIG_DESCRIPTION = GUI for VLE
QMAKE_PKGCONFIG_PREFIX = $$PREFIX
QMAKE_PKGCONFIG_LIBDIR = $$LIBDIR
QMAKE_PKGCONFIG_INCDIR = $$INCLUDEDIR
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5Gui Qt5Widgets Qt5Help Qt5Xml
QMAKE_PKGCONFIG_VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

INSTALLS += target header_files_gvle
