usb-subdir-y				+= $(src)/drivers/usb/core
usb-subdir-$(CONFIG_AT90USB)		+= $(src)/drivers/usb/at90usb
usb-subdir-$(CONFIG_UDC)		+= $(src)/drivers/usb/udi
usb-subdir-$(CONFIG_USBB)		+= $(src)/drivers/usb/usbb

include $(addsuffix /subdir.mk, $(usb-subdir-y))

src-y			+= $(addprefix drivers/usb/,$(usb-src-y))

mkfiles			+= $(src)/drivers/usb/subdir.mk
