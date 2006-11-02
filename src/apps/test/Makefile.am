AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @THREAD_CFLAGS@

bin_PROGRAMS = vlemodeltest vletranslatortest

vlemodeltest_LDFLAGS = @BINARY_LDFLAGS@
vlemodeltest_SOURCES = modeltest.cpp
vlemodeltest_LDADD = @VLE_LIBS@ @XML_LIBS@ @THREAD_LIBS@ @SQL_LIBS@ \
	    $(top_srcdir)/src/vle/geometry/libgeometry.la \
	    $(top_srcdir)/src/vle/utils/libutils.la \
	    $(top_srcdir)/src/vle/data/libdata.la \
	    $(top_srcdir)/src/vle/value/libvalue.la \
	    $(top_srcdir)/src/vle/graph/libgraph.la \
	    $(top_srcdir)/src/vle/vpz/libvpz.la \
	    $(top_srcdir)/src/vle/devs/libdevs.la \
	    $(top_srcdir)/src/vle/extension/libextension.la \
	    $(top_srcdir)/src/vle/manager/libmanager.la

vletranslatortest_LDFLAGS = @BINARY_LDFLAGS@
vletranslatortest_SOURCES = translatortest.cpp
vletranslatortest_LDADD = @VLE_LIBS@ @XML_LIBS@ @THREAD_LIBS@ @SQL_LIBS@ \
	    $(top_srcdir)/src/vle/geometry/libgeometry.la \
	    $(top_srcdir)/src/vle/utils/libutils.la \
	    $(top_srcdir)/src/vle/data/libdata.la \
	    $(top_srcdir)/src/vle/value/libvalue.la \
	    $(top_srcdir)/src/vle/graph/libgraph.la \
	    $(top_srcdir)/src/vle/vpz/libvpz.la \
	    $(top_srcdir)/src/vle/devs/libdevs.la \
	    $(top_srcdir)/src/vle/extension/libextension.la \
	    $(top_srcdir)/src/vle/manager/libmanager.la
