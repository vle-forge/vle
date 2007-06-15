INCLUDES = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @BOOST_CPPFLAGS@

if USE_TEST
check_PROGRAMS = test1 test2 test3

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

test2_SOURCES = test2.cpp
test2_LDFLAGS = @BOOST_LDFLAGS@
test2_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
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

test3_SOURCES = test3.cpp
test3_LDFLAGS = @BOOST_LDFLAGS@
test3_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
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

vletranslator_LTLIBRARIES = libtr1.la
libtr1_la_LDFLAGS = @MODEL_PLUGIN_LDFLAGS@
libtr1_la_SOURCES = tr1.hpp tr1.cpp

TESTS = test1 test2 test3
endif

EXTRA_DIST = test1.cpp test2.cpp test3.cpp tr1.hpp tr1.cpp
