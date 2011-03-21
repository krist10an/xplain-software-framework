# Copyright (C) 2008, Atmel Corporation All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. The name of ATMEL may not be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
# SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Compression
quiet_cmd_gzip = GZIP    $@
      cmd_gzip = gzip -f -9 < $> > $@

# Objcopy
quiet_cmd_objcopy = OBJCOPY $@
      cmd_objcopy = $(OBJCOPY) $(OBJCOPYFLAGS) $(OBJCOPYFLAGS_$(@F)) $< $@

# Remove all files in $(clean-files)
quiet_cmd_rmfiles = $(if $(wildcard $(clean-files)),CLEAN   $(wildcard $(clean-files)))
      cmd_rmfiles = rm -f $(clean-files)

# Memory Programming
quiet_cmd_program = PROG    $<
quiet_cmd_reset = RESET
quiet_cmd_run = RUN

ifeq ($(PROGTOOL),avr32program)
include $(src)/make/avr32program.mk
else ifeq ($(PROGTOOL),avrdude)
include $(src)/make/avrdude.mk
else
cmd_program = echo Unknown programmer tool: $(PROGTOOL); \
	echo Please verify the PROGTOOL variable
cmd_reset = $(cmd_program)
cmd_run = $(cmd_program)
endif

# May be overridden by programming tool makefile
prog-img        ?= $(app-elf)
PHONY           += program reset run
program: $(prog-img)
	$(call cmd,program)
reset:
	$(call cmd,reset)
run:
	$(call cmd,run)
