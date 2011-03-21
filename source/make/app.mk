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

.PHONY: all
all: delegate

include $(src)/make/common.mk

appsrc          ?= $(src)/apps/$(app)
appsrc-rel      := $(patsubst $(src)/%,%,$(appsrc))

ifeq ($(CONFIG),)
  CONFIG        := $(DEFAULT_CONFIG)
endif
ifeq ($(TOOLCHAIN),)
  TOOLCHAIN     := $(DEFAULT_TOOLCHAIN)
endif

config_mk       := $(appsrc)/config-$(CONFIG).mk

include $(config_mk)

obj             := $(src)/build/$(app)/$(CONFIG)/$(TOOLCHAIN)

.DEFAULT: delegate

delegate-targets := clean docs program reset run filelist
delegate-targets += archive-check tar-archive help

.PHONY: $(delegate-targets)
$(delegate-targets): delegate

.PHONY: delegate
delegate: $(obj)/Makefile
	$(Q)$(MAKE) -C $(obj) $(MAKECMDGOALS)

genmakefile-flags       = -e 's%@APP@%$(app)%g' -e 's%@SRCTREE@%../../../..%'
genmakefile-flags       += -e 's%@CONFIG@%$(CONFIG)%g'
genmakefile-flags       += -e 's%@TOOLCHAIN@%$(TOOLCHAIN)%g'
genmakefile-flags       += -e 's%@APPSRC@%$$(src)/$(appsrc-rel)%g'
quiet_cmd_genmakefile   = GEN     $@
      cmd_genmakefile   = sed $(genmakefile-flags) $< > $@ || rm -f $@

$(obj)/Makefile: $(src)/make/Makefile.app.in
	$(call cmd,mkdir)
	$(call cmd,genmakefile)
