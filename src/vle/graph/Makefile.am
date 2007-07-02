AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libgraph_coreheaders = AtomicModel.hpp CoupledModel.hpp \
		       graph.hpp Model.hpp NoVLEModel.hpp \
		       ModelPort.hpp ModelPortList.hpp
			 

libgraph_coresources = AtomicModel.cpp CoupledModel.cpp \
		       Model.cpp NoVLEModel.cpp Port.cpp \
		       ModelPort.cpp ModelPortList.hpp

libvle_LTLIBRARIES = libgraph.la
libgraph_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libgraph_la_SOURCES = $(libgraph_coreheaders) $(libgraph_coresources)
libgraphincludedir = $(vleincludedir)/vle/graph
libgraphinclude_HEADERS = $(libgraph_coreheaders)
libgraph_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		     $(top_srcdir)/src/vle/utils/libutils.la
