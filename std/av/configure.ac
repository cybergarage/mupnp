#############################################################################
#
#	mUPnP for C
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
AC_INIT([clinkcav],[3.0])
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
#AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_AWK
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AM_PROG_CC_C_O

#############################################################################
# Checks for CYGWIN an MinGW
#############################################################################

#m4_warn([obsolete],
[AC_CYGWIN is obsolete: use AC_CANONICAL_HOST and check if $host_os
matches *cygwin*])dnl
AC_CANONICAL_HOST
case $host_os in
  *cygwin* ) CYGWIN=yes;;
	 * ) CYGWIN=no;;
esac

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

m4_warn([obsolete],
[The preprocessor macro `STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  ISO C90 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h unistd.h sys/ioctl.h sys/filio.h])


##############################
# Checks for CyberLink
##############################

AC_CHECK_HEADERS([cybergarage/cupnp.h])
AC_CHECK_LIB([clinkc],[cg_upnp_controlpoint_new],,[AC_MSG_ERROR(CyberLink/AV needs CyberLink >= $CLINKC_VERSION)],[-lpthread -lexpat -lxml2])

##############################
# Makefiles
##############################

AC_CONFIG_FILES([
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
AC_OUTPUT
