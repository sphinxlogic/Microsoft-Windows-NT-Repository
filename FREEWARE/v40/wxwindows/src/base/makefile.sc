;    Last change:  JS    2 Jul 97    5:53 pm
# Symantec C++ makefile for the base objects
# called from src\makefile.sc

# configuration section (see src\makefile.sc) ###########################

WXDIR = $(WXWIN)

!include $(WXDIR)\src\makesc.env

INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

# default values overridden by src\makefile.sc

CC=sc
CFLAGS = -o -ml -W -Dwx_msw

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\fafa

OPTIONS=

# end of configuration section ##########################################

OBJS = wb_win.obj wb_data.obj wb_frame.obj wb_panel.obj wb_utils.obj wb_main.obj \
wb_item.obj wb_list.obj wb_obj.obj wb_text.obj wb_gdi.obj wb_dialg.obj \
wb_canvs.obj wb_dc.obj wb_mf.obj wb_ps.obj wx_enhdg.obj wb_hash.obj \
wb_ipc.obj wb_form.obj wb_timer.obj wb_help.obj wb_sysev.obj wb_stdev.obj \
wb_types.obj wb_mgstr.obj wb_res.obj wb_vlbox.obj wb_scrol.obj wb_stat.obj \
wx_lay.obj wx_doc.obj wb_print.obj wx_tbar.obj wx_bbar.obj wx_mem.obj wx_date.obj \
wb_cmdlg.obj wx_time.obj wx_frac.obj wx_split.obj

HDRS = $(BASEINC)\wb_item.h $(BASEINC)\wb_buttn.h \
$(BASEINC)\wb_check.h $(BASEINC)\wb_choic.h $(BASEINC)\wb_menu.h \
$(BASEINC)\wb_messg.h $(BASEINC)\wb_txt.h $(BASEINC)\wb_mtxt.h \
$(BASEINC)\wb_lbox.h $(BASEINC)\wb_slidr.h $(BASEINC)\wb_combo.h

all: $(OBJS)

wb_obj.obj: $(BASEINC)\wx_obj.h

wb_print.obj:     wb_print.$(SRCSUFF) $(BASEINC)\wx_print.h

wx_mem.obj:     wx_mem.$(SRCSUFF) $(BASEINC)\wx_mem.h

wb_win.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h

wb_main.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wb_frame.h $(BASEINC)\wx_utils.h

