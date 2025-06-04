$! INT_PASS.COM - Process INTOUCH pass commands
$! Copyright (c) 1987, Touch Technologies, Inc.
$!
$ define/job/nolog int_pass_status "1"
$ set noverify
$ set nocontrol=(t)
$ set working_set/adjust/limit=400/nolog
$ owner = f$getjpi("","owner")
$ old_default = f$environment("DEFAULT")
$ set noon
$ open/read INPUT_'owner' TTI_MBX_'owner':
$ open cmd_file sys$command
$ do_read:
$   set noon
$   set default 'old_default'
$   define/process sys$input sys$command/nolog
$   read input_'owner' z0
$   cmd_line == z0
$   if cmd_line .eqs. "$" then goto loop_around
$   'cmd_line'
$ finish_cmd:
$   z = '$status'
$   define/job/nolog int_pass_status 'z'
$   set noverify
$   read input_'owner' dummy
$ goto do_read
$!
$ loop_around:
$ write sys$output "Passed to VMS...  Press  RETURN  when done.  ''old_default'"
$ read_loop:
$   define/process sys$input sys$command/nolog
$    inquire/nopunctuation z0 "$$ "
$!   read/prompt="$$ "/end_of_file=finish_cmd cmd_file z0
$   cmd_line == z0
$   if cmd_line .eqs. "" then goto finish_cmd
$   'cmd_line'
$ goto read_loop
