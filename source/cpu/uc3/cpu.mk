incdir-y		+= $(src)/cpu/uc3/include

src-gnu-y		+= cpu/uc3/intc_entry.S

src-$(CONFIG_PHYSMEM)	+= cpu/uc3/physmem_pools.c

hdr-y			+= cpu/uc3/include/cpu/addrspace.h
hdr-y			+= cpu/uc3/include/cpu/dma.h
hdr-y			+= include/generic/dma_nommu.h
hdr-$(CONFIG_DMAPOOL)	+= cpu/uc3/include/cpu/dmapool.h
hdr-$(CONFIG_DMAPOOL)	+= include/generic/dmapool_nommu.h
hdr-y			+= cpu/uc3/include/cpu/intc.h
hdr-$(CONFIG_PHYSMEM)	+= cpu/uc3/include/cpu/physmem.h
hdr-$(CONFIG_PHYSMEM)	+= include/generic/physmem_nommu.h
hdr-y			+= cpu/uc3/include/cpu/sleep.h
hdr-y			+= cpu/uc3/include/cpu/sysreg.h
hdr-y			+= cpu/uc3/include/cpu/unaligned.h

include $(src)/cpu/uc3/string/subdir.mk
