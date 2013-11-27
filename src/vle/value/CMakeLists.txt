add_sources(vlelib Boolean.cpp Boolean.hpp Double.cpp Double.hpp
  Integer.cpp Integer.hpp Map.cpp Map.hpp Matrix.cpp Matrix.hpp
  Null.cpp Null.hpp Set.cpp Set.hpp String.cpp String.hpp Table.cpp
  Table.hpp Tuple.cpp Tuple.hpp User.hpp Value.cpp Value.hpp XML.cpp
  XML.hpp)

install(FILES Boolean.hpp Double.hpp Integer.hpp Map.hpp Matrix.hpp
  Null.hpp Set.hpp String.hpp Table.hpp Tuple.hpp User.hpp Value.hpp
  XML.hpp DESTINATION ${VLE_INCLUDE_DIRS}/value)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
