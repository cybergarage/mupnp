#############################################################################
#
# mUPnP for C
# 
# Copyright (C) Satoshi Konno 2005
#
#############################################################################

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS(config.h)

AC_INIT([mupnp],[3.0.2])
AC_CONFIG_SRCDIR([include/mupnp/typedef.h])
AM_INIT_AUTOMAKE([subdir-objects])

##############################
# Constants
##############################

# Expat version.
EXPAT_VERSION=1.95

# Libxml2 version.
LIBXML2_VERSION=2.6.0

#Libcurl version
LIBCURL3_VERSION=7.13.0

#CppUnit version
CPPUNIT_VERSION=1.6.0

##############################
# Checks for programs.
##############################

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_LANG([C++])
AM_PROG_CC_C_O
AM_PROG_AR

##############################
# Checks for header files.
##############################

AC_CHECK_HEADERS([stdbool.h])

##############################
# Code coverage
##############################

AX_CODE_COVERAGE

##############################
# Checks for Long Long
##############################

AC_MSG_CHECKING([whether $CC supports long long])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <stdlib.h>
typedef long long CgInt64;
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_LONGLONG],1,[LONGLONG])],[AC_MSG_RESULT(no)
])

#############################################################################
# Check whether libtool should be used
#############################################################################

AC_ARG_WITH(libtool,
	AS_HELP_STRING([--with-libtool],
		       [use libtool to build the library (default = yes)]),
		       [],
		       [with_libtool=yes])
AM_CONDITIONAL(HAVE_LIBTOOL, test $with_libtool = yes)

#############################################################################
# Check whether timing info should be printed
#############################################################################
AC_ARG_WITH(timings,
	AS_HELP_STRING([--with-timings],
		       [show timings in debug-info (default = no)]),
		       [],
		       [with_timings=no])
if [ test $with_timings = yes ]; then
  CFLAGS="$CFLAGS -DMUPNP_SHOW_TIMINGS"
fi

##############################
# Checks for header files.
##############################

AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h unistd.h sys/ioctl.h sys/filio.h])

##############################
# Checks for libxml2 libraries.
##############################

AC_ARG_ENABLE(libxml2,
	AS_HELP_STRING([--enable-libxml2],
			[enable Libxml2 support (default = no)]),
	[
		if test $enableval = no; then
			USE_LIBXML2=no
		else
			USE_LIBXML2=yes
		fi
	],[
		USE_LIBXML2=no
	]
)

if [ test $USE_LIBXML2 = yes ]; then
	AC_CHECK_LIB(xml2, xmlSAXUserParseMemory,
	[
		AC_DEFINE([MUPNP_XMLPARSER_LIBXML2],1,[LIBXML2])

		XML_CFLAGS=`pkg-config libxml-2.0 --cflags`
		AC_SUBST(XML_CFLAGS)

		XML_LIBS=`pkg-config libxml-2.0 --libs`
		AC_SUBST(XML_LIBS)
	],[
		AC_MSG_ERROR(mUPnP needs libxml2-devel >= $LIBXML2_VERSION)
	])
fi

##############################
# Checks for Expat libraries.
##############################

if [ test $USE_LIBXML2 != yes ]; then
	AC_CHECK_HEADERS([expat/expat.h])
	AC_CHECK_HEADERS([expat.h])
	AC_CHECK_LIB([expat],[main],
  [
    AC_DEFINE([MUPNP_XMLPARSER_EXPAT],1,[EXPAT])
		XML_LIBS="-lexpat"
		AC_SUBST(XML_LIBS)
  ],[
    AC_MSG_ERROR(mUPnP needs Expat >= $EXPAT_VERSION)
  ])
fi

##############################
# Enable compiler warnings
##############################

AC_ARG_ENABLE(compiler_warnings,
	AS_HELP_STRING([--enable-compiler-warnings],
	[enable compiler warnings (default = yes)]),
	[
		if test $enableval = no; then
			USE_COMPILER_WARNINGS=no
		else
			USE_COMPILER_WARNINGS=yes
		fi
	],[
		USE_COMPILER_WARNINGS=yes
	]
)

