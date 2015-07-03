add_subdirectory(vle)

if (VLE_HAVE_MVLE OR VLE_HAVE_CVLE)
  add_subdirectory(mvle)
endif ()

if (VLE_HAVE_GVLE)
  add_subdirectory(gvle)
endif ()

