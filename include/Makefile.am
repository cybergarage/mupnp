############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

mupnp_headers = \
	./mupnp/xml/xml.h \
	./mupnp/service.h \
	./mupnp/control/control.h \
	./mupnp/action.h \
	./mupnp/version.h \
	./mupnp/device.h \
	./mupnp/argument.h \
	./mupnp/upnp_function.h \
	./mupnp/util/thread.h \
	./mupnp/util/time.h \
	./mupnp/util/mutex.h \
	./mupnp/util/string.h \
	./mupnp/util/log.h \
	./mupnp/util/cond.h \
	./mupnp/util/list.h \
	./mupnp/util/dictionary.h \
	./mupnp/soap/soap.h \
	./mupnp/typedef.h \
	./mupnp/upnp.h \
	./mupnp/net/socket.h \
	./mupnp/net/interface.h \
	./mupnp/net/url.h \
	./mupnp/net/uri.h \
	./mupnp/http/http.h \
	./mupnp/upnp_status.h \
	./mupnp/io/file.h \
	./mupnp/ssdp/ssdp.h \
	./mupnp/ssdp/ssdp_server.h \
	./mupnp/icon.h \
	./mupnp/controlpoint.h \
	./mupnp/upnp_limit.h \
	./mupnp/event/property.h \
	./mupnp/event/notify.h \
	./mupnp/event/subscription.h \
	./mupnp/event/event.h \
	./mupnp/event/subscriber.h \
	./mupnp/statevariable.h


mupnp/upnp.h: $(top_builddir)/config.h
	sed -e "s/MUPNP_VER/MUPNP_VER \"$(PACKAGE_VERSION)\"/" \
	$(srcdir)/mupnp/upnp.h.in > $(srcdir)/mupnp/upnp.h

nobase_include_HEADERS = \
	$(mupnp_headers)
