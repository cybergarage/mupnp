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

clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean
# distclean-local: code-coverage-dist-clean

##############################
# Dist
##############################

devdocsdir = $(datadir)/doc/mupnp-dev
docsdir = $(datadir)/doc/mupnp0

EXTRA_DIST = test/platform-smoke.c.in \
	debian/changelog \
	debian/clinkc0.install \
	debian/clinkc-dev.install \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules \
	debian/docs \
	examples/CMakeLists.txt \
	examples/binarylight/CMakeLists.txt \
	examples/clock/CMakeLists.txt \
	examples/upnpavdump/CMakeLists.txt \
	examples/upnpdump/CMakeLists.txt \
	examples/upnpigddump/CMakeLists.txt \
	include/Makefile.am.build \
	lib/unix/Makefile.am.build \
	test/CMakeLists.txt \
	test/espidf/host/README.md \
	test/espidf/host/check_generators.sh \
	test/espidf/host/run.sh \
	test/espidf/host/run_socket.sh \
	test/espidf/host/run_xml_alloc.sh \
	test/espidf/host/shims/esp_timer.h \
	test/espidf/host/shims/sdkconfig.h \
	test/espidf/host/socket_shutdown_test.c \
	test/espidf/host/thread_lifecycle_test.c \
	test/espidf/host/xml_alloc_failure_test.c \
	test/unix/Makefile.am.build \
	include/mupnp/upnp.h.in \
	include/mupnp/version.h.build \
	include/mupnp/version.h.gen \
	test/LocationChangeTest.cpp \
	bootstrap \
	CMakeLists.txt \
	CMakeLists.txt.build \
	Kconfig \
	idf_component.yml \
	doc/espidf.md \
	examples/common/content_directory.h \
	examples/espidf/control_point/CMakeLists.txt \
	examples/espidf/control_point/README.md \
	examples/espidf/control_point/dependencies.lock \
	examples/espidf/control_point/sdkconfig.defaults \
	examples/espidf/control_point/sdkconfig.ci \
	examples/espidf/control_point/main/CMakeLists.txt \
	examples/espidf/control_point/main/Kconfig.projbuild \
	examples/espidf/control_point/main/control_point.c

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