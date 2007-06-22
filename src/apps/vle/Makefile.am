AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @THREAD_CFLAGS@

bin_PROGRAMS = vle
vle_LDFLAGS = @BINARY_LDFLAGS@
vle_SOURCES = main.cpp OptionGroup.cpp OptionGroup.hpp
vle_LDADD = @VLE_LIBS@ @XML_LIBS@ @THREAD_LIBS@ @SQL_LIBS@ \
	    $(top_srcdir)/src/vle/geometry/libgeometry.la \
	    $(top_srcdir)/src/vle/utils/libutils.la \
	    $(top_srcdir)/src/vle/data/libdata.la \
	    $(top_srcdir)/src/vle/value/libvalue.la \
	    $(top_srcdir)/src/vle/graph/libgraph.la \
	    $(top_srcdir)/src/vle/vpz/libvpz.la \
	    $(top_srcdir)/src/vle/devs/libdevs.la \
	    $(top_srcdir)/src/vle/extension/libextension.la \
	    $(top_srcdir)/src/vle/manager/libmanager.la \
	    $(top_srcdir)/src/vle/translator/libtranslator.la
man_MANS = vle.1
