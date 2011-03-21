include $(src)/arch/avr8/config.mk

CONFIG_CPU_MEGA=y

config_mk		+= $(src)/cpu/mega/config.mk
platform-mkfiles	+= $(src)/cpu/mega/cpu.mk
