# Set global options according to configuration settings
ifneq ($(CONFIG_PROGRAMMER),)
 avr32program-flags-y                           += -p $(CONFIG_PROGRAMMER)
endif
ifneq ($(CONFIG_PROG_PORT),)
 avr32program-flags-y                           += -c $(CONFIG_PROG_PORT)
endif

# Set options specific to programming
avr32program-prog-flags-$(CONFIG_PROG_VERIFY)   += -v

# Add user-specified flags
avr32program-flags-y            += $(PROGTOOL_FLAGS)
avr32program-prog-flags-y       += $(PROGTOOL_PROGRAM_FLAGS)

# Shortcuts for the commands below
progtool-flags                   = $(avr32program-flags-y)
program-flags                    = -e $(avr32program-prog-flags-y)

cmd_program = $(PROGTOOL) $(progtool-flags) program $(program-flags) $<
cmd_reset = $(PROGTOOL) $(progtool-flags) reset
cmd_run = $(PROGTOOL) $(progtool-flags) run
