#
# Maintainence productions for the Lisp directory
#
SOURCES = $(srcdir_dev)[$(srcdir_dir)]*.el, -
	$(srcdir_dev)[$(srcdir_dir)]COPYING, -
	$(srcdir_dev)[$(srcdir_dir)]descrip.mms
lisptagsfiles = $(srcdir_dev)[$(srcdir_dir)]*.el, -
	$(srcdir_dev)[$(srcdir_dir).term]*.el

unlock :
	set file/prot=(o:rwed) $(SOURCES)
	! chmod u+w $(SOURCES)

relock :
	set file/prot=(o:re) $(SOURCES)
	set file/prot=(o:rwed) version.el, finder-inf.el, loaddefs.el
	! chmod -w $(SOURCES)
	! chmod +w version.el finder-inf.el loaddefs.el

TAGS : $(lisptagsfiles)
	mcr sys$disk:[-.lib-src]etags $(lisptagsfiles)
	! ../lib-src/etags $(lisptagsfiles)
