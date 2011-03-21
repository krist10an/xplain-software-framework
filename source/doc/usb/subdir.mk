doc-y				+= doc/usb/usb_core_doc.c
doc-$(CONFIG_UDC)		+= doc/usb/usb_dev_doc.c

mkfiles				+= $(src)/doc/usb/subdir.mk
