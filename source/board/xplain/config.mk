include $(src)/chip/atxmega128a1/config.mk

CONFIG_BOARD_XPLAIN=y

CONFIG_EXTRAM_BASE=0x800000
CONFIG_EXTRAM_END=0xFFFFFF

config_mk		+= $(src)/board/xplain/config.mk
platform-mkfiles	+= $(src)/board/xplain/board.mk
