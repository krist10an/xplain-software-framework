hdr-$(CONFIG_AT45)		+= include/flash/at45.h
hdr-$(CONFIG_AT45)		+= include/flash/at45_device.h

src-$(CONFIG_AT45)		+= drivers/flash/at45_device.c

mkfiles				+= $(src)/drivers/flash/subdir.mk
