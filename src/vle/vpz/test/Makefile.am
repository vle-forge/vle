INCLUDES = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@ @BOOST_CPPFLAGS@

if USE_TEST
check_PROGRAMS = test1
test1_LDFLAGS = @BOOST_LDFLAGS@
test1_LDADD = @BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
	      $(top_srcdir)/src/vle/vpz/libvpz.la
test1_SOURCES = test1.cpp

TESTS = test1
endif

EXTRA_DIST = test1.cpp
