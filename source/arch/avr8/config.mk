CROSS_COMPILE		?= avr-
ARCH			:= avr
IAR_PATH		?= $(IAR_AVR_PATH)

CONFIG_ARCH_AVR=y
CONFIG_GENERIC_STRING_IMPL=y

# Defaults which may be overridden by application-specific config file
CONFIG_DEBUG_DWARF2=y
CONFIG_OPTIMIZE_SIZE=y
CONFIG_OPTIMIZE_LEVEL=3
CONFIG_GC_SECTIONS=y
CONFIG_RELAX=y

# Stack configuration
#
# On GCC the CSTACK and RSTACK are combined into a common stack area for both
# data and return stack. While IAR has different segments for each stack type.
#
# Data stack
CONFIG_CSTACK_SIZE=0x80
# Return stack
CONFIG_RSTACK_SIZE=0x20

config_mk		+= $(src)/arch/avr8/config.mk
platform-mkfiles	+= $(src)/arch/avr8/arch.mk
