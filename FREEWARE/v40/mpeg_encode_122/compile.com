$ def sys sys$library
$ CC/DEFINE=VMS CHENDCT.C
$ CC/DEFINE=VMS CODEC.C
$ CC/DEFINE=VMS HUFFMAN.C
$ CC/DEFINE=VMS IO.C
$ CC/DEFINE=VMS LEXER.C
$ CC/DEFINE=VMS MARKER.C
$ CC/DEFINE=VMS ME.C
$ CC/DEFINE=VMS MEM.C
$ CC/DEFINE=VMS MPEG.C
$ CC/DEFINE=VMS STAT.C
$ CC/DEFINE=VMS STREAM.C
$ CC/DEFINE=VMS TRANSFORM.C
