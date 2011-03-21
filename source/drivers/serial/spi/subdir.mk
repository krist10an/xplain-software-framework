hdr-y					+= include/spi.h

src-polled-y				+= drivers/serial/spi/spi_polled.c
src-polled-$(CONFIG_SPI_BUF_LIST_API)	+= drivers/serial/spi/spi_polled_buf_list.c

hdr-polled-y				+= include/spi/spi_polled.h

src-mega-xmega-y			+= $(src-polled-y)
src-mega-xmega-y			+= drivers/serial/spi/spi_mega_xmega.c

src-$(CONFIG_CPU_MEGA)			+= $(src-mega-xmega-y)
src-$(CONFIG_CPU_XMEGA)			+= $(src-mega-xmega-y)

hdr-mega-xmega-y			+= $(hdr-polled-y)
hdr-mega-xmega-$(CONFIG_CPU_MEGA)	+= include/spi/spi_mega.h
hdr-mega-xmega-$(CONFIG_CPU_XMEGA)	+= include/spi/spi_xmega.h
hdr-mega-xmega-$(CONFIG_CPU_XMEGA)	+= include/regs/xmega_spi.h
hdr-mega-xmega-y			+= include/spi/spi_mega_xmega.h

hdr-$(CONFIG_CPU_MEGA)			+= $(hdr-mega-xmega-y)
hdr-$(CONFIG_CPU_XMEGA)			+= $(hdr-mega-xmega-y)

mkfiles					+= $(src)/drivers/serial/spi/subdir.mk
