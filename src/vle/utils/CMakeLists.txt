INCLUDE_DIRECTORIES(
  ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXML_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS})

IF(WIN32)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  SET(OS_SPECIFIC_PATH_IMPL PathWin32.cpp ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
ELSE(WIN32)
  SET(OS_SPECIFIC_PATH_IMPL PathUnix.cpp)
ENDIF(WIN32)

SET(UTILS_CPP DateTime.cpp DownloadManager.cpp Host.cpp ModuleManager.cpp
  Package.cpp PackageTable.cpp Parser.cpp Path.cpp Preferences.cpp Rand.cpp
  RemoteManager.cpp SharedLibraryManager.cpp Socket.cpp SocketImpl.c
  Template.cpp Tools.cpp Trace.cpp ${OS_SPECIFIC_PATH_IMPL})

SET(UTILS_HPP Algo.hpp DateTime.hpp Deprecated.hpp DllDefines.hpp
  DownloadManager.hpp Exception.hpp Host.hpp i18n.hpp ModuleManager.hpp
  Package.hpp PackageTable.hpp Parser.hpp Path.hpp Preferences.hpp Rand.hpp
  RemoteManager.hpp SharedLibraryManager.hpp Socket.hpp SocketImpl.h
  Template.hpp Tools.hpp Trace.hpp Types.hpp)

ADD_LIBRARY(vleutils SHARED ${UTILS_CPP};${UTILS_HPP})

SET_TARGET_PROPERTIES(vleutils PROPERTIES ${VLE_LIBRARY_PROPERTIES})
SET_TARGET_PROPERTIES(vleutils PROPERTIES OUTPUT_NAME
  "vleutils-${VLE_VERSION_SHORT}")

IF (NOT WIN32)
  SET(OS_SPECIFIC_LIBRARIES ${OS_SPECIFIC_LIBRARIES} dl)
ENDIF (NOT WIN32)

TARGET_LINK_LIBRARIES(vleutils
  ${GLIBMM_LIBRARIES}
  ${LIBXML_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${MODULE_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${OS_SPECIFIC_LIBRARIES})

INSTALL(TARGETS vleutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${UTILS_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/utils)

IF (HAVE_UNITTESTFRAMEWORK)
  ADD_SUBDIRECTORY(test)
ENDIF (HAVE_UNITTESTFRAMEWORK)
