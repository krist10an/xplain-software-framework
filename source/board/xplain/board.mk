incdir-y		+= $(src)/board/xplain/include

src-y			+= board/xplain/init.c
src-$(CONFIG_EXTRAM_SDRAM)      += board/xplain/board_physmem_pools.c

hdr-y			+= board/xplain/include/board.h
hdr-y			+= board/xplain/include/board/led.h
hdr-y                   += board/xplain/include/board/physmem.h
hdr-$(CONFIG_EXTRAM_SDRAM)    += board/xplain/include/board/sdram.h
hdr-$(CONFIG_SPI)	+= board/xplain/include/board/spi.h
hdr-$(CONFIG_TOUCH_RESISTIVE) += board/xplain/include/board/touch/resistive/touch.h
hdr-$(CONFIG_GFX_HX8347A)     += board/xplain/include/board/hx8347a.h
