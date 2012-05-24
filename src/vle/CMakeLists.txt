include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${GLIBMM_INCLUDE_DIRS} ${GLADEMM_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS} ${GTKSOURCEVIEWMM_INCLUDE_DIRS}
  ${GTKMM_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})

link_directories(${GLADEMM_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS}
  ${GTHREAD_LIBRARY_DIRS} ${LIBXML_LIBRARY_DIR}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS} ${GLADEMM_LIBRARY_DIRS})

#
# Explore subfolders to find source code, plug-ins etc.
#

add_subdirectory(utils)
add_subdirectory(value)
add_subdirectory(oov)
add_subdirectory(vpz)
add_subdirectory(devs)
add_subdirectory(manager)
add_subdirectory(translator)

add_sources(vlelib DllDefines.hpp vle.hpp vle.cpp)

if (HAVE_GVLE)
  include_directories(${CAIROMM_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS}
    ${GLADEMM_INCLUDE_DIRS})
  add_subdirectory(gvle)
endif (HAVE_GVLE)

get_property(vlelib_SRCS GLOBAL PROPERTY vlelib_SRCS)

#
# Assign variable for the portabily
#

if (WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vfl.o
    COMMAND ${CMAKE_RC_COMPILER}
    -I${CMAKE_BINARY_DIR}/share
    -i${CMAKE_BINARY_DIR}/share/vfl.rc
    -o${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_PATH_IMPL ${CMAKE_CURRENT_BINARY_DIR}/vfl.o)
  set(OS_SPECIFIC_LIBRARIES dl -shared-libgcc wsock32 netapi32)
else (WIN32)
  set(OS_SPECIFIC_PATH_IMPL)
  set(OS_SPECIFIC_LIBRARIES dl)
endif (WIN32)

#
# Build the shared or static library. If user assigns BUILD_SHARED_LIBS to ON,
# then, we must define VLE_DLL to use correctly the DllDefines.hpp
#

add_library(vlelib
  DllDefines.hpp;${vlelib_SRCS};${OS_SPECIFIC_PATH_IMPL})

if (BUILD_SHARED_LIBS)
  set_target_properties(vlelib PROPERTIES DEFINE_SYMBOL
    "VLE_DLL")

  set_target_properties(vlelib PROPERTIES COMPILE_FLAGS
    "-fvisibility=hidden -fvisibility-inlines-hidden")
endif ()

target_link_libraries(vlelib ${LIBXML2_LIBRARIES} ${GLIBMM_LIBRARIES}
  ${GTKMM_LIBRARIES} ${LIBXML_LIBRARIES} ${GTHREAD_LIBRARIES}
  ${OS_SPECIFIC_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY}
  ${Boost_REGEX_LIBRARY} ${Boost_THREAD_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY}
  ${CAIROMM_LIBRARIES} ${GTKSOURCEVIEWMM_LIBRARIES}
  ${GLADEMM_LIBRARIES} ${CURL_LIBRARIES})

#
# Section to assign a version and a soversion to the vlelib
# target. Change the name of the filename build by CMake to correspond
# to vle-x-y.so.x.0. Then install the library into classic paths.
#

set_target_properties(vlelib PROPERTIES VERSION 0)

set_target_properties(vlelib PROPERTIES OUTPUT_NAME
  "vle-${VLE_VERSION_SHORT}")

install(TARGETS vlelib RUNTIME DESTINATION bin LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

#
# Configure the version.h.in file to produce in the binary directory a
# filled header version.hpp.
#

configure_file(version.hpp.in ${VLE_BINARY_DIR}/src/vle/version.hpp)

install(FILES ${VLE_BINARY_DIR}/src/vle/version.hpp DESTINATION
  ${VLE_INCLUDE_DIRS})

install(FILES DllDefines.hpp DESTINATION ${VLE_INCLUDE_DIRS})

#
# Configure the file vle.pc.in to produce in the binary directory a
# filled pkg-config script vle.pc.
#

if (WIN32)
  set(VLE_PKGCONFIG_PREFIXPATH "/target")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "\${includedir}")
  set(VLE_PKGCONFIG_LIBDIR "bin")
else()
  set(VLE_PKGCONFIG_PREFIXPATH "${CMAKE_INSTALL_PREFIX}")
  set(VLE_PKGCONFIG_BOOSTINCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
  set(VLE_PKGCONFIG_LIBDIR "lib")
endif()

configure_file(vle.pc.in
  ${VLE_BINARY_DIR}/src/vle/vle-${VLE_VERSION_SHORT}.pc @ONLY)

install(FILES ${VLE_BINARY_DIR}/src/vle/vle-${VLE_VERSION_SHORT}.pc
  DESTINATION lib/pkgconfig)

install(FILES DllDefines.hpp DESTINATION ${VLE_INCLUDE_DIRS})