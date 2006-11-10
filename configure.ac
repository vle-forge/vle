dnl ***************************************************************************
dnl Process this file with autoconf to produce a configure script.
dnl ***************************************************************************

AC_PREREQ(2.59)
AC_INIT([vle],[0.4.0],[quesnel@users.sourceforge.net])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CONFIG_SRCDIR(config.h.in)
AC_CONFIG_HEADERS([config.h])

dnl ***************************************************************************
dnl check for programs
dnl ***************************************************************************

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ***************************************************************************
dnl checks for typedefs, structures, and compiler characteristics
dnl ***************************************************************************

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL

dnl ***************************************************************************
dnl check for header files
dnl ***************************************************************************

AC_HEADER_STDC
AC_CHECK_HEADERS([sstream])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([memory.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([unistd.h])

dnl ***************************************************************************
dnl Checks for library functions
dnl ***************************************************************************

AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([gethostbyname])
AC_CHECK_FUNCS([inet_ntoa])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([socket ])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strchr])

dnl ***************************************************************************
dnl Library strict depends
dnl ***************************************************************************

AX_CXX_GCC_ABI_DEMANGLE
AX_BOOST_BASE([1.32.0])
AX_BOOST_UNIT_TEST_FRAMEWORK
AM_CONDITIONAL(USE_TEST, test ! x$BOOST_UNIT_TEST_FRAMEWORK_LIB = xyes)

PKG_CHECK_MODULES(VLE, glibmm-2.4 >= 2.4.0)
AC_SUBST(VLE_LIBS)
AC_SUBST(VLE_CFLAGS)

PKG_CHECK_MODULES(XML, libxml++-2.6 >= 2.6.0)
AC_SUBST(XML_LIBS)
AC_SUBST(XML_CFLAGS)

PKG_CHECK_MODULES(THREAD, gthread-2.0 >= 2.0)
AC_SUBST(THREAD_LIBS)
AC_SUBST(THREAD_CFLAGS)

dnl ***************************************************************************
dnl Build output path
dnl ***************************************************************************

imgdir="\$(datadir)/pixmaps/vle"
AC_SUBST(imgdir)
gladedir="\$(datadir)/vle"
AC_SUBST(gladedir)
libvledir="\$(libdir)/vle"
AC_SUBST(libvledir)

vleincludedir="\$(includedir)/vle-0.4"
AC_SUBST(vleincludedir)
gvleincludedir="\$(includedir)/gvle-0.4"
AC_SUBST(gvleincludedir)
avleincludedir="\$(includedir)/avle-0.4"
AC_SUBST(avleincludedir)
eovincludedir="\$(includedir)/eov-0.4"
AC_SUBST(eovincludedir)

vleexampledir="\$(datadir)/vle/examples"
AC_SUBST(vleexampledir)
vlemodeldir="\$(libdir)/vle/models"
AC_SUBST(vlemodeldir)
vletranslatordir="\$(libdir)/vle/translator"
AC_SUBST(vletranslatordir)
vlestreamdir="\$(libdir)/vle/stream"
AC_SUBST(vlestreamdir)

gvleplugindir="\$(libdir)/vle/plugins"
AC_SUBST(gvleplugindir)
gvleobserverdir="\$(libdir)/vle/observers"
AC_SUBST(gvleobserverdir)

eovplugindir="\$(libdir)/vle/eovplugin"
AC_SUBST(eovplugindir)

avleplugindir="\$(libdir)/vle/avleplugin"
AC_SUBST(avleplugindir)

pythondir="\$(libdir)/vle/python"
AC_SUBST(pythondir)

dnl ***************************************************************************
dnl Checks for Win32 platform
dnl ***************************************************************************

AC_MSG_CHECKING([for some Win32 platform])
case "$host" in
  *-*-mingw*|*-*-cygwin*)
    platform_win32=yes
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win32" = "yes")

AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    os_win32=yes
    ;;
  *)
    os_win32=no
    ;;
esac
AC_MSG_RESULT([$os_win32])
AM_CONDITIONAL(OS_WIN32, test "$os_win32" = "yes")

dnl ***************************************************************************
dnl libsqlxx dependencie
dnl ***************************************************************************

AC_MSG_CHECKING(for SQL extension via libsqlxx)
AC_ARG_WITH(sql,
	    AS_HELP_STRING([--with-sql],[use SQL for extension]),
	    [enable_sql=$withval], [enable_sql=yes])

if test "x$enable_sql" = "xyes"; then
	old_LIBS="$LIBS"
	LIBS="$LIBS -lsqlxx"

	AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sqlxx.h>]], [[sqlxx::CSQL a;]])],[sqlxxlib="yes"],[sqlxxlib="no"])

	if test "x$sqlxxlib" = "xyes"; then
		SQL_LIBS="-lsqlxx"
		enable_sql="yes"
	else
		SQL_LIBS=""
		enable_sql="no"
	fi
	LIBS="$old_LIBS"
