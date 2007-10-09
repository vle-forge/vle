ADD_SUBDIRECTORY(geometry)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(value)
ADD_SUBDIRECTORY(graph)
ADD_SUBDIRECTORY(vpz)
ADD_SUBDIRECTORY(oov)
ADD_SUBDIRECTORY(devs)
ADD_SUBDIRECTORY(extension)
ADD_SUBDIRECTORY(manager)
ADD_SUBDIRECTORY(translator)

INSTALL(FILES data.hpp devs.hpp extension.hpp geometry.hpp graph.hpp
  manager.hpp oov.hpp translator.hpp utils.hpp value.hpp vpz.hpp DESTINATION
  ${VLE_INCLUDE_DIRS})
