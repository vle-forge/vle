AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@

if USE_SQL
libdata_coreheaders = DataReader.hpp SQLReader.hpp SQLResult.hpp \
		      CSVReader.hpp Result.hpp CSVResult.hpp 

libdata_coresources = DataReader.cpp SQLReader.cpp SQLResult.cpp \
		      CSVReader.cpp CSVResult.cpp 
else
libdata_coreheaders = DataReader.hpp CSVReader.hpp Result.hpp \
		      CSVResult.hpp 

libdata_coresources = DataReader.cpp CSVReader.cpp CSVResult.cpp 
endif

libvle_LTLIBRARIES = libdata.la
libdata_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libdata_la_SOURCES = $(libdata_coreheaders) $(libdata_coresources)
libdataincludedir = $(vleincludedir)/vle/data
libdatainclude_HEADERS = $(libdata_coreheaders)



if PLATFORM_WIN32
libdata_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		    $(top_srcdir)/src/vle/utils/libutils.la
endif

