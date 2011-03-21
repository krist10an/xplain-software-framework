# Both toolchains support generating ELF and HEX images
appimg-y                += $(app).elf $(app).hex

# If PROGTOOL hasn't been set by the user, try AVR32_PROGTOOL. If none
# of them have been set, default to avr32program.
AVR32_PROGTOOL          ?= avr32program
PROGTOOL                ?= $(AVR32_PROGTOOL)

incdir-y                += $(src)/arch/avr32/include
cflags-gnu-y            += -mno-pic
asflags-gnu-y           += -mno-pic

ldflags-gnu-$(CONFIG_RELAX)     += --direct-data

ldscript-gnu-y          := arch/avr32/link.lds
ldscript-gnu-src-y      := arch/avr32/link.lds.S

ldscript-iar-y          := arch/avr32/link-iar.xcl
ldscript-iar-src-y      := arch/avr32/link-iar.xcl.S

cppflags-iar-y          += -I$(IAR_PATH)/avr32/inc

# Teach the assembler to be silent, and don't warn about non-native line
# endings.
asflags-iar-y           += --silent --diag_suppress Pa050

src-y                   += arch/avr32/entry.S
src-$(CONFIG_AVR32_PM_V2) += arch/avr32/osc_setup.S
src-y                   += arch/avr32/runtime_init.S
src-iar-y               += arch/avr32/iar_stack.S
src-y                   += arch/avr32/delay.c
src-y                   += arch/avr32/intc.c
src-$(CONFIG_SETJMP)    += arch/avr32/setjmp.S
src-$(CONFIG_SOFTIRQ)   += arch/avr32/softirq.c

hdr-y                   += arch/avr32/include/arch/assembler-iar.h
hdr-y                   += arch/avr32/include/arch/atomic.h
hdr-y                   += arch/avr32/include/arch/bitops.h
hdr-y                   += arch/avr32/include/arch/byteorder.h
hdr-y                   += arch/avr32/include/arch/compiler-gcc.h
hdr-y                   += arch/avr32/include/arch/compiler-iar.h
hdr-y                   += arch/avr32/include/arch/intc.h
hdr-y                   += arch/avr32/include/arch/interrupt.h
hdr-y                   += arch/avr32/include/arch/io.h
hdr-$(CONFIG_HUGEMEM)   += arch/avr32/include/arch/hugemem.h
hdr-y                   += arch/avr32/include/arch/progmem.h
hdr-y                   += include/generic/progmem_von_neumann.h
hdr-$(CONFIG_SETJMP)    += arch/avr32/include/arch/setjmp.h
hdr-$(CONFIG_SOFTIRQ)   += arch/avr32/include/arch/softirq.h
hdr-y                   += arch/avr32/include/arch/stdint.h
hdr-y                   += arch/avr32/include/arch/string.h
hdr-y                   += arch/avr32/include/arch/sysreg.h

hdr-$(CONFIG_HUGEMEM)   += include/generic/hugemem.h

include $(src)/arch/avr32/string/subdir.mk
