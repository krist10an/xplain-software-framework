hdr-y                   += include/gfx/gfx_hx8347a.h
hdr-y                   += include/gfx/gfx_generic.h

hdr-$(CONFIG_EBI_PARAMS_HX8347A) += drivers/gfx/hx8347a/hx8347a_ebi.h
hdr-y                            += drivers/gfx/hx8347a/hx8347a_regs.h
hdr-$(CONFIG_CPU_XMEGA)          += drivers/gfx/hx8347a/hx8347a_xmega.h

src-y                   += drivers/gfx/hx8347a/gfx_hx8347a.c
src-y                   += drivers/gfx/gfx_generic.c
src-y                   += drivers/gfx/gfx_text.c

mkfiles                 += $(src)/drivers/gfx/hx8347a/subdir.mk
