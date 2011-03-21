usb-src-$(CONFIG_UDI_CDC_ACM)	+= udi/cdc_acm.c
usb-src-$(CONFIG_UDI_MSC_BULK)	+= udi/msc_bulk.c

hdr-$(CONFIG_UDI_CDC_ACM)	+= include/usb/cdc_protocol.h
hdr-$(CONFIG_UDI_CDC_ACM)	+= include/usb/udi_cdc.h
hdr-$(CONFIG_UDI_MSC_BULK)	+= include/scsi/cdb.h
hdr-$(CONFIG_UDI_MSC_BULK)	+= include/scsi/sbc_protocol.h
hdr-$(CONFIG_UDI_MSC_BULK)	+= include/scsi/spc_protocol.h
hdr-$(CONFIG_UDI_MSC_BULK)	+= include/usb/msc_protocol.h
hdr-$(CONFIG_UDI_MSC_BULK)	+= include/usb/udi_msc_bulk.h

mkfiles				+= $(src)/drivers/usb/udi/subdir.mk
