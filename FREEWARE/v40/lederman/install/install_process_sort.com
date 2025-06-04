$ SORT /STAT install_process.seq /KEY = (POSI = 106, SIZE = 18, DESC) -
    /KEY = (POSI = 65, SIZE = 32) -
    install_process.seq
$ EXIT