if [ test $USE_COMPILER_WARNINGS = yes ]; then
	CFLAGS="$CFLAGS -Wall"
fi

##############################
# Debug flag checks
##############################

AC_ARG_ENABLE(clog_debug,
	AS_HELP_STRING([--enable-clog-debug],
		[enable common clog debug messages (default = no)]),
		[
			if test $enableval = no; then
				USE_CLOG_DEBUG=no;
			else
				USE_CLOG_DEBUG=yes;
			fi
		],[
			USE_CLOG_DEBUG=no;
		]
)

AC_ARG_ENABLE(clog_debug_stack,
        AS_HELP_STRING([--enable-clog-debug-stack],
                [enable clog stack debug messages (default = no)]),
                [
                        if test $enableval = no; then
                                USE_CLOG_DEBUG_STACK=no;
                        else
                                USE_CLOG_DEBUG_STACK=yes;
                        fi
                ],[
                        USE_CLOG_DEBUG_STACK=no;
                ]
)

AC_ARG_ENABLE(clog_debug_application,
        AS_HELP_STRING([--enable-clog-debug-application],
                [enable clog application debug messages (default = no)]),
                [
                        if test $enableval = no; then
                                USE_CLOG_DEBUG_APPLICATION=no;
                        else
                                USE_CLOG_DEBUG_APPLICATION=yes;
                        fi
                ],[
                        USE_CLOG_DEBUG_APPLICATION=no;
                ]
)

CLOG_DEBUG_FLAGS=""

if [ test $USE_CLOG_DEBUG = yes ]; then
	CLOG_DEBUG_FLAGS="$CLOG_DEBUG_FLAGS -DCLOG_DEBUG"
fi

if [ test $USE_CLOG_DEBUG_STACK = yes ]; then
	CLOG_DEBUG_FLAGS="$CLOG_DEBUG_FLAGS -DCLOG_DEBUG_STACK"
fi

if [ test $USE_CLOG_DEBUG_APPLICATION = yes ]; then
	CLOG_DEBUG_FLAGS="$CLOG_DEBUG_FLAGS -DCLOG_DEBUG_APPLICATION"
fi

AC_SUBST(CLOG_DEBUG_FLAGS)

##############################
# Check whether thread locking should be traced
##############################

AC_ARG_WITH(thread-lock-trace,
        AS_HELP_STRING([--with-thread-lock-trace],
                       [Trace thread locking and fail fast on errors, errors can be found also from /tmp/cg_td_* (default = no)]),
                       [],
                       [with_cp_lock_trace=no])
if [test "$with_thread_lock_trace" = yes]; then
        CFLAGS="$CFLAGS -DWITH_THREAD_LOCK_TRACE"
fi

##############################
# Checks for persistent connections usage.
##############################

AC_ARG_ENABLE(persistent-connections,
	AS_HELP_STRING([--enable-persistent-connections],
			[enable persistent connections support (default = yes)]),
	[
		if test $enableval = yes; then
			CFLAGS="$CFLAGS -DMUPNP_HTTP_USE_PERSISTENT_CONNECTIONS"
		fi
	],[
		CFLAGS="$CFLAGS -DMUPNP_HTTP_USE_PERSISTENT_CONNECTIONS"
	]
)

##############################
# Checks for fast mode.
##############################

AC_ARG_ENABLE(optimized-cp,
	AS_HELP_STRING([--enable-optimized-cp],
			[enable optimized control point mode (default = no)]),
	[
		if test $enableval = yes; then
			CFLAGS="$CFLAGS -DMUPNP_OPTIMIZED_CP_MODE"
		fi
	],
)

