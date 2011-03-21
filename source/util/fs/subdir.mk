hdr-$(CONFIG_FS_TSFS)           += include/fs/tsfs.h

src-$(CONFIG_FS_TSFS)           += util/fs/tsfs.c

mkfiles                         += $(src)/util/fs/subdir.mk
