AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @PYTHON_CPPFLAGS@

if USE_PYTHON
python_coresources = Pyutils.cpp 
python_coreheaders = Pyutils.hpp

libvle_LTLIBRARIES = libpython.la
libpython_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libpython_la_SOURCES = $(python_coresources) $(python_coreheaders)

pythonincludedir = $(vleincludedir)/vle/scripts/python
pythoninclude_HEADERS = $(python_coreheaders)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vlepython.pc

if PLATFORM_WIN32
libpython_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ @PYTHON_LDFLAGS@ \
		      @PYTHON_EXTRA_LIBS@ @PYTHON_EXTRA_LDFLAGS@ \
		      $(top_srcdir)/src/vle/utils/libutils.la \
		      $(top_srcdir)/src/vle/graph/libgraph.la
endif
endif
