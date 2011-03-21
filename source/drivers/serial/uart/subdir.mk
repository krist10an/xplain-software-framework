src-$(CONFIG_DEBUG_UART)	+= drivers/serial/uart/debug_uart.c

hdr-y				+= include/uart.h
hdr-$(CONFIG_ARCH_AVR32)	+= include/uart/uart_avr32.h
hdr-$(CONFIG_ARCH_AVR32)	+= include/regs/avr32_usart.h
hdr-$(CONFIG_CPU_XMEGA)		+= include/uart/uart_xmega.h
hdr-$(CONFIG_CPU_XMEGA)		+= include/regs/xmega_usart.h
hdr-$(CONFIG_CPU_MEGA)		+= include/uart/uart_mega.h

stream-src-y			:=
stream-hdr-y			:=
stream-src-$(CONFIG_ARCH_AVR32)	+= drivers/serial/uart/uart_avr32_stream.c
stream-hdr-$(CONFIG_ARCH_AVR32)	+= include/uart/stream_avr32.h
stream-src-$(CONFIG_CPU_MEGA)	+= drivers/serial/uart/uart_mega_stream.c
stream-hdr-$(CONFIG_CPU_MEGA)	+= include/uart/stream_mega.h
stream-src-$(CONFIG_CPU_XMEGA)	+= drivers/serial/uart/uart_xmega_stream.c
stream-hdr-$(CONFIG_CPU_XMEGA)	+= include/uart/stream_xmega.h
ctrl-src-y			:=
ctrl-hdr-y			:=
ctrl-src-$(CONFIG_ARCH_AVR32)	+= drivers/serial/uart/uart_avr32_ctrl.c
ctrl-hdr-$(CONFIG_ARCH_AVR32)	+= include/uart/ctrl_avr32.h
ctrl-src-$(CONFIG_CPU_XMEGA)	+= drivers/serial/uart/uart_xmega_ctrl.c
ctrl-hdr-$(CONFIG_CPU_XMEGA)	+= include/uart/ctrl_xmega.h
ctrl-src-$(CONFIG_CPU_MEGA)	+= drivers/serial/uart/uart_mega_ctrl.c
ctrl-hdr-$(CONFIG_CPU_MEGA)	+= include/uart/ctrl_mega.h

cdc-hdr-y                       += include/uart/cdc.h
cdc-hdr-$(CONFIG_ARCH_AVR32)    += include/uart/cdc_avr32.h
cdc-hdr-$(CONFIG_CPU_MEGA)      += include/uart/cdc_mega.h
cdc-hdr-$(CONFIG_CPU_XMEGA)     += include/uart/cdc_xmega.h

src-$(CONFIG_UART_STREAM)	+= $(stream-src-y)
hdr-$(CONFIG_UART_STREAM)	+= include/uart/stream.h $(stream-hdr-y)
src-$(CONFIG_UART_CTRL)		+= $(ctrl-src-y)
hdr-$(CONFIG_UART_CTRL)		+= include/uart/ctrl.h $(ctrl-hdr-y)
hdr-$(CONFIG_UDI_CDC_ACM)       += $(cdc-hdr-y)

mkfiles				+= $(src)/drivers/serial/uart/subdir.mk
