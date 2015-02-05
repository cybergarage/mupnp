samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpdump

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I../ -I../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@

noinst_HEADERS = \
	../../common/kb_util.h

upnpdump_SOURCES = \
	../upnpdump.c \
	../../common/kb_util.c

#if HAVE_LIBTOOL
#upnpdump_LDADD = ../../../lib/unix/libmupnp.la @XML_LIBS@
#else
upnpdump_LDADD = ../../../lib/unix/libmupnp.a @XML_LIBS@
#endif
