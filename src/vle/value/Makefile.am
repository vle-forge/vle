AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libvalue_coreheaders = Boolean.hpp ConstPointer.hpp Coordinate.hpp \
		       Direction.hpp Double.hpp Integer.hpp Map.hpp Set.hpp \
		       SMAValue.hpp Speed.hpp String.hpp Value.hpp

libvalue_coresources = Boolean.cpp Double.cpp Integer.cpp Map.cpp Set.cpp \
		       String.cpp Value.cpp

libvle_LTLIBRARIES = libvalue.la
libvalue_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libvalue_la_SOURCES = $(libvalue_coreheaders) $(libvalue_coresources)
libvalueincludedir = $(vleincludedir)/vle/value
libvalueinclude_HEADERS = $(libvalue_coreheaders)



if PLATFORM_WIN32
libvalue_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		     $(top_srcdir)/src/vle/utils/libutils.la \
		     $(top_srcdir)/src/vle/geometry/libgeometry.la
endif

