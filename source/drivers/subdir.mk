subdir-$(CONFIG_BLOCK)		+= drivers/block
subdir-y                        += drivers/bus
subdir-y			+= drivers/clk
subdir-y			+= drivers/flash
subdir-$(CONFIG_GFX)		+= drivers/gfx
subdir-y			+= drivers/gpio
subdir-y			+= drivers/serial
subdir-$(CONFIG_USB)		+= drivers/usb
subdir-$(CONFIG_MEMORY)		+= drivers/memory
subdir-y			+= drivers/tc
subdir-y			+= drivers/touch

mkfiles		+= $(src)/drivers/subdir.mk
