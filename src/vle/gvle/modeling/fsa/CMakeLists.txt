INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

ADD_LIBRARY(modeling-fsa-statechart SHARED Plugin.hpp
  Plugin.cpp SourceDialog.hpp SourceDialog.cpp
  Statechart.hpp Statechart.cpp
  StatechartDrawingArea.hpp StatechartDrawingArea.cpp
  StateDialog.hpp StateDialog.cpp
  TimeStepDialog.hpp TimeStepDialog.cpp
  TransitionDialog.hpp TransitionDialog.cpp)

SET_TARGET_PROPERTIES(modeling-fsa-statechart PROPERTIES OUTPUT_NAME
  "Statechart")

TARGET_LINK_LIBRARIES(modeling-fsa-statechart
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
  ${GTKSOURCEVIEWMM_LIBRARIES}
  vlemanager
  vleutils
  vlevalue
  vlegraph
  vlevpz
  vleoov
  vlevpz
  vledevs
  vleextension
  vlemanager
  vletranslator
  vlegvle)

INSTALL(TARGETS modeling-fsa-statechart
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/modeling"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/modeling")

INSTALL(FILES FSA.glade DESTINATION
  ${VLE_SHARE_DIRS}/modeling/glade)
