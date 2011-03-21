cflags-gnu-y	+= -std=gnu99

incdir-y	+= $(src)/apps/display-demo/include
src-y		+= apps/$(app)/main.c
src-y		+= apps/$(app)/app_desktop.c
src-y		+= apps/$(app)/app_calibrate.c
src-y		+= apps/$(app)/app_calc.c
src-y		+= apps/$(app)/app_clock.c
src-$(CONFIG_FS_TSFS)   += apps/$(app)/app_fonts.c
src-$(CONFIG_FS_TSFS)   += apps/$(app)/app_files.c
src-$(CONFIG_FS_TSFS)   += apps/$(app)/app_slideshow.c
src-$(CONFIG_FS_TSFS)	+= apps/$(app)/app_tank.c
src-$(CONFIG_FS_TSFS)   += apps/$(app)/app_memgame.c
src-y		+= apps/$(app)/app_widget.c

src-$(CONFIG_FS_TSFS)	+= apps/$(app)/file_loader.c

app-hdr-y	+= softirq.h version.h membag.h sysfont.h win.h wtk.h
hdr-y		+= $(addprefix apps/display-demo/include/app/,$(app-hdr-y))

hdr-y		+= apps/$(app)/app_desktop.h
hdr-y		+= apps/$(app)/app_calibrate.h
hdr-y		+= apps/$(app)/app_calc.h
hdr-y		+= apps/$(app)/app_clock.h
hdr-$(CONFIG_FS_TSFS)	+= apps/$(app)/app_fonts.h
hdr-$(CONFIG_FS_TSFS)   += apps/$(app)/app_files.h
hdr-$(CONFIG_FS_TSFS)   += apps/$(app)/app_slideshow.h
hdr-$(CONFIG_FS_TSFS)	+= apps/$(app)/app_tank.h
hdr-$(CONFIG_FS_TSFS)   += apps/$(app)/app_memgame.h
hdr-y		+= apps/$(app)/app_widget.h

hdr-$(CONFIG_FS_TSFS)	+= apps/$(app)/file_loader.h
