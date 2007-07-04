AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libvalue_coreheaders = Boolean.hpp Double.hpp Integer.hpp Map.hpp Set.hpp \
		       String.hpp Table.hpp Tuple.hpp Value.hpp XML.hpp

libvalue_coresources = Boolean.cpp Double.cpp Integer.cpp Map.cpp Set.cpp \
		       String.cpp Table.cpp Tuple.cpp Value.cpp XML.cpp

libvle_LTLIBRARIES = libvalue.la
libvalue_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libvalue_la_SOURCES = $(libvalue_coreheaders) $(libvalue_coresources)
libvalueincludedir = $(vleincludedir)/vle/value
libvalueinclude_HEADERS = $(libvalue_coreheaders)
libvalue_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		     $(top_srcdir)/src/vle/utils/libutils.la
