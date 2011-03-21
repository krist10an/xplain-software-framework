doc-subdir-$(CONFIG_USB)	+= $(src)/doc/usb

doc-y				+= doc/build_system_doc.c
doc-y				+= doc/clk_doc.c
doc-y				+= doc/impl_notes.c
doc-y                           += doc/mem_allocators_doc.c

include $(addsuffix /subdir.mk,$(doc-subdir-y))

mkfiles				+= $(src)/doc/subdir.mk
