AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @THREAD_CFLAGS@

libvle_coresources = ExperimentGenerator.cpp Manager.cpp \
		     TotalExperimentGenerator.cpp Simulator.cpp VLE.cpp

libvle_coreheaders = ExperimentGenerator.hpp Manager.hpp \
		     TotalExperimentGenerator.hpp Simulator.hpp VLE.hpp

libvle_LTLIBRARIES = libmanager.la
libmanager_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libmanager_la_SOURCES = $(libvle_coresources) $(libvle_coreheaders)
vlemanagerincludedir = $(vleincludedir)/vle/manager
vlemanagerinclude_HEADERS = $(libvle_coreheaders)



if PLATFORM_WIN32
libmanager_la_LIBADD = @VLE_vle@ @XML_vle@ @THREAD_vle@ \
		       $(top_srcdir)/src/vle/devs/libdevs.la \
		       $(top_srcdir)/src/vle/utils/libutils.la \
		       $(top_srcdir)/src/vle/value/libvalue.la \
		       $(top_srcdir)/src/vle/vpz/libvpz.la \
		       $(top_srcdir)/src/vle/graph/libgraph.la \
		       $(top_srcdir)/src/vle/value/libvalue.la \
		       $(top_srcdir)/src/vle/geometry/libgeometry.la \
		       $(top_srcdir)/src/vle/extension/libextension.la
endif

