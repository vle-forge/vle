INCLUDE_DIRECTORIES(
  ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${GLIBMM_INCLUDE_DIRS}
  ${LIBXMLPP_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${GLIBMM_LIBRARY_DIRS}
  ${LIBXMLPP_LIBRARY_DIR})

IF (HAVE_SQLXX)
  SET(DATA_CPP CSVReader.cpp CSVResult.cpp DataReader.cpp DataWriter.cpp
    SQLReader.cpp SQLResult.cpp SQLWriter.cpp)
  SET(DATA_HPP CSVReader.hpp CSVResult.hpp DataReader.hpp DataWriter.hpp
    Result.hpp SQLReader.hpp SQLResult.hpp SQLWriter.hpp)
ELSE (HAVE_SQLXX)
  SET(DATA_CPP CSVReader.cpp CSVResult.cpp DataReader.cpp DataWriter.cpp)
  SET(DATA_HPP CSVReader.hpp CSVResult.hpp DataReader.hpp DataWriter.hpp
    Result.hpp)
ENDIF (HAVE_SQLXX)

ADD_LIBRARY(vledata SHARED ${DATA_HPP};${DATA_CPP})

TARGET_LINK_LIBRARIES(vledata
  ${GLIBMM_LIBRARIES}
  ${LIBXMLPP_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${LIBSQLXX_LIBRARIES}
  vlegeometry
  vleutils)

INSTALL(TARGETS vledata
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(FILES ${DATA_HPP} DESTINATION ${VLE_INCLUDE_DIRS}/data)
