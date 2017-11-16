add_subdirectory(vle.output)
add_subdirectory(vle.adaptative-qss)
add_subdirectory(vle.generic.builder)

if (VLE_HAVE_GVLE)
  add_subdirectory(gvle.default)
endif ()

add_subdirectory(test)
