hdr-y				+= include/stream.h

src-y				+= util/stream/stream_core.c
src-y                           += util/stream/stream_string.c
src-$(CONFIG_DEBUG_CONSOLE)	+= util/stream/debug_console.c

mkfiles				+= $(src)/util/stream/subdir.mk
