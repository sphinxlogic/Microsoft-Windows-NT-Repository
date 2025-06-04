objects = bank_demo.obj, banking.obj

bank_demo.exe	:  $(objects)
	link $(LINKFLAGS) $(objects)
