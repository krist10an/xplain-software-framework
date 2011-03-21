src-$(CONFIG_CPU_XMEGA)         += drivers/clk/xmega_pr.c

hdr-$(CONFIG_HAVE_GENCLK)	+= include/clk/genclk.h
hdr-$(CONFIG_HAVE_OSC)		+= include/clk/osc.h
hdr-$(CONFIG_HAVE_PLL)		+= include/clk/pll.h
hdr-y				+= include/clk/sys.h
hdr-$(CONFIG_AVR32_PM_V2)	+= include/clk/avr32_pm/osc.h
hdr-$(CONFIG_AVR32_PM_V2)	+= include/clk/avr32_pm/pll.h
hdr-$(CONFIG_AVR32_PM_V2)	+= include/clk/avr32_pm/genclk.h
hdr-$(CONFIG_CPU_XMEGA)         += include/clk/xmega/osc.h
hdr-$(CONFIG_AVR32_PM_V2)	+= include/regs/avr32_pm_v2.h
hdr-$(CONFIG_CPU_XMEGA)         += include/regs/xmega_clk.h
hdr-$(CONFIG_CPU_XMEGA)         += include/regs/xmega_osc.h
hdr-$(CONFIG_CPU_XMEGA)         += include/regs/xmega_pr.h

mkfiles				+= $(src)/drivers/clk/subdir.mk
