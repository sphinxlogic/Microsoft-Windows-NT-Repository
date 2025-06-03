$ !
$ ! BUILD.COM - command file to build DECtape driver components
$ !
$ macro/list [.src]dtdriver+sys$library:lib/library
$ macro/list [.src]tcrandom
$ link/map/full/share dtdriver+sys$system:sys.stb/selective+sys$input/option
base=0
$ link/notrace/map/full tcrandom
