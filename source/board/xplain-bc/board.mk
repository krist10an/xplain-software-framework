incdir-y		+= $(src)/board/xplain-bc/include

src-y			+= board/xplain-bc/init.c

hdr-y			+= board/xplain-bc/include/board.h
hdr-y			+= board/xplain-bc/include/board/led.h
hdr-$(CONFIG_SPI)	+= board/xplain-bc/include/board/spi.h
