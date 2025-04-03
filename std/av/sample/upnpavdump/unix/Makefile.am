noinst_PROGRAMS = upnpavdump

AM_CPPFLAGS = -I$(top_srcdir)/include -I../ -I../../../common

noinst_HEADERS = \
	../../common/ckeyboard.h

upnpavdump_SOURCES = \
	../../common/ckeyboard.c \
	../upnpavdump.c

upnpavdump_LDADD = -lmupnp ../../../lib/unix/libmupnpav.a
