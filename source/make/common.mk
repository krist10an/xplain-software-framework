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

# Definitions common to all makefiles

# Don't use make's built-in rules and variables
MAKEFLAGS       += -rR

# Don't print 'Entering directory ...'
MAKEFLAGS       += --no-print-directory

# Look for make include files relative to the top-level source directory
MAKEFLAGS       += --include-dir=$(src)

#
# Hide command output by default, but allow the user to override this
# by adding V=1 on the command line.
# 
# This is inspired by the Kbuild system used by the Linux kernel.
#
ifdef V
  ifeq ("$(origin V)", "command line")
    VERBOSE = $(V)
  endif
endif
ifndef VERBOSE
  VERBOSE = 0
endif

ifeq ($(VERBOSE), 1)
  quiet =
  Q =
else
  quiet = quiet_
  Q = @
endif

# Characters that can't be used in a straightforward manner
comma           := ,
squote          := '
# Make editors happy: '
empty           :=
space           := $(empty) $(empty)

# Escape single quotes
escsq           = $(subst $(squote),'\$(squote)', $1)

prepend         = $(dir $(2))$(1)$(notdir $(2))
depfile         = $(subst $(comma),_,$(call prepend,.,$@).d)

# Echo the 'quiet' version of a command
echo-cmd        = $(if $($(quiet)cmd_$(1)),     \
	echo '  $(call escsq,$($(quiet)cmd_$(1)))';)

# Run a command, echoing the 'quiet' version or the command itself,
# depending on the setting of KBUILD_VERBOSE.
cmd = @$(echo-cmd) $(cmd_$(1))

# Set ldflags specific to a given target
target-ldflags  = $(eval $(foreach tgt,$(2),ldflags-$(tgt)-y += $(1)))

# Create a directory if it doesn't already exist.
quiet_cmd_mkdir = $(if $(wildcard $(dir $@)),,MKDIR   $(dir $@))
      cmd_mkdir = $(if $(wildcard $(dir $@)),,mkdir -p $(dir $@))

PHONY           += FORCE
FORCE:
