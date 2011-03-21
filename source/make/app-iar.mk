# Include user configuration if available
-include $(src)/config.mk

ldscript-iar-y  :=
cppflags-iar-y  := $(CPPFLAGS)
asflags-iar-y   := $(ASFLAGS)
cflags-iar-y    := $(CFLAGS)
ldflags-iar-y   := $(LDFLAGS)

CC               = icc$(ARCH)
CPP              = $(CC) --preprocess=n $@
AS               = a$(ARCH)
LD               = xlink

# Pa050: non-native end of line sequence detected (this diagnostic is only
#        issued once)
# Pe188: enumerated type mixed with another type
# Pe111: statement is unreachable
cflags-iar-y    += --diag_suppress Pa050,Pe188,Pe111

include $(src)/make/app-build.mk

ifeq ($(IAR_PATH),)
$(info )
$(info Please specify IAR_AVR_PATH and/or IAR_AVR32_PATH)
$(info in the top-level config.mk file)
$(info )
$(error IAR_PATH is not set)
endif

appimg-y        += $(appimg-iar-y)
clean-files     += $(appimg-y)

_all: $(appimg-y)

obj-y           += $(addsuffix .o,$(basename $(src-y) $(src-iar-y)))

cppflags-iar-y  := $(addprefix -I,$(incdir-y)) $(cppflags-iar-y)

# Always enable debug information and language extensions
cflags-iar-y    += --debug -e

# Cope with seriously weird IAR optimization levels
ifeq ($(CONFIG_OPTIMIZE_LEVEL),1)
IAR_OPTIMIZE_LEVEL      := 3
else ifeq ($(CONFIG_OPTIMIZE_LEVEL),2)
IAR_OPTIMIZE_LEVEL      := 6
else ifeq ($(CONFIG_OPTIMIZE_LEVEL),3)
IAR_OPTIMIZE_LEVEL      := 9
else
IAR_OPTIMIZE_LEVEL      := 2
endif

# Select IAR memory model
#
# IAR supports three different memory models, which alters the size of the
# pointer used in the application and thereby limiting the address space. Set
# by default per chip, but can be overridden by application configuration.
#
# When using the CONFIG_MEMORY_MODEL_* symbols in source code, always check
# from smallest (tiny) to largest (large), as the default defined is the
# largest, while application code might override to additionally define the
# smaller symbols.
ifeq ($(CONFIG_MEMORY_MODEL_TINY),y)
cflags-iar-y            += --memory_model=tiny
else ifeq ($(CONFIG_MEMORY_MODEL_SMALL),y)
cflags-iar-y            += --memory_model=small
else ifeq ($(CONFIG_MEMORY_MODEL_LARGE),y)
cflags-iar-y            += --memory_model=large
endif

optflag-y                       := -s$(IAR_OPTIMIZE_LEVEL)
optflag-$(CONFIG_OPTIMIZE_SIZE) := -z$(IAR_OPTIMIZE_LEVEL)
cflags-iar-y                    += $(optflag-y)

# Don't print lots of uninteresting stuff
cflags-iar-y    += --silent
ldflags-iar-y   += -S

c_flags          = $(cflags-iar-y) $(cppflags-iar-y)
a_flags          = $(asflags-iar-y) $(cppflags-iar-y) -D__ASSEMBLY__
l_flags          = -f $(ldscript-iar-y) $(ldflags-iar-y) $(ldflags-$(@F)-y)
xcl_flags        = $(cppflags-iar-y) --silent --diag_suppress Pa050
xcl_flags       += -D__ASSEMBLY__

quiet_cmd_cc_o_c        = CC      $@
      cmd_cc_o_c        = $(CC) $< $(c_flags) -o $@

ifeq ($(CONFIG_ARCH_AVR),y)
# The IAR assembler for AVR doesn't support --preinclude, so we have to
# do it "manually"...
quiet_cmd_as_o_S        = AS      $@
      cmd_as_o_S        = cat $(config_h) $< | $(AS) $(a_flags) -G -o $@
else
quiet_cmd_as_o_S        = AS      $@
      cmd_as_o_S        = $(AS) $(a_flags) --preinclude $(config_h) $< -o $@
endif
quiet_cmd_cpp_i_c       = CPP     $@
      cmd_cpp_i_c       = $(CPP) $(c_flags) $<
quiet_cmd_cpp_xcl_S     = XCL     $@
      cmd_cpp_xcl_S     = $(CPP) $(xcl_flags) --preinclude $(config_h) $<
quiet_cmd_link          = LD      $@
      cmd_link          = $(LD) $(l_flags) -o $@ $(obj-y) $(ldlibs-iar-y)

%.o: %.c $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cc_o_c)

%.i: %.c $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cpp_i_c)

%.o: %.S $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,as_o_S)

ifneq ($(ldscript-iar-src-y),)
$(ldscript-iar-y): %.xcl: $(src)/%.xcl.S $(config_h) $(mkfiles)
	$(call cmd,mkdir)
	$(call cmd,cpp_xcl_S)
endif

# On IAR, we run a full link with different flags for each object format
$(call target-ldflags,-FELF -yp,$(app-elf))
$(call target-ldflags,-FUBROF8,$(app-dbg))
$(call target-ldflags,-FINTEL-EXTENDED,$(app-hex))

$(appimg-y): $(obj-y) $(ldscript-iar-y)
	$(call cmd,mkdir)
	$(call cmd,link)

include $(src)/make/commands.mk

.PHONY: $(PHONY)
