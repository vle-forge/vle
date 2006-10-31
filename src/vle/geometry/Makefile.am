AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libgeometry_coreheaders = Box.hpp geometry.hpp Point.hpp Tuple.hpp Vector.hpp

libvle_LTLIBRARIES = libgeometry.la
libgeometry_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libgeometry_la_SOURCES = $(libgeometry_coreheaders)
libgeometryincludedir = $(vleincludedir)/vle/geometry
libgeometryinclude_HEADERS = $(libgeometry_coreheaders)
