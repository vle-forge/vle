add_subdirectory(vle)

if (WITH_CVLE)
  add_subdirectory(mvle)
endif ()

if (WITH_GVLE)
  add_subdirectory(gvle)
endif ()

