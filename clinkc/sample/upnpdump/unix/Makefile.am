noinst_PROGRAMS = upnpdump

INCLUDES = -I$(top_srcdir)/include -I../ -I../../common @XML_CFLAGS@

noinst_HEADERS = \
	../../common/kb_util.h

upnpdump_SOURCES = \
	../upnpdump.c \
	../../common/kb_util.c

upnpdump_LDADD = ../../../lib/unix/libclinkc.a @XML_LIBS@

