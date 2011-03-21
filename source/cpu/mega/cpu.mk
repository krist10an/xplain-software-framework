incdir-y		+= $(src)/cpu/mega/include

src-$(CONFIG_PHYSMEM)	+= cpu/mega/physmem_pools.c

hdr-y			+= cpu/mega/include/cpu/dma.h
hdr-y			+= include/generic/dma_nommu.h
hdr-$(CONFIG_DMAPOOL)	+= cpu/mega/include/cpu/dmapool.h
hdr-$(CONFIG_DMAPOOL)	+= include/generic/dmapool_nommu.h
hdr-y			+= cpu/mega/include/cpu/io.h
hdr-$(CONFIG_PHYSMEM)	+= cpu/mega/include/cpu/physmem.h
hdr-$(CONFIG_PHYSMEM)	+= include/generic/physmem_nommu.h
hdr-y			+= cpu/mega/include/cpu/regs.h
hdr-y			+= cpu/mega/include/cpu/sleep.h
hdr-y			+= cpu/mega/include/cpu/unaligned.h
hdr-y			+= include/generic/unaligned-direct.h
