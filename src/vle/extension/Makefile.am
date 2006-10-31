AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libextension_coresources = DifferenceEquation.cpp Equation.cpp QSS.cpp \
			   QSS2.cpp CellDevs.cpp CellQSS.cpp DSDevs.cpp

libextension_coreheaders = DifferenceEquation.hpp Equation.hpp QSS.hpp \
			   QSS2.hpp CellQSS.hpp CellDevs.hpp DSDevs.hpp

libvle_LTLIBRARIES = libextension.la
libextension_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libextension_la_SOURCES = $(libextension_coresources) \
			  $(libextension_coreheaders) 
libextensionincludedir = $(vleincludedir)/vle/extension
libextensioninclude_HEADERS = $(libextension_coreheaders)



if PLATFORM_WIN32
libextension_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
			 $(top_srcdir)/src/vle/devs/libdevs.la \
			 $(top_srcdir)/src/vle/utils/libutils.la
endif

