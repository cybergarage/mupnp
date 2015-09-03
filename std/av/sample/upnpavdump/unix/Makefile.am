noinst_PROGRAMS = upnpavdump

AM_CPPFLAGS = -I$(top_srcdir)/include -I../ -I../../../common

noinst_HEADERS = \
	../../common/ckeyboard.h

upnpavdump_SOURCES = \
	../../common/ckeyboard.c \
	../upnpavdump.c

if HAVE_LIBTOOL
upnpavdump_LDADD = -lmupnp ../../../lib/unix/libmupnpav.la
else
upnpavdump_LDADD = -lmupnp ../../../lib/unix/libmupnpav.a
endif
