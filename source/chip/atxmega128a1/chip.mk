cflags-gnu-y		+= -mmcu=atxmega128a1
asflags-gnu-y		+= -mmcu=atxmega128a1

avrdude-flags-y         += -p x128a1

ldscript-iar-y		:= cpu/xmega/cfgxmega6loim.xcl
ldscript-iar-src-y	:= cpu/xmega/cfgxmega6loim.xcl.S

cppflags-iar-y		+= -D__ATxmega128A1__
cflags-iar-y		+= --cpu=xm128a1

ifeq ($(CONFIG_MEMORY_MODEL_TINY),y)
# Not supported by ATxmega128A1
else ifeq ($(CONFIG_MEMORY_MODEL_SMALL),y)
cflags-iar-y		+= --dlib_config $(IAR_PATH)/avr/lib/dlib/dlAVR-6s-xmega-n.h
else ifeq ($(CONFIG_MEMORY_MODEL_LARGE),y)
cflags-iar-y		+= --dlib_config $(IAR_PATH)/avr/lib/dlib/dlAVR-6l-xmega-n.h
endif

asflags-iar-y		+= -u_enhancedCore

ifeq ($(CONFIG_MEMORY_MODEL_TINY),y)
# Not supported by ATxmega128A1
else ifeq ($(CONFIG_MEMORY_MODEL_SMALL),y)
ldlibs-iar-y		+= $(IAR_PATH)/avr/lib/dlib/dlAVR-6s-xmega-n.r90
else ifeq ($(CONFIG_MEMORY_MODEL_LARGE),y)
ldlibs-iar-y		+= $(IAR_PATH)/avr/lib/dlib/dlAVR-6l-xmega-n.r90
endif

incdir-y		+= $(src)/chip/atxmega128a1/include

src-y                   += chip/atxmega128a1/sysclk.c

hdr-y			+= chip/atxmega128a1/include/chip/gpio.h
hdr-y			+= chip/atxmega128a1/include/chip/memory-map.h
hdr-y			+= chip/atxmega128a1/include/chip/nvm_regs.h
hdr-y			+= chip/atxmega128a1/include/chip/osc.h
hdr-y			+= chip/atxmega128a1/include/chip/pll.h
hdr-y			+= chip/atxmega128a1/include/chip/pmic.h
hdr-$(CONFIG_SPI)	+= chip/atxmega128a1/include/chip/spi.h
hdr-y			+= chip/atxmega128a1/include/chip/sysclk.h
hdr-y			+= chip/atxmega128a1/include/chip/uart.h
hdr-$(CONFIG_XMEGA_USB) += chip/atxmega128a1/include/chip/usb.h
hdr-$(CONFIG_TC)        += chip/atxmega128a1/include/chip/tc.h
hdr-$(CONFIG_TIMER)     += chip/atxmega128a1/include/chip/timer.h
hdr-$(CONFIG_TIMEKEEPER)        += chip/atxmega128a1/include/chip/timekeeper.h
