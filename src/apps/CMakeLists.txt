add_subdirectory(vle)
add_subdirectory(oov)

if (HAVE_GVLE)
  add_subdirectory(gvle)
endif (HAVE_GVLE)
