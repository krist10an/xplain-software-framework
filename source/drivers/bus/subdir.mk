hdr-$(CONFIG_EBI)               += include/bus/ebi/core.h
hdr-$(CONFIG_EBI_SRAM)          += include/bus/ebi/sram.h
hdr-$(CONFIG_EBI_PARAMS_HX8347A) += include/bus/ebi/params/hx8347a.h
hdr-$(CONFIG_HAVE_HSB)          += include/bus/hsb.h
hdr-$(CONFIG_HAVE_HSB)          += include/regs/hmatrix.h

mkfiles                         += $(src)/drivers/bus/subdir.mk
