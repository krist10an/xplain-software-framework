cflags-gnu-y	+= -std=gnu99

incdir-y	+= $(src)/apps/display-training/include
src-y		+= apps/$(app)/main.c
src-y		+= apps/$(app)/app_calibrate.c
src-y		+= apps/$(app)/app_widget.c

app-hdr-y	+= softirq.h version.h membag.h sysfont.h win.h wtk.h
hdr-y		+= $(addprefix apps/display-training/include/app/,$(app-hdr-y))

hdr-y		+= apps/$(app)/app_calibrate.h
hdr-y		+= apps/$(app)/app_widget.h
