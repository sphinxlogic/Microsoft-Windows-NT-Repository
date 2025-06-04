$! include annotate_imagerec.sci
$ scan annotate_main.scn
$ scan annotate_read_map.scn
$ cc annotate_convert_hex.c
$ cc annotate_lookup_address.c
$ link/exe=annotate sys$input/opt
annotate_main
annotate_read_map
annotate_convert_hex
annotate_lookup_address
sys$share:vaxcrtl/share
