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

PHONY           := all _all
all: _all

build           := build/$(app)/$(CONFIG)/$(TOOLCHAIN)

# Start from a clean slate
subdir-y        :=
src-y           :=
hdr-y           :=
obj-y           :=

incdir-y        :=
mkfiles         :=

include $(src)/make/common.mk

appsrc          ?= $(src)/apps/$(app)

# Convenience variables for returning respectively all .elf, .dbg and
# .hex files that are to be built
app-elf         = $(filter %.elf,$(appimg-y))
app-dbg         = $(filter %.dbg,$(appimg-y))
app-hex         = $(filter %.hex,$(appimg-y))
app-bin         = $(filter %.bin,$(appimg-y))

# Look for source files relative to the top-level source directory
VPATH           := $(src)

# The list of config files in $(config_mk) is built mostly by the config files
# themselves. Later entries override the ones that come before.
config_mk       :=

include $(appsrc)/config-$(CONFIG).mk

# Add the application-specific config file; it is always the last entry.
config_mk       += $(appsrc)/config-$(CONFIG).mk

config_h        := include/autoconf.h

quiet_cmd_genconfig = CONF    $@
      cmd_genconfig = cat $^ | $(src)/make/genconfig.sh > $@ || rm -f $@

$(config_h): $(config_mk)
	$(call cmd,mkdir)
	$(call cmd,genconfig)

mkfiles         += $(config_mk)

# First, config.h and other generated header files
cppflags-gnu-y  += -include $(config_h) -Iinclude
cflags-iar-y    += --preinclude $(config_h)
cppflags-iar-y  += -Iinclude

include $(appsrc)/$(app).mk
mkfiles         += $(appsrc)/$(app).mk
include $(src)/make/app-subdirs.mk
subdir-mkfiles  := $(patsubst %,$(src)/%/subdir.mk,$(subdir-y))
include $(subdir-mkfiles)

mkfiles         += $(src)/make/app-build.mk

# Then, chip- and arch-specific header files
include $(platform-mkfiles)

mkfiles         += $(platform-mkfiles)

all-targets      = $(obj-y) $(ldscript-gnu-y)
clean-files     += $(app).s $(obj-y) $(obj-y:.o=.s)
clean-files     += $(config_h) $(ldscript-gnu-y)

# Finally, generic header files
incdir-y        += $(src)/include

doc-y           += LICENSE

doxyfile_sedflags        = -e 's,@PROJECT_NAME@,$(app),'
doxyfile_sedflags       += -e 's,@INPUT@,$(src-y) $(hdr-y) $(doc-y),'
doxyfile_sedflags       += -e 's,@OUTPUT@,$(build)/doc,'

quiet_cmd_gendoxyfile = DOX     $@
      cmd_gendoxyfile = sed $(doxyfile_sedflags) < $< > $@ || rm -f $@

PHONY           += docs
docs: Doxyfile
	cd $(src); doxygen $(build)/$<

Doxyfile: $(src)/make/Doxyfile.in $(mkfiles)
	$(call cmd,gendoxyfile)

PHONY           += clean
clean:
	$(call cmd,rmfiles)

# Build system
make-files       = make/app.mk make/app-subdirs.mk make/commands.mk
make-files      += make/common.mk make/genconfig.sh make/Makefile.app.in
make-files      += make/app-build.mk make/app-gnu.mk make/app-iar.mk
make-files      += make/avr32program.mk make/avrdude.mk
archive-files    = $(addprefix $(src)/,$(make-files))

# Other makefiles
archive-files   += $(appsrc)/Makefile
archive-files   += $(mkfiles)
archive-files   += make/Doxyfile.in

# Source and header files defined by makefiles around the tree
archive-files   += $(src-y) $(src-gnu-y) $(src-iar-y)
archive-files   += $(hdr-y) $(hdr-gnu-y) $(hdr-iar-y)
archive-files   += $(ldscript-gnu-src-y)

quiet_cmd_tar_archive = TAR     $@
      cmd_tar_archive = rm -rf tmp-archive;                             \
			mkdir -p tmp-archive/$(app);                    \
			tar cC $(src) -T $< | tar xC tmp-archive/$(app); \
			tar cjf $@ -Ctmp-archive $(app);                \
			rm -rf tmp-archive

PHONY           += tar-archive
tar-archive: $(app).tar.bz2

# Archives need to be updated both when the list of files changes as
# well as when the files themselves change
$(app).tar.bz2: filelist.txt $(archive-files)
	$(call cmd,tar_archive)

quiet_cmd_filelist = FILES   filelist.txt
      cmd_filelist = $(src)/make/gen-filelist.sh                \
		     $(patsubst $(src)/%,%,$(archive-files))

PHONY           += archive-check
archive-check: $(app).tar.bz2
	mkdir tmp-archive
	tar xjvf $< -C tmp-archive
	make -C tmp-archive/$(app)/apps/$(app) CONFIG="$(CONFIG)"       \
		TOOLCHAIN="$(TOOLCHAIN)"
	rm -rf tmp-archive

PHONY           += filelist
filelist: filelist.txt

filelist.txt: $(archive-files) FORCE
	$(call cmd,filelist)

PHONY           += help
help:
	@echo ""
	@echo "Usage: make [CONFIG_VARIABLE=VALUE]... [TARGET]..."
	@echo ""
	@echo "Available targets:"
	@echo "  all             Default target if no target is specified."
	@echo "                  This will build the default configuration"
	@echo "                  for this application."
	@echo "  docs            Generate documentation for the selected"
	@echo "                  configuration."
	@echo "  tar-archive     Pack all application files into an archive."
	@echo "  archive-check   Pack all application files into an archive,"
	@echo "                  unpack them again and build the application"
	@echo "                  from the archive files."
	@echo "  clean           Delete ELF file and all intermediate build"
	@echo "                  products."
	@echo "  program         Build the ELF file and program it onto the"
	@echo "                  flash on the target."
	@echo "  run             Start executing code on the target."
	@echo "  reset           Reset the target."
	@echo "  help            This help text."
	@echo ""
	@echo "Build system configuration variables:"
	@echo "  CONFIG          Configuration to build. If none is given the"
	@echo "                  default configuration is used. Each"
	@echo "                  configuration needs a corresponding config"
	@echo "                  file in the application directory. E.g. to"
	@echo "                  build the config-atevk1104.mk configuration,"
	@echo "                  run 'make CONFIG=atevk1104'."
	@echo "  V               Set to 1 for verbose output."
	@echo "  TOOLCHAIN       Toolchain to use for the build, GNU or IAR."
	@echo "                  The default toolchain is specified in the"
	@echo "                  application Makefile."
	@echo "  IAR_PATH        Path to IAR EWB installation."
	@echo ""
	@echo "Further information is available in doc/build_system_doc.c"
	@echo ""