AC_ARG_ENABLE(anyaddr,
	AS_HELP_STRING([--enable-anyaddr],
			[enables the usage of IPV4_ADDRANY instead of
			 registering an explicit server for each
			 interface (default = yes)]),
	[
		if test $enableval = yes; then
			CFLAGS="$CFLAGS -DMUPNP_NET_USE_ANYADDR"
		fi
	],
)

##############################
# Checks for file functions.
##############################

AC_ARG_ENABLE(cfile,
        AS_HELP_STRING([--enable-cfile],
                        [enable file functions (default = no)]),
        [
                if test $enableval = yes; then
                        CFLAGS="$CFLAGS -DMUPNP_USE_CFILE"
                fi
        ],
)

##############################
# Checks for SSL functions
##############################

AC_ARG_ENABLE(openssl,
        AS_HELP_STRING([--enable-openssl],
                        [enable file functions (default = no)]),
        [
                if test $enableval = yes; then
                        CFLAGS="$CFLAGS -DMUPNP_USE_OPENSSL"
                fi
        ],
)

##############################
# Checks for libcurl libraries.
##############################

AC_ARG_ENABLE(libcurl,
	AS_HELP_STRING([--enable-libcurl],
			[enable Libcurl support (default = no)]),
	[
		if test $enableval = no; then
			USE_LIBCURL=no
		else
			USE_LIBCURL=yes
		fi
	],[
		USE_LIBCURL=no
	]
)

if [ test $USE_LIBCURL = yes ]; then
	AC_CHECK_LIB(curl, curl_easy_perform,
	[
		AC_DEFINE([MUPNP_HTTP_CURL],1,[LIBCURL])

		HTTP_CFLAGS=`curl-config --cflags`
		AC_SUBST(HTTP_CFLAGS)

		HTTP_LIBS=`curl-config --libs`
		AC_SUBST(HTTP_LIBS)
	],[
		AC_MSG_ERROR(mUPnP needs libcurl3-dev >= $LIBCURL_VERSION)
	])
fi

##############################
# Checks for iconv
##############################

AC_CHECK_FUNCS([iconv])
AC_CHECK_HEADERS([iconv.h])
AC_CHECK_LIB([iconv],[main])

##############################
# Checks for pthread
##############################

AC_CHECK_HEADERS([pthread.h],,[AC_MSG_ERROR(mUPnP needs POSIX thread library)])
AC_CHECK_LIB([pthread],[main])

##############################
# Checks for UUID
##############################

AC_CHECK_LIB([uuid],[main])
AC_CHECK_HEADERS([uuid/uuid.h])

##############################
# Checks for typedefs, structures, and compiler characteristics.
##############################

#AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM

##############################
# Checks for Network functions
##############################

AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([inet_ntoa])
AC_CHECK_HEADERS([ifaddrs.h])
AC_CHECK_FUNCS([getifaddrs])

##### socklen_t ####
AC_MSG_CHECKING(for socklen_t)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
socklen_t foosocket;
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_SOCKLEN_T],1,[SOCKLEN_T])],[AC_MSG_RESULT(no)
])

##### SIOCGIFHWADDR ####
AC_MSG_CHECKING(for SIOCGIFHWADDR)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
void func()
{
  int sock;
  struct ifreq ifr;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
  ifr.ifr_addr.sa_family = AF_INET;
  ioctl(sock, SIOCGIFHWADDR, &ifr);
}
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_SIOCGIFHWADDR],1,[SIOCGIFHWADDR])],[AC_MSG_RESULT(no)
])

##### sockaddr_dl ####
AC_MSG_CHECKING(for sockaddr_dl)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
void func()
{
  struct ifaddrs *ifaddr;
  struct ifaddrs *i;
  struct sockaddr_dl *dl;
  unsigned char *macaddr;
  getifaddrs(&ifaddr);
  dl = (struct sockaddr_dl *)(ifaddr->ifa_addr);
  LLADDR(dl);
  freeifaddrs(ifaddr);
}
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_SOCKADDR_DL],1,[SOCKADDR_DL])],[AC_MSG_RESULT(no)
])

