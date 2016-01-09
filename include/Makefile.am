############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

mupnp_headers = \
	./mupnp/action.h \
	./mupnp/statevariable.h \
	./mupnp/io/file.h \
	./mupnp/upnp.h \
	./mupnp/net/url.h \
	./mupnp/net/uri.h \
	./mupnp/net/interface.h \
	./mupnp/net/socket.h \
	./mupnp/xml/xml.h \
	./mupnp/device.h \
	./mupnp/upnp_status.h \
	./mupnp/version.h \
	./mupnp/service.h \
	./mupnp/ssdp/ssdp_server.h \
	./mupnp/ssdp/ssdp.h \
	./mupnp/event/subscription.h \
	./mupnp/event/subscriber.h \
	./mupnp/event/notify.h \
	./mupnp/event/event.h \
	./mupnp/event/property.h \
	./mupnp/soap/soap.h \
	./mupnp/icon.h \
	./mupnp/upnp_function.h \
	./mupnp/typedef.h \
	./mupnp/http/http.h \
	./mupnp/upnp_limit.h \
	./mupnp/argument.h \
	./mupnp/control/control.h \
	./mupnp/controlpoint.h \
	./mupnp/util/cond.h \
	./mupnp/util/time.h \
	./mupnp/util/list.h \
	./mupnp/util/mutex.h \
	./mupnp/util/log.h \
	./mupnp/util/thread.h \
	./mupnp/util/string.h \
	./mupnp/util/dictionary.h


mupnp/upnp.h: $(top_builddir)/config.h
	sed -e "s/MUPNP_VER/MUPNP_VER \"$(PACKAGE_VERSION)\"/" \
	$(srcdir)/mupnp/upnp.h.in > $(srcdir)/mupnp/upnp.h

nobase_include_HEADERS = \
	$(mupnp_headers)
