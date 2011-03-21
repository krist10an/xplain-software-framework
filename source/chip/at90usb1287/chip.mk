cflags-gnu-y		+= -mmcu=at90usb1287
asflags-gnu-y		+= -mmcu=at90usb1287

avrdude-flags-y         += -p usb1287

ldscript-iar-y		:= $(src)/cpu/mega/lnk3s.xcl
cppflags-iar-y		+= -D__AT90USB1287__
cflags-iar-y		+= --cpu=usb1287

ifeq ($(CONFIG_MEMORY_MODEL_TINY),y)
cflags-iar-y		+= --dlib_config $(IAR_PATH)/avr/lib/dlib/dlAVR-3t-ec_mul-n.h
else ifeq ($(CONFIG_MEMORY_MODEL_SMALL),y)
cflags-iar-y		+= --dlib_config $(IAR_PATH)/avr/lib/dlib/dlAVR-3s-ec_mul-n.h
else ifeq ($(CONFIG_MEMORY_MODEL_LARGE),y)
# Not supported by AT90USB1287
endif

asflags-iar-y		+= -u_enhancedCore

ifeq ($(CONFIG_MEMORY_MODEL_TINY),y)
ldlibs-iar-y		+= $(IAR_PATH)/avr/lib/dlib/dlAVR-3t-ec_mul-n.r90
else ifeq ($(CONFIG_MEMORY_MODEL_SMALL),y)
ldlibs-iar-y		+= $(IAR_PATH)/avr/lib/dlib/dlAVR-3s-ec_mul-n.r90
else ifeq ($(CONFIG_MEMORY_MODEL_LARGE),y)
# Not supported by AT90USB1287
endif

incdir-y		+= $(src)/chip/at90usb1287/include

hdr-$(CONFIG_AT90USB)   += chip/at90usb1287/include/chip/at90usb.h
hdr-y			+= chip/at90usb1287/include/chip/clk.h
hdr-y			+= chip/at90usb1287/include/chip/gpio.h
hdr-y			+= chip/at90usb1287/include/chip/irq-map.h
hdr-y			+= chip/at90usb1287/include/chip/memory-map.h
hdr-y			+= chip/at90usb1287/include/chip/regs.h
hdr-$(CONFIG_SPI)	+= chip/at90usb1287/include/chip/spi.h
hdr-y			+= chip/at90usb1287/include/chip/sysclk.h
hdr-$(CONFIG_UART_CTRL) += chip/at90usb1287/include/chip/uart.h
