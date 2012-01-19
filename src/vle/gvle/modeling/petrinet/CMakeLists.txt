INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS}
  ${GTHREAD_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS})

ADD_LIBRARY(modeling-petrinet SHARED
  ArcDialog.hpp ArcDialog.cpp
  Plugin.hpp Plugin.cpp
  PetriNet.hpp PetriNet.cpp
  PetriNetDrawingArea.hpp PetriNetDrawingArea.cpp
  PlaceDialog.hpp PlaceDialog.cpp
  TransitionDialog.hpp TransitionDialog.cpp
)

SET_TARGET_PROPERTIES(modeling-petrinet PROPERTIES OUTPUT_NAME
  "PetriNet")

TARGET_LINK_LIBRARIES(modeling-petrinet
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${CAIROMM_LIBRARIES}
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

INSTALL(TARGETS modeling-petrinet
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/modeling"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/modeling")

INSTALL(FILES PetriNet.glade DESTINATION
  ${VLE_SHARE_DIRS}/modeling/glade)
