incdir-y        += $(src)/apps/xplain-bc/include
src-y           += apps/xplain-bc/main.c
src-y           += apps/xplain-bc/dataflash.c

app-hdr-y       += dmapool.h softirq.h usb.h version.h
hdr-y           += $(addprefix apps/xplain-bc/include/app/,$(app-hdr-y))
hdr-y           += apps/xplain-bc/xplain-bc.h
