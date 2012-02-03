ADD_LIBRARY(gtkdefault MODULE GtkDefault.hpp GtkDefault.cpp)

TARGET_LINK_LIBRARIES(gtkdefault vlelib)

INSTALL(TARGETS gtkdefault
  RUNTIME DESTINATION "${VLE_LIBRARY_DIRS}/stream"
  LIBRARY DESTINATION "${VLE_LIBRARY_DIRS}/stream")
