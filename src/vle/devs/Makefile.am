AM_CPPFLAGS = -I$(top_srcdir)/src @VLE_CFLAGS@ @XML_CFLAGS@

libdevs_coreheaders = Attribute.hpp Dynamics.hpp Event.hpp EventList.hpp \
		      EventObserver.hpp EventTable.hpp ExternalEvent.hpp \
		      ExternalEventList.hpp InitEvent.hpp \
		      InstantaneousEvent.hpp InstantaneousEventList.hpp \
		      InternalEvent.hpp Measure.hpp ModelFactory.hpp \
		      Observer.hpp Simulator.hpp \
		      Simulator.hpp StateEvent.hpp Coordinator.hpp \
		      Time.hpp TimedObserver.hpp DevsTypes.hpp Stream.hpp \
		      RootCoordinator.hpp

libdevs_coresources = Dynamics.cpp Event.cpp EventObserver.cpp EventTable.cpp \
		      ExternalEvent.cpp InitEvent.cpp InstantaneousEvent.cpp \
		      InternalEvent.cpp Measure.cpp ModelFactory.cpp \
		      Observer.cpp Simulator.cpp \
		      Simulator.cpp StateEvent.cpp Time.cpp \
		      Coordinator.cpp TimedObserver.cpp Stream.cpp \
		      RootCoordinator.cpp

libvle_LTLIBRARIES = libdevs.la
libdevs_la_LDFLAGS = @SHARED_LIBRARY_LDFLAGS@
libdevs_la_SOURCES = $(libdevs_coreheaders) $(libdevs_coresources)
libdevsincludedir = $(vleincludedir)/vle/devs
libdevsinclude_HEADERS = $(libdevs_coreheaders)
libdevs_la_LIBADD = @VLE_LIBS@ @XML_LIBS@ \
		    $(top_srcdir)/src/vle/value/libvalue.la \
		    $(top_srcdir)/src/vle/utils/libutils.la \
		    $(top_srcdir)/src/vle/graph/libgraph.la \
		    $(top_srcdir)/src/vle/vpz/libvpz.la
