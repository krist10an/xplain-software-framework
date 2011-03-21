src-avr32-$(CONFIG_HAVE_TC)	+= drivers/tc/timer/tc_timer_avr32.c
src-avr32-$(CONFIG_HAVE_TC)	+= drivers/tc/tc_ints_avr32.c

src-$(CONFIG_ARCH_AVR32)	+= $(src-avr32-y)

src-xmega-tc-$(CONFIG_HAVE_TC)   = drivers/tc/timer/tc_timer_xmega.c
src-$(CONFIG_CPU_XMEGA)         += $(src-xmega-tc-y)

hdr-avr32-$(CONFIG_HAVE_TC)	+= include/timer/tc_timer_avr32.h
hdr-avr32-$(CONFIG_HAVE_TC)	+= include/tc/tc_ints_avr32.h
hdr-avr32-$(CONFIG_HAVE_TC)	+= include/tc/tc_avr32.h

hdr-y				+= include/timer.h
hdr-$(CONFIG_ARCH_AVR32)	+= include/timer/timer_avr32.h
hdr-$(CONFIG_ARCH_AVR32)	+= $(hdr-avr32-y)

hdr-xmega-tc-$(CONFIG_HAVE_TC)   = include/timer/tc_timer_xmega.h
hdr-xmega-tc-$(CONFIG_HAVE_TC)  += include/regs/xmega_tc.h
hdr-xmega-tc-$(CONFIG_HAVE_TC)  += include/tc/tc_xmega.h
hdr-$(CONFIG_CPU_XMEGA)         += $(hdr-xmega-tc-y)
hdr-$(CONFIG_CPU_XMEGA)         += include/timer/timer_xmega.h

mkfiles                         += $(src)/drivers/tc/timer/subdir.mk
