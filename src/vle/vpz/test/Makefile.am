INCLUDES = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @BOOST_CPPFLAGS@

if USE_TEST
check_PROGRAMS = test1
test1_LDFLAGS = @BOOST_LDFLAGS@
test1_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
	      @VLE_LIBS@ @XML_LIBS@ @THREAD_LIBS@ @SQL_LIBS@ \
	      $(top_srcdir)/src/vle/vpz/libvpz.la \
	      $(top_srcdir)/src/vle/geometry/libgeometry.la \
	      $(top_srcdir)/src/vle/utils/libutils.la \
	      $(top_srcdir)/src/vle/data/libdata.la \
	      $(top_srcdir)/src/vle/value/libvalue.la \
	      $(top_srcdir)/src/vle/graph/libgraph.la \
	      $(top_srcdir)/src/vle/vpz/libvpz.la \
	      $(top_srcdir)/src/vle/devs/libdevs.la \
	      $(top_srcdir)/src/vle/extension/libextension.la \
	      $(top_srcdir)/src/vle/manager/libmanager.la

test1_SOURCES = test1.cpp

TESTS = test1
endif

EXTRA_DIST = test1.cpp