wb_frame.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h \
$(BASEINC)\wx_obj.h $(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h \
wb_frame.$(SRCSUFF) $(BASEINC)\wx_stdev.h

wb_panel.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h \
$(BASEINC)\wx_obj.h $(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h \
$(BASEINC)\wb_panel.h wb_panel.$(SRCSUFF) $(BASEINC)\wx_stdev.h

wb_text.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h $(BASEINC)\wb_text.h \
wb_text.$(SRCSUFF) $(BASEINC)\wx_stdev.h

wb_canvs.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h \
$(BASEINC)\wx_obj.h $(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h \
$(BASEINC)\wb_canvs.h wb_canvs.$(SRCSUFF) $(BASEINC)\wx_stdev.h \
$(BASEINC)\wb_gdi.h $(BASEINC)\wb_dc.h

wb_dc.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h $(BASEINC)\wb_canvs.h \
wb_dc.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(BASEINC)\wb_gdi.h $(BASEINC)\wb_dc.h

wb_ps.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h $(BASEINC)\wb_canvs.h \
wb_ps.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(BASEINC)\wb_gdi.h $(BASEINC)\wb_dc.h

wb_mf.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h $(BASEINC)\wb_canvs.h \
wb_mf.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(BASEINC)\wb_gdi.h $(BASEINC)\wb_mf.h

wb_item.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_frame.h $(HDRS) wb_item.$(SRCSUFF) \
$(BASEINC)\wx_stdev.h

wb_utils.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h wb_utils.$(SRCSUFF)

wb_ipc.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(BASEINC)\wb_ipc.h wb_ipc.$(SRCSUFF)

wb_list.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_list.h $(BASEINC)\wx_utils.h wb_list.$(SRCSUFF)

wb_hash.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_hash.h wb_hash.$(SRCSUFF) $(BASEINC)\wx_list.h \
$(BASEINC)\wx_utils.h

wb_event.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_stdev.h $(BASEINC)\wx_utils.h wb_event.$(SRCSUFF)

wb_gdi.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wb_gdi.h \
$(BASEINC)\wx_utils.h wb_gdi.$(SRCSUFF)

wb_dialg.obj: $(BASEINC)\wx_defs.h wb_dialg.$(SRCSUFF) $(BASEINC)\wb_dialg.h \
$(BASEINC)\wb_win.h $(BASEINC)\wx_utils.h $(BASEINC)\wb_panel.h $(HDRS)

wb_form.obj: $(BASEINC)\wx_defs.h wb_form.$(SRCSUFF) $(BASEINC)\wx_form.h \
$(BASEINC)\wb_win.h $(BASEINC)\wx_utils.h $(BASEINC)\wb_panel.h

wb_timer.obj: $(BASEINC)\wx_defs.h wb_timer.$(SRCSUFF) $(BASEINC)\wb_timer.h

wb_help.obj: $(BASEINC)\wx_defs.h wb_help.$(SRCSUFF) $(BASEINC)\wx_help.h \
$(MSWINC)\wx_ipc.h

wb_types.obj: $(BASEINC)\wx_defs.h wb_types.$(SRCSUFF) $(BASEINC)\wx_types.h

wb_mgstr.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_mgstr.h wb_mgstr.$(SRCSUFF)

wb_sysev.obj: $(BASEINC)\wx_defs.h wb_sysev.$(SRCSUFF) $(BASEINC)\wx_sysev.h \
$(BASEINC)\wx_types.h

wb_stdev.obj: $(BASEINC)\wx_defs.h wb_stdev.$(SRCSUFF) $(BASEINC)\wx_stdev.h \
$(BASEINC)\wx_types.h $(BASEINC)\wx_sysev.h

wx_enhdg.obj: $(BASEINC)\wx_defs.h wx_enhdg.$(SRCSUFF) $(BASEINC)\wx_enhdg.h

wb_data.obj: $(BASEINC)\wx_defs.h wb_data.$(SRCSUFF)

wb_res.obj: $(BASEINC)\wx_res.h wb_res.$(SRCSUFF)

wb_stat.obj: $(MSWINC)\wx_stat.h wb_stat.$(SRCSUFF)

wb_scrol.obj: $(MSWINC)\wx_scrol.h wb_scrol.$(SRCSUFF)

wb_vlbox.obj: $(MSWINC)\wx_vlbox.h wb_vlbox.$(SRCSUFF)

wx_lay.obj: $(BASEINC)\wx_lay.h wx_lay.$(SRCSUFF)

wx_doc.obj: $(BASEINC)\wx_doc.h wx_doc.$(SRCSUFF)

wx_tbar.obj: $(BASEINC)\wx_tbar.h wx_tbar.$(SRCSUFF)

wx_bbar.obj: $(BASEINC)\wx_bbar.h wx_bbar.$(SRCSUFF)

wx_date.obj: $(BASEINC)\wx_date.h wx_date.$(SRCSUFF)

wx_time.obj: $(BASEINC)\wx_time.h wx_time.$(SRCSUFF)

wx_frac.obj: $(BASEINC)\wx_frac.h wx_frac.$(SRCSUFF)

wx_split.obj: $(BASEINC)\wx_split.h wx_split.$(SRCSUFF)

wb_cmdlg.obj: $(BASEINC)\wb_cmdlg.h wb_cmdlg.$(SRCSUFF)

clean:
	-del *.obj
