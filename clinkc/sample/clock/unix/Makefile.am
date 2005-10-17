noinst_PROGRAMS = upnpclock

INCLUDES = -I$(top_srcdir)/include -I../ -I../../common @XML_CFLAGS@

noinst_HEADERS = \
	../clock.h \
	../clock_device.h \
	../../common/kb_util.h

upnpclock_SOURCES = \
	clock_main.c \
	../clock.c \
	../clock_device.c \
	../../common/kb_util.c

upnpclock_LDADD = ../../../lib/unix/libclinkc.a @XML_LIBS@

