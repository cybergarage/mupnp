samplesdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)/samples

samples_PROGRAMS = upnpavdump

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I../ -I../../common @XML_CFLAGS@ @CLOG_DEBUG_FLAGS@

noinst_HEADERS = \
	../../common/kb_util.h

upnpavdump_SOURCES = \
	../upnpavdump.c \
	../../common/kb_util.c

upnpavdump_LDADD = ../../../lib/unix/libmupnp.a @XML_LIBS@
