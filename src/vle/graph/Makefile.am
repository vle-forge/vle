AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libgraph_coreheaders = AtomicModel.hpp Connection.hpp CoupledModel.hpp \
			 graph.hpp Model.hpp NoVLEModel.hpp Port.hpp \
			 TargetPort.hpp

libgraph_coresources = AtomicModel.cpp Connection.cpp CoupledModel.cpp \
		       Model.cpp NoVLEModel.cpp Port.cpp TargetPort.cpp

libvle_LTLIBRARIES = libgraph.la
libgraph_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libgraph_la_SOURCES = $(libgraph_coreheaders) $(libgraph_coresources)
libgraphincludedir = $(vleincludedir)/vle/graph
libgraphinclude_HEADERS = $(libgraph_coreheaders)



if PLATFORM_WIN32
libgraph_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		     $(top_srcdir)/src/vle/utils/libutils.la
endif
