include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

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

if (VLE_HAVE_GVLE)
  add_subdirectory(gvle)
endif ()

add_sources(vlelib DllDefines.hpp vle.hpp vle.cpp)
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
  # set(OS_SPECIFIC_LIBRARIES ws2_32)
else (WIN32)
  set(OS_SPECIFIC_PATH_IMPL)
  set(OS_SPECIFIC_LIBRARIES dl)
endif (WIN32)

#
# Build the shared library.
#

add_library(vlelib SHARED
  DllDefines.hpp;${vlelib_SRCS};${OS_SPECIFIC_PATH_IMPL})

set_target_properties(vlelib PROPERTIES
  VERSION 0
  OUTPUT_NAME "vle-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "vlelib_EXPORTS"
  COMPILE_DEFINITIONS "VLE_DLL"
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

target_compile_definitions(vlelib
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
  $<$<BOOL:${WITH_DEBUG}>:VLE_DISABLE_DEBUG>
  VERSION_MAJOR=${VLE_MAJOR}
  VERSION_MINOR=${VLE_MINOR}
  VERSION_PATCH=${VLE_PATCH}
  VERSION_EXTRA=${VLE_EXTRA})

target_include_directories(vlelib PUBLIC ${VLEDEPS_INCLUDE_DIRS})

target_link_libraries(vlelib ${VLEDEPS_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT} ${OS_SPECIFIC_LIBRARIES})

install(TARGETS vlelib RUNTIME DESTINATION bin LIBRARY DESTINATION
  lib ARCHIVE DESTINATION lib)

#
# Build the static library.
#

add_library(vlelibstatic STATIC
  DllDefines.hpp;${vlelib_SRCS};${OS_SPECIFIC_PATH_IMPL})

set_target_properties(vlelibstatic PROPERTIES
  VERSION 0
  OUTPUT_NAME "vle-${VLE_VERSION_SHORT}"
  DEFINE_SYMBOL "vlelib_EXPORTS"
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden"
  CLEAN_DIRECT_OUTPUT 1)

target_compile_definitions(vlelibstatic
  PRIVATE
  $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
  $<$<BOOL:${WITH_DEBUG}>:VLE_DISABLE_DEBUG>
  VERSION_MAJOR=${VLE_MAJOR}
  VERSION_MINOR=${VLE_MINOR}
  VERSION_PATCH=${VLE_PATCH}
  VERSION_EXTRA=${VLE_EXTRA})

install(TARGETS vlelibstatic RUNTIME DESTINATION bin LIBRARY
  DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES vle.hpp DllDefines.hpp DESTINATION ${VLE_INCLUDE_DIRS})

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
