src-$(CONFIG_CPU_XMEGA)		+= drivers/touch/resistive/touch.c

hdr-y				+= include/touch/touch.h

mkfiles				+= $(src)/drivers/touch/resistive/subdir.mk
