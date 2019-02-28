find_package(QCustomPlot)

if (NOT QCustomPlot_FOUND)
  message("Plugin DefaultsimSubpanel disabled. Please insteall QCustomplot package.")
else ()
  DeclareGvle(simulating DefaultSimSubpanel gvle.default DefaultSimSubpanel
    DefaultSimSubpanel.cpp DefaultSimSubpanel.h simpanelleft.ui simpanelright.ui)
endif ()
