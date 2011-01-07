INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR})

ADD_LIBRARY(dummy SHARED Dummy.cpp Dummy.hpp)
ADD_LIBRARY(file SHARED File.cpp File.hpp FileType.cpp FileType.hpp)
ADD_LIBRARY(storage SHARED Storage.cpp Storage.hpp)
ADD_LIBRARY(console SHARED Console.cpp)

TARGET_LINK_LIBRARIES(dummy
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov)

TARGET_LINK_LIBRARIES(file
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov)

TARGET_LINK_LIBRARIES(console
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov)

TARGET_LINK_LIBRARIES(storage
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  vlegeometry
  vleutils
  vlevalue
  vlegraph
  vleoov)

INSTALL(TARGETS dummy
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")

INSTALL(TARGETS file
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")

INSTALL(TARGETS storage
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")

INSTALL(TARGETS console
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")

INSTALL(FILES Storage.hpp DESTINATION ${VLE_INCLUDE_DIRS}/oov/plugins)

# TODO Add a default Cairo plugin to illustrate the use of the API.
#IF (VLE_HAVE_CAIRO)
#  ADD_SUBDIRECTORY(cairo)
#ENDIF (VLE_HAVE_CAIRO)
