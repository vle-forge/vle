include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

link_directories(
  ${VLE_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS})

add_library(Simple SHARED Simple.cpp)

target_link_libraries(Simple
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

install(TARGETS Simple
  RUNTIME DESTINATION plugins/simulator
  LIBRARY DESTINATION plugins/simulator
  ARCHIVE DESTINATION plugins/simulator)

