hdr-y				+= include/block/device.h
hdr-$(CONFIG_BLOCK_DATAFLASH)	+= include/block/dataflash.h
hdr-$(CONFIG_BLOCK_DUMMY)	+= include/block/dummy.h
hdr-$(CONFIG_BLOCK_PARTITION)	+= include/block/partition.h
hdr-$(CONFIG_BLOCK_PART_MBR)	+= include/block/partition_mbr.h
hdr-$(CONFIG_BLOCK_RAMDISK)	+= include/block/ramdisk.h

hdr-$(CONFIG_BLOCK_DATAFLASH)	+= include/flash/at45.h

src-y				+= drivers/block/block_core.c
src-$(CONFIG_BLOCK_DATAFLASH)	+= drivers/block/dataflash.c
src-$(CONFIG_BLOCK_DUMMY)	+= drivers/block/dummy.c
src-$(CONFIG_BLOCK_PART_MBR)	+= drivers/block/partition_mbr.c
src-$(CONFIG_BLOCK_RAMDISK)	+= drivers/block/ramdisk.c

mkfiles				+= $(src)/drivers/block/subdir.mk
