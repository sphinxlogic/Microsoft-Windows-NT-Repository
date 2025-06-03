$   where = f$element(0, "]", f$environment("procedure")) + "]"
$   define  misc   'where'
$   sdf	    :==	@misc:set_def sdf=
$   asdf    :==	@misc:across par=
$   defdir  :==	@misc:def_dir par=
$   down    :==	@misc:down down=
$   find    :==	@misc:find
