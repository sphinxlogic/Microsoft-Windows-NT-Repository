$ lib/insert wx.olb [-.contrib.wxxpm.libxpm_34b.lib]*.obj /log
$ lib/insert wx.olb [-.utils.image.src]*.obj              /log
$ lib/insert wx.olb [-.contrib.wxstring]*.obj             /log
