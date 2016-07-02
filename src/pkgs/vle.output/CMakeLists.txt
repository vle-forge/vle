include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})

link_directories(${VLEDEPS_LIBRARY_DIRS})

add_library(pkg_dummy MODULE Dummy.cpp)
add_library(pkg_file MODULE File.cpp FileType.cpp)
add_library(pkg_storage MODULE Storage.cpp)
add_library(pkg_console MODULE Console)

install(TARGETS pkg_dummy pkg_file pkg_storage pkg_console
  RUNTIME DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/output
  LIBRARY DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/output
  ARCHIVE DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output/plugins/output)

set_target_properties(pkg_dummy PROPERTIES
  OUTPUT_NAME dummy
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

set_target_properties(pkg_file PROPERTIES
  OUTPUT_NAME file
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

set_target_properties(pkg_storage PROPERTIES
  OUTPUT_NAME storage
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

set_target_properties(pkg_console PROPERTIES
  OUTPUT_NAME storage
  COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

install(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION lib/vle-${VLE_VERSION_SHORT}/pkgs/vle.output)
