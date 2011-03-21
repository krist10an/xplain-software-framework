src-y			+= util/string/generic_memcpy.c
src-y			+= util/string/generic_memset.c
src-y			+= util/string/generic_strlen.c
src-y			+= util/string/generic_strcmp.c

hdr-y			+= include/generic/string.h

mkfiles			+= $(src)/util/string/subdir.mk
