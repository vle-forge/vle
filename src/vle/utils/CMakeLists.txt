add_subdirectory(details)

if (WIN32)
  set(UTILS_SPECIFIC_PATH_IMPL PathWin32.cpp)
else (WIN32)
  SET(UTILS_SPECIFIC_PATH_IMPL PathUnix.cpp)
endif (WIN32)

add_sources(vlelib Algo.hpp DateTime.cpp DateTime.hpp Deprecated.hpp
  DownloadManager.cpp DownloadManager.hpp Exception.hpp i18n.hpp
  ModuleManager.cpp ModuleManager.hpp Package.cpp Package.hpp
  PackageTable.cpp PackageTable.hpp Parser.cpp Parser.hpp Path.cpp
  Path.hpp ${UTILS_SPECIFIC_PATH_IMPL} Preferences.cpp Preferences.hpp
  Rand.cpp Rand.hpp RemoteManager.cpp RemoteManager.hpp Spawn.hpp
  Template.cpp Template.hpp Tools.cpp Tools.hpp Trace.cpp Trace.hpp
  Types.hpp)

install(FILES Algo.hpp DateTime.hpp Deprecated.hpp DownloadManager.hpp
  Exception.hpp i18n.hpp ModuleManager.hpp Package.hpp PackageTable.hpp
  Parser.hpp Path.hpp Preferences.hpp Rand.hpp RemoteManager.hpp
  Spawn.hpp Template.hpp Tools.hpp Trace.hpp Types.hpp DESTINATION
  ${VLE_INCLUDE_DIRS}/utils)

if (VLE_HAVE_UNITTESTFRAMEWORK)
  add_subdirectory(test)
endif ()
