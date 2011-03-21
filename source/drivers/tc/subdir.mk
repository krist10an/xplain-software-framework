timer-subdir-$(CONFIG_TIMER)		+= $(src)/drivers/tc/timer
timekeeper-subdir-$(CONFIG_TIMEKEEPER)	+= $(src)/drivers/tc/timekeeper

include $(addsuffix /subdir.mk, $(timer-subdir-y))
include $(addsuffix /subdir.mk, $(timekeeper-subdir-y))

mkfiles					+= $(src)/drivers/tc/subdir.mk
