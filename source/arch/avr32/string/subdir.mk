avr32-string-y		+= memset.S

src-y			+= $(addprefix arch/avr32/string/,$(avr32-string-y))

mkfiles			+= $(src)/arch/avr32/string/subdir.mk
