hdr-$(CONFIG_GFX_SYSFONT)       += include/gfx/sysfont.h
hdr-$(CONFIG_GFX_SYSFONT)       += include/gfx/default/sysfont.h
hdr-$(CONFIG_GFX_WIN)           += include/gfx/win.h
hdr-$(CONFIG_GFX_WIN)           += include/gfx/default/win.h
hdr-$(CONFIG_GFX_WTK)           += include/gfx/wtk.h
hdr-$(CONFIG_GFX_WTK)           += include/gfx/default/wtk.h

src-$(CONFIG_GFX_SYSFONT)       += util/gfx/sysfont.c
src-$(CONFIG_GFX_WIN)           += util/gfx/win.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_basic_frame.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_button.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_check_box.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_frame.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_label.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_progress_bar.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_radio_button.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_slider.c
src-$(CONFIG_GFX_WTK)           += util/gfx/wtk_dialogue_box.c

mkfiles                         += $(src)/util/gfx/subdir.mk
