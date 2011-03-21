include $(src)/chip/at32uc3a3/config.mk

CONFIG_BOARD_ATEVK1104=y

CONFIG_EBI=y
CONFIG_EBI_SRAM=y
CONFIG_EBI_PARAMS_HX8347A=y

config_mk		+= $(src)/board/atevk1104/config.mk
platform-mkfiles	+= $(src)/board/atevk1104/board.mk
