usb-src-$(CONFIG_USB_DEV_MUX)	+= core/dev_mux.c
usb-src-y			+= core/request.c
usb-src-y			+= core/udc_lib.c

hdr-y				+= include/usb/dev.h
hdr-$(CONFIG_USB_DEV_MUX)	+= include/usb/dev_mux.h
hdr-y				+= include/usb/request.h
hdr-y				+= include/usb/udc.h
hdr-y				+= include/usb/usb_ids.h
hdr-y				+= include/usb/udc_lib.h
hdr-y				+= include/usb/usb_protocol.h

mkfiles				+= $(src)/drivers/usb/core/subdir.mk
