# Set global options according to configuration settings
ifneq ($(CONFIG_PROGRAMMER),)
 ifeq ($(CONFIG_PROGRAMMER),jtagicemkii)
  avrdude-flags-y               += -c jtagmkii
 else
  avrdude-flags-y               += -c $(CONFIG_PROGRAMMER)
 endif
endif
ifneq ($(CONFIG_PROG_PORT),)
 avrdude-flags-y                += -P $(CONFIG_PROG_PORT)
endif

# Add user-specified flags
avrdude-flags-y                 += $(PROGTOOL_FLAGS)
avrdude-prog-flags-y            += $(PROGTOOL_PROGRAM_FLAGS)

# avrdude doesn't support ELF, so we need to override the prerequisite
prog-img                        := $(app-hex)

# Shortcuts for the commands below
progtool-flags                   = $(avrdude-flags-y)
program-flags                    = $(progtool-flags) $(avrdude-prog-flags-y)

cmd_program = $(PROGTOOL) $(program-flags) -U flash:w:$<
cmd_reset = $(PROGTOOL) $(progtool-flags)
cmd_run = $(PROGTOOL) $(progtool-flags)
