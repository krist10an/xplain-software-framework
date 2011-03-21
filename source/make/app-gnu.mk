# Include user configuration if available
-include $(src)/config.mk

ldscript-gnu-y  :=
cppflags-gnu-y  := $(CPPFLAGS)
asflags-gnu-y   := $(ASFLAGS)
cflags-gnu-y    := $(CFLAGS)
ldflags-gnu-y   := $(LDFLAGS)

CC               = $(CROSS_COMPILE)gcc
CPP              = $(CC) -E
AS               = $(CROSS_COMPILE)as
LD               = $(CROSS_COMPILE)ld
OBJDUMP          = $(CROSS_COMPILE)objdump
OBJCOPY          = $(CROSS_COMPILE)objcopy
SIZE             = $(CROSS_COMPILE)size
CHECK            = sparse

# Call this to set OBJCOPYFLAGS for a given target
target-objcopyflags = $(eval $(foreach tgt,$(2),OBJCOPYFLAGS_$(tgt) += $(1)))

# Always enable warnings. And be very careful about implicit
# declarations
cflags-gnu-y    += -Wall -Wstrict-prototypes -Wmissing-prototypes
cflags-gnu-y    += -Werror-implicit-function-declaration
# IAR doesn't allow arithmetic on void pointers, so warn about that
cflags-gnu-y    += -Wpointer-arith

include $(src)/make/app-build.mk

appimg-y        += $(appimg-gnu-y)
clean-files     += $(appimg-y)

_all: $(appimg-y)

obj-y           += $(addsuffix .o,$(basename $(src-y) $(src-gnu-y)))
all-targets     := $(obj-y) $(ldscript-gnu-y)

cppflags-gnu-y  += $(addprefix -I,$(incdir-y))
libgcc          := $(shell $(CC) $(cflags-gnu-y) -print-libgcc-file-name)
ldlibs-gnu-y    += "$(libgcc)"

gcc-dir          = "$(dir $(libgcc))"
checkflags-y    := -Wall -I$(gcc-dir)/include $(cppflags-gnu-y)

# Either optimize for speed at the specified level or optimize for size
optflag-y                               := -O$(CONFIG_OPTIMIZE_LEVEL)
optflag-$(CONFIG_OPTIMIZE_SIZE)         := -Os
dbgflag-y                               := -g
dbgflag-$(CONFIG_DEBUG_DWARF2)          := -gdwarf-2

cflags-gnu-y                            += $(optflag-y) $(dbgflag-y)
cflags-gnu-$(CONFIG_RELAX)              += -mrelax
asflags-gnu-$(CONFIG_RELAX)             += -mrelax
ldflags-gnu-$(CONFIG_RELAX)             += --relax
cflags-gnu-$(CONFIG_GC_SECTIONS)        += -ffunction-sections -fdata-sections
ldflags-gnu-$(CONFIG_GC_SECTIONS)       += --gc-sections

depflags         = -Wp,-MD,$(depfile) -MQ $@ -MP
c_flags          = $(depflags) $(cppflags-gnu-y) $(cflags-gnu-y)
a_flags          = $(depflags) $(cppflags-gnu-y) $(asflags-gnu-y) -D__ASSEMBLY__
l_flags          = -T $(ldscript-gnu-y) $(ldflags-gnu-y) -Map $@.map --cref

quiet_cmd_cc_o_c        = CC      $@
      cmd_cc_o_c        = $(CC) $(c_flags) -c -o $@ $<
quiet_cmd_cc_s_c        = CC      $@
      cmd_cc_s_c        = $(CC) $(c_flags) -fverbose-asm -S -o $@ $<
quiet_cmd_as_o_S        = AS      $@
      cmd_as_o_S        = $(CC) $(a_flags) -c -o $@ $<
quiet_cmd_cpp_i_c       = CPP     $@
      cmd_cpp_i_c       = $(CPP) $(c_flags) -o $@ $<
quiet_cmd_cpp_lds_S     = LDS     $@
      cmd_cpp_lds_S     = $(CPP) -xassembler-with-cpp $(a_flags) -P -o $@ $<
quiet_cmd_link          = LD      $@
      cmd_link          = $(LD) $(l_flags) -o $@ $(obj-y) $(ldlibs-y) $(ldlibs-gnu-y)

quiet_cmd_check         = CHECK   $<
      cmd_check         = $(CHECK) $(checkflags-y) $<

%.o: %.c $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cc_o_c)
ifeq ($(CONFIG_SPARSE),y)
	$(call cmd,check)
endif

%.i: %.c $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cpp_i_c)

%.s: %.c $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cc_s_c)

%.o: %.S $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,as_o_S)

%.lds: %.lds.S $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cpp_lds_S)

# On GNU, we always build an ELF file and use objcopy to generate
# different formats from it
$(app-elf): %.elf: $(obj-y) $(ldscript-gnu-y)
	$(call cmd,mkdir)
	$(call cmd,link)
	$(Q)$(SIZE) $@
	$(Q)cp $@ $(src)/apps/$(app)/$@

$(call target-objcopyflags,-O ihex,$(app-hex))
$(app-hex): %.hex: %.elf
	$(call cmd,objcopy)

$(call target-objcopyflags,-O binary,$(app-bin))
$(app-bin): %.bin: %.elf
	$(call cmd,objcopy)

# Disassembling the final binary
$(app).s: $(app-elf)
	$(OBJDUMP) -d $< > $@ || rm -f $@

include $(src)/make/commands.mk

dep-files := $(wildcard $(foreach f,$(all-targets),$(dir $(f)).$(notdir $(f)).d))
clean-files += $(dep-files)

include $(dep-files)

.PHONY: $(PHONY)
