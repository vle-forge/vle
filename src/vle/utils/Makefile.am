AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

AM_CXXFLAGS = \
	-DBINDIR=\"$(bindir)\" \
	-DIMGDIR=\"$(datadir)/pixmaps/vle\" \
	-DGLADEDIR=\"$(datadir)/vle\" \
	-DLIBDIR=\"$(libdir)/vle\" \
	-DPLUGINDIR=\"$(libdir)/vle/plugins\" \
	-DMODELDIR=\"$(libdir)/vle/models\" \
	-DEOVPLUGINDIR=\"$(libdir)/vle/eovplugin\" \
	-DOBSERVERPLUGINDIR=\"$(libdir)/vle/observers\" \
	-DSTREAMDIR=\"$(libdir)/vle/stream\" \
	-DPYTHONDIR=\"$(pythondir)\" \
	-DTRANSLATORDIR=\"$(libdir)/vle/translator\"

libutils_coresources = Host.cpp Path.cpp Rand.cpp Tools.cpp Trace.cpp XML.cpp \
		       Simpson.cpp Socket.cpp

libutils_coreheaders = Debug.hpp Exception.hpp Host.hpp Path.hpp Rand.hpp \
		       Tools.hpp Trace.hpp utils.hpp XML.hpp Simpson.hpp \
		       Socket.hpp

libvle_LTLIBRARIES = libutils.la
libutils_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libutils_la_SOURCES = $(libutils_coresources) $(libutils_coreheaders)
libutilsincludedir = $(vleincludedir)/vle/utils
libutilsinclude_HEADERS = $(libutils_coreheaders)



if PLATFORM_WIN32
libutils_la_LIBADD = @VLE_LIBS@ @XML_LIBS@
endif

