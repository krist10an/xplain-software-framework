include $(src)/arch/avr8/config.mk

CONFIG_CPU_XMEGA=y
CONFIG_SRAM_BASE=0x2000

CONFIG_SYSCLK_SOURCE=SYSCLK_SRC_RC2MHZ
CONFIG_SYSCLK_PSADIV=XMEGA_CLK_PSADIV_1
CONFIG_SYSCLK_PSBCDIV=XMEGA_CLK_PSBCDIV_1_1

config_mk		+= $(src)/cpu/xmega/config.mk
platform-mkfiles	+= $(src)/cpu/xmega/cpu.mk