##### IP_PKTINFO ####
AC_MSG_CHECKING(for IP_PKTINFO)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
void func()
{
  int on=1;
  setsockopt(0, IPPROTO_IP, IP_PKTINFO,  &on, sizeof(on));
}
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_IP_PKTINFO],1,[IP_PKTINFO])],[AC_MSG_RESULT(no)
])

##### SO_NOSIGPIPE ####
AC_MSG_CHECKING(for SO_NOSIGPIPE)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
void func()
{
  int on=1;
  setsockopt(0, SOL_SOCKET, SO_NOSIGPIPE,  &on, sizeof(on));
}
]], [[]])],[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_SO_NOSIGPIPE],1,[SO_NOSIGPIPES])],[AC_MSG_RESULT(no)
])

##############################
# Checks for other headers.
##############################

AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([pthread.h])

##############################
# Checks for other functions.
##############################

AC_CHECK_FUNCS([fopen])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([realpath])
AC_CHECK_FUNCS([localtime_r])
AC_CHECK_FUNCS([gmtime_r])
AC_CHECK_FUNCS([snprintf])
AC_CHECK_FUNCS([uname])
AC_CHECK_FUNCS([strdup])

AC_ARG_ENABLE(mudflap-debug,
	AS_HELP_STRING([--enable-mudflap-debug],
		[enable libmudflap debug (default = no)]),
		[
			if test $enableval = no; then
				USE_MUDFLAP_DEBUG=no;
			else
				USE_MUDFLAP_DEBUG=yes;
			fi
		],[
			USE_MUDFLAP_DEBUG=no;
		]
)

if [ test $USE_MUDFLAP_DEBUG = yes ]; then
	CFLAGS="$CFLAGS -fmudflap -fmudflapth -O0"
	LDFLAGS="$LDFLAGS -lmudflapth"
fi

AC_CHECK_FUNCS([uuid_unparse_lower])

# Solaris TCP stack
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket inet])

##############################
# Examples
##############################

AC_ARG_ENABLE(
  [examples],
  AS_HELP_STRING([--enable-examples],[ build examples (default = yes) ]),
  [case "${enableval}" in
    yes | no ) build_examples="${enableval}" ;;
  esac],
  [build_examples="yes"]
)
AM_CONDITIONAL(MUPNP_ENABLE_EXAMPLES,test "$build_examples" = yes)

##############################
# Test
##############################

AC_ARG_ENABLE([test], AS_HELP_STRING([--enable-test],[ build test (default = no) ]), [build_test="yes"], [])
AM_CONDITIONAL(MUPNP_ENABLE_TEST,test "$build_test" = yes)
if [ test "$build_test" = yes ]; then
	AC_CHECK_LIB([boost_unit_test_framework-mt],[main],,
		[AC_CHECK_LIB([boost_unit_test_framework],[main],,[AC_MSG_ERROR(mUPnP needs boost::boost_unit_test_framework)])])
	AC_CHECK_LIB([gcov],[main])
fi

##############################
# Coverage
##############################

AC_ARG_ENABLE([coverage],
  [AS_HELP_STRING([--enable-coverage], [Enable code coverage using gcov])],
  [enable_coverage=yes], [enable_coverage=no])

if test "x$enable_coverage" = "xyes"; then
  CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
  LDFLAGS="$LDFLAGS -fprofile-arcs -ftest-coverage"
  AC_MSG_NOTICE([Code coverage enabled])
fi

##############################
# Makefiles
##############################

AC_CONFIG_FILES([
mupnp.pc
Makefile
include/Makefile
lib/Makefile
lib/unix/Makefile
examples/Makefile
examples/clock/Makefile
examples/clock/unix/Makefile
examples/upnpdump/Makefile
examples/upnpdump/unix/Makefile
examples/upnpavdump/Makefile
examples/upnpavdump/unix/Makefile
examples/upnpigddump/Makefile
examples/upnpigddump/unix/Makefile
test/Makefile
test/unix/Makefile
])
AC_OUTPUT
