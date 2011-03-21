touch-subdir-$(CONFIG_TOUCH_RESISTIVE)	+= $(src)/drivers/touch/resistive

include $(addsuffix /subdir.mk, $(touch-subdir-y))

mkfiles					+= $(src)/drivers/touch/subdir.mk
