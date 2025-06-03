$ config_file= f$parse ("TCP_SECURITY.DAT;",f$environment("Procedure"))
$ define tcp_security$cfg 'f$element (0,";",config_file)! allow version change
