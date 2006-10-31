AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libvpz_coreheaders = Base.hpp Classes.hpp Class.hpp Condition.hpp \
		     Conditions.hpp Dynamic.hpp Dynamics.hpp EOV.hpp \
		     Experiment.hpp Graphic.hpp Graphics.hpp Measure.hpp \
		     Measures.hpp Model.hpp NoVLE.hpp NoVLEs.hpp \
		     Observable.hpp Output.hpp Outputs.hpp Project.hpp \
		     Replicas.hpp Translator.hpp Vpz.hpp

libvpz_coresources = Class.cpp Classes.cpp Condition.cpp Conditions.cpp \
		     Dynamic.cpp Dynamics.cpp EOV.cpp Experiment.cpp \
		     Graphic.cpp Graphics.cpp Measure.cpp Measures.cpp \
		     Model.cpp NoVLE.cpp NoVLEs.cpp Observable.cpp Output.cpp \
		     Outputs.cpp Project.cpp Translator.cpp Replicas.cpp Vpz.cpp

libvle_LTLIBRARIES = libvpz.la
libvpz_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libvpz_la_SOURCES = $(libvpz_coreheaders) $(libvpz_coresources)
libvpzincludedir = $(vleincludedir)/vle/vpz
libvpzinclude_HEADERS = $(libvpz_coreheaders)



if PLATFORM_WIN32
libvpz_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		   $(top_srcdir)/src/vle/utils/libutils.la \
		   $(top_srcdir)/src/vle/graph/libgraph.la
endif

