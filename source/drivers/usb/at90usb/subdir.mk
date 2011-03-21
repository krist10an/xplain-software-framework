usb-src-y		+= at90usb/at90usb_core.c
usb-src-$(CONFIG_UDC)	+= at90usb/at90usb_udc.c

hdr-y			+= drivers/usb/at90usb/at90usb_internal.h
hdr-y			+= drivers/usb/at90usb/at90usb_regs.h
hdr-$(CONFIG_UDC)       += include/usb/udc_at90usb.h

mkfiles			+= $(src)/drivers/usb/at90usb/subdir.mk
