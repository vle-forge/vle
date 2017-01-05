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

TARGET = DefaultCppPanel

HEADERS = DefaultCppPanel.h

SOURCES = DefaultCppPanel.cpp

target.path = $$LIBSDIR/pkgs/gvle.default/plugins/gvle/modeling

INSTALLS += target
