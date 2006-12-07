SUBDIRS = include lib sample 

devdocsdir = $(datadir)/doc/clinkc-dev
docsdir = $(datadir)/doc/clinkc0

EXTRA_DIST = debian/changelog \
	debian/clinkc0.install \
	debian/clinkc-dev.install \
	debian/clinkc-tests.install \
	debian/clinkcmaemo0.install \
	debian/clinkcmaemo-tests.install \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules \
	debian/docs

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clinkc.pc

dist_docs_DATA = \
	COPYING

dist_devdocs_DATA = \
	ChangeLog \
	COPYING \
	Doxyfile

nobase_devdocs_DATA = \
	doxygen/html/*

doxygen/html/*:
	cd $(srcdir) && doxygen Doxyfile
