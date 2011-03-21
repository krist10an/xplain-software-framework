src-$(CONFIG_PORTMUX_GPIO)	+= drivers/gpio/portmux_gpio.c
src-$(CONFIG_PORT_GPIO)		+= drivers/gpio/port.c

hdr-y				+= include/gpio.h
hdr-$(CONFIG_PORTMUX_GPIO)	+= include/gpio/portmux_gpio.h
hdr-$(CONFIG_PORTMUX_GPIO)	+= include/gpio/portmux_gpio_regs.h
hdr-$(CONFIG_PORT_GPIO)		+= include/gpio/port.h
hdr-$(CONFIG_PORT_GPIO)		+= include/regs/xmega_port.h
hdr-$(CONFIG_PORT_GPIO)		+= include/regs/xmega_portcfg.h
hdr-$(CONFIG_CPU_MEGA)		+= include/gpio/gpio_mega.h

mkfiles				+= $(src)/drivers/gpio/subdir.mk
