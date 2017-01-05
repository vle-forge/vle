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

TARGET = DefaultDataPanel

HEADERS = DefaultDataPanel.h

SOURCES = DefaultDataPanel.cpp

target.path = $$LIBSDIR/pkgs/gvle.default/plugins/gvle/data

INSTALLS += target
