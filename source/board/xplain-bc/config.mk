include $(src)/chip/at90usb1287/config.mk

CONFIG_BOARD_XPLAIN=y

config_mk		+= $(src)/board/xplain-bc/config.mk
platform-mkfiles	+= $(src)/board/xplain-bc/board.mk
