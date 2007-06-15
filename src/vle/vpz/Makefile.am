AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libvpz_coreheaders = Base.hpp Classes.hpp Class.hpp Condition.hpp \
		     Conditions.hpp Dynamic.hpp Dynamics.hpp EOV.hpp \
		     Experiment.hpp Measure.hpp Observables.hpp \
		     Measures.hpp Model.hpp NoVLE.hpp NoVLEs.hpp \
		     Observable.hpp Output.hpp Outputs.hpp Port.hpp \
		     Project.hpp SaxStackValue.hpp SaxVPZ.hpp Replicas.hpp \
		     Translator.hpp Structures.hpp Vpz.hpp

libvpz_coresources = Class.cpp Classes.cpp Condition.cpp Conditions.cpp \
		     Dynamic.cpp Dynamics.cpp EOV.cpp Experiment.cpp \
		     Measure.cpp Measures.cpp Observables.cpp \
		     Model.cpp NoVLE.cpp NoVLEs.cpp Observable.cpp Output.cpp \
		     Outputs.cpp Project.cpp SaxStackValue.cpp SaxVPZ.cpp \
		     Translator.cpp Replicas.cpp Vpz.cpp

libvle_LTLIBRARIES = libvpz.la
libvpz_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libvpz_la_SOURCES = $(libvpz_coreheaders) $(libvpz_coresources)
libvpzincludedir = $(vleincludedir)/vle/vpz
libvpzinclude_HEADERS = $(libvpz_coreheaders)
libvpz_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		   $(top_srcdir)/src/vle/value/libvalue.la \
		   $(top_srcdir)/src/vle/utils/libutils.la \
		   $(top_srcdir)/src/vle/graph/libgraph.la

if USE_TEST
SUBDIRS = test
endif
