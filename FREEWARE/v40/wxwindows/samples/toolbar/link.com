$ link /trace /nomap /nodebug /exec=test.exe test.obj,[DECW_PORT.WXWINDOWS.lib]wx.olb/lib,[DECW_PORT.WXWINDOWS.lib]wxstring.olb/lib,[DECW_PORT.WXWINDOWS.src]motif.opt/option
