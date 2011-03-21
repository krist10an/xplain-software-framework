# If PROGTOOL hasn't been set by the user, try AVR_PROGTOOL. If none
# of them have been set, default to avrdude.
AVR_PROGTOOL            ?= avrdude
PROGTOOL                ?= $(AVR_PROGTOOL)

# GNU supports ELF images, IAR supports UBROF of various kinds. Both
# support HEX
appimg-gnu-y            += $(app).elf
appimg-iar-y            += $(app).dbg
appimg-y                += $(app).hex

incdir-y                += $(src)/arch/avr8/include
cflags-gnu-y            += -fshort-enums

ldscript-gnu-y          := arch/avr8/link.lds
ldscript-gnu-src-y      := arch/avr8/link.lds.S

cppflags-iar-y          += -I$(IAR_PATH)/avr/inc -I$(IAR_PATH)/avr/inc/dlib
cppflags-iar-y          += -DENABLE_BIT_DEFINITIONS
cflags-iar-y            += -y --initializers_in_flash
cflags-iar-y            += --eeprom_size=$(CONFIG_EEPROM_SIZE)
asflags-iar-y           += -v$(CONFIG_AVR_CORE)

# Teach the assembler to be silent, and don't warn about non-native line
# endings.
asflags-iar-y           += -S -wPa050

INTVEC_SIZE         := $(shell (echo "obase = 16; 4 * $(CONFIG_NR_IRQS)" | bc))

APPLICATION_LMA_HEX := $(shell (echo $(CONFIG_APPLICATION_LMA) | sed -e s/0x//))
CSTACK_SIZE_HEX     := $(shell (echo $(CONFIG_CSTACK_SIZE)     | sed -e s/0x//))
EEPROM_START_HEX    := $(shell (echo $(CONFIG_EEPROM_START)    | sed -e s/0x//))
EEPROM_END_HEX      := $(shell (echo $(CONFIG_EEPROM_END)      | sed -e s/0x//))
FLASH_END_HEX       := $(shell (echo $(CONFIG_FLASH_END)       | sed -e s/0x//))
FLASH_NEND_HEX      := $(shell (echo $(CONFIG_FLASH_NEND)      | sed -e s/0x//))
RSTACK_SIZE_HEX     := $(shell (echo $(CONFIG_RSTACK_SIZE)     | sed -e s/0x//))
SRAM_BASE_HEX       := $(shell (echo $(CONFIG_SRAM_BASE)       | sed -e s/0x//))
SRAM_END_HEX        := $(shell (echo $(CONFIG_SRAM_END)        | sed -e s/0x//))
SRAM_TBASE_HEX      := $(shell (echo $(CONFIG_SRAM_TBASE)      | sed -e s/0x//))
SRAM_TSIZE_HEX      := $(shell (echo $(CONFIG_SRAM_TSIZE)      | sed -e s/0x//))

CSTACK_BASE_INT := $(shell (echo "ibase = 16; $(SRAM_END_HEX) -                \
			$(CSTACK_SIZE_HEX) - $(RSTACK_SIZE_HEX) + 1" | bc))
CSTACK_END_INT  := $(shell (echo "ibase = 16; $(SRAM_END_HEX) -                \
			$(RSTACK_SIZE_HEX)" | bc))
RSTACK_BASE_INT := $(shell (echo "ibase = 16; $(SRAM_END_HEX) -                \
			$(RSTACK_SIZE_HEX) + 1" | bc))

CSTACK_BASE     := $(shell (echo "obase = 16; $(CSTACK_BASE_INT)" | bc))
CSTACK_END      := $(shell (echo "obase = 16; $(CSTACK_END_INT)"  | bc))
RSTACK_BASE     := $(shell (echo "obase = 16; $(RSTACK_BASE_INT)" | bc))

ldflags-iar-y   += -D_..X_INTVEC_SIZE=$(INTVEC_SIZE)
ldflags-iar-y   += -D_..X_FLASH_BASE=_..X_INTVEC_SIZE
ldflags-iar-y   += -D_..X_FLASH_NEND=$(FLASH_NEND_HEX)
ldflags-iar-y   += -D_..X_FLASH_END=$(FLASH_END_HEX)
ldflags-iar-y   += -D_..X_RSTACK_SIZE=$(RSTACK_SIZE_HEX)
ldflags-iar-y   += -D_..X_CSTACK_SIZE=$(CSTACK_SIZE_HEX)
ldflags-iar-y   += -D_..X_SRAM_BASE=$(SRAM_BASE_HEX)
ldflags-iar-y   += -D_..X_SRAM_END=$(SRAM_END_HEX)
ldflags-iar-y   += -D_..X_SRAM_TBASE=$(SRAM_TBASE_HEX)
ldflags-iar-y   += -D_..X_SRAM_TSIZE=$(SRAM_TSIZE_HEX)
ldflags-iar-y   += -D_..X_EEPROM_END=$(EEPROM_END_HEX)
ldflags-iar-y   += -D_..X_EEPROM_START=$(EEPROM_START_HEX)
ldflags-iar-y   += -D_..X_CSTACK_BASE=$(CSTACK_BASE)
ldflags-iar-y   += -D_..X_CSTACK_END=$(CSTACK_END)
ldflags-iar-y   += -D_..X_RSTACK_BASE=$(RSTACK_BASE)
ldflags-iar-y   += -D_..X_RSTACK_END=$(SRAM_END_HEX)

ldflags-iar-y   += -s __program_start

src-gnu-y               += arch/avr8/entry.S
src-y                   += arch/avr8/delay.c
src-$(CONFIG_SETJMP)    += arch/avr8/setjmp.S
src-$(CONFIG_HUGEMEM)   += arch/avr8/arch_hugemem.c

hdr-y                   += arch/avr8/include/arch/assembler-iar.h
hdr-y                   += arch/avr8/include/arch/atomic.h
hdr-y                   += arch/avr8/include/arch/bitops.h
hdr-y                   += arch/avr8/include/arch/byteorder.h
hdr-y                   += arch/avr8/include/arch/compiler-gcc.h
hdr-y                   += arch/avr8/include/arch/compiler-iar.h
hdr-y                   += arch/avr8/include/arch/intc.h
hdr-y                   += arch/avr8/include/arch/interrupt.h
hdr-y                   += arch/avr8/include/arch/io.h
hdr-$(CONFIG_HUGEMEM)   += arch/avr8/include/arch/hugemem.h
hdr-y                   += arch/avr8/include/arch/progmem.h
hdr-$(CONFIG_SETJMP)    += arch/avr8/include/arch/setjmp.h
hdr-$(CONFIG_SOFTIRQ)   += arch/avr8/include/arch/softirq.h
hdr-y                   += arch/avr8/include/arch/stdint.h
hdr-y                   += arch/avr8/include/arch/string.h