else
	SQL_LIBS=""
	enable_sql="no"
fi

AC_SUBST(SQL_LIBS)
AC_MSG_RESULT([$enable_sql])
AM_CONDITIONAL(USE_SQL, test "x$enable_sql" = "xyes")

dnl ***************************************************************************
dnl Python checking (from Inkscape program)
dnl ***************************************************************************

AC_PYTHON_DEVEL([>= '2.3.0'])
if test x"$PYTHON_LDFLAGS" = x || test x$"PYTHON_CPPFLAGS" = x
then
	enable_python="no"
else
	enable_python="yes"
fi
AM_CONDITIONAL(USE_PYTHON, test "x$enable_python" = "xyes")

dnl ***************************************************************************
dnl Check for debug
dnl ***************************************************************************

AC_MSG_CHECKING(for debugging environment)
AC_ARG_ENABLE(debug,	[  --disable-debug          turn off debug compiler. default=no],,disable_debug=no)

if test x"$disable_debug" = xno ; then
	CXXFLAGS="-O0 -Wall -W -g3 -Werror -pipe"
	CFLAGS="-O0 -Wall -W -g3 -Werror -pipe"
else
	if test x"$CFLAGS" == x ; then
		CFLAGS="-O2 -DNDEBUG -pipe"
	else
		CFLAGS="$CFLAGS -DNDEBUG"
	fi

	if test x"$CXXFLAGS" == x ; then
		CXXFLAGS="-O2 -DNDEBUG -pipe"
	else
		CXXFLAGS="$CXXFLAGS -DNDEBUG"
	fi
fi

AC_MSG_RESULT([$enable_debug])
AC_SUBST(CXXFLAGS)
AC_SUBST(CFLAGS)

dnl ***************************************************************************
dnl Build ldflags for platform.
dnl ***************************************************************************

MODEL_PLUGIN_LDFLAGS="-module -no-undefined -avoid-version"
AC_SUBST(MODEL_PLUGIN_LDFLAGS)

PLUGINS_PLUGIN_LDFLAGS="-module -no-undefined -avoid-version"
AC_SUBST(PLUGINS_PLUGIN_LDFLAGS)

OBSERVER_PLUGIN_LDFLAGS="-module -no-undefined -avoid-version"
AC_SUBST(OBSERVER_PLUGIN_LDFLAGS)

SHARED_LIBRARY_LDFLAGS="-no-undefined -version-info 0:4:0"
AC_SUBST(SHARED_LIBRARY_LDFLAGS)

BINARY_LDFLAGS="-export-dynamic -no-undefined"
AC_SUBST(BINARY_LDFLAGS)

dnl ***************************************************************************
dnl Creating Makefile
dnl ***************************************************************************

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 src/vle/Makefile
		 src/vle/vle.pc
		 src/vle/geometry/Makefile
		 src/vle/value/Makefile
		 src/vle/data/Makefile
		 src/vle/utils/Makefile
		 src/vle/graph/Makefile
		 src/vle/vpz/Makefile
		 src/vle/devs/Makefile
		 src/vle/stream/Makefile
		 src/vle/stream/plugins/eovstream/Makefile
		 src/vle/stream/plugins/Makefile
		 src/vle/stream/plugins/txtstream/Makefile
		 src/vle/stream/plugins/sdmlstream/Makefile
		 src/vle/extension/Makefile
		 src/vle/manager/Makefile
		 src/scripts/Makefile
		 src/scripts/python/Makefile
		 src/scripts/python/vlepython.pc
		 src/apps/Makefile
		 src/apps/vle/Makefile
		 src/apps/test/Makefile
		 ])

AC_OUTPUT

dnl ***************************************************************************
dnl Print resume message
dnl ***************************************************************************

AC_MSG_RESULT([*])
AC_MSG_RESULT([* $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([*])
AC_MSG_RESULT([* Using '$prefix' for installation.])
AC_MSG_RESULT([* Using '$CXX' for C++ compiler.])
AC_MSG_RESULT([* Using '$CC' for C compiler.])
AC_MSG_RESULT([* Building with '$CFLAGS' for C compiler flags.])
AC_MSG_RESULT([* Building with '$CXXFLAGS' for C++ compiler flags.])
AC_MSG_RESULT([*])
AC_MSG_RESULT([* Build with SQL extension.....: $enable_sql])
AC_MSG_RESULT([* Build with python extension..: $enable_python])
AC_MSG_RESULT([* Build for win32 platform.....: $platform_win32])
AC_MSG_RESULT([*])
AC_MSG_RESULT([* Now you may build with 'make' and install with 'make install'])
