AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

AM_CXXFLAGS = \
	-DBINDIR=\"$(bindir)\" \
	-DIMGDIR=\"$(imgdir)\" \
	-DGLADEDIR=\"$(gladedir)\" \
	-DLIBVLEDIR=\"$(libvledir)\" \
	-DVLEEXAMPLEDIR=\"$(vleexampledir)\" \
	-DVLEMODELDIR=\"$(vlemodeldir)\" \
	-DVLETRANSLATORDIR=\"$(vletranslatordir)\" \
	-DVLESTREAMDIR=\"$(vlestreamdir)\" \
	-DGVLEPLUGINDIR=\"$(gvleplugindir)\" \
	-DGVLEOBSERVERDIR=\"$(gvleobserverdir)\" \
	-DEOVPLUGINDIR=\"$(eovplugindir)\" \
	-DAVLEPLUGINDIR=\"$(avleplugindir)\" \
	-DPYTHONDIR=\"$(pythondir)\"

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

