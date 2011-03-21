incdir-y		+= $(src)/cpu/xmega/include

src-y                   += cpu/xmega/ccp.S
src-$(CONFIG_PHYSMEM)	+= cpu/xmega/physmem_pools.c

hdr-y			+= cpu/xmega/include/cpu/dma.h
hdr-y			+= include/generic/dma_nommu.h
hdr-$(CONFIG_DMAPOOL)	+= cpu/xmega/include/cpu/dmapool.h
hdr-$(CONFIG_DMAPOOL)	+= include/generic/dmapool_nommu.h
hdr-y			+= cpu/xmega/include/cpu/io.h
hdr-$(CONFIG_PHYSMEM)	+= cpu/xmega/include/cpu/physmem.h
hdr-$(CONFIG_PHYSMEM)	+= include/generic/physmem_nommu.h
hdr-y			+= cpu/xmega/include/cpu/regs.h
hdr-y			+= cpu/xmega/include/cpu/sleep.h
hdr-y			+= include/pmic.h
hdr-y			+= include/pmic_regs.h
hdr-$(CONFIG_HAVE_EBI)  += include/regs/xmega_ebi.h
hdr-y			+= cpu/xmega/include/cpu/unaligned.h
hdr-y			+= include/generic/unaligned-direct.h
hdr-$(CONFIG_TOUCH_RESISTIVE)	+= cpu/xmega/include/cpu/touch/resistive/touch.h
hdr-$(CONFIG_TOUCH_RESISTIVE)     += include/regs/xmega_adc.h
