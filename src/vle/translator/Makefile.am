AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libtranslator_coresources = MatrixTranslator.cpp

libtranslator_coreheaders = MatrixTranslator.hpp

libvle_LTLIBRARIES = libtranslator.la
libtranslator_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libtranslator_la_SOURCES = $(libtranslator_coresources) \
			   $(libtranslator_coreheaders) 
libtranslatorincludedir = $(vleincludedir)/vle/translator
libtranslatorinclude_HEADERS = $(libtranslator_coreheaders)
libtranslator_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
			 $(top_srcdir)/src/vle/devs/libdevs.la \
			 $(top_srcdir)/src/vle/graph/libgraph.la \
			 $(top_srcdir)/src/vle/value/libvalue.la \
			 $(top_srcdir)/src/vle/utils/libutils.la
