include $(src)/arch/avr32/config.mk

CONFIG_SUBARCH_AVR32A=y
CONFIG_CPU_UC3=y
CONFIG_HSB=y

config_mk		+= $(src)/cpu/uc3/config.mk
platform-mkfiles	+= $(src)/cpu/uc3/cpu.mk
