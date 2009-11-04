INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${GLADEMM_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${GLADEMM_LIBRARY_DIRS}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS})

ADD_LIBRARY(input-simple MODULE Simple.hpp Simple.cpp
  NameValue.cpp NameValue.hpp Mapping.hpp Mapping.cpp
  TimeStep.hpp TimeStep.cpp)
SET_TARGET_PROPERTIES(input-simple PROPERTIES OUTPUT_NAME
  "DifferenceEquationSimple")

ADD_LIBRARY(input-multiple MODULE Multiple.hpp Multiple.cpp
  Mapping.hpp Mapping.cpp Variables.hpp Variables.cpp
  TimeStep.hpp TimeStep.cpp)
SET_TARGET_PROPERTIES(input-multiple PROPERTIES OUTPUT_NAME
  "DifferenceEquationMultiple")

ADD_LIBRARY(input-generic MODULE Generic.hpp Generic.cpp
  NameValue.cpp NameValue.hpp TimeStep.hpp TimeStep.cpp)
SET_TARGET_PROPERTIES(input-generic PROPERTIES OUTPUT_NAME
  "DifferenceEquationGeneric")

TARGET_LINK_LIBRARIES(input-simple
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
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

TARGET_LINK_LIBRARIES(input-multiple
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
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

TARGET_LINK_LIBRARIES(input-generic
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
  ${GLADEMM_LIBRARIES}
  ${GTKMM_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES}
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

INSTALL(TARGETS input-simple
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/conditions"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/conditions")

INSTALL(TARGETS input-multiple
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/conditions"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/conditions")

INSTALL(TARGETS input-generic
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/conditions"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/conditions")

INSTALL(FILES DifferenceEquation.glade DESTINATION
  ${VLE_SHARE_DIRS}/conditions/glade)
