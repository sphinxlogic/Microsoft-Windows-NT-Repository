objects = demo_insurance.obj, insure.obj

demo_insurance.exe	:  $(objects)
	link $(LINKFLAGS) $(objects)
