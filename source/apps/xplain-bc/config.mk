CONFIG_BLOCK=y
CONFIG_BLOCK_FIXED_BLOCK_SIZE=512
CONFIG_BUFFER=y
CONFIG_DMAPOOL=y
CONFIG_DMAPOOL_GENERIC_POOLS=y
CONFIG_MAINLOOP=y
CONFIG_MALLOC_SIMPLE=y
CONFIG_MEMPOOL=y
CONFIG_PHYSMEM=y
CONFIG_SOFTIRQ=y

CONFIG_USB=y
CONFIG_USB_DEV_MUX=y
CONFIG_UDC=y
CONFIG_UDI_MSC_BULK=y
CONFIG_UDI_MSC_REMOVABLE=y

config_mk	+= $(appsrc)/config.mk
