objects = act_demo.obj, banking.obj, field.obj, dist.obj, -
		labor.obj, info_serve.obj, financial.obj

act_demo.exe	:  $(objects)
	link $(LINKFLAGS) $(objects)

