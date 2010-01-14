INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${GLADEMM_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${GLADEMM_LIBRARY_DIRS}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

ADD_LIBRARY(modeling-DE-simple MODULE Simple.hpp Simple.cpp Plugin.hpp
  Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp SourceDialog.cpp
  NameValue.hpp NameValue.cpp TimeStep.hpp TimeStep.cpp Mapping.hpp
  Mapping.cpp)
SET_TARGET_PROPERTIES(modeling-DE-simple PROPERTIES OUTPUT_NAME
  "DifferenceEquationSimple")

TARGET_LINK_LIBRARIES(modeling-DE-simple
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
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

INSTALL(TARGETS modeling-DE-simple
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/modeling"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/modeling")

ADD_LIBRARY(modeling-DE-generic MODULE Generic.hpp Generic.cpp Plugin.hpp
  Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp SourceDialog.cpp
  NameValue.hpp NameValue.cpp TimeStep.hpp TimeStep.cpp Mapping.hpp
  Mapping.cpp)
SET_TARGET_PROPERTIES(modeling-DE-generic PROPERTIES OUTPUT_NAME
  "DifferenceEquationGeneric")

TARGET_LINK_LIBRARIES(modeling-DE-generic
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
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

INSTALL(TARGETS modeling-DE-generic
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/modeling"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/modeling")

ADD_LIBRARY(modeling-DE-multiple MODULE Multiple.hpp Multiple.cpp Plugin.hpp
  Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp SourceDialog.cpp
  Mapping.hpp Mapping.cpp TimeStep.hpp TimeStep.cpp Variables.hpp
  Variables.cpp)
SET_TARGET_PROPERTIES(modeling-DE-multiple PROPERTIES OUTPUT_NAME
  "DifferenceEquationMultiple")

TARGET_LINK_LIBRARIES(modeling-DE-multiple
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
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

INSTALL(TARGETS modeling-DE-multiple
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/modeling"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/modeling")

INSTALL(FILES DifferenceEquation.glade DESTINATION
  ${VLE_SHARE_DIRS}/modeling/glade)
