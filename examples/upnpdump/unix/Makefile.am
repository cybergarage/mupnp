samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpdump

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I../ -I../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@ $(CODE_COVERAGE_CFLAGS)

noinst_HEADERS = \
	../../common/kb_util.h

upnpdump_SOURCES = \
	../upnpdump.c \
	../../common/kb_util.c

upnpdump_LDADD = ../../../lib/unix/libmupnp.a @XML_LIBS@ $(CODE_COVERAGE_LIBS)
