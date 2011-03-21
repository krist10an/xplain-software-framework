CROSS_COMPILE		?= avr32-
ARCH			:= avr32
IAR_PATH		?= $(IAR_AVR32_PATH)

CONFIG_ARCH_AVR32=y
CONFIG_GENERIC_STRING_IMPL=y

# Defaults which may be overridden by application-specific config file
CONFIG_DEBUG_DWARF2=y
CONFIG_OPTIMIZE_SIZE=y
CONFIG_OPTIMIZE_LEVEL=3
CONFIG_GC_SECTIONS=y
CONFIG_RELAX=y

# Default stack size in bytes
CONFIG_STACK_SIZE=0x400

config_mk		+= $(src)/arch/avr32/config.mk
platform-mkfiles	+= $(src)/arch/avr32/arch.mk
