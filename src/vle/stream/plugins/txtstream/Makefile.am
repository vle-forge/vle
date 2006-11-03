AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

vlestream_LTLIBRARIES = libtextstream.la
libtextstream_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libtextstream_la_SOURCES = TextStream.cpp TextStream.hpp



if PLATFORM_WIN32
libtxtstream_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
			 $(top_srcdir)/src/vle/devs/libdevs.la \
			 $(top_srcdir)/src/vle/utils/libutils.la \
			 $(top_srcdir)/src/vle/value/libvalue.la \
			 $(top_srcdir)/src/vle/stream/libstream.la
endif
