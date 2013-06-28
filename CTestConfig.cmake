set(CTEST_NIGHTLY_START_TIME "23:00:00 EDT")
if (CTEST_DROP_METHOD MATCHES http)
  set(CTEST_DROP_SITE "public.kitware.com")
  set(CTEST_DROP_LOCATION "/cgi-bin/HTTPUploadDartFile.cgi")
else (CTEST_DROP_METHOD MATCHES http)
  set(CTEST_DROP_SITE "public.kitware.com")
  set(CTEST_DROP_LOCATION "/incoming")
  set(CTEST_DROP_SITE_USER "ftpuser")
  set(CTEST_DROP_SITE_PASSWORD "public")
endif (CTEST_DROP_METHOD MATCHES http)

SET (CTEST_TRIGGER_SITE "http://${DROP_SITE}/cgi-bin/Submit-vtk-TestingResults.pl")

set(CTEST_SOURCE_DIRECTORY "/tmp/dashboards/vle/source")
set(CTEST_BINARY_DIRECTORY "/tmp/dashboards/vle/build")
set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

set(CTEST_PROJECT_NAME "vle-1.1")
set(CTEST_SITE "github.org/vle-forge")
set(CTEST_BUILD_NAME "debian-x86_64-sid-linux-gcc-4.8.1")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_CONFIGURATION "Profiling")
set(CTEST_BUILD_OPTIONS "-DWITH_GVLE=OFF")
set(WITH_MEMCHECK TRUE)
set(WITH_COVERAGE TRUE)

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

find_program(CTEST_GIT_COMMAND NAMES git)
find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)

set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE ${CTEST_SOURCE_DIRECTORY}/tests/valgrind.supp)

if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone git://github.com/vle-forge/vle.git ${CTEST_SOURCE_DIRECTORY}")
endif()

set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CTEST_BUILD_CONFIGURATION} \"-G${CTEST_CMAKE_GENERATOR}\" \"${CTEST_SOURCE_DIRECTORY}\"")

ctest_start("Nightly")
#ctest_update(RETURN_VALUE count)
#if (count GREATER 0)
ctest_update()
ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}")
ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}")
ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}")

if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
  ctest_coverage()
endif ()

if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
  ctest_memcheck()
endif ()

#ctest_submit()
#endif ()

# vim:tw=0:ts=8:tw=0:sw=2:sts=2
