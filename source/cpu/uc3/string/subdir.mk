uc3-string-y		+= memcpy.S

src-y			+= $(addprefix cpu/uc3/string/,$(uc3-string-y))

mkfiles			+= $(src)/cpu/uc3/string/subdir.mk
