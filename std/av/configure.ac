#############################################################################
#
#	CyberLink for C
#
#	Copyright (C) Satoshi Konno 2005
#
#	11/04/05
#		- first release.
#
#   16/09/11 lundman
#       - tests for libraries required. In particular, expat or xml2.
#
#############################################################################

AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_INIT([clinkcav], [3.0])
AC_CONFIG_SRCDIR(include/cybergarage/upnp/std/av/cmediaserver.h)
AM_INIT_AUTOMAKE([subdir-objects])

##############################
# Constants
##############################

# CyberLink version.
CLINKC_VERSION=2.4

##############################
# Checks for programs.
##############################
AC_PROG_CC
AC_PROG_CXX
#AC_LANG_CPLUSPLUS
AC_PROG_INSTALL
AC_PROG_AWK
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AM_PROG_CC_C_O

#############################################################################
# Checks for CYGWIN an MinGW
#############################################################################

#AC_CYGWIN
#if [ test $CYGWIN = yes || test $MINGW = yes ]; then
  #AC_LIBTOOL_WIN32_DLL
  #AM_DISABLE_STATIC
  AC_CHECK_LIB([ws2_32],[main])
  #AC_CHECK_LIB([iphlpapi],[main])
#fi

#############################################################################
# Check whether libtool should be used
#############################################################################

AC_ARG_WITH(libtool,
	AS_HELP_STRING([--with-libtool],
		       [use libtool to build the library (default = yes)]),
		       [with_libtool=no],
		       [with_libtool=yes])
AM_CONDITIONAL(HAVE_LIBTOOL, test $with_libtool = yes)

#############################################################################
# Other libraries
#############################################################################

# Solaris TCP stack
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket inet])

# Separate libuuid
AC_SEARCH_LIBS([uuid_unparse], [uuid])

AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h unistd.h sys/ioctl.h sys/filio.h])


##############################
# Checks for CyberLink
##############################

AC_CHECK_HEADERS([cybergarage/cupnp.h])
AC_CHECK_LIB([clinkc],[cg_upnp_controlpoint_new],,[AC_MSG_ERROR(CyberLink/AV needs CyberLink >= $CLINKC_VERSION)],[-lpthread -lexpat -lxml2])

##############################
# Makefiles
##############################

AC_OUTPUT([
Makefile
include/Makefile
lib/Makefile
lib/unix/Makefile
sample/Makefile
sample/upnpavserver/Makefile
sample/upnpavserver/filesystem/Makefile
sample/upnpavserver/filesystem/unix/Makefile
sample/upnpavdump/Makefile
sample/upnpavdump/unix/Makefile
])
