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

TARGET = DefaultOutPanel

HEADERS = DefaultOutPanel.h

SOURCES = DefaultOutPanel.cpp

target.path = $$LIBSDIR/pkgs/gvle.default/plugins/gvle/out

INSTALLS += target
