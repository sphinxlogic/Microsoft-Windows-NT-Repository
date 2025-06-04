objects = telecomm.obj, vendor_change.obj, custom_service.obj, -
	  field.obj, dist.obj

telecomm.exe	:  $(objects)
	link $(LINKFLAGS) $(objects)

