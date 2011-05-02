gfx-subdir-$(CONFIG_GFX_HX8347A)        += $(src)/drivers/gfx/hx8347a

include $(addsuffix /subdir.mk, $(gfx-subdir-y))

src-y                   += drivers/gfx/gfx_bitmap.c
src-y                   += drivers/gfx/gfx_gradient.c

hdr-y                   += include/gfx/gfx.h

mkfiles                 += $(src)/drivers/gfx/subdir.mk
