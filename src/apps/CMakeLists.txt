add_subdirectory(vle)

if (VLE_HAVE_MPI)
  add_subdirectory(mvle)
endif ()