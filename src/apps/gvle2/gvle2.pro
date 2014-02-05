#
# @file gvle2/gvle2.pro
#
# This file is part of VLE, a framework for multi-modeling, simulation
# and analysis of complex dynamical systems
# http://www.vle-project.org
#
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gvle2
TEMPLATE = app

SOURCES += main.cpp     \
           gvle2win.cpp \
    aboutbox.cpp \
    widgetprojecttree.cpp \
    filevpzview.cpp \
    logger.cpp \
    help.cpp \
    simulation.cpp

HEADERS  += gvle2win.h \
    aboutbox.h \
    widgetprojecttree.h \
    filevpzview.h \
    logger.h \
    help.h \
    simulation.h \
    plugin_sim.h

INCLUDEPATH += /opt/vle-1.c/include/vle-1.1/  \
               /opt/qwt-6.0.0/include/    \
               /usr/include/glib-2.0      \
               /usr/lib/glib-2.0/include/ \
               /usr/include/libxml2/

LIBS += -L/opt/vle-1.c/lib -lvle-1.1
#-L/opt/qwt-6.0.0/lib -lqwt

FORMS    += gvle2win.ui \
    aboutbox.ui \
    help.ui \
    filevpzview.ui \
    simulationview.ui \
    simulationrtool.ui

TRANSLATIONS = languages/tr_en.ts \
               languages/tr_fr.ts

RESOURCES += \
    gvle2.qrc
