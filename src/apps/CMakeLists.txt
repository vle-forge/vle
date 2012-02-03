add_subdirectory(vle)
add_subdirectory(oov)

if (HAVE_EOV)
  add_subdirectory(eov)
endif (HAVE_EOV)

if (HAVE_GVLE)
  add_subdirectory(gvle)
endif (HAVE_GVLE)
