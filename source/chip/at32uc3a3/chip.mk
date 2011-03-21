cflags-gnu-y		+= -march=ucr2

cflags-iar-y		+= --cpu=at32uc3a0256
cflags-iar-y		+= --code_model=large --data_model=large

avr32program-prog-flags-y += -finternal@0x80000000

incdir-y		+= $(src)/chip/at32uc3a3/include

src-y			+= chip/at32uc3a3/portmux.c
src-y			+= chip/at32uc3a3/sysclk.c

hdr-$(CONFIG_EBI)       += chip/at32uc3a3/include/chip/ebi_core.h
hdr-$(CONFIG_EBI)       += chip/at32uc3a3/include/chip/ebi_port.h
hdr-$(CONFIG_EBI_SRAM)  += chip/at32uc3a3/include/chip/ebi_sram.h
hdr-y			+= chip/at32uc3a3/include/chip/genclk.h
hdr-y			+= chip/at32uc3a3/include/chip/gpio.h
hdr-y			+= chip/at32uc3a3/include/chip/hsb.h
hdr-y			+= chip/at32uc3a3/include/chip/irq-map.h
hdr-y			+= chip/at32uc3a3/include/chip/memory-map.h
hdr-y			+= chip/at32uc3a3/include/chip/osc.h
hdr-y			+= chip/at32uc3a3/include/chip/pll.h
hdr-y			+= chip/at32uc3a3/include/chip/portmux.h
hdr-y			+= chip/at32uc3a3/include/chip/sleep.h
hdr-y			+= chip/at32uc3a3/include/chip/sysclk.h
hdr-y			+= chip/at32uc3a3/include/chip/uart.h
hdr-$(CONFIG_USBB)	+= chip/at32uc3a3/include/chip/usbb.h
hdr-y			+= chip/at32uc3a3/include/chip/timer.h
hdr-y			+= chip/at32uc3a3/include/chip/tc.h
hdr-y			+= chip/at32uc3a3/include/chip/timekeeper.h
