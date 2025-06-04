default :
	__arch__= f$getsyi ("Arch_name")
	if f$getsyi ("Arch_name").eqs."Alpha" then __alpha__ =1
	if f$search ("$(__ARCH__).DIR;1").eqs."" then create/dir/log [.$(__ARCH__)]
	$(MMS) $(MMS$QUALIFIERS) arch_build
#

ARCH_DIR= [.$(__ARCH__)]
SRC_DIR= [.SRC]

.ifdef __ALPHA__
SUFF= _ALPHA
SYSEXE=/SYSEXE
acc_ckpt$unl=
.suffixes : .mar .obj
.mar.obj :
	macro sys$share:arch_defs+'f$search("$?") $(MFLAGS)
.else
SUFF=_VAX
SYSEXE=,sys$system:sys.stb/sel
acc_ckpt$unl=$(ARCH_DIR)acc_ckpt$unl.exe
.endif

arch_build : $(ARCH_DIR)acc_ckpt.exe $(ARCH_DIR)acc_ckpt$main.exe $(acc_ckpt$unl)
	@ write sys$output "Build done..."

$(ARCH_DIR)acc_ckpt.exe : $(ARCH_DIR)acc_ckpt.obj, descrip.mms, \
		$(SRC_DIR)acc_ckpt$(SUFF).opt, $(SRC_DIR)acc_ckpt.opt
	link /notrace/share=$@/map=$(ARCH_DIR)/full \
		$(ARCH_DIR)acc_ckpt.obj,\
			$(SRC_DIR)acc_ckpt$(SUFF).opt/option, \
			$(SRC_DIR)acc_ckpt.opt/option $(SYSEXE)

$(ARCH_DIR)acc_ckpt$main.exe : $(ARCH_DIR)acc_ckpt$main.obj, descrip.mms, \
			  $(SRC_DIR)acc_ckpt.opt
	link /exe=$@ $(LFLAGS) $(ARCH_DIR)acc_ckpt$main.obj,\
				$(SRC_DIR)acc_ckpt.opt/option $(SYSEXE)

$(ARCH_DIR)acc_ckpt$unl.exe : $(ARCH_DIR)acc_ckpt$unl.obj, descrip.mms, \
			  $(SRC_DIR)acc_ckpt.opt
	link /exe=$@ $(LFLAGS) $(ARCH_DIR)acc_ckpt$unl.obj,\
				$(SRC_DIR)acc_ckpt.opt/option $(SYSEXE) 

$(ARCH_DIR)acc_ckpt.obj : $(SRC_DIR)acc_ckpt.mar
$(ARCH_DIR)acc_ckpt$main.obj : $(SRC_DIR)acc_ckpt$main.mar
$(ARCH_DIR)acc_ckpt$unl.obj : $(SRC_DIR)acc_ckpt$unl.mar
