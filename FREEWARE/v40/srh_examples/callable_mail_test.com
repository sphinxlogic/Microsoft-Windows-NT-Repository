$ cc callable_mail_test
$ link callable_mail_test,sys$input/option
sys$share:vaxcrtl/share
sys$share:mailshr/share
$ purge callable_mail_test.*/log
