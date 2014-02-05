add_subdirectory(vle)

if (VLE_HAVE_MPI)
  add_subdirectory(mvle)
endif ()

if (VLE_HAVE_GVLE)
  add_subdirectory(gvle)
endif ()

if (VLE_HAVE_GVLE2)
  add_subdirectory(gvle2)
endif ()
