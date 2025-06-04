$ define/nolog mme                 MMOV$INCLUDE:,[]
$ define/nolog x11                 MMOV$INCLUDE:,DECW$INCLUDE
$ define/nolog decc$user_include   mme,decw$include:
$ define/nolog sys                 decc$user_include
$ define/nolog decc$system_include decc$user_include
