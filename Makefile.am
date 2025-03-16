SUBDIRS = include lib
if MUPNP_ENABLE_EXAMPLES
SUBDIRS += examples
endif
if MUPNP_ENABLE_TEST
SUBDIRS += test
endif

##############################
# Code coverage
##############################

include $(top_srcdir)/aminclude_static.am
# include $(top_srcdir)/aminclude_static.am

CFLAGS="$CFLAGS $(CODE_COVERAGE_CFLAGS)"
CPPFLAGS="$CPPFLAGS $(CODE_COVERAGE_CPPFLAGS)"
CXXFLAGS="$CXXFLAGS $(CODE_COVERAGE_CXXFLAGS)"

# clean-local: code-coverage-clean
# distclean-local: code-coverage-dist-clean

##############################
# Dist
##############################

devdocsdir = $(datadir)/doc/mupnp-dev
docsdir = $(datadir)/doc/mupnp0

EXTRA_DIST = debian/changelog \
	debian/mupnp0.install \
	debian/mupnp-dev.install \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules \
	debian/docs \
	include/cybergarage/upnp/cupnp.h.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mupnp.pc

dist_docs_DATA = \
	COPYING

dist_devdocs_DATA = \
	ChangeLog \
	COPYING \
	Doxyfile

#nobase_devdocs_DATA = \
#	doxygen/html/* \
#	doxygen-objc/html/*

deb: dist
	-mkdir $(top_builddir)/debian-build
	-cp $(top_builddir)/../$(PACKAGE)*.orig.tar.gz $(top_builddir)/debian-build/
	cd $(top_builddir)/debian-build && tar zxf ../$(top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	cd $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot -sa -D
	-rm -rf $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION)

##############################
# Doxygen
##############################

doxygen: doxygen/html/* doxygen/html/*

doxygen/html/*:
	cd $(srcdir) && doxygen Doxyfile
# scp -r doxygen skonno,clinkc@frs.sourceforge.net:/home/groups/c/cl/clinkc/htdocs

doxygen-objc/html/*:
	cd $(srcdir) && doxygen Doxyfile.objc
# scp -r doxygen-objc skonno,clinkobjc@frs.sourceforge.net:/home/groups/c/cl/clinkobjc/htdocs