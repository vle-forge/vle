AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

vlestream_LTLIBRARIES = libeovstream.la
libeovstream_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libeovstream_la_SOURCES = NetStream.cpp NetStream.hpp
libeovstream_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
			 $(top_srcdir)/src/vle/devs/libdevs.la \
			 $(top_srcdir)/src/vle/utils/libutils.la \
			 $(top_srcdir)/src/vle/value/libvalue.la
