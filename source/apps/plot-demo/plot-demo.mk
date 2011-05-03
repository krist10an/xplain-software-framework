cflags-gnu-y	+= -std=gnu99

incdir-y	+= $(src)/apps/plot-demo/include
src-y		+= apps/$(app)/main.c
src-y		+= apps/$(app)/app_calibrate.c
src-y		+= apps/$(app)/app_plot.c

app-hdr-y	+= softirq.h version.h membag.h sysfont.h win.h wtk.h
hdr-y		+= $(addprefix apps/plot-demo/include/app/,$(app-hdr-y))

hdr-y		+= apps/$(app)/app_calibrate.h
hdr-y		+= apps/$(app)/app_plot.h
