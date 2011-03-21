serial-subdir-$(CONFIG_SERIAL_UART)	+= $(src)/drivers/serial/uart
serial-subdir-$(CONFIG_SPI)		+= $(src)/drivers/serial/spi

include $(addsuffix /subdir.mk, $(serial-subdir-y))

mkfiles		+= $(src)/drivers/serial/subdir.mk
