############################################################
#
# mUPnP for C
#
# Copyright (C) Satoshi Konno 2005
#
############################################################

mupnp_headers = \
	./mupnp/icon.h \
	./mupnp/statevariable.h \
	./mupnp/xml/xml.h \
	./mupnp/net/interface.h \
	./mupnp/net/uri.h \
	./mupnp/net/url.h \
	./mupnp/net/socket.h \
	./mupnp/service.h \
	./mupnp/event/notify.h \
	./mupnp/event/property.h \
	./mupnp/event/subscriber.h \
	./mupnp/event/event.h \
	./mupnp/event/subscription.h \
	./mupnp/typedef.h \
	./mupnp/upnp_status.h \
	./mupnp/http/http.h \
	./mupnp/controlpoint.h \
	./mupnp/soap/soap.h \
	./mupnp/version.h \
	./mupnp/io/file.h \
	./mupnp/upnp_function.h \
	./mupnp/argument.h \
	./mupnp/device.h \
	./mupnp/ssdp/ssdp.h \
	./mupnp/ssdp/ssdp_server.h \
	./mupnp/action.h \
	./mupnp/util/time.h \
	./mupnp/util/list.h \
	./mupnp/util/mutex.h \
	./mupnp/util/log.h \
	./mupnp/util/cond.h \
	./mupnp/util/string.h \
	./mupnp/util/dictionary.h \
	./mupnp/util/thread.h \
	./mupnp/upnp_limit.h \
	./mupnp/control/control.h \
	./mupnp/upnp.h


mupnp/upnp.h: $(top_builddir)/config.h
	sed -e "s/MUPNP_VER/MUPNP_VER \"$(PACKAGE_VERSION)\"/" \
	$(srcdir)/mupnp/upnp.h.in > $(srcdir)/mupnp/upnp.h

nobase_include_HEADERS = \
	$(mupnp_headers)
